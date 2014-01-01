
/*
 * Copyright (C) 2013 Palmer Dabbelt
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

#ifndef LIBMH__DB__CONNECTION_HXX
#define LIBMH__DB__CONNECTION_HXX

#include <memory>

namespace mh {
    namespace db {
        class connection;
        class query;

        /* This is almost certainly how you want to connect to a
         * database, by accessing it through a shared pointer so we
         * don't end up opening two connections to the same
         * database. */
        typedef std::shared_ptr<connection> connection_ptr;
    }
}


#include "result_list.h++"
#include "query.h++"
#include <memory>
#include <string>
#include <sqlite3.h>
#include <vector>

namespace mh {
    namespace db {
        /* Stores a single connection to the database that's used to
         * store all small bits of information.  You probably don't
         * want to open more than a single one of these to a single
         * database inside a single process, which is what the shared
         * pointer is for. */
        class connection {
        public:
            friend class query;

        private:
            /* The SQLite3 pointer. */
            struct sqlite3 *_s;

            /* FIXME: Is this used now that we have shared pointers? */
            /* The number of references that remain to this */
            int references;

            /* Here we have a weak pointer to ourself, used to pass a
             * shared pointer to some subclasses. */
            std::weak_ptr<connection> self_ref;

            /* The transaction counter, which counts the number of
             * tras_up()s that have been done.  When this is
             * trans_up()ed and is 0 then a transaction is started,
             * and when it is trans_down()ed to 0 then the transaction
             * will be terminated. */
            int trans_cnt;

        public:
            /* Calls the constructor below, wrapping it in a shared
             * pointer. */
            static connection_ptr create(const std::string path);

            /* Cleans up this connection, committing and releasing all
             * SQLite state. */
            ~connection(void);

            /* Returns TRUE if the given table exists, and FALSE
             * otherwise. */
            bool table_exists(const std::string table_name);

            /* Transaction support, see the comment in mhdir for more
             * information.  */
            void trans_up(void);
            void trans_down(void);

            /* Returns the _ROWID_ of the last row that was inserted
             * into this database.  Note that this implicitly
             * references the last TABLE that was inserted to, but
             * it's probably not an issue because you need to put this
             * in a transaction either way to be even remotely
             * useful. */
            uint64_t last_insert_rowid(void);

        private:
            /* Opens a new connection to a SQLite database, given the
             * filename of that database, which is expected to be a
             * "*.sqlite3" file. */
            connection(const std::string path);
        };
    }
}

#endif
