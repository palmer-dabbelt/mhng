
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

#ifndef LIBMHIMAP__CLIENT_HXX
#define LIBMHIMAP__CLIENT_HXX

#include <string>

namespace mhimap {
    /* An IMAP client, which represents a client-side connection to an
     * IMAP server. */
    class client {
    public:
        /* Creates a new connection to an IMAP server, given the
         * connection object that will be used to create it.  You
         * should probably be using one of the myriad of other client
         * classes that extend this one rather than calling this
         * directly. */
        client(void);
    };
}

#endif
