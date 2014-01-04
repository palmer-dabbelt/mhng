
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

#ifndef LIBMH__DATE_ITER_HXX
#define LIBMH__DATE_ITER_HXX

#include "date.h++"
#include "vector_iter.h++"
#include <vector>

namespace mh {
    /* Iterates through an arbitrary list of dates. */
    class date_iter : public vector_iter<date> {
    public:
        date_iter(const std::vector<date> items)
            : vector_iter<date>(items)
            {
            }
    };
}

#endif
