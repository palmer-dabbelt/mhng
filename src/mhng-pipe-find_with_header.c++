/* Copyright (C) 2016 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <string.h>

int main(int argc, const char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "mhng-pipe-find_with_header <header name>: Prints a header\n");
        return 1;
    }

    const char *empty_argv[] = {argv[0], NULL};
    auto args = mhng::args::parse_all_messages(1, empty_argv);

    auto messages = args->messages();

    for (const auto& msg: messages) {
        for (const auto& header: msg->mime()->root()->headers()) {
            if (strcasecmp(argv[1], header->key().c_str()) != 0)
                continue;
            if (strcasecmp(argv[2], header->utf8().c_str()) != 0)
                continue;

            printf("%u\n",
                   msg->seq()->to_uint()
                );
        }
    }

    return 0;
}
