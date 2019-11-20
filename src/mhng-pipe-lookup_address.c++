/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>

int main(int argc, const char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "mhng-pipe-lookup_address <address>\n");
        return 1;
    }

    const char *empty_argv[] = {argv[0], NULL};
    auto args = mhng::args::parse_all_messages(1, empty_argv);

    std::vector<std::string> addresses = {argv[1]};

    auto mail_rc = args->mbox()->mrc();

    for (const auto& addr_str: addresses) {
        auto addr = mail_rc->emailias(addr_str);
        if (addr->email_known() == false)
            continue;

        printf("%s\n", addr->rfc().c_str());
    }

    return 0;
}
