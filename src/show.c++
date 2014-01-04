
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

    /* FIXME: The current folder name should be read from some sort of
     * internal table, but I haven't actually set that up yet. */
    mh::folder folder = dir.open_folder("inbox");

    /* FIXME: The current message name should be read from some sort
     * of internal table, but I haven't actually set that up yet. */
    mh::message message = folder.open_seq(1);

    /* Reads the full message contents from disk. */
    mh::message_file mf = message.read();

    /* Passes everything through a pager. */
    FILE *less = popen("less", "w");

    /* Format the headers. */
    for (auto it = mf.headers("From"); !it.done(); ++it) {
        fprintf(less, "From:    %s\n", (*it).c_str());
    }
    for (auto it = mf.headers("To"); !it.done(); ++it) {
        fprintf(less, "To:      %s\n", (*it).c_str());
    }
    for (auto it = mf.headers("CC"); !it.done(); ++it) {
        fprintf(less, "CC:      %s\n", (*it).c_str());
    }
    for (auto it = mf.headers("BCC"); !it.done(); ++it) {
        fprintf(less, "BCC:     %s\n", (*it).c_str());
    }
    for (auto it = mf.headers("Subject"); !it.done(); ++it) {
        fprintf(less, "Subject: %s\n", (*it).c_str());
    }
    for (auto it = mf.headers("Date"); !it.done(); ++it) {
        fprintf(less, "Date:    %s\n", (*it).c_str());
    }

    pclose(less);

    return 0;
}

