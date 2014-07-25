
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

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_folders(argc, argv);

    /* Walks through every folder in this directory. */
    for (const auto& folder: args->folders()) {
        /* Here we get an exclusive transaction because this is just
         * such a dangerous operation that we don't want anyone else
         * touching anything at all! */
        auto tr = args->mbox()->db()->exclusive_transaction();

        unsigned seq = 1;
        for (const auto& message: folder->messages()) {
            auto sn = std::make_shared<mhng::sequence_number>(seq);
            message->set_sequence_number(sn);
            seq++;
        }
    }

    return 0;
}
