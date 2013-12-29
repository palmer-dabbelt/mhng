
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

#ifndef LIBMH__MHDIR_HXX
#define LIBMH__MHDIR_HXX

namespace mh {
    class mhdir;
}

#include "folder.h++"
#include "options.h++"
#include "db/connection.h++"
#include <string>

namespace mh {
    /* Represents the state of this system's .mhdir, which contains
     * the entire configuration state as well as all the
     * locally-cached files and such.  */
    class mhdir {
    private:
        const options_ptr _o;
        db::connection_ptr _db;

    public:
        /* Opens up the MHng directory specified by the given set of
         * options. */
        mhdir(const options_ptr o);

        /* Opens a folder by name.  There's also a function to check
         * if the given folder exists, which you should call to make
         * sure the requested folder exists. */
        bool folder_exists(const std::string folder_name);
        folder open_folder(const std::string folder_name);
    };
}

#endif
