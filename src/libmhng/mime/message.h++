
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

#ifndef MHNG__MIME__MESSAGE_HXX
#define MHNG__MIME__MESSAGE_HXX

#include <memory>

namespace mhng {
    namespace mime {
        class message;
        typedef std::shared_ptr<message> message_ptr;
    }
}

#include "header.h++"
#include "part.h++"
#include <string>
#include <vector>

namespace mhng {
    namespace mime {
        /* Stores a single MIME message, which itself is actually just
         * a container around a root MIME object that deals with the
         * fact that some email messages aren't actually in MIME
         * format. */
        class message {
        private:
            part_ptr _root;

        public:
            /* Creates a MIME message, given the raw text of the
             * messages as dumped by IMAP. */
            message(const std::vector<std::string>& raw);

        public:
            /* Returns a pointer to the root of the MIME message, in
             * case someone wants to walk the tree directly. */
            const part_ptr& root(void) const { return _root; }

            /* Returns all the header that match the given string in
             * the top-level MIME message. */
            std::vector<header_ptr> header(const std::string name) const;
        };
    }
}

#endif
