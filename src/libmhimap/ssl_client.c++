/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "ssl_client.h++"
#include "logger.h++"
#include <arpa/inet.h>
#include <gnutls/gnutlsxx.h>
#include <gnutls/x509.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <netdb.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#ifdef __APPLE__
#include <arpa/nameser.h>
#include <resolv.h>
#endif

using namespace mhimap;

#ifndef CAFILE
#ifdef __APPLE__
#define CAFILE "/opt/homebrew/share/ca-certificates/cacert.pem"
#elif defined(__LINUX__)
#define CAFILE "/usr/share/ca-certificates/mozilla/GTS_Root_R1.crt"
#else
#error "Unknown default CA target"
#endif
#endif

#ifndef CAFMT
#define CAFMT GNUTLS_X509_FMT_PEM
#endif

#ifndef RETRIES
#define RETRIES 10
#endif

#define cstr_len(str) str.c_str(), strlen(str.c_str())

static void gnutls_ssl_init(void) __attribute__((constructor));
static inline int gnutls_tcp_connect(const std::string hostname,
                                     uint16_t port);
static inline void *get_in_addr(const struct sockaddr *sa);

ssl_client::ssl_client(const std::string _hostname,
                       uint16_t _port,
                       std::string _username,
                       libmhoauth::access_token token,
                       std::string _priority)
    : client(account(_username)),
      hostname(_hostname),
      port(_port),
      username(_username),
      priority(_priority),
      session(),
      credentials(),
      buffer(new char[buffer_size])
{
    basic_init([this, _username, token]{ return authenticate(_username, token); });
}

ssl_client::ssl_client(const std::string _hostname,
                       uint16_t _port,
                       std::string _username,
                       std::string password,
                       std::string _priority)
    : client(account(_username)),
      hostname(_hostname),
      port(_port),
      username(_username),
      priority(_priority),
      session(),
      credentials(),
      buffer(new char[buffer_size])
{
    basic_init([this, _username, password]{ return authenticate(_username, password); });
}

ssl_client::~ssl_client(void) {
    logout();
}

bool ssl_client::is_connected(void) const
{
    /* FIXME: How should we detect a disconnected GNUTLS socket? */
    return true;
}

ssize_t ssl_client::read(char *buffer, ssize_t buffer_size)
{
    if (buffer_size == 0) {
        fprintf(stderr, "ssl_client::read(): invalid buffer size of 0\n");
        abort();
    }

    for (size_t i = 0; i < RETRIES; ++i) {
        try {
            return session.recv(buffer, buffer_size);
        } catch (const gnutls::exception& e) {
            std::cerr << "GNUTLS exception thrown during read(), retrying\n";
            std::cerr << "  " << std::string(e.what()) << "\n";
        }
    }

    std::cerr << "Too many GNUTLS exceptions thrown\n";
    abort();
}

ssize_t ssl_client::write(char *buffer, ssize_t buffer_size)
{
    return session.send(buffer, buffer_size);
}

void gnutls_ssl_init(void)
{
    gnutls_global_init();
}

/* A single resolved endpoint, flattened out of whatever resolver
 * produced it so the connect loop doesn't care about the source. */
namespace {
    struct resolved_addr {
        struct sockaddr_storage addr;
        socklen_t addrlen;
        int family;
    };
}

#ifndef __APPLE__
/* Flatten a getaddrinfo() result list into our vector. */
static void append_addrinfo(std::vector<resolved_addr> &out,
                            struct addrinfo *result)
{
    for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
        resolved_addr ra;
        memset(&ra, 0, sizeof(ra));
        memcpy(&ra.addr, rp->ai_addr, rp->ai_addrlen);
        ra.addrlen = rp->ai_addrlen;
        ra.family = rp->ai_family;
        out.push_back(ra);
    }
}
#endif

#ifdef __APPLE__
/* Direct DNS resolution that bypasses mDNSResponder.
 *
 * On macOS getaddrinfo() reaches DNS through mDNSResponder over the
 * process's Mach bootstrap port.  A daemon that inherited a bootstrap
 * port which can't reach mDNSResponder sees getaddrinfo() fail with
 * EAI_NONAME forever -- and so does every IMAP child it fork()+exec()s,
 * since the broken port is inherited and a fresh exec() doesn't replace
 * it.  We only ever resolve public IMAP/SMTP hostnames here, so there's
 * no reason to involve mDNS at all: res_*() talks straight to the
 * nameservers in resolv.conf over UDP/TCP, which is immune to the
 * bootstrap problem.  'type' is ns_t_a or ns_t_aaaa. */
static void append_direct_dns(std::vector<resolved_addr> &out,
                              const std::string &hostname,
                              uint16_t port,
                              int type)
{
    struct __res_state state;
    memset(&state, 0, sizeof(state));
    if (res_ninit(&state) != 0) {
        fprintf(stderr, "res_ninit() failed during direct DNS resolution\n");
        return;
    }

    unsigned char answer[NS_PACKETSZ];
    int len = res_nsearch(&state, hostname.c_str(), ns_c_in, type,
                          answer, sizeof(answer));
    if (len < 0) {
        res_ndestroy(&state);
        return;
    }

    ns_msg msg;
    if (ns_initparse(answer, len, &msg) < 0) {
        res_ndestroy(&state);
        return;
    }

    int count = ns_msg_count(msg, ns_s_an);
    for (int i = 0; i < count; ++i) {
        ns_rr rr;
        if (ns_parserr(&msg, ns_s_an, i, &rr) < 0)
            continue;
        if (ns_rr_type(rr) != type)
            continue;

        resolved_addr ra;
        memset(&ra, 0, sizeof(ra));
        if (type == ns_t_a && ns_rr_rdlen(rr) == sizeof(struct in_addr)) {
            struct sockaddr_in *sin = (struct sockaddr_in *)&ra.addr;
            sin->sin_family = AF_INET;
            sin->sin_port = htons(port);
            memcpy(&sin->sin_addr, ns_rr_rdata(rr), sizeof(struct in_addr));
            ra.addrlen = sizeof(struct sockaddr_in);
            ra.family = AF_INET;
            out.push_back(ra);
        } else if (type == ns_t_aaaa &&
                   ns_rr_rdlen(rr) == sizeof(struct in6_addr)) {
            struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&ra.addr;
            sin6->sin6_family = AF_INET6;
            sin6->sin6_port = htons(port);
            memcpy(&sin6->sin6_addr, ns_rr_rdata(rr), sizeof(struct in6_addr));
            ra.addrlen = sizeof(struct sockaddr_in6);
            ra.family = AF_INET6;
            out.push_back(ra);
        }
    }

    res_ndestroy(&state);
}
#endif

/* Resolve hostname:port into a list of connectable endpoints.  macOS
 * queries the nameservers directly (see append_direct_dns); everywhere
 * else uses getaddrinfo().  Throws if nothing resolves. */
static std::vector<resolved_addr> resolve_host(const std::string &hostname,
                                               uint16_t port)
{
    logger l("resolve_host('%s', %d)", hostname.c_str(), port);
    std::vector<resolved_addr> out;

#ifdef __APPLE__
    l.printf("direct DNS for '%s'", hostname.c_str());
    append_direct_dns(out, hostname, port, ns_t_a);
    append_direct_dns(out, hostname, port, ns_t_aaaa);
    if (out.empty()) {
        fprintf(stderr, "direct DNS resolution failed for '%s'\n",
                hostname.c_str());
        throw std::runtime_error("direct DNS resolution failed for '"
                                 + hostname + "'");
    }
    return out;
#else
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    char port_s[16];
    snprintf(port_s, sizeof(port_s), "%u", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_protocol = 0;

    l.printf("getaddrinfo('%s', '%s')", hostname.c_str(), port_s);
    int err = getaddrinfo(hostname.c_str(), port_s, &hints, &result);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: '%s'\n", gai_strerror(err));
        throw std::runtime_error(std::string("getaddrinfo: ") + gai_strerror(err));
    }

    append_addrinfo(out, result);
    freeaddrinfo(result);
    return out;
#endif
}

int gnutls_tcp_connect(const std::string hostname, uint16_t port)
{
    logger l("gnutls_tcp_connect('%s', %d)", hostname.c_str(), port);

    std::vector<resolved_addr> addrs = resolve_host(hostname, port);

    for (const auto &ra : addrs) {
        const struct sockaddr *sa = (const struct sockaddr *)&ra.addr;

        l.printf("socket(%d) v4=%d,v6=%d", ra.family, AF_INET, AF_INET6);
        int sock = socket(ra.family, SOCK_STREAM, 0);
        l.printf("  => %d", sock);
        if (sock == -1)
            continue;

        /* Format and print the IP address we're connecting to. */
        {
            char buffer[INET6_ADDRSTRLEN + 1];
            const char *bp = inet_ntop(ra.family, get_in_addr(sa),
                                       buffer, sizeof(buffer));
            if (bp == NULL) {
                fprintf(stderr, "Unable to lookup IP address\n");
                perror("inet_ntop() failed");
                abort();
            }

            l.printf("connect(%d, '%s')", sock, bp);
        }

        /* Disable Naegel's algorithm, apparently it screws with TLS.
         * Use IPPROTO_TCP directly rather than getprotobyname("TCP"),
         * which routes through the same (possibly wedged) system
         * resolver we just worked around. */
        l.printf("setsockopt(%d, %d, TCP_NODELAY, 1)", sock, IPPROTO_TCP);
        int v = 1;
        int r = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v));
        if (r < 0) {
            perror("setsockopt(TCP_NODELAY)");
            abort();
        }

        /* Attempt to connect, if that works then return the socket. */
        if (connect(sock, sa, ra.addrlen) != -1)
            return sock;

        close(sock);
    }

    fprintf(stderr, "Could not connect to '%s:%d'\n", hostname.c_str(), port);
    return -1;
}

void *get_in_addr(const struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sa)->sin_addr);
    if (sa->sa_family == AF_INET6)
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);

    abort();
}

void ssl_client::basic_init(std::function<int()> authenticate)
{
    logger l("ssl_client::ssl_client('%s', %d, '%s', ..., '%s')",
             hostname.c_str(), port, username.c_str(), priority.c_str());

    try {
        /* GNUTLS boilerplate from the manual. */
        try {
            l.printf("server_fd = gnutls_tcp_connect('%s', %d)",
                     hostname.c_str(), port);
            server_fd = gnutls_tcp_connect(hostname.c_str(), port);
        } catch (const gnutls::exception& e) {
            std::cerr << "GNUTLS exception thrown during gnutls_tcp_connect\n";
            throw e;
        }

        try {
            l.printf("credentials.set_x509_trust_file('%s', %d)", CAFILE, CAFMT);
            credentials.set_x509_trust_file(CAFILE, CAFMT);
            session.set_credentials(credentials);
        } catch (const gnutls::exception& e) {
            std::cerr << "GNUTLS exception thrown during session.set_credentials\n";
            throw e;
        }

        try {
            l.printf("session.set_priority('%s', NULL)", priority.c_str());
            session.set_priority(priority.c_str(), NULL);
        } catch (const gnutls::exception& e) {
            std::cerr << "GNUTLS exception thrown during session.set_priority\n";
            throw e;
        }

        try {
            l.printf("session.set_server_name(..._DNS, '%s')", hostname.c_str());
            session.set_server_name(GNUTLS_NAME_DNS, cstr_len(hostname));
        } catch (const gnutls::exception& e) {
            std::cerr << "GNUTLS exception thrown during session.set_server_name\n";
            throw e;
        }

        try {
            l.printf("session.set_transport_ptr(%d)", server_fd);
            session.set_transport_ptr((gnutls_transport_ptr_t) (ptrdiff_t) server_fd);
        } catch (const gnutls::exception& e) {
            std::cerr << "GNUTLS exception thrown during session.set_transport_ptr\n";
            throw e;
        }

        l.printf("session.handshake()");
        auto finished = false;
        for (size_t i = 0; !finished && i < RETRIES; ++i) {
            try {
                ssize_t ret = session.handshake();
                if (ret < 0) {
                    throw std::runtime_error("Handshake failed");
                }
                finished = true;
            } catch (const gnutls::exception& e) {
                std::cerr << "GNUTLS exception thrown during handshake, retrying\n";
                std::cerr << "  " << std::string(e.what()) << "\n";
            }
        }

        /* We're already secure, so we can proceed directly to the
         * authentication phase. */
        try {
            if (eat_hello() != 0) {
                fprintf(stderr, "Unexpected hello from IMAP\n");
                abort();
            }
        } catch (const gnutls::exception& e) {
            std::cerr << "GNUTLS exception thrown during eat_hello\n";
            throw e;
        }

        try {
            if (authenticate() != 0) {
                fprintf(stderr, "Unable to authenticate as '%s'\n", username.c_str());
                abort();
            }
        } catch (const gnutls::exception& e) {
            std::cerr << "GNUTLS exception thrown during authentication\n";
            throw e;
        }
    } catch (const gnutls::exception& e) {
        std::cerr << "GNUTLS exception thrown\n";
        std::cerr << "  " << std::string(e.what()) << "\n";
        abort();
    } catch (...) {
        throw;
    }
}


