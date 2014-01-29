
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

#ifndef LIBMH__FOLDER_HXX
#define LIBMH__FOLDER_HXX

namespace mh {
    class folder;
}

#include "message.h++"
#include "mhdir.h++"
#include "options.h++"
#include "db/connection.h++"
#include <string>

namespace mh {
    /* Represents a single MH folder. */
    class folder {
    private:
        const std::string _name;
        const options_ptr _o;
        db::connection_ptr _db;

    public:
        /* Creates a new folder, given the name of that folder and a
         * database connection to use in order to query that folder's
         * contents.  You almost certainly don't want to use this but
         * instead want to open the folder from an mhdir. */
        folder(const std::string n, options_ptr o, db::connection_ptr db);

        /* Accessor functions. */
        const std::string name(void) const { return _name; }

        /* Prints the full path to this folder.  You probably don't
         * want to use this directly, but should instead just open
         * messages using other functions here (or move them using
         * mhdir), which will have built-in protection against
         * races. */
        const std::string full_path(void) const;

        /* Opens the current message in this folder -- note that this
         * requires a database lookup in order to determine exactly
         * which message should be opened. */
        message open_current(void) const;

        /* Opens a message based on the sequence number associated
         * with a message.  This is a short, integer name that's
         * designed to be easily used by the commandline. */
        message open_seq(int seq) const;

        /* Lists all the messages in this folder. */
        message_iter messages(void) const;
    };
}

#endif
