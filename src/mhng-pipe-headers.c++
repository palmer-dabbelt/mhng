
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
#include <string.h>

int main(int argc, const char **argv)
{
#if defined(HEADERS)
    auto args = mhng::args::parse_normal(argc, argv);
#elif defined(HEADER)
    if (argc < 2) {
        fprintf(stderr, "mhng-header <header name> [messages]: Prints a header\n");
        return 1;
    }

    const char **filtered_argv = argv + 1;
    auto args = mhng::args::parse_normal(argc - 1, filtered_argv);
#else
#error "Define HEADERS or HEADER"
#endif

    auto folders = args->folders();
    auto messages = args->messages();

    for (const auto& msg: messages) {
        for (const auto& header: msg->mime()->root()->headers()) {
#if defined(HEADERS)
            printf("%s: %s\n",
                   header->key_downcase().c_str(),
                   header->utf8().c_str()
                );
#elif defined(HEADER)
            if (strcasecmp(argv[1], header->key().c_str()) != 0)
                continue;

            printf("%s\n",
                   header->utf8().c_str()
                );
#else
#error "Define HEADERS or HEADER"
#endif
        }
    }

    return 0;
}
