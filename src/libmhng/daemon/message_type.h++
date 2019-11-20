/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__DAEMON__MESSAGE_TYPE_HXX
#define MHNG__DAEMON__MESSAGE_TYPE_HXX

#include <stdint.h>

namespace mhng {
    namespace daemon {
        enum class message_type {
            RESPONSE     = 0,
            SYNC         = 1,
            NET_UP       = 2,
            NET_DOWN     = 3,
            NEW_MESSAGE  = 4,
            FOLDER_EVENT = 5,
        };

        /* These let me break type safety so I can push these messages
         * over a stream. */
        static inline uint32_t message_type2uint(message_type t)
        { return (uint32_t)t; }
        static inline message_type uint2message_type(uint32_t i)
        { return (message_type)i; }
    }
}

#endif
