
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

#ifndef LIBMHIMAP__FOLDER_HXX
#define LIBMHIMAP__FOLDER_HXX

namespace mhimap {
    class folder;
}

#include <string>

namespace mhimap {
    /* References a single IMAP folder (called mailboxes in the proper
     * IMAP spec).  This wraps the response of an IMAP SELECT command
     * up so code outside the library doesn't have to be fully exposed
     * to that.  */
    class folder {
    private:
        /* Stores the full name (starting from the mailbox root) of
         * this folder. */
        const std::string _name;

    public:
        /* Creates a new folder just given its name. */
        folder(const std::string name);

        /* Accessor functions. */
        const std::string name(void) const { return _name; }
    };
}

#endif
