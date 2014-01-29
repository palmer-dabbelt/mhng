
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

#include "folder.h++"
#include "db/create_table.h++"

using namespace mh;
using namespace mh::db;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#ifndef TABLE
#define TABLE "MH__current"
#endif

folder::folder(const std::string n, options_ptr o, db::connection_ptr db)
    : _name(n),
      _o(o),
      _db(db)
{
}

const std::string folder::full_path(void) const
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s/mail/%s",
             _o->mhdir().c_str(), _name.c_str());
    return buffer;
}

message folder::open_current(void) const
{
    /* This table holds the current sequence number for every folder,
     * which allows us to look up the message that should be opened by
     * default. */
    if (_db->table_exists(TABLE) == false) {
        fprintf(stderr, "Table '%s' doesn't exist\n", TABLE);

        create_table create(_db, TABLE,
                            table_col("folder", col_type::STRING, true),
                            table_col("seq", col_type::INTEGER)
            );

        if (create.successful() == false) {
            fprintf(stderr, "Error creating table '%s'\n", TABLE);
            fprintf(stderr, "  %d: '%s'\n",
                    create.error_code(), create.error_string());
            abort();
        }
    }

    /* If a message was given on the commandline then we should update
     * the database so that message is now set to the current message
     * -- we do it here to avoid any race conditions. */
    if (_o->seq_valid() == true) {
        query seq(_db, "REPLACE INTO %s (folder, seq) VALUES ('%s', %d);",
                  TABLE, _name.c_str(), _o->seq());

        if (seq.successful() == false) {
            fprintf(stderr, "Error updating current for folder '%s'\n",
                    _name.c_str());
            fprintf(stderr, "  %d: '%s'\n",
                    seq.error_code(), seq.error_string());
            abort();
        }

        return open_seq(_o->seq());
    }

    query seq(_db, "SELECT (seq) FROM %s WHERE folder='%s';",
              TABLE, _name.c_str());

    for (auto it = seq.results(); !it.done(); ++it) {
        int seq = atoi((*it).get("seq").c_str());
        return open_seq(seq);
    }

    /* If there was no default and nothing was specified then just go
     * ahead and open the first message, that's a reasonable
     * default... */
    return open_seq(1);
}

message folder::open_seq(int seq) const
{
    return message::folder_search(_name, _o, _db, seq);
}

message_iter folder::messages(void) const
{
    std::vector<message> messages;

    db::query select(_db, "SELECT (uid) FROM %s WHERE folder='%s';",
                     "MH__messages", _name.c_str());

    for (auto it = select.results(); !it.done(); ++it) {
        uid id((*it).get("uid"));
        messages.push_back(message::link(id, _o, _db));
    }

    return message_iter(messages);
}
