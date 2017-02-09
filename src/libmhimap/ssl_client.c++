
/*
 * Copyright (C) 2013 Palmer Dabbelt
 *   <palmer@dabbelt.com>
 *
 * This file is part of mhng.
 *
 * mhng is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mhng is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with mhng.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    : client(),
      session(),
      credentials(),
      buffer(new char[buffer_size])
{
    logger l("ssl_client::ssl_client('%s', %d, '%s', ..., '%s')",
             hostname.c_str(), port, username.c_str(), priority.c_str());

    /* GNUTLS boilerplate from the manual. */
    l.printf("server_fd = gnutls_tcp_connect('%s', %d)",
             hostname.c_str(), port);
    server_fd = gnutls_tcp_connect(hostname.c_str(), port);

    l.printf("credentials.set_x509_trust_file('%s', %d)", CAFILE, CAFMT);
    credentials.set_x509_trust_file(CAFILE, CAFMT);
    session.set_credentials(credentials);

    l.printf("session.set_priority('%s', NULL)", priority.c_str());
    session.set_priority(priority.c_str(), NULL);

    l.printf("session.set_server_name(..._DNS, '%s')", hostname.c_str());
    session.set_server_name(GNUTLS_NAME_DNS, cstr_len(hostname));

    l.printf("session.set_transport_ptr(%d)", server_fd);
    session.set_transport_ptr((gnutls_transport_ptr_t) (ptrdiff_t) server_fd);

    l.printf("session.handshake()");
    ssize_t ret = session.handshake();
    if (ret < 0) {
        throw std::runtime_error("Handshake failed");
    }

    /* We're already secure, so we can proceed directly to the
     * authentication phase. */
    if (eat_hello() != 0) {
        fprintf(stderr, "Unexpected hello from IMAP\n");
        abort();
    }

    if (authenticate(username, password) != 0) {
        fprintf(stderr, "Unable to authenticate as '%s'\n", username.c_str());
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

    return session.recv(buffer, buffer_size);
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
