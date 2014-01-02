
/*
 * Copyright (C) 2013 Palmer Dabbelt
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

#ifndef LIBMH__MESSAGE_FILE_HXX
#define LIBMH__MESSAGE_FILE_HXX

#include "string_iter.h++"
#include <map>
#include <string>
#include <vector>

namespace mh {
    /* This holds all the data for a message.  Creating one of these
     * objects involves actually opening and reading a file, so it
     * should be avoided if possible.  That said, it's not horribly
     * slow or anything, just slower than the SQL cache. */
    class message_file {
    private:
        /* Holds a map of the lower-case name of a header to a list of
         * the contents of that header. */
        std::map<std::string, std::vector<std::string> > _headers;

    public:
        /* Opens up the given file and parses it into a message
         * structure.  You probably don't want to call this directly
         * but instead want to convert a "class message" into one of
         * these, which allows the directory naming to be handled
         * automatically. */
        message_file(const std::string full_path);

        /* Iterates over all the headers whole name matches the given
         * string.  Matches are case-insensitive, and comma-seperated
         * address lists show up as just the email address with a
         * single item per entry. */
        string_iter headers(const std::string header_name) const;

    private:
        /* Adds a header to the big list of headers. */
        void add_header(const std::string h, const std::string v);
    };
}

#endif
