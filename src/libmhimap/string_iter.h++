
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

#ifndef LIBMHIMAP__STRING_ITER_HXX
#define LIBMHIMAP__STRING_ITER_HXX

#include <string>
#include <vector>

namespace mhimap {
    /* Iterates through an arbitrary list of strings.  This isn't
     * really type-safe, but I think it's going to be as good as it
     * gets thanks to IMAP.  Note that this is a const_iterator
     * because we can'tt change anything about what's on the IMAP
     * server this way. */
    class string_iter {
    private:
        /* Quite simply a list of the items that we're going to
         * iterate through. */
        const std::vector<std::string> _items;

        /* This is the iterator that we'll be mirroring. */
        std::vector<std::string>::const_iterator _it;
        
    public:
        /* Creates a new string iterator, given the list of strings
         * that it will be iterating over.  Note that this is
         * essentially the simplest version, but it requires a whole
         * bunch of copying so it may not be ideal -- it's probably OK
         * for now, though, as all we're really doing is iterating
         * through folder names. */
        string_iter(const std::vector<std::string> items);

        /* These just make this look like an iterator. */
        const std::string operator*(void) const { return *_it; }
        void operator++(void) { ++_it; }
        bool done(void) const { return _it == _items.end(); }
    };
}

#endif

