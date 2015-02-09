
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
    if (argc != 2) {
        fprintf(stderr, "mhng-pipe-lookup_address <address>\n");
        return 1;
    }

    const char *empty_argv[] = {argv[0], NULL};
    auto args = mhng::args::parse_all_messages(1, empty_argv);

    std::vector<std::string> addresses = {argv[1]};

    auto mail_rc = args->mbox()->mrc();

    for (const auto& addr_str: addresses) {
        auto addr = mail_rc->emailias(addr_str);
        if (addr->email_known() == false)
            continue;

        printf("%s\n", addr->rfc().c_str());
    }

    return 0;
}
