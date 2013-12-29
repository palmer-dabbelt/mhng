
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

namespace mh {
    namespace db {
        class connection;
    }
}

#include "result_iter.h++"
#include "query.h++"
#include <memory>
#include <string>
#include <sqlite3.h>

namespace mh {
    namespace db {
        /* This is almost certainly how you want to connect to a
         * database, by accessing it through a shared pointer so we
         * don't end up opening two connections to the same
         * database. */
        typedef std::shared_ptr<connection> connection_ptr;

        /* Stores a single connection to the database that's used to
         * store all small bits of information.  You probably don't
         * want to open more than a single one of these to a single
         * database inside a single process, which is what the shared
         * pointer is for. */
        class connection {
        private:
            /* The SQLite3 pointer. */
            struct sqlite3 *_s;

            /* The number of references that remain to this */
            int references;

        public:
            /* Calls the constructor below, wrapping it in a shared
             * pointer. */
            static connection_ptr create(const std::string path);

            /* Cleans up this connection, committing and releasing all
             * SQLite state. */
            ~connection(void);

            /* Submits a query to the database over this connection,
             * returning an iterator that lists the results. */
            result_iter run(const query &q);

        private:
            /* Opens a new connection to a SQLite database, given the
             * filename of that database, which is expected to be a
             * "*.sqlite3" file. */
            connection(const std::string path);
        };
    }
}

#endif
