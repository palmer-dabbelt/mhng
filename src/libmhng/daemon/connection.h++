/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

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
