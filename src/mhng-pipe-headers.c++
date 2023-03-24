/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <string.h>

int main(int argc, const char **argv)
{
#if defined(HEADERS) || defined(FROM)
    auto args = mhng::args::parse_normal(argc, argv);
#elif defined(HEADER)
    if (argc < 2) {
        fprintf(stderr, "mhng-header <header name> [messages]: Prints a header\n");
        return 1;
    }

    const char **filtered_argv = argv + 1;
    auto args = mhng::args::parse_normal(argc - 1, filtered_argv);
#else
#error "Define HEADERS or HEADER"
#endif

    auto folders = args->folders();
    auto messages = args->messages();

    for (const auto& msg: messages) {
#if defined(FROM)
        for (const auto& addr: msg->from())
            printf("%s\n", addr->email().c_str());
#else /* defined(FROM) */
        for (const auto& header: msg->mime()->root()->headers()) {
# if defined(HEADERS)
            printf("%s: %s\n",
                   header->key_downcase().c_str(),
                   header->utf8().c_str()
                );
# elif defined(HEADER)
            if (strcasecmp(argv[1], header->key().c_str()) != 0)
                continue;

            printf("%s\n",
                   header->utf8().c_str()
                );
# else /* defined(HEADERS) || defined(HEADER) */
# error "Define HEADERS or HEADER"
# endif
        }
#endif /* defined(FROM) */
    }

    return 0;
}
