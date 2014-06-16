
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

    if (c0.unique == true && c1.unique == false) {
        snprintf(buffer, BUFFER_SIZE,
                 "CREATE TABLE %s(%s %s, %s %s, UNIQUE(%s));", name.c_str(),
                 c0.name.c_str(), to_string(c0.type),
                 c1.name.c_str(), to_string(c1.type),
                 c0.name.c_str()
            );
    } else if (c0.unique == true && c1.unique == true) {
        snprintf(buffer, BUFFER_SIZE,
                 "CREATE TABLE %s(%s %s, %s %s, UNIQUE(%s, %s));", name.c_str(),
                 c0.name.c_str(), to_string(c0.type),
                 c1.name.c_str(), to_string(c1.type),
                 c0.name.c_str(), c1.name.c_str()
            );
    } else {
        fprintf(stderr, "Fix unique handling\n");
        abort();
    }

    run(buffer);
}

create_table::create_table(connection_ptr db, const std::string name,
                           table_col c0, table_col c1, table_col c2)
    : query(db)
{
    char buffer[BUFFER_SIZE];

    if (c0.unique == true && c1.unique == true && c2.unique == true) {
        snprintf(buffer, BUFFER_SIZE,
                 "CREATE TABLE %s(%s %s, %s %s, %s %s, UNIQUE(%s, %s, %s));",
                 name.c_str(),
                 c0.name.c_str(), to_string(c0.type),
                 c1.name.c_str(), to_string(c1.type),
                 c2.name.c_str(), to_string(c2.type),
                 c0.name.c_str(), c1.name.c_str(), c2.name.c_str()
            );
    } else {
        fprintf(stderr, "Fix unique handling\n");
        abort();
    }

    run(buffer);
}

create_table::create_table(connection_ptr db, const std::string name,
                           table_col c0, table_col c1, table_col c2,
                           table_col c3)
    : query(db)
{
    char buffer[BUFFER_SIZE];

    if (c0.unique == true && c1.unique == true && c2.unique == true && c3.unique == false) {
        snprintf(buffer, BUFFER_SIZE,
                 "CREATE TABLE %s(%s %s, %s %s, %s %s, %s %s, UNIQUE(%s, %s, %s));",
                 name.c_str(),
                 c0.name.c_str(), to_string(c0.type),
                 c1.name.c_str(), to_string(c1.type),
                 c2.name.c_str(), to_string(c2.type),
                 c3.name.c_str(), to_string(c3.type),
                 c0.name.c_str(), c1.name.c_str(), c2.name.c_str()
            );
    } else {
        fprintf(stderr, "Fix unique handling\n");
        abort();
    }

    run(buffer);
}

create_table::create_table(connection_ptr db, const std::string name,
                           table_col c0, table_col c1, table_col c2,
                           table_col c3, table_col c4, table_col c5,
                           table_col c6, table_col c7)
    : query(db)
{
    char buffer[BUFFER_SIZE];

    if (c1.unique == true && c2.unique == true) {
        snprintf(buffer, BUFFER_SIZE,
                 "CREATE TABLE %s(%s %s, %s %s, %s %s, %s %s, %s %s, %s %s, %s %s, %s %s, UNIQUE(%s, %s));",
                 name.c_str(),
                 c0.name.c_str(), to_string(c0.type),
                 c1.name.c_str(), to_string(c1.type),
                 c2.name.c_str(), to_string(c2.type),
                 c3.name.c_str(), to_string(c3.type),
                 c4.name.c_str(), to_string(c4.type),
                 c5.name.c_str(), to_string(c5.type),
                 c6.name.c_str(), to_string(c6.type),
                 c7.name.c_str(), to_string(c7.type),
                 c1.name.c_str(), c2.name.c_str()
            );
    } else {
        fprintf(stderr, "Fix unique handling\n");
        abort();
    }

    run(buffer);
}
