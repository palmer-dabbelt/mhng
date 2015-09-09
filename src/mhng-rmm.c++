
/*
 * Copyright (C) 2014 Palmer Dabbelt
 *   <palmer@dabbelt.com>
 *
 * This file is part of mhng.
 *
 * mhng is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mhng is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with mhng.  If not, see <http://www.gnu.org/licenses/>.
 */

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
