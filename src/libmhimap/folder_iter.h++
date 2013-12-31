
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

#ifndef LIBMHIMAP__FOLDER_ITER_HXX
#define LIBMHIMAP__FOLDER_ITER_HXX

#include "folder.h++"
#include "vector_iter.h++"

namespace mhimap {
    class folder_iter: public vector_iter<folder> {
    public:
        folder_iter(const std::vector<folder> items)
            : vector_iter<folder>(items)
            {
            }
    };
}

#endif
