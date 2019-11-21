/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__DB__IMAP_UIDVALIDITY_HXX
#define MHNG__DB__IMAP_UIDVALIDITY_HXX

#include <libmhng/mailbox.h++>
#include <libmhng/mailrc.h++>
#include <libmhng/unknown.h++>
#include <psqlite/table.h++>
#include <psqlite/connection.h++>

namespace mhng {
    namespace db {
        /* Holds auxiliary IMAP information for messages. */
        class imap_uidvalidity {
        private:
            psqlite::table::ptr _table;
            mailbox_ptr _mbox;

        public:
            /* In order to do anything with this table we need to give
             * it a mechanism for accessing the database.  Note that
             * this will create the necessary table if it doesn't
             * exist. */
            imap_uidvalidity(const mailbox_ptr& mbox);

        public:
            unknown<uint32_t> select(std::string folder, std::string account);
            void update(std::string folder, uint32_t uidv, std::string account);
            void insert(std::string folder, uint32_t uidv, std::string account);
        };
    }
}

#endif
