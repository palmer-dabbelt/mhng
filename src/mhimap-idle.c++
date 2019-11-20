/* Copyright (C) 2013-2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhimap/gmail_client.h++>
#include <libmhng/args.h++>

#ifndef MHIMAP_MAIN
#define MHIMAP_MAIN main
#endif

int MHIMAP_MAIN(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_folders(argc, argv);

    fprintf(stderr, "IDLE Logging In: %s\n",
            mhng::date::now()->local().c_str());
    if (args->mbox()->accounts().size() != 1) {
        std::cerr << "MHng only supports a single account\n";
        abort();
    }
    auto account = args->mbox()->accounts()[0];
    mhimap::gmail_client client(account->name(), account->access_token());
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
