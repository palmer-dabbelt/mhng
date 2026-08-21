/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "ssl_client.h++"
#include "logger.h++"
#include <arpa/inet.h>
#include <errno.h>
#include <gnutls/gnutlsxx.h>
#include <gnutls/x509.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
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

/* CAFILE isn't the only place a CA bundle can live, and on macOS it's
 * a Homebrew path that vanishes for a moment every time
 * ca-certificates is upgraded -- which used to leave every IMAP
 * connection failing with "Error while reading file" until someone
 * noticed.  These are tried in order when CAFILE doesn't pan out. */
static const char *ca_bundle_paths[] = {
    CAFILE,
    "/etc/ssl/cert.pem",                      /* macOS, FreeBSD */
    "/opt/homebrew/etc/ca-certificates/cert.pem",
    "/opt/homebrew/share/ca-certificates/cacert.pem",
    "/usr/local/etc/ca-certificates/cert.pem",
    "/etc/ssl/certs/ca-certificates.crt",     /* Debian, Alpine */
    "/etc/pki/tls/certs/ca-bundle.crt",       /* Fedora, RHEL */
};

#ifndef RETRIES
#define RETRIES 10
#endif

#define cstr_len(str) str.c_str(), strlen(str.c_str())

/* How long to wait for the transport to become ready before giving up
 * on a GNUTLS_E_AGAIN.  The server has nothing to say for minutes at a
 * time during an IDLE, so this has to be generous. */
#ifndef TRANSPORT_TIMEOUT_MS
#define TRANSPORT_TIMEOUT_MS (30 * 60 * 1000)
#endif

static void gnutls_ssl_init(void) __attribute__((constructor));
static inline int gnutls_tcp_connect(const std::string hostname,
                                     uint16_t port);
static inline void *get_in_addr(const struct sockaddr *sa);
static void wait_for_transport(int fd, bool want_write);

bool mhimap::tls_error_is_retryable(int code)
{
    return code == GNUTLS_E_AGAIN || code == GNUTLS_E_INTERRUPTED;
}

int trust_credentials::load_trust_files(const char * const *paths, size_t count)
{
    logger l("trust_credentials::load_trust_files(..., " SIZET_FORMAT ")", count);

    for (size_t i = 0; i < count; ++i) {
        l.printf("gnutls_certificate_set_x509_trust_file('%s')", paths[i]);

        /* This counts the CAs it managed to load, so a bundle that's
         * readable but empty is a miss here just like a missing one --
         * which is what we want, as it can't verify anything. */
        int certs = gnutls_certificate_set_x509_trust_file(cred, paths[i],
                                                           CAFMT);
        if (certs > 0) {
            l.printf("  => loaded %d CAs", certs);
            return certs;
        }
    }

    return 0;
}

void trust_credentials::load_trust(void)
{
    logger l("trust_credentials::load_trust()");
    size_t count = sizeof(ca_bundle_paths) / sizeof(*ca_bundle_paths);

    if (load_trust_files(ca_bundle_paths, count) > 0)
        return;

    /* GNUTLS' own notion of a system trust store: a keychain on some
     * platforms, whatever bundle GNUTLS was built against on the rest.
     * It's last because on macOS it usually points right back at the
     * Homebrew path we just failed to read. */
    l.printf("gnutls_certificate_set_x509_system_trust()");
    int certs = gnutls_certificate_set_x509_system_trust(cred);
    if (certs > 0) {
        l.printf("  => loaded %d CAs", certs);
        return;
    }

    std::string tried;
    for (size_t i = 0; i < count; ++i) {
        tried += "\n  ";
        tried += ca_bundle_paths[i];
    }

    throw std::runtime_error("Unable to load any trusted CAs, tried the "
                             "GNUTLS system trust store and:" + tried);
}

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
            ssize_t n_read = session.recv(buffer, buffer_size);

            /* A zero-length read is a clean shutdown by the server.
             * Callers keep asking for data until they've got a whole
             * line, so they need to be told there won't be one. */
            if (n_read == 0) {
                std::cerr << "IMAP server closed the connection\n";
                return -1;
            }

            return n_read;
        } catch (gnutls::exception& e) {
            /* Every error but GNUTLS_E_AGAIN and GNUTLS_E_INTERRUPTED
             * invalidates the session, after which recv() just keeps
             * returning GNUTLS_E_INVALID_SESSION.  Retrying those only
             * bought us ten copies of the same message before we gave
             * up, so report the error that actually mattered and let
             * the caller deal with a dead connection. */
            if (!tls_error_is_retryable(e.get_code())) {
                std::cerr << "Lost the connection to the IMAP server\n";
                std::cerr << "  " << std::string(e.what()) << "\n";
                return -1;
            }

            std::cerr << "GNUTLS exception thrown during read(), retrying\n";
            std::cerr << "  " << std::string(e.what()) << "\n";
            wait_for_transport(server_fd, session.get_record_direction());
        }
    }

    std::cerr << "Too many GNUTLS exceptions thrown\n";
    return -1;
}

void wait_for_transport(int fd, bool want_write)
{
    /* GNUTLS_E_AGAIN means the transport couldn't make progress yet,
     * so wait for it rather than spinning through the retries as fast
     * as the CPU allows.  gnutls_record_get_direction() says which way
     * GNUTLS wants to move next. */
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = want_write ? POLLOUT : POLLIN;
    pfd.revents = 0;

    if (poll(&pfd, 1, TRANSPORT_TIMEOUT_MS) < 0 && errno != EINTR)
        perror("poll() failed while waiting for the IMAP server");
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
            l.printf("credentials.load_trust()");
            credentials.load_trust();
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
            } catch (gnutls::exception& e) {
                /* Only GNUTLS_E_AGAIN and friends leave the session in
                 * a state where another handshake() can get anywhere:
                 * anything else has invalidated it, so retrying just
                 * burns through the retry count. */
                if (!tls_error_is_retryable(e.get_code()))
                    throw;

                std::cerr << "GNUTLS exception thrown during handshake, retrying\n";
                std::cerr << "  " << std::string(e.what()) << "\n";
                wait_for_transport(server_fd, session.get_record_direction());
            }
        }

        /* Falling out of the loop unfinished used to look exactly like
         * a successful handshake, which turned into a pile of
         * confusing errors from the very first read(). */
        if (!finished)
            throw std::runtime_error("TLS handshake with '" + hostname
                                     + "' never completed");

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
        /* Failing to set a connection up is something servers, CA
         * bundles and networks do to us all the time, so hand it to
         * the caller as an error rather than dumping core. */
        throw std::runtime_error("GNUTLS error while connecting to '"
                                 + hostname + "': " + e.what());
    } catch (...) {
        throw;
    }
}


