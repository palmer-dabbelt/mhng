/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

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
