
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

#ifndef LIBMH__IMAP_STORE_HXX
#define LIBMH__IMAP_STORE_HXX

namespace mh {
    class imap_store;
}

#include "options.h++"
#include "db/connection.h++"

namespace mh {
    /* This deals with the extra information that we need to store in
     * a persistant database in order for IMAP synchronization to work
     * properly. */
    class imap_store {
    private:
        const options_ptr _o;
        db::connection_ptr _db;

    public:
        /* This is effectively a way of hiding the actual database
         * connection from an IMAP synchronization layer while still
         * allowing access to the database.  Effectively, non-mh code
         * won't be able to call this function because it'll never
         * actually be able to obtain a db::connection_ptr, you'll
         * instead need to get one of these from an mhdir. */
        imap_store(const options_ptr o, db::connection_ptr db);

        /* Creates a new folder entry in the database, returning TRUE
         * if that folder used to exist and FALSE if the folder didn't
         * exist before creation.  If FALSE is returned then the
         * database will be unchanged, while if TRUE was returned then
         * the database will be updated with the new information. */
        bool try_insert_folder(const std::string folder_name,
                               uint32_t uidvalidity);

        /* Checks the UIDVALIDITY of a given folder, by name. */
        uint32_t check_uidvalidity(const std::string folder_name);
    };
}

#endif
