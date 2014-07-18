
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

#ifndef MHNG__DB__IMAP_MESSAGES_HXX
#define MHNG__DB__IMAP_MESSAGES_HXX

#include <libmhng/mailbox.h++>
#include <libmhng/mailrc.h++>
#include <libmhng/message.h++>
#include <libmhng/sqlite/table.h++>
#include <libmhng/sqlite/connection.h++>

namespace mhng {
    namespace db {
        /* Holds auxiliary IMAP information for messages. */
        class imap_messages {
        private:
            sqlite::table_ptr _table;
            mailbox_ptr _mbox;

        public:
            /* In order to do anything with this table we need to give
             * it a mechanism for accessing the database.  Note that
             * this will create the necessary table if it doesn't
             * exist. */
            imap_messages(const mailbox_ptr& mbox);

        public:
            /* We only allow the selection of an IMAP message by
             * UID. */
            message_ptr select(uint64_t uid);

            /* Drops a message from the IMAP store. */
            void remove(uint64_t uid);

            /* Updates a message to contain a new "purge" field. */
            void update_purge(uint64_t uid, bool purge);
        };
    }
}

#endif
