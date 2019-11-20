/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHNG__DAEMON__RESPONSE_HXX
#define LIBMHNG__DAEMON__RESPONSE_HXX

#include <memory>

namespace mhng {
    namespace daemon {
        class response;
        typedef std::shared_ptr<response> response_ptr;
        typedef std::weak_ptr<response> response_wptr;
    }
}

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <stdint.h>
#include <string>

namespace mhng {
    namespace daemon {
        /* This holds the promise of a response from the server, which
         * can be blocked on if you actually want that message to come
         * back.  Note that this can also come back asynchronusly, so
         * it can be fast... */
        class response {
        private:
            std::atomic<uint32_t> _id;
            std::atomic<uint64_t> _ticket;

            bool _finished;
            std::mutex _lock;
            std::condition_variable _signal;

        public:
            response(uint32_t id);

        public:
            /* Accessor functions. */
            uint32_t id(void) const { return _id; }
            uint64_t ticket(void) const { return _ticket; }

        public:
            /* Waits for the server to actually return this response
             * code. */
            void wait(void);

            /* Fills out this return code. */
            void fill(uint64_t ticket);
        };
    }
}

#endif
