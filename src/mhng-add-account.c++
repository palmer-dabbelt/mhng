/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <libmhoauth/pkce.h++>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_account(argc, argv);
#if defined(OAUTH2)
    args->mbox()->add_oauth2_account(args->account());
#elif defined(USERPASS)
    args->mbox()->add_userpass_account(args->account());
#else
# error "Unknown auth mechanism"
#endif
    return 0;
}
