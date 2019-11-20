/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

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
        ssize_t recharge_size(void);
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
