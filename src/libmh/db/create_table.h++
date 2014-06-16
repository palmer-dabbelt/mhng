
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

#ifndef LIBMH__DB__CREATE_TABLE_HXX
#define LIBMH__DB__CREATE_TABLE_HXX

#include "col_type.h++"
#include "table_col.h++"
#include "query.h++"

namespace mh {
    namespace db {
        /* This handles a CREATE TABLE request from a user. */
        class create_table: public query {
        public:
            create_table(connection_ptr db, const std::string name,
                         table_col c0, table_col c1);

            create_table(connection_ptr db, const std::string name,
                         table_col c0, table_col c1, table_col c2);

            create_table(connection_ptr db, const std::string name,
                         table_col c0, table_col c1, table_col c2,
                         table_col c3);

            create_table(connection_ptr db, const std::string name,
                         table_col c0, table_col c1, table_col c2,
                         table_col c3, table_col c4, table_col c5,
                         table_col c6, table_col c7);

        };
    }
}

#endif
