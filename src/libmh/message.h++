
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

#ifndef LIBMH__MESSAGE_HXX
#define LIBMH__MESSAGE_HXX

namespace mh {
    class message;
}

#include "message_iter.h++"
#include "mhdir.h++"
#include "options.h++"
#include "uid.h++"
#include "db/connection.h++"
#include <string>

namespace mh {
    /* Represents a single MH message. */
    class message {
    private:
        const uid _id;
        const options_ptr _o;
        db::connection_ptr _db;

    public:
        /* Creates a link to a message that's already in the database,
         * which just consists of obtaining the unique ID of the
         * message inside the database so we can refer to it later. */
        static message link(const uid id, options_ptr o, connection_ptr db);

        /* Parses the relevant information from the given message
         * file, inserts a record into the database cooresponding to
         * that message, reads back the newly-created UID, and then
         * returns link(new_uid, ...). */
        static message insert(const uid id,
                              options_ptr o,
                              connection_ptr db,
                              const std::string filename);
    };
}

#endif
