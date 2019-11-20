/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_folders(argc, argv);

    /* Prints a little header so we can see what's going on. */
    printf("%20s %10s %10s\n",
           "FOLDER NAME",
           "MESSAGES",
           "CURRENT"
        );

    /* Walks through every folder in this directory. */
    for (const auto& folder: args->folders()) {
        auto cur = folder->current_message();
        std::string seq = "(none)";
        if (cur != NULL)
            seq = std::to_string(cur->seq()->to_int());

        printf("%20s %10llu %10s\n",
               folder->name().c_str(),
               (long long unsigned)folder->message_count(),
               seq.c_str()
            );
    }

    return 0;
}
