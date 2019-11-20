/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <string.h>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_messages(argc, argv);

    for (const auto& msg: args->messages())
        printf("%u\n", msg->seq()->to_uint());

    return 0;
}
