
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

#include "date_iter.h++"
#include "mime.h++"
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

        /* For plain-text messages this is set to TRUE, and for MIME
         * messages this is set to FALSE. */
        bool _plain_text;

        /* For plain-text messages, this contains the body text.  For
         * MIME messages, this is just empty. */
        std::vector<std::string> _body;

        /* For MIME messages, this holds the root of the MIME tree of
         * this message.  For plain-text messages, this is just
         * empty. */
        mime *_mime;

    public:
        /* Opens up the given file and parses it into a message
         * structure.  You probably don't want to call this directly
         * but instead want to convert a "class message" into one of
         * these, which allows the directory naming to be handled
         * automatically. */
        message_file(const std::string full_path);

        ~message_file(void);

        /* Iterates over all the headers whole name matches the given
         * string.  Matches are case-insensitive, and comma-seperated
         * address lists show up as just the email address with a
         * single item per entry. */
        string_iter headers(const std::string header_name) const;

        /* These are just like headers(), but they attempt to parse
         * the headers into more user-friendly sorts of formats. */
        string_iter headers_address(const std::string header_name) const;
        date_iter headers_date(const std::string header_name) const;

        /* Returns at iterator that lists all the files in the body of
         * this message. */
        string_iter body(void) const;

        /* These are quick access mechanisms that just return the
         * first header of a given type. */
        const std::string header(const std::string header_name) const
            {
                for (auto it = headers(header_name); !it.done(); ++it)
                    return *it;

                return "";
            }
        const std::string header_address(const std::string header_name) const
            {
                for (auto it = headers_address(header_name); !it.done(); ++it)
                    return *it;
                abort();
                return "";
            }
        const date header_date(const std::string header_name) const
            {
                for (auto it = headers_date(header_name); !it.done(); ++it)
                    return *it;
                abort();
                return date("");
            }

    private:
        /* Adds a header to the big list of headers. */
        void add_header(const std::string h, const std::string v);
    };
}

#endif
