
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

#ifndef LIBMH__FOLDER_ITER_HXX
#define LIBMH__FOLDER_ITER_HXX

namespace mh {
    class folder_iter;
}

#include "folder.h++"

namespace mh {
    /* This deals with iterating through the list of folders.  You're
     * almost certainly not going to want to create one of these
     * yourself, but instead are going to want get one from a
     * mhdir. */
    class folder_iter : public vector_iter<folder> {
    public:
        folder_iter(const std::vector<folder> items)
            : vector_iter<folder>(items)
            {
            }
    };
}

#endif
