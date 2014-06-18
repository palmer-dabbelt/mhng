
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

namespace mh {
    class temp_file;
    class message;
}

#include <stdio.h>
#include <string>

namespace mh {
    /* Holds a temporary file that points somewhere within the MH
     * directory . */
    class temp_file {
    public:
        friend class message;

    private:
        FILE *_fp;
        const std::string _path;

    public:
        /* Creates a new temporary file inside the given folder. */
        static temp_file open(const std::string folder);

        ~temp_file(void);

        /* Returns the FILE* cooresponding to this temprary file.
         * Note you have to be a bit careful here because it's a raw
         * pointer that can be invalidated by the sync() method. */
        FILE *fp(void) const { return _fp; }

        /* fclose()s and fsync()s this temporary file, setting _fp to
         * NULL.  At this point essentially the one thing that remains
         * is the filename. */
        void finish(void);

        /* You really shouldn't be accessing the filename of this
         * exactly, but should instead be passing this to a message in
         * order to insert it into the database. */
        const std::string path(void) const { return _path; }

    private:
        /* Don't use this, it's just to work around const. */
        temp_file(FILE *fp, const std::string path);
    };
}

#endif
