/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__DB__MH_NEXTID_HXX
#define MHNG__DB__MH_NEXTID_HXX

#include <libmhng/mailbox.h++>
#include <libmhng/message.h++>
#include <psqlite/table.h++>
#include <psqlite/connection.h++>

namespace mhng {
    namespace db {
        /* Just holds a single value, the next UID to use. */
        class mh_nextid {
        private:
            psqlite::table::ptr _table;
            mailbox_ptr _mbox;

        public:
            /* This is a tiny little table that maps every folder to
             * the current message in it (by sequence number). */
            mh_nextid(const mailbox_ptr& mbox);

        public:
            /* Lists the single item in this table.  */
            uint64_t select(void);

            /* Replaces the current next-id with something else. */
            void update(uint64_t id);
        };
    }
}

#endif
