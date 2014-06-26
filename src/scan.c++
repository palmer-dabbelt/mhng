
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

#include <libmh/message.h++>
#include <libmh/mhdir.h++>
#include <libmh/options.h++>
#include <libmh/global_mailrc.h++>
#include <string.h>
#include <termcap.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

static char termbuf[2048];

int main(const int argc, const char **argv)
{
    auto o = mh::options::create(argc, argv);
    mh::mhdir dir(o);

#if defined(SCAN)
    /* Opens the default folder. */
    mh::folder folder = dir.open_folder(true);
#elif defined(INBOX)
    /* Opens the inbox. */
    mh::folder folder = dir.open_folder("inbox", true);
#elif defined(POST)
    /* Opens the drafts folder, but doesn't commit it. */
    mh::folder folder = dir.open_folder("drafts", false);
#endif

    /* Opens up a mailrc. */
    auto mailrc = mh::global_mailrc();

    /* Figure out the terminal size. */
    int terminal_width = 80;
    char *termtype = getenv("TERM");
    if (tgetent(termbuf, termtype) >= 0) {
        terminal_width = tgetnum("co");
    }
    int from_width = (terminal_width * 25) / 80;
    int seq_width = (terminal_width > 120) ? 3 : 2;
    int subject_width = terminal_width - from_width - seq_width - 11;

    /* Loops through every message in the folder, printing out
     * information about it. */
    for (auto mit = folder.messages(); !mit.done(); ++mit) {
        printf("%c %*d %s %-*.*s %-*.*s%c\n",
               (*mit).cur() ? '*' : ' ',
               seq_width, (*mit).seq(),
               (*mit).date().ddmm().c_str(),
#if defined(SCAN) || defined(INBOX)
               from_width, from_width, mailrc->mail2name((*mit).from()).c_str(),
#elif defined(POST)
               from_width, from_width, mailrc->mail2name((*mit).to()).c_str(),
#endif
               subject_width, subject_width, (*mit).subject().c_str(),
               strlen((*mit).subject().c_str()) > (size_t)(subject_width) ? '\\' : ' '
            );

#if defined(POST)
        FILE *msmtp = popen("msmtp -t", "w");
        FILE *msg = fopen((*mit).on_disk_path().c_str(), "r");

        char line[BUFFER_SIZE];
        while (fgets(line, BUFFER_SIZE, msg) != NULL)
            fprintf(msmtp, "%s", line);
        
        int err = pclose(msmtp);
        if (err != 0) {
            perror("Unable to SMTP mail");
            abort();
        }

        /* Remove the message that we just sent, it'll end up in the
         * sent mail box  */
        dir.remove(*mit);
        dir.get_imap_store().mark_for_removal(*mit);
#endif
    }

    return 0;
}
