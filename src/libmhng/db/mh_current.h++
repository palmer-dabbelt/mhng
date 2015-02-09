
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
#include <psqlite/table.h++>
#include <psqlite/connection.h++>

namespace mhng {
    namespace db {
        /* Represents the main MH table, which stores every message
         * that exists in the mailbox. */
        class mh_current {
        private:
            psqlite::table::ptr _table;
            mailbox_ptr _mbox;

        public:
            /* This is a tiny little table that maps every folder to
             * the current message in it (by sequence number). */
            mh_current(const mailbox_ptr& mbox);

        public:
            /* Lists every folder known to MHng. */
            std::vector<std::string> select(void);

            /* Returns the current message in the given folder. */
            unsigned select(const std::string& folder_name);

            /* Replaces the folder name with a sequence number. */
            void update(const std::string& folder_name,
                        unsigned new_seq);

            /* Allows for the modification of the current flag, which
             * determines which folder is the one that is open right
             * now.  Note that you'll have to make sure to put these
             * within a transaction because there's a UNIQUE
             * constraint on the table! */
            void clear_current(const std::string& folder_name);
            void set_current(const std::string& folder_name);

            /* Returns the name of the folder that's currently set. */
            std::string select_current(void);

            /* These probably shouldn't just be jammed into this
             * folder, but I'm fairly lazy so I don't really want to
             * deal with it.  Note that a return code of "-1" means
             * there is no uidvalidity set. */
            int64_t uid_validity(const std::string& folder_name);
            void update_uid_validity(const std::string& folder_name,
                                     uint32_t uid_validity);
        };
    }
}

#endif
