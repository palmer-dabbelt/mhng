
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

#ifndef LIBMHIMAP__LINE_BUFFER_HXX
#define LIBMHIMAP__LINE_BUFFER_HXX

#include <unistd.h>

namespace mhimap {
    /* This class deals with converting between buffered and
     * non-buffered IO. */
    class line_buffer {
    private:
        char *data;
        ssize_t used;
        ssize_t count;

    public:
        /* Creates a new, empty line buffer.  The starting size
         * argument is optional: it's the default buffer size. */
        line_buffer(ssize_t starting_size = 1024);

        ~line_buffer(void);

        /* Returns TRUE whenever this buffer has a line left in it,
         * and FALSE otherwise. */
        bool has_line(void) const;

        /* Returns a buffer that can be used to recharge this buffer
         * with data.  The size of this buffer will never be 0, but it
         * could be exceedingly small (ie, 1).  */
        ssize_t recharge_size(void) const;
        char *recharge_buffer(void) const;

        /* Commits a recharge request based upon the actual size that
         * was read from some sort of input stream. */
        void recharge_commit(ssize_t n_read);

        /* Pulls a line out of this buffer, writing it into a
         * different buffer. */
        ssize_t get(char *buffer, ssize_t buffer_size);
    };
}

#endif
