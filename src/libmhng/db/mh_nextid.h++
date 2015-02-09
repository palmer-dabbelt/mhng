
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
