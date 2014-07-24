
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

#ifndef MHNG__DB__MH_CURRENT_HXX
#define MHNG__DB__MH_CURRENT_HXX

#include <libmhng/mailbox.h++>
#include <libmhng/message.h++>
#include <libmhng/sqlite/table.h++>
#include <libmhng/sqlite/connection.h++>

namespace mhng {
    namespace db {
        /* Represents the main MH table, which stores every message
         * that exists in the mailbox. */
        class mh_current {
        private:
            sqlite::table_ptr _table;
            mailbox_ptr _mbox;

        public:
            /* This is a tiny little table that maps every folder to
             * the current message in it (by sequence number). */
            mh_current(const mailbox_ptr& mbox);

        public:
            /* Returns the current message in the given folder. */
            unsigned select(const std::string& folder_name);

            /* Replaces the folder name with a new value. */
            void update(const std::string& folder_name,
                        unsigned new_current);
        };
    }
}

#endif
