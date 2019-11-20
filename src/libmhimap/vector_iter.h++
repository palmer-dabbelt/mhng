/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

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
