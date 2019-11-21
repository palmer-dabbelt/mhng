/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <libmhoauth/pkce.h++>
#include <iomanip>
#include <thread>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_account(argc, argv);
    auto account = args->mbox()->account(args->account());

    while (true) {
        std::cerr << "Refreshing oauth2 token for " << account->name() << "\n";
        auto expired_at = std::chrono::system_clock::to_time_t(account->access_token().expires_at());
        std::cerr << "    from " << std::put_time(std::gmtime(&expired_at), "%c %Z") << "\n";
        auto ref = account->refresh();
        auto expires_at = std::chrono::system_clock::to_time_t(ref.expires_at());
        std::cerr << "    to   " << std::put_time(std::gmtime(&expires_at), "%c %Z") << "\n";

        /* FIXME: Hopefully these last for an hour... */
        std::this_thread::sleep_until(ref.expires_at() - std::chrono::minutes(10));
    }

    return 0;
}
