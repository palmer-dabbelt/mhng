
/*
 * Copyright (C) 2014 Palmer Dabbelt
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

#ifndef LIBMHNG__DAEMON__DUMMY_CONNECTION_HXX
#define LIBMHNG__DAEMON__DUMMY_CONNECTION_HXX

#include <memory>

namespace mhng {
    namespace daemon {
        class dummy_connection;
        typedef std::shared_ptr<dummy_connection> dummy_connection_ptr;
    }
}

#include "connection.h++"

namespace mhng {
    namespace daemon {
        /* Holds a client connection to the MHng daemon running on
         * this system. */
        class dummy_connection: public connection {
        private:

        public:
            /* Creates a new connection to the server. */
            dummy_connection(void);

            /* Closes the connection to the server. */
            ~dummy_connection(void);

        public:
            virtual response_ptr send(const message_ptr& message);
        };
    }
}

#endif
