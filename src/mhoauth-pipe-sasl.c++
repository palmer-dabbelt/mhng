/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <libmhoauth/pkce.h++>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_account(argc, argv);
    std::cout << args->mbox()->account(args->account())->sasl() << "\n";
    return 0;
}
