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

using namespace mhimap;

#ifndef CAFILE
#define CAFILE "/usr/share/ca-certificates/mozilla/VeriSign_Universal_Root_Certification_Authority.crt"
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

ssl_client::ssl_client(const std::string hostname,
                       uint16_t port,
                       const std::string username,
                       const std::string password,
                       const std::string priority)
    : client(account(username)),
      session(),
      credentials(),
      buffer(new char[buffer_size])
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
            if (authenticate(username, password) != 0) {
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
        std::cerr << "GNUTLS exception thrown\n";
        std::cerr << "  Maybe " CAFILE " doesn't exist?\n";
        abort();
    }
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

int gnutls_tcp_connect(const std::string hostname, uint16_t port)
{
    logger l("gnutls_tcp_connect('%s', %d)", hostname.c_str(), port);

    int err;
    struct addrinfo hints;
    struct addrinfo *result;
    char port_s[1024];

    snprintf(port_s, 1024, "%d", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_protocol = 0;

    l.printf("getaddrinfo('%s', '%s')", hostname.c_str(), port_s);
    err = getaddrinfo(hostname.c_str(), port_s, &hints, &result);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo: '%s'\n", gai_strerror(err));
        abort();
    }

    for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
        l.printf("socket(%d) v4=%d,v6=%d", rp->ai_family, AF_INET, AF_INET6);
        int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        l.printf("  => %d", sock);
        if (sock == -1)
            continue;

        /* Format and print the IP address we're connecting to. */
        {
            char buffer[INET_ADDRSTRLEN + INET6_ADDRSTRLEN + 1];
            const char *bp;
            void *in_addr = get_in_addr(rp->ai_addr);
            bp = inet_ntop(rp->ai_family, in_addr, buffer, rp->ai_addrlen);
            if (bp == NULL) {
                fprintf(stderr, "Unable to lookup IP address\n");
                perror("inet_ntop() failed");
                abort();
            }

            l.printf("connect(%d, '%s')", sock, bp);
        }

        /* Disable Naegel's algorithm, apparently it screws with TLS. */
        struct protoent *pe = getprotobyname("TCP");
        if (pe == NULL) {
            fprintf(stderr, "Unable to find TCP protocol\n");
            perror("getprotobyname('TCP')");
            abort();
        }

        l.printf("setsockopt(%d, %d, TCP_NODELAY, 1)", sock, pe->p_proto);
        int v = 1;
        int r = setsockopt(sock, pe->p_proto, TCP_NODELAY, &v, sizeof(v));
        if (r < 0) {
            perror("setsockopt(TCP_NODELAY)");
            abort();
        }

        /* Attempt to connect, if that works then return the socket. */
        if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1) {
            freeaddrinfo(result);
            return sock;
        }

        close(sock);
    }

    freeaddrinfo(result);

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
