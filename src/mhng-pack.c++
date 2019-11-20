/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

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
