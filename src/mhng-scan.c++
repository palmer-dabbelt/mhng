
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

#include <libmhng/args.h++>
#include <string.h>
#include <termcap.h>

static char termbuf[2048];

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_messages(argc, argv);

    /* Find some information about the terminal. */
    size_t terminal_width = 80;
    char *termtype = getenv("TERM");
    if (tgetent(termbuf, termtype) >= 0) {
        terminal_width = tgetnum("co");
    }

    size_t from_width = (terminal_width * 25) / 80;
    size_t seq_width = (terminal_width > 120) ? 3 : 2;
    size_t subject_width = terminal_width - from_width - seq_width - 11;

    /* At this point that argument list contains the entire set of
     * messages that should be examined as part of the scan. */
    for (const auto& msg: args->messages()) {
        printf("%c %*u %s %-*.*s %-*.*s%c\n",
               msg->cur() ? '*' : ' ',
               (int)seq_width, msg->seq()->to_uint(),
               msg->date()->ddmm().c_str(),
               (int)from_width, (int)from_width, msg->from().c_str(),
               (int)subject_width, (int)subject_width, msg->subject().c_str(),
               strlen(msg->subject().c_str()) > subject_width ? '\\' : ' '
            );
    }

    return 0;
}
