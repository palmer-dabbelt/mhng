
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

#include "create_table.h++"
#include <stdlib.h>

using namespace mh;
using namespace mh::db;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

create_table::create_table(connection_ptr db, const std::string name,
                           table_col c0, table_col c1)
    : query(db)
{
    char buffer[BUFFER_SIZE];

    snprintf(buffer, BUFFER_SIZE,
             "CREATE TABLE %s(%s %s, %s %s);", name.c_str(),
             c0.name.c_str(), to_string(c0.type),
             c1.name.c_str(), to_string(c1.type)
        );

    run(buffer);
}
