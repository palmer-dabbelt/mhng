
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

#ifndef LIBMHIMAP__GMAIL_CLIENT_HXX
#define LIBMHIMAP__GMAIL_CLIENT_HXX

#include "ssl_client.h++"
#include <string>

namespace mhimap {
    /* A GMail client, which is pretty much an IMAP client but with a
     * special set of rules to prevent it from causing trouble when
     * connecting to a GMail server. */
    class gmail_client: public ssl_client {
    private:
    public:
        /* Opens a new connection to a GMail server, given just the
         * username and password.  The set of supported authentication
         * mechanisms and such is hard-coded into this class, which is
         * really why it exists. */
        gmail_client(const std::string username,
                     const std::string password
            );
    };
}

#endif
