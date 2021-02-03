/* Copyright (C) 2019-2020 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <libmhoauth/pkce.h++>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_account(argc, argv);
    args->mbox()->redo_account_oauth(args->account());
    return 0;
}
