/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_folders(argc, argv);
    auto daemon = args->mbox()->daemon();

#if defined(UP)
    auto message = mhng::daemon::message::net_up();
#elif defined(DOWN)
    auto message = mhng::daemon::message::net_down();
#else
#error "Define UP or DOWN"
#endif

    daemon->send(message);
        
    return 0;
}
