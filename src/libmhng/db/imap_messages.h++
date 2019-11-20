/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__DB__IMAP_MESSAGES_HXX
#define MHNG__DB__IMAP_MESSAGES_HXX

#include <libmhng/mailbox.h++>
#include <libmhng/mailrc.h++>
#include <libmhng/message.h++>
#include <psqlite/table.h++>
#include <psqlite/connection.h++>

namespace mhng {
    namespace db {
        /* Holds auxiliary IMAP information for messages. */
        class imap_messages {
        private:
            psqlite::table::ptr _table;
            mailbox_ptr _mbox;

        public:
            /* In order to do anything with this table we need to give
             * it a mechanism for accessing the database.  Note that
             * this will create the necessary table if it doesn't
             * exist. */
            imap_messages(const mailbox_ptr& mbox);

        public:
            /* Lists the IMAP UID for a message that matches the given
             * UID. */
            int64_t select(uint64_t uid);

            /* Lists the UID for a message that matches the given IMAP
             * folder/uid pair. */
            uint64_t select(std::string folder, uint32_t imapid);

            /* Drops a message from the IMAP store. */
            void remove(uint64_t uid);
            void remove(std::string folder, uint32_t imapid);

            /* Updates a message to contain a new "purge" field. */
            void update_purge(uint64_t uid, bool purge);

            /* Lists all the messages with the purge flag set. */
            std::vector<uint32_t> select_purge(std::string folder);

            /* Inserts a new message into this table. */
            void insert(std::string folder, uint32_t imapid, uint64_t mhid);
        };
    }
}

#endif
