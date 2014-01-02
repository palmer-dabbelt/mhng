
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

#ifndef LIBMH__STRING_ITER_HXX
#define LIBMH__STRING_ITER_HXX

#include "vector_iter.h++"
#include <string>
#include <vector>

namespace mh {
    /* Iterates through an arbitrary list of strings.  This isn't
     * really type-safe, but I think it's going to be as good as it
     * gets thanks to IMAP.  Note that this is a const_iterator
     * because we can'tt change anything about what's on the IMAP
     * server this way. */
    class string_iter : public vector_iter<std::string> {
    public:
        string_iter(const std::vector<std::string> items)
            : vector_iter<std::string>(items)
            {
            }
    };
}

#endif
