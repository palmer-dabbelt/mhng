/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__DB__MH_ACCOUNTS_HXX
#define MHNG__DB__MH_ACCOUNTS_HXX

#include <libmhng/mailbox.h++>
#include <libmhng/account.h++>
#include <psqlite/table.h++>
#include <psqlite/connection.h++>

namespace mhng {
    namespace db {
        /* Represents the main MH table, which stores every message
         * that exists in the mailbox. */
        class mh_accounts {
        private:
            psqlite::table::ptr _table;
            mailbox_ptr _mbox;

        public:
            /* In order to do anything with this table we need to give
             * it a mechanism for accessing the database.  Note that
             * this will create the necessary table if it doesn't
             * exist. */
            mh_accounts(const mailbox_ptr& mbox);

        public:
            /* Just gets every single account. */
            std::vector<account_ptr> select(void);

            /* Selects a single account, by name. */
            account_ptr select(std::string name);

            /* Creates a new account. */
            void insert(std::string name,
                        std::string client_id,
                        std::string access_token,
                        std::string refresh_token,
                        putil::chrono::datetime expires_at);

            /* Updates the credentials for an account. */
            void update(std::string name,
                        std::string access_token,
                        std::string refresh_token,
                        putil::chrono::datetime expires_at);

            /* Performs the login sequence again, in case the grant has been
             * invalidated (like if you changed your password, for example). */
            void get_new_grant(std::string name,
                               std::string client_id,
                               std::string access_token,
                               std::string refresh_token,
                               putil::chrono::datetime expires_at);

        };
    }
}

#endif
