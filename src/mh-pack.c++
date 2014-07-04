
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

#include <libmh/mhdir.h++>
#include <libmh/options.h++>

int main(int argc, const char **argv)
{
    auto o = mh::options::create(argc, argv);
    mh::mhdir dir(o);

    /* Walks through every folder in this directory. */
    for (auto it = dir.folders(); !it.done(); ++it) {
        auto folder = *it;

        int cur = 1;
        /* Walks through every message in the folder, packing sequence
         * numbers better. */
        for (auto mit = folder.messages(); !mit.done(); ++mit) {
            auto message = *mit;
            int seq = message.seq();

            if (seq != cur)
                message.reseq(cur);

            cur++;
        }
    }

    return 0;
}
