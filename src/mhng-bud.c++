
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
#include <algorithm>
#include <string.h>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_folders(argc, argv);
    bool did_out = false;

    std::vector<std::string> important_folders = {"inbox", "drafts", "lists"};

    for (const auto& folder: args->folders()) {
        auto f = find(important_folders.begin(),
                      important_folders.end(),
                      folder->name());
        if (f == important_folders.end())
            continue;

        size_t unread_count = 0;
        for (const auto& msg: folder->messages())
            if (msg->unread() == true)
                unread_count++;

        if (unread_count == 0) {
        }
        else if (unread_count == 1) {
            printf("%c",
                   folder->name().c_str()[0]
                );
        } else {
            printf("%lu%c",
                   unread_count,
                   folder->name().c_str()[0]
                );
        }

        if (unread_count > 0)
            did_out = true;
    }

    if (did_out)
        printf(" ");

    return 0;
}
