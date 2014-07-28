
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

#ifndef MHNG__DB__MH_MESSAGES_HXX
#define MHNG__DB__MH_MESSAGES_HXX

#include <libmhng/mailbox.h++>
#include <libmhng/mailrc.h++>
#include <libmhng/message.h++>
#include <libmhng/sqlite/table.h++>
#include <libmhng/sqlite/connection.h++>

namespace mhng {
    namespace db {
        /* Represents the main MH table, which stores every message
         * that exists in the mailbox. */
        class mh_messages {
        private:
            sqlite::table_ptr _table;
            mailbox_ptr _mbox;

        public:
            /* In order to do anything with this table we need to give
             * it a mechanism for accessing the database.  Note that
             * this will create the necessary table if it doesn't
             * exist. */
            mh_messages(const mailbox_ptr& mbox);

        public:
            /* There are two sane ways to select a message: either by
             * folder name and sequence number (which you shouldn't do
             * unless you're parsing some user input), or by a unique
             * ID. */
            message_ptr select(const std::string& folder_name,
                               const sequence_number_ptr& seq);
            message_ptr select(uint64_t uid);

            /* Searches for a message offset */
            message_ptr select(const std::string& folder_name,
                               const sequence_number_ptr& seq,
                               int offset);

            /* Updates a message with a new sequence number, given the
             * message's unique ID. */
            void update(uint64_t uid,
                        const sequence_number_ptr& seq);

            /* This selects all the messages in a given folder. */
            std::vector<message_ptr> select(const std::string& folder_name);

            /* Inserts a new message into the database. */
            void insert(unsigned seq,
                        std::string folder,
                        std::string date,
                        std::string from,
                        std::string to,
                        std::string subject,
                        uint64_t uid);

            /* This removes a single message.  Here we only allow
             * messages to be removed by their unique ID as otherwise
             * we could easily have race conditions. */
            void remove(uint64_t uid);
        };
    }
}

#endif
