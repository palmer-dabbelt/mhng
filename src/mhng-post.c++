
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
    /* Overwrite the default argument list by */
    const char **new_argv = new const char*[argc+1];
    new_argv[0] = argv[0];
    new_argv[1] = "+drafts";
    for (int i = 1; i < argc; ++i)
        new_argv[i+1] = argv[i];

    auto args = mhng::args::parse_all_messages(argc+1, new_argv);

    /* Find some information about the terminal. */
    size_t terminal_width = 80;
    char *termtype = getenv("TERM");
    if (tgetent(termbuf, termtype) >= 0) {
        terminal_width = tgetnum("co");
    }

    size_t from_width = (terminal_width * 25) / 80;
    size_t seq_width = (terminal_width > 120) ? 3 : 2;
    size_t subject_width = terminal_width - from_width - seq_width - 9;

    /* At this point that argument list contains the entire set of
     * messages that should be examined as part of the scan. */
    for (const auto& msg: args->messages()) {
        auto subj = msg->first_subject();
        auto to = msg->first_to();
        printf("%*u %s %-*.*s %-*.*s%c\n",
               (int)seq_width, msg->seq()->to_uint(),
               msg->first_date()->ddmm().c_str(),
               (int)from_width, (int)from_width, to->nom().c_str(),
               (int)subject_width, (int)subject_width, subj.c_str(),
               strlen(subj.c_str()) > subject_width ? '\\' : ' '
            );

        FILE *smtp = popen("msmtp -t", "w");

        for (const auto& line: msg->raw())
            fprintf(smtp, "%s", line.c_str());

        int err = pclose(smtp);
        if (err != 0) {
            perror("Unable to SMTP message");
            fprintf(stderr, " Sequence Number: %u\n",
                    msg->seq()->to_uint());
            continue;
        }

        msg->remove();
    }

    return 0;
}
