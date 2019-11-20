/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <libmhoauth/pkce.h++>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_account(argc, argv);

    if (args->mbox()->accounts().size() > 0) {
        std::cerr << "MHng currently only supports a single account\n";
        return 1;
    }

    args->mbox()->add_account(args->account());

    return 0;
}
