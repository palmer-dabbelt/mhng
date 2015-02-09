
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
#include <algorithm>

static char termbuf[2048];

static std::string remove_other_white(const std::string& in);

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_messages(argc, argv);
    args->mbox()->set_current_folder(args->folders()[0]);

    /* Find some information about the terminal. */
    size_t terminal_width = 80;
    char *termtype = getenv("TERM");
    if (tgetent(termbuf, termtype) >= 0) {
        terminal_width = tgetnum((char *)"co");
    }

    size_t from_width = (terminal_width * 25) / 80;
    if (from_width > 35)
        from_width = 35;
    size_t seq_width = (terminal_width > 120) ? 3 : 2;
    if (args->messages().size() >= 100)
        seq_width = 3;
    if (args->messages().size() >= 1000)
        seq_width = 4;
    if (args->messages().size() >= 10000)
        seq_width = 5;
    if (args->messages().size() >= 100000)
        seq_width = 6;
    if (args->messages().size() >= 1000000)
        seq_width = 7;

    size_t subject_width = terminal_width - from_width - seq_width - 11;

    /* At this point that argument list contains the entire set of
     * messages that should be examined as part of the scan. */
    for (const auto& msg: args->messages()) {
        auto subj = remove_other_white(msg->first_subject());

        auto from = msg->first_from();
        if (strcmp(msg->folder()->name().c_str(), "sent") == 0)
            from = msg->first_to();

        printf("%c %*u %s %-*.*s %-*.*s%c\n",
               msg->cur() ? '*' : ' ',
               (int)seq_width, msg->seq()->to_uint(),
               msg->first_date()->ddmm().c_str(),
               (int)from_width, (int)from_width, from->nom().c_str(),
               (int)subject_width, (int)subject_width, subj.c_str(),
               strlen(subj.c_str()) > subject_width ? '\\' : ' '
            );
    }

    return 0;
}

std::string remove_other_white(const std::string& in)
{
    std::string out(in.length(), ' ');

    std::transform(in.begin(), in.end(), out.begin(),
                   [](char inc) -> char
                   {
                       if (isspace(inc))
                           return ' ';
                       return inc;
                   });

    return out;
}

