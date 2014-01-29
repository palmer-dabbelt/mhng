
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
#include <libmh/message_file.h++>
#include <libmh/mhdir.h++>
#include <libmh/options.h++>

int main(int argc, const char **argv)
{
    auto o = mh::options::create(argc, argv);
    mh::mhdir dir(o);

    /* Opens the default folder. */
    mh::folder folder = dir.open_folder();

    /* Reads the current message from the given folder. */
    mh::message message = folder.open_current();

    /* Reads the full message contents from disk. */
    mh::message_file mf = message.read();

    /* Passes everything through a pager. */
    FILE *less = popen("less", "w");

    /* Format the headers. */
    for (auto it = mf.headers_address("From"); !it.done(); ++it) {
        fprintf(less, "From:    %s\n", (*it).c_str());
    }
    for (auto it = mf.headers_address("To"); !it.done(); ++it) {
        fprintf(less, "To:      %s\n", (*it).c_str());
    }
    for (auto it = mf.headers_address("CC"); !it.done(); ++it) {
        fprintf(less, "CC:      %s\n", (*it).c_str());
    }
    for (auto it = mf.headers_address("BCC"); !it.done(); ++it) {
        fprintf(less, "BCC:     %s\n", (*it).c_str());
    }
    for (auto it = mf.headers("Subject"); !it.done(); ++it) {
        fprintf(less, "Subject: %s\n", (*it).c_str());
    }
    for (auto it = mf.headers_date("Date"); !it.done(); ++it) {
        fprintf(less, "Date:    %s\n", (*it).local().c_str());
    }

    /* Mail messages have a blank line between the headers and the
     * body.  Here we attempt to more-or-less keep things that way
     * inside the output of show.  In other words, "show" should print
     * something parsable as a regular mail message. */
    fprintf(less, "\n");

    /* Print the body text of the message. */
    for (auto it = mf.body(); !it.done(); ++it) {
        fprintf(less, "%s\n", (*it).c_str());
    }

    pclose(less);

    return 0;
}

