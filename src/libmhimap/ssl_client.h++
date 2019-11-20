/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHIMAP__SSL_CLIENT_HXX
#define LIBMHIMAP__SSL_CLIENT_HXX

#include "client.h++"
#include <libmhoauth/access_token.h++>
#include <gnutls/gnutlsxx.h>
#include <string>

namespace mhimap {
    /* An IMAP client, which represents a client-side connection to an
     * IMAP server. */
    class ssl_client: public client {
    public:
        /* The length of the internal line bufer. */
        const static size_t buffer_size = 1024;

    private:
        /* Holds the connection to the server. */
        int server_fd;
        gnutls::client_session session;
        gnutls::certificate_credentials credentials;

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

        ~ssl_client(void);

        /* vtable methods from libmhimap::client. */
    public:
        bool is_connected(void) const;
    protected:
        ssize_t read(char *buffer, ssize_t buffer_size);
        ssize_t write(char *buffer, ssize_t buffer_size);
    };
}

#endif
