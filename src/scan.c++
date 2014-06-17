
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

int main(const int argc, const char **argv)
{
    auto o = mh::options::create(argc, argv);
    mh::mhdir dir(o);

    /* Opens the default folder. */
    mh::folder folder = dir.open_folder(true);

    /* Opens up a mailrc. */
    auto mailrc = mh::global_mailrc();

    /* Loops through every message in the folder, printing out
     * information about it. */
    for (auto mit = folder.messages(); !mit.done(); ++mit) {
        printf("%c %2d %s %-25.25s %-42.42s%c\n",
               (*mit).cur() ? '*' : ' ',
               (*mit).seq(),
               (*mit).date().ddmm().c_str(),
               mailrc->mail2name((*mit).from()).c_str(),
               (*mit).subject().c_str(),
               strlen((*mit).subject().c_str()) > 42 ? '\\' : ' '
            );
    }

    return 0;
}
