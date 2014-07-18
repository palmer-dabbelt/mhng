
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

#ifndef MHNG__DB__MH_DEFAULT_HXX
#define MHNG__DB__MH_DEFAULT_HXX

#include <libmhng/mailbox.h++>
#include <libmhng/mailrc.h++>
#include <libmhng/message.h++>
#include <libmhng/sqlite/table.h++>
#include <libmhng/sqlite/connection.h++>

namespace mhng {
    namespace db {
        /* Represents the main MH table, which stores every message
         * that exists in the mailbox. */
        class mh_default {
        private:
            sqlite::table_ptr _table;
            mailbox_ptr _mbox;

        public:
            /* In order to do anything with this table we need to give
             * it a mechanism for accessing the database.  Note that
             * this will create the necessary table if it doesn't
             * exist. */
            mh_default(const mailbox_ptr& mbox);

        public:
            /* This isn't really a table, it just stores a single
             * entry.  Thus there's really only one entry you can
             * select! */
            folder_ptr select(void);
        };
    }
}

#endif
