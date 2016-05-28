/*
 * Copyright (C) 2016 Palmer Dabbelt
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
#include <string.h>

int main(int argc, const char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "mhng-pipe-find_with_header <header name>: Prints a header\n");
        return 1;
    }

    const char *empty_argv[] = {argv[0], NULL};
    auto args = mhng::args::parse_all_messages(1, empty_argv);

    auto messages = args->messages();

    for (const auto& msg: messages) {
        for (const auto& header: msg->mime()->root()->headers()) {
            if (strcasecmp(argv[1], header->key().c_str()) != 0)
                continue;
            if (strcasecmp(argv[2], header->utf8().c_str()) != 0)
                continue;

            printf("%u\n",
                   msg->seq()->to_uint()
                );
        }
    }

    return 0;
}
