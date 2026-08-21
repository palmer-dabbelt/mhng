/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHIMAP__SSL_CLIENT_HXX
#define LIBMHIMAP__SSL_CLIENT_HXX

#define GNUTLS_GNUTLSXX_NO_HEADERONLY

#include "client.h++"
#include <libmhoauth/access_token.h++>
#include <gnutls/gnutlsxx.h>
#include <functional>
#include <string>

namespace mhimap {
    /* TRUE when retrying whatever GNUTLS call produced 'code' could
     * actually get somewhere.  Everything else has invalidated the
     * session, after which every call just fails the same way. */
    bool tls_error_is_retryable(int code);

    /* gnutlsxx keeps the C credentials handle protected and doesn't
     * wrap gnutls_certificate_set_x509_system_trust() at all, so this
     * exists to expose just enough of it to load a CA bundle the way
     * we want to. */
    class trust_credentials: public gnutls::certificate_credentials {
    public:
        /* Loads a set of trusted CAs, trying the compiled-in bundle,
         * then a list of well-known bundle paths, then GNUTLS' idea of
         * the system trust store.  Throws a std::runtime_error naming
         * everything it tried if none of them produce any CAs. */
        void load_trust(void);

        /* Loads the CAs from the first of 'paths' that has any,
         * returning how many were loaded and 0 if none of them do.
         * Split out of load_trust() so it can be pointed at something
         * other than the machine's real trust store. */
        int load_trust_files(const char * const *paths, size_t count);
    };

    /* An IMAP client, which represents a client-side connection to an
     * IMAP server. */
    class ssl_client: public client {
    public:
        /* The length of the internal line bufer. */
        const static size_t buffer_size = 1024;

    private:
        std::string hostname;
        uint16_t port;
        std::string username;
        std::string priority;

        /* Holds the connection to the server. */
        int server_fd;
        gnutls::client_session session;
        trust_credentials credentials;

        /* Here's where we hold a single line buffer. */
        char *buffer;

    public:
        /* Creates a connection to an IMAP client that speaks SSL. */
        ssl_client(const std::string hostname,
                   uint16_t port,
                   std::string username,
                   libmhoauth::access_token token,
                   std::string priority="NORMAL"
            );

        ssl_client(const std::string hostname,
                   uint16_t port,
                   std::string username,
                   std::string password,
                   std::string priority="NORMAL"
            );

        ~ssl_client(void);

        /* vtable methods from libmhimap::client. */
    public:
        bool is_connected(void) const;
    protected:
        ssize_t read(char *buffer, ssize_t buffer_size);
        ssize_t write(char *buffer, ssize_t buffer_size);
    private:
        void basic_init(std::function<int()> auth);
    };
}

#endif
