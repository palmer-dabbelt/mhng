
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

#ifndef LIBMH__DB__QUERY_HXX
#define LIBMH__DB__QUERY_HXX

namespace mh {
    namespace db {
        class query;
    }
}

#include "connection.h++"
#include <string>
#include <sqlite3.h>
#include <vector>

namespace mh {
    namespace db {
        class query {
        public:
            friend class connection;

        private:
            /* Contains a pointer to the database, that's mapped as a
             * shared pointer. */
            connection_ptr _db;

            /* Contains the list of results from that came from
             * running this query.  It's probably not great to store
             * this in memory, but I'm just going to have to live with
             * it for now! */
            std::vector<result> _results;

            /* Holds the response code from SQLite's query
             * function. */
            int _err;
            char *_err_string;

        public:
            /* Creates a new query from some free-form text, runs it
             * against the provided SQL connection, and stores the
             * results for later. */
            query(connection_ptr db, const char *format, ...)
                __attribute__(( format(printf, 3, 4) ));

            /* Counts the number of results in a query. */
            size_t result_count(void) const { return _results.size(); }

            /* Returns TRUE when the command succeeded, FALSE otherwise. */
            bool successful(void) const { return (_err == SQLITE_OK); }

            /* Returns the error string associated with this command,
             * which is hopefully NULL.  You should check that the
             * command was unsuccessful before looking at this in case
             * SQLite doesn't play nice. */
            const char *error_string(void) const { return _err_string; }

            /* Returns the exact error code suggested by SQLite, in
             * case anyone wants to actually examine it exactly. */
            int error_code(void) const { return _err; }

        protected:
            /* Creates a new query but doesn't actually run the query
             * -- this is designed for subclasses that need to do
             * magic before running the constructor. */
            query(connection_ptr db);

            /* Actually runs the query, filling in the _results array.
             * This is only supposed to be used by sub-classes inside
             * their constructors. */
            void run(const std::string sql);
        };
    }
}

#endif
