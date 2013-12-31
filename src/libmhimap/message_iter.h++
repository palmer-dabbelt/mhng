
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

#ifndef LIBMHIMAP__MESSAGE_ITER_HXX
#define LIBMHIMAP__MESSAGE_ITER_HXX

namespace mhimap {
    class message_iter;
}

#include "message.h++"
#include "vector_iter.h++"

namespace mhimap {
    class message_iter: public vector_iter<message> {
    public:
        message_iter(const std::vector<message> items)
            : vector_iter<message>(items)
            {
            }
    };
}

#endif

