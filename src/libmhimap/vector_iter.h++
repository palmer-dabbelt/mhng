
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

#ifndef LIBMHIMAP__VECTOR_ITER_HXX
#define LIBMHIMAP__VECTOR_ITER_HXX

#include <vector>

namespace mhimap {
    /* This contains a generic vector iterator that takes in a vector
     * as part of the constructor and then just allows for iteration
     * over that vector by users.  The general idea is that this is
     * easy to create but inefficient.  I want to avoid just returning
     * a vector because I don't want to build that inefficiency into
     * the library, but I also don't want to write fancy custom
     * iterators right now. */
    template<class T>
    class vector_iter {
    private:
        /* Quite simply a list of the items that we're going to
         * iterate through. */
        const std::vector<T> _items;

        /* This is the iterator that we'll be mirroring. */
        typename std::vector<T>::const_iterator _it;

    public:
        /* Creates a new vector iterator given the list of items to
         * iterate over. */
        vector_iter(const std::vector<T> items)
            : _items(items),
              _it(_items.begin())
            {
            }

        /* These just work like any other iterator. */
        const T operator*(void) const { return *_it; }
        void operator++(void) { ++_it; }
        bool done(void) const { return _it == _items.end(); }
    };
}

#endif
