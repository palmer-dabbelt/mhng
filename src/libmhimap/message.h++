
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

#ifndef LIBMHIMAP__MESSAGE_HXX
#define LIBMHIMAP__MESSAGE_HXX

namespace mhimap {
    class message;
}

#include "folder.h++"
#include <stdint.h>
#include <string>

namespace mhimap {
    /* References a single IMAP message. */
    class message {
    private:
        /* Each message lives within a single folder. */
        const folder _folder;

        /* Each message contains a single immutable identifier. */
        const uint32_t _uid;

    public:
        /* Copy constructor... */
        message(const message &that)
            : _folder(that._folder), _uid(that._uid)
            {
            }

        /* Creates a new message given the two options that define it
         * -- essentially a message is just a container for this
         * pair. */
        message(const folder folder, const uint32_t uid);

        /* Accessor functions. */
        uint32_t uid(void) const { return _uid; }
        const std::string folder_name(void) const { return _folder.name(); }
    };
}

#endif
