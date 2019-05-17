
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
#include <libmhng/util/string.h++>
#include <string.h>
#include <termcap.h>
#include <algorithm>

#ifndef PIPE_SCAN
static char termbuf[2048];
#endif

static std::string remove_other_white(const std::string& in);

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_messages(argc, argv);
    args->mbox()->set_current_folder(args->folders()[0]);

    /* Find some information about the terminal. */
#ifndef PIPE_SCAN
    size_t terminal_width = 80;
    char *terminal_bold = (char *)"";
    char *terminal_norm = (char *)"";
    char *termtype = getenv("TERM");
    if (tgetent(termbuf, termtype) >= 0) {
        terminal_width = tgetnum((char *)"co");
        terminal_bold = tgetstr((char *)"md", NULL);
        terminal_norm = tgetstr((char *)"me", NULL);
    }

    size_t from_width = (terminal_width * 25) / 80;
    if (from_width > 35)
        from_width = 35;
    /* FIXME: log10? */
    size_t seq_width = (terminal_width > 120) ? 3 : 2;
    auto message_count = args->messages().size();
    if (message_count > 0) {
        auto last_message = args->messages()[message_count-1];
        if (last_message->seq()->to_uint() >= 100)
            seq_width = 3;
        if (last_message->seq()->to_uint() >= 1000)
            seq_width = 4;
        if (last_message->seq()->to_uint() >= 10000)
            seq_width = 5;
        if (last_message->seq()->to_uint() >= 100000)
            seq_width = 6;
        if (last_message->seq()->to_uint() >= 1000000)
            seq_width = 7;
        if (last_message->seq()->to_uint() >= 10000000)
            seq_width = 8;
    }

    size_t subject_width = terminal_width - from_width - seq_width - 11;
#endif

    /* At this point that argument list contains the entire set of
     * messages that should be examined as part of the scan. */
    for (const auto& msg: args->messages()) {
        auto subj = remove_other_white(msg->first_subject());

        auto from = msg->first_from();
        if (strcmp(msg->folder()->name().c_str(), "sent") == 0)
            from = msg->first_to();
        if (strcmp(msg->folder()->name().c_str(), "drafts") == 0)
            from = msg->first_to();

#ifdef PIPE_SCAN
        printf("%u %s %s %s\n",
               msg->seq()->to_uint(),
               msg->first_date()->ddmm().c_str(),
               from->email().c_str(),
               subj.c_str()
            );
#else
        printf("%s%c %*u %s %s %s%c%s\n",
               msg->unread() ? terminal_bold : "",
               msg->cur() ? '*' : ' ',
               (int)seq_width, msg->seq()->to_uint(),
               msg->first_date()->ddmm().c_str(),
               mhng::util::string::utf8_pad_to_length(from->nom(), from_width).c_str(),
               mhng::util::string::utf8_pad_to_length(subj, subject_width).c_str(),
               strlen(subj.c_str()) > subject_width ? '\\' : ' ',
               terminal_norm
            );
#endif
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

