
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

#ifndef MHNG__SEQUENCE_NUMBER_HXX
#define MHNG__SEQUENCE_NUMBER_HXX

#include <memory>

namespace mhng {
    class sequence_number;
    typedef std::shared_ptr<sequence_number> sequence_number_ptr;
}

#include "folder.h++"
#include "promise.h++"
#include <psqlite/connection.h++>
#include <string>

namespace mhng {
    /* A thin wrapper around integers that allows them to represent
     * sequence numbers. */
    class sequence_number {
    private:
        unsigned _num;

    public:
        /* Allows a sequence number to be created from a plain-old
         * integer.  Note that you should probably only be using this
         * if you're formatting strings, otherwise it's a bit
         * unsafe. */
        sequence_number(unsigned num);

    public:
        /* Converts to an integer. */
        unsigned to_uint(void) const { return _num; }
        int to_int(void) const { return (int)_num; }
    };
}

#endif
