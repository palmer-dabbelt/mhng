
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

#ifndef LIBMH__UID_HXX
#define LIBMH__UID_HXX

namespace mh {
    class uid;
}

#include <stdint.h>
#include <string>

namespace mh {
    /* Stores a unique ID for a message that's unique among all
     * mailboxes in the system.  This is how information is looked up
     * in the database. */
    class uid {
    private:
        /* At least for now I'm going to represent IDs as integers.  I
         * figure 64 bits is good enough for a single machine...  Note
         * that these are actually going to end up being allocated by
         * the SQL database as some primary key, so I think this is
         * somewhat reasonable... */
        const uint64_t _id;

    public:
        /* Creates a new UID by parsing a string, such as one that may
         * have come from a database. */
        uid(const std::string id)
            : _id(atol(id.c_str()))
            {
            }

        /* Returns this UID as a string. */
        const std::string string(void) const
            {
                char buffer[64];
                snprintf(buffer, 64, "%lu", _id);
                return buffer;
            }
    };
}

#endif
