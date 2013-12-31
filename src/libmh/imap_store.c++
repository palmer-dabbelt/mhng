
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

#include "imap_store.h++"
#include "db/create_table.h++"
#include "db/query.h++"

using namespace mh;
using namespace mh::db;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

imap_store::imap_store(const options_ptr o, db::connection_ptr db)
    : _o(o),
      _db(db)
{
}

bool imap_store::try_insert_folder(const std::string name,
                                   uint32_t uidvalidity)
{
    /* Checks if the correct table exists, if it doesn't then we'll
     * have to create it. */
    {
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "IMAP__%s", name.c_str());

        if (_db->table_exists(buffer) == false) {
            fprintf(stderr, "Table '%s' doesn't exist, creating\n", buffer);

            create_table create(_db, buffer,
                                table_col("name", col_type::STRING),
                                table_col("uidvalidity", col_type::INTEGER));
            
            if (create.successful() == false) {
                fprintf(stderr, "Error creating table\n");
                fprintf(stderr, "  %d: '%s'\n",
                        create.error_code(), create.error_string());
                abort();
            }
        }
    }

    uidvalidity++;
    uidvalidity++;

    abort();
    return false;
}
