/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <algorithm>
#include <string.h>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_folders(argc, argv);
    bool did_out = false;

    std::vector<std::string> important_folders = {"inbox", "drafts"};

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
            printf("%llu%c",
                   (long long unsigned)unread_count,
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
