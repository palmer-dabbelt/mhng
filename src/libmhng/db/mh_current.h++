/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

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
