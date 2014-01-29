
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

#include "line_buffer.h++"
#include "logger.h++"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace mhimap;

line_buffer::line_buffer(ssize_t starting_size)
    : data(new char[starting_size]),
      used(0),
      count(starting_size)
{
}

line_buffer::~line_buffer(void)
{
    delete data;
}

bool line_buffer::has_line(void) const
{
    assert(used >= 0);
    assert(used < count);

    for (ssize_t i = 0; i < used; i++)
        if (data[i] == '\n')
            return true;

    return false;
}

ssize_t line_buffer::recharge_size(void)
{
    assert(used >= 0);
    assert(used < count);

    logger l("line_buffer::rechange_size()");

    ssize_t out = (count - used) - 1;
    l.printf("==> %ld", out);

    if (out == 0) {
        auto new_data = new char[count * 2];
        memcpy(new_data, data, count);
        auto old_data = data;
        data = new_data;
        count *= 2;
        delete old_data;

        return recharge_size();
    }

    return out;
}

char *line_buffer::recharge_buffer(void) const
{
    assert(used >= 0);
    assert(used < count);

    logger l("line_buffer::rechange_buffer()");

    return &data[used];
}

void line_buffer::recharge_commit(ssize_t n_read)
{
    assert(used >= 0);
    assert(used < count);

    used += n_read;

    if (used > count) {
        fprintf(stderr, "Detected possible corruption: used > count");
        abort();
    }
}

ssize_t line_buffer::get(char *buffer, ssize_t buffer_size)
{
    logger l("line_buffer::get(..., %ld)", buffer_size);

    for (ssize_t i = 0; i < used; i++) {
        if (data[i] == '\n') {
            /* Check if we should back up to eat the additional '\r'
             * before this. */
            if (i > 0 && data[i-1] == '\r')
                i--;

            /* Figure out how much data to stick into the buffer,
             * ensuring that it doesn't overflow anything.  Note that
             * this means we will DROP data when lines are too long --
             * the user can detect this by checking the size. */
            ssize_t to_copy = (i > buffer_size) ? buffer_size : i;
            memcpy(buffer, data, to_copy);
            buffer[to_copy] = '\0';

            /* Skip _exactly_ one newline, attempting to auto-detect
             * what the server sent us.  The general idea is that we
             * skip at most 2 characters, and that they must be
             * diferent characters. */
            if (i < used && data[i] == '\r') {
                i++;
                if (i < used && data[i] == '\n')
                    i++;
            } else if (i < used && data[i] == '\n') {
                i++;
                if (i < used && data[i] == '\r')
                    i++;
            }

            /* Remove the returned characters from the buffer. */
            assert(used >= 0);
            assert(used < count);
            assert(used >= i);
            used -= i;
            assert(used >= 0);
            assert(used < count);

            /* FIXME: This is a poorly implemented buffer because this
             * copy shouldn't be necessary. */
            memmove(data, data + i, used);
            return i;
        }
    }

    return -1;
}
