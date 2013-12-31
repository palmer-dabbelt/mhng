
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

#ifndef LIBMH__TEMP_FILE_HXX
#define LIBMH__TEMP_FILE_HXX

#include <stdio.h>
#include <string>

namespace mh {
    /* Holds a temporary file that points somewhere within the MH
     * directory . */
    class temp_file {
    private:
        FILE *_fp;
        const std::string _path;

    public:
        /* Creates a new temporary file inside the given folder. */
        static temp_file open(const std::string folder);

        ~temp_file(void);

        /* Returns the FILE* cooresponding to this temprary file. */
        FILE *fp(void) const { return _fp; }

    private:
        /* Don't use this, it's just to work around const. */
        temp_file(FILE *fp, const std::string path);
    };
}

#endif
