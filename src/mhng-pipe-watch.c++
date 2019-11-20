/* Copyright (C) 2014-2019 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <cassert>
#include <cstring>
#include <iostream>
#include <regex>
#include <sstream>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_folders(argc, argv);
    auto daemon = args->mbox()->daemon();

    uint32_t last_ticket = 0;

    while (true) {
        auto message = mhng::daemon::message::folder_event(last_ticket);
        auto resp = daemon->send(message);
        resp->wait();
        assert(last_ticket < resp->ticket());
        last_ticket = resp->ticket();
        std::cout << std::to_string(last_ticket) << std::endl;
    }
        
    return 0;
}
