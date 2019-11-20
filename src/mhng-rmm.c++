/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_normal(argc, argv);

    /* This is all in a single transaction for performance reasons:
     * when removing a lot of messages it's better to only sync once
     * rather than once per message. */
    {
        auto t = args->mbox()->db()->deferred_transaction();
        for (const auto& msg: args->messages())
            msg->remove();
    }

    {
        auto daemon = args->mbox()->daemon();
        auto message = mhng::daemon::message::sync();
        daemon->send(message);
    }

    return 0;
}
