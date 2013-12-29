
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

#ifndef LIBMH__FOLDER_HXX
#define LIBMH__FOLDER_HXX

namespace mh {
    class folder;
}

#include "mhdir.h++"
#include "options.h++"
#include "db/connection.h++"
#include <string>

namespace mh {
    /* Represents a single MH folder. */
    class folder {
    public:
        friend class mhdir;

    private:

    protected:
        /* Creates a new folder, given the name of that folder and a
         * database connection to use in order to query that folder's
         * contents. */

    public:
        
    };
}

#endif
