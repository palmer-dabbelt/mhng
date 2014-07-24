
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

#ifndef MHNG__MIME__PART_HXX
#define MHNG__MIME__PART_HXX

#include <memory>

namespace mhng {
    namespace mime {
        class part;
        typedef std::shared_ptr<part> part_ptr;
    }
}

#include "header.h++"
#include <libmhng/unknown.h++>
#include <string>
#include <vector>

namespace mhng {
    namespace mime {
        /* Stores a single MIME part, which is the basic building
         * block of a MIME message. */
        class part {
        private:
            /* Contains the raw text of a message, in case anyone
             * wants to look at it. */
            std::vector<std::string> _raw;

            /* Holds the content type of this MIME part, with a bit of
             * parsing having been applied to it already. */
            unknown<std::string> _content_type;
            unknown<std::string> _boundary;
            unknown<std::string> _end_boundary;

            /* Contains a vector that lists all the children of this
             * MIME part. */
            std::vector<part_ptr> _children;

            /* Holds a list of the headers that were present inside
             * this message. */
            std::vector<header_ptr> _headers;

            /* Holds a list of RAW (ie, with newline and not uft-8
             * parsed) strings that constitute the body of this
             * message. */
            std::vector<std::string> _body_raw;

        public:
            /* Creates a new MIME part, which */
            part(const std::vector<std::string>& raw);

        public:
            /* Accessor functions. */
            const std::vector<part_ptr>& children(void) const
                { return _children; }

            std::string content_type(void) const
                { return _content_type.data(); }

            std::string name(void) const { return "unknown"; }

            const std::vector<std::string>& body_raw(void) const
                { return _body_raw; }

        private:
            /* Returns TRUE if the boundary is known and the given
             * line matches the boundary. */
            bool matches_boundary(const std::string& line);
            bool matches_end_boundary(const std::string& line);
        };
    }
}

#endif
