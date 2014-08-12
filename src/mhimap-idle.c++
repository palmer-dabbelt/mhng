
/*
 * Copyright (C) 2013,2014 Palmer Dabbelt
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

#include <libmhimap/gmail_client.h++>
#include <libmhng/args.h++>

#ifndef MHIMAP_MAIN
#define MHIMAP_MAIN main
#endif

int MHIMAP_MAIN(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_folders(argc, argv);

    fprintf(stderr, "IDLE Logging In\n");
    mhimap::gmail_client client(GMAIL_USERNAME, GMAIL_PASSWORD);
    while (true) {
        fprintf(stderr, "Sending IDLE\n");
        client.send_idle("inbox");
        
        auto daemon = args->mbox()->daemon();
        auto message = mhng::daemon::message::sync();
        daemon->send(message);
        
        mhimap::idle_response res = client.wait_idle();
        switch (res) {
        case mhimap::idle_response::TIMEOUT:
        case mhimap::idle_response::DATA:
            break;

        case mhimap::idle_response::DISCONNECT:
            abort();
            break;
        }
    }

    return 0;
}
