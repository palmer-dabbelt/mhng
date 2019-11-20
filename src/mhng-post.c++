/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <string.h>
#include <termcap.h>

static char termbuf[2048];

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_messages(argc, argv, {"+drafts"});

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

        auto command = getenv("MHNG_POST_MAILER") == nullptr
            ? std::string("msmtp -t --from=") + msg->first_from()->email()
            : std::string(getenv("MHNG_POST_MAILER"));
        FILE *smtp = popen(command.c_str(), "w");

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
