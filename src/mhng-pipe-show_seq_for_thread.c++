/* Copyright (C) 2016 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <libmhng/message.h++>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_normal(argc, argv);

    for (const auto& msg: args->messages()) {
        printf("%u\n", msg->seq()->to_uint());
        for (const auto& mit: msg->get_messages_in_thread())
            printf("%u\n", mit->seq()->to_uint());
    }

    return 0;
}
