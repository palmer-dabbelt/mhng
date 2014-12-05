
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

#ifndef LIBMHNG__DAEMON__CONNECTION_HXX
#define LIBMHNG__DAEMON__CONNECTION_HXX

#include <memory>

namespace mhng {
    namespace daemon {
        class connection;
        typedef std::shared_ptr<connection> connection_ptr;
    }
}

#include <map>
#include <mutex>
#include <string>
#include <thread>
#include "message.h++"
#include "response.h++"

namespace mhng {
    namespace daemon {
        /* An interface that can either contain a real connection to
         * the daemon, or a dummy connection that doesn't do anything
         * at all. */
        class connection {
        public:
            /* Sends a message and returns an object that can be used
             * to interrogate the state of a response from the server.
             * Note that this doesn't block, if you want it to block
             * you'll need to query the response object directly. */
            virtual response_ptr send(const message_ptr& message) = 0;
        };
    }
}

#endif
