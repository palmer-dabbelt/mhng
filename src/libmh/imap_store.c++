
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

#define FOLDERS_TBL "IMAP__folders"
#define MSG_TBL "IMAP__messages"

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
        if (_db->table_exists(FOLDERS_TBL) == false) {
            fprintf(stderr, "Table '%s' doesn't exist\n", FOLDERS_TBL);

            create_table create(_db, FOLDERS_TBL,
                                table_col("name", col_type::STRING, true),
                                table_col("uidv", col_type::INTEGER));
            
            if (create.successful() == false) {
                fprintf(stderr, "Error creating table '%s'\n", FOLDERS_TBL);
                fprintf(stderr, "  %d: '%s'\n",
                        create.error_code(), create.error_string());
                abort();
            }
        }
    }

    /* Attempts to insert a new pair into the table.  Note that
     * because the "name" field is specified as unique this WILL fail
     * whenever a duplicate is specified.  That's correct because we
     * don't want to overwrite any UIDVALIDITY values in the table,
     * just store new ones. */
    query insert(_db, "INSERT into %s (name, uidv) VALUES ('%s', %d);",
                 FOLDERS_TBL, name.c_str(), uidvalidity);

    return !insert.successful();
}

uint32_t imap_store::get_uidvalidity(const std::string folder_name)
{
    query select(_db, "SELECT * from %s where name='%s';",
                 FOLDERS_TBL, folder_name.c_str());

    if (select.result_count() > 1) {
        fprintf(stderr, "UNIQUE not respected on %s.name\n", FOLDERS_TBL);
        abort();
    }

    if (select.result_count() == 0) {
        fprintf(stderr, "No folder named '%s'\n", folder_name.c_str());
        fprintf(stderr, "Call try_insert_folder() first\n");
        abort();
    }

    uint32_t out;
    for (auto it = select.results(); !it.done(); ++it) {
        std::string uidvstr = (*it).get("uidv");
        out = atol(uidvstr.c_str());
    }
    return out;
}

bool imap_store::has(const std::string folder, uint32_t uid)
{
    /* Checks if the correct table exists, if it doesn't then we'll
     * have to create it. */
    {
        if (_db->table_exists(MSG_TBL) == false) {
            fprintf(stderr, "Table '%s' doesn't exist\n", MSG_TBL);

            create_table create(_db, MSG_TBL,
                                table_col("folder", col_type::STRING, true),
                                table_col("uid", col_type::INTEGER, true));
            
            if (create.successful() == false) {
                fprintf(stderr, "Error creating table '%s'\n", MSG_TBL);
                fprintf(stderr, "  %d: '%s'\n",
                        create.error_code(), create.error_string());
                abort();
            }
        }
    }

    /* Here's where we actually search for the message. */
    query select(_db, "SELECT * from %s where folder='%s' AND uid='%u';",
                 MSG_TBL, folder.c_str(), uid);

    if (select.result_count() > 1) {
        fprintf(stderr, "UNIQUE not respected on %s.(folder&uid)\n", MSG_TBL);
        abort();
    }

    return (select.result_count() == 1);
}

std::vector<uint32_t> imap_store::uids(const std::string folder)
{
    query select(_db, "SELECT (uid) from %s where folder='%s';",
                 MSG_TBL, folder.c_str());

    std::vector<uint32_t> out;

    for (auto it = select.results(); !it.done(); ++it) {
        std::string uidstr = (*it).get("uid");
        uint32_t uid = atol(uidstr.c_str());
        out.push_back(uid);
    }

    return out;
}

void imap_store::insert(const mhimap::message &m)
{
    query ins(_db, "INSERT into %s (folder, uid) VALUES ('%s', %d);",
              MSG_TBL, m.folder_name().c_str(), m.uid());

    if (ins.successful() == false) {
        fprintf(stderr, "Unable to insert message\n");
        fprintf(stderr, "  (%d): %s\n", ins.error_code(),
                ins.error_string());
        abort();
    }
}
