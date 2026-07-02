/* Copyright (C) 2013-2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhimap/gmail_client.h++>
#include <libmhimap/logger.h++>
#include <libmhng/args.h++>
#include <vector>

#ifndef MHIMAP_MAIN
#define MHIMAP_MAIN main
#endif

int MHIMAP_MAIN(int argc, const char **argv)
{
    std::vector<const char *> fargv;
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "--verbose") == 0)
            mhimap::logger::set_debug(true);
        else
            fargv.push_back(argv[i]);
    }
    auto args = mhng::args::parse_account(fargv.size(), fargv.data());

    fprintf(stderr, "IDLE Logging In: %s\n",
            mhng::date::now()->local().c_str());
    auto account = args->mbox()->account(args->account());
    auto client = account->is_oauth2()
        ? mhimap::gmail_client(account->name(), account->access_token())
        : mhimap::gmail_client(account->name(), account->password());

    while (true) {
        fprintf(stderr, "Sending IDLE\n");
        client.send_idle("inbox");
        
        auto daemon = args->mbox()->daemon();
        auto message = mhng::daemon::message::sync();
        daemon->send(message);
        
        mhimap::idle_response res = client.wait_idle();
        switch (res) {
        case mhimap::idle_response::DATA:
            fprintf(stderr, "Got DATA response\n");
            break;

        case mhimap::idle_response::TIMEOUT:
            fprintf(stderr, "Got TIMEOUT response\n");
            break;

        case mhimap::idle_response::DISCONNECT:
            fprintf(stderr, "Got DISCONNECT response\n");
            abort();
            break;
        }
    }

    return 0;
}
