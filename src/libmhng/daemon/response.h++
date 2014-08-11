
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
            uint32_t _id;

        public:
            response(uint32_t id);

        public:
            /* Accessor functions. */
            uint32_t id(void) const { return _id; }
        };
    }
}

#endif
