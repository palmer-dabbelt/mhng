
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

#ifndef MHNG__SQLITE__CONNECTION_HXX
#define MHNG__SQLITE__CONNECTION_HXX

#include <memory>

namespace mhng {
    namespace sqlite {
        class connection;
        typedef std::shared_ptr<connection> connection_ptr;
    }
}

#include "result.h++"
#include "table.h++"
#include <sqlite3.h>
#include <string>
#include <vector>

namespace mhng {
    namespace sqlite {
        /* Holds a single database connection.  To avoid deadlock you
         * almost certainly only want to open one of these per
         * process! */
        class connection {
        private:
            /* The whole point of this class is to wrap a regular
             * SQLite3 database connection in a way that's ammenable
             * to C++11. */
            struct sqlite3 *_db;

        public:
            /* Opens a new connection to a SQLite database given the
             * full path to the file that contains that database. */
            connection(const std::string& path);

        public:
            /* Runs a select query against the given table, returning
             * a list of results that match the query.  Note that
             * there are a few different forms of this call: by
             * default this will select every row and column, but it's
             * strongly suggested that you use better query
             * methods. */
            result_ptr select(const table_ptr& table);
            result_ptr select(const table_ptr& table,
                                           const std::vector<column_ptr>& c);
            result_ptr select(const table_ptr& table,
                                           const char *format,
                                           ...);
            result_ptr select(const table_ptr& table,
                                           const char *format,
                                           va_list args);
            result_ptr select(const table_ptr& table,
                                           const std::vector<column_ptr>& c,
                                           const char *format,
                                           ...);
            result_ptr select(const table_ptr& table,
                              const std::vector<column_ptr>& c,
                              const char *format,
                              va_list args);
                                       
        };
    }
}

#endif