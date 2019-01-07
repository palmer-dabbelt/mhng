
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

#ifndef MHNG__DAEMON__MESSAGE_TYPE_HXX
#define MHNG__DAEMON__MESSAGE_TYPE_HXX

#include <stdint.h>

namespace mhng {
    namespace daemon {
        enum class message_type {
            SYNC,
            NET_UP,
            NET_DOWN,
            NEW_MESSAGE,
            RESPONSE,
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
