/* Copyright (C) 2014-2021 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <string.h>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_normal(argc, argv);
    auto folders = args->folders();
    auto messages = args->messages();

    for (const auto& msg: messages)
    	printf("%s\n", msg->imap_account_name().c_str());

    return 0;
}
