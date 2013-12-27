
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

#ifndef LIBMHIMAP__SSL_CLIENT_HXX
#define LIBMHIMAP__SSL_CLIENT_HXX

#include "client.h++"
#include <string>
#include <gnutls/gnutlsxx.h>

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
                   const std::string username,
                   const std::string password,
                   const std::string priority="NORMAL"
            );

    protected:
        /* vtable methods from libmhimap::client. */
        ssize_t read(char *buffer, ssize_t buffer_size);
        ssize_t write(char *buffer, ssize_t buffer_size);
    };
}

#endif
