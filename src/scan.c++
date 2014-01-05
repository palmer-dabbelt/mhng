
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

int main(const int argc, const char **argv)
{
    auto o = mh::options::create(argc, argv);
    mh::mhdir dir(o);

    /* FIXME: The current folder name should be read from some sort of
     * internal table, but I haven't actually set that up yet. */
    mh::folder folder = dir.open_folder("inbox");

    /* Loops through every message in the folder, printing out
     * information about it. */
    for (auto mit = folder.messages(); !mit.done(); ++mit) {
        printf("%3d %s %-25.25s %-40.40s\n",
               (*mit).seq(),
               (*mit).date().ddmm().c_str(),
               (*mit).from().c_str(),
               (*mit).subject().c_str()
            );
    }

    return 0;
}
