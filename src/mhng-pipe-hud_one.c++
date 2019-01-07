
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
    auto args = mhng::args::parse_all_folders(argc, argv);

    /* Find some information about the terminal. */
    size_t terminal_width = 80;
    char *terminal_bold = (char *)"";
    char *terminal_norm = (char *)"";
    char *termtype = getenv("TERM");
    if (tgetent(termbuf, termtype) >= 0) {
        terminal_width = tgetnum((char *)"co");
        terminal_bold = tgetstr((char *)"md", NULL);
        terminal_norm = tgetstr((char *)"me", NULL);
    }

    /* Create a list of all the messages. */
    size_t message_count = 0;
    for (const auto& folder: args->folders())
        message_count += folder->messages().size();

    size_t from_width = (terminal_width * 25) / 80;
    if (from_width > 35)
        from_width = 35;
    size_t seq_width = (terminal_width > 120) ? 3 : 2;
    if (message_count >= 100)
        seq_width = 3;
    if (message_count >= 1000)
        seq_width = 4;
    if (message_count >= 10000)
        seq_width = 5;
    if (message_count >= 100000)
        seq_width = 6;
    if (message_count >= 1000000)
        seq_width = 7;

    size_t subject_width = terminal_width - from_width - seq_width - 13;

    for (const auto& folder: args->folders()) {
        if (folder->messages().size() != 0)
            printf("%s\n", folder->name().c_str());

        for (const auto& msg: folder->messages()) {
            auto subj = msg->first_subject();

            auto from = msg->first_from();
            if (strcmp(msg->folder()->name().c_str(), "sent") == 0)
                from = msg->first_to();

            printf("  %s%c %*u %s %-*.*s %-*.*s%c%s\n",
                   msg->unread() ? terminal_bold : "",
                   msg->cur() ? '*' : ' ',
                   (int)seq_width, msg->seq()->to_uint(),
                   msg->first_date()->ddmm().c_str(),
                   (int)from_width, (int)from_width, from->nom().c_str(),
                   (int)subject_width, (int)subject_width, subj.c_str(),
                   strlen(subj.c_str()) > subject_width ? '\\' : ' ',
                   terminal_norm
                );
        }

        if (folder->messages().size() != 0)
            printf("\n");
    }

    return 0;
}
