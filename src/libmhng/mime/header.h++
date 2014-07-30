
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

#ifndef MHNG__MIME__HEADER_HXX
#define MHNG__MIME__HEADER_HXX

#include <memory>

namespace mhng {
    namespace mime {
        class header;
        typedef std::shared_ptr<header> header_ptr;
    }
}

#include <vector>
#include <string>

namespace mhng {
    namespace mime {
        /* Stores a single MIME header in its original format (a block
         * of lines) along with mechanisms for formatting those
         * headers in a sane way. */
        class header {
        private:
            std::vector<std::string> _raw;

        public:
            /* Creates a new, empty header.  Note that it's pretty
             * much required that you end up adding information to
             * this header in order to make it actually valid... */
            header(const std::string& first_line);

        public:
            /* Adds a new line of header to this header. */
            void add_line(const std::string& line);

            /* Forms the header (only the value part, not the key) as
             * a single line, stripping all but one space when the
             * line changes over. */
            std::string single_line(void) const;

            /* Splits up this header based on commas. */
            std::vector<std::string> split_commas(void) const;

            /* Returns the key of this header, which is the stuff
             * before the colon. */
            std::string key(void) const;
            std::string key_downcase(void) const;

            /* Checks to see if the key of this header matches the
             * given key. */
            bool match(const std::string& key);
            bool match(const std::vector<std::string>& key);

            /* Returns the raw bytes of this header. */
            const std::vector<std::string>& raw(void) const
                { return _raw; }
        };
    }
}

#endif
