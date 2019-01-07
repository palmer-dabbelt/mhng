
/*
 * Copyright (C) 2014, 2019 Palmer Dabbelt
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
