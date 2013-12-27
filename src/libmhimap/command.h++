
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

#ifndef LIBMHIMAP__COMMAND_HXX
#define LIBMHIMAP__COMMAND_HXX

#include "client.h++"
#include <stdint.h>

namespace mhimap {
    /* This holds an IMAP command and deals with parsing when the
     * reply ends.  It deals with stuff like having suprious values
     * get stuck anywhere. */
    class command {
    private:
        /* We need a pointer to the client in order to call read/write
         * on it (as well as obtain a sequence number). */
        client *c;

        /* The sequence number of this command. */
        uint32_t sequence;

    public:
        /* Creates a new command, given the string that should be sent
         * to the IMAP server (but without the number attached to the
         * front, that's inserted by this). */
        command(client *c, const char *fmt, ...)
            __attribute__(( format(printf, 3, 4) ));

        /* Returns TRUE if the given line ends this command, and FALSE
         * otherwise. */
        bool is_end(const char *line);

        /* Returns TRUE if the given line is an erronoues end to the
         * given command, and FALSE otherwise.  Note that if this
         * _doesn't_ check is_end(), it's expected that the user does
         * that already. */
        bool is_error_end(const char *line);

        /* These are mechanisms for filtering out unwanted lines --
         * I'm not entirely sure what the proper thing to do here is,
         * so for now I'm just going to filter them out.  I think it's
         * probably best to have some kind of more complicated client
         * state machine that has two streams: the command in progress
         * and the suprious stuff. */
        bool is_list(const char *line);

        /* Parses the folder name out of a list command, potentially
         * destroying "line" in the process.  The returned pointer
         * will be inside of "line", so there's nothing additional to
         * free. */
        char *list_get_folder(char *line);
    };
}

#endif
