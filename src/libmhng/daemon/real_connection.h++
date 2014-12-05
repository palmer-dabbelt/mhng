
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

#ifndef LIBMHNG__DAEMON__REAL_CONNECTION_HXX
#define LIBMHNG__DAEMON__REAL_CONNECTION_HXX

#include <memory>

namespace mhng {
    namespace daemon {
        class real_connection;
        typedef std::shared_ptr<real_connection> real_connection_ptr;
    }
}

#include <map>
#include <mutex>
#include <string>
#include <thread>
#include "connection.h++"
#include "message.h++"
#include "response.h++"

namespace mhng {
    namespace daemon {
        /* Holds a client real_connection to the MHng daemon running on
         * this system. */
        class real_connection: public connection {
        private:
            int _socket;

            /* A map that holds the outstanding responses that could
             * come back from the server. */
            std::map<uint32_t, response_wptr> _outstanding;
            std::mutex _outstanding_lock;

            /* A thread that waits for */
            std::thread _recv_thread;

        public:
            /* Creates a new real_connection to the server. */
            real_connection(const std::string& path);

            /* Closes the real_connection to the server. */
            ~real_connection(void);

        public:
            /* Sends a message and returns an object that can be used
             * to interrogate the state of a response from the server.
             * Note that this doesn't block, if you want it to block
             * you'll need to query the response object directly. */
            virtual response_ptr send(const message_ptr& message);

        private:
            static void recv_thread_wrapper(real_connection *c)
                { c->recv_thread_main(); }
            void recv_thread_main(void);
        };
    }
}

#endif
