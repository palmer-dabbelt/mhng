
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

void folder::canonicalize_current(void)
{
    /* If we've never done anything then just don't touch stuff at
     * all. */
    if (_db->table_exists(TABLE) == false)
        return;

    /* Try and figure out what the current sequency number is, if
     * there isn't any then just give up right now. */
    query seq(_db, "SELECT (seq) FROM %s WHERE folder='%s';",
              TABLE, _name.c_str());
    int cur = -1;
    for (auto it = seq.results(); !it.done(); ++it)
        cur = atoi((*it).get("seq").c_str());
    if (cur == -1)
        return;

    /* Check to see if the sequence number exists and if it does then
     * just don't do anything else, we're already OK and don't want to
     * force too many writes. */
    query msg(_db, "SELECT (seq) FROM %s WHERE folder='%s' AND seq=%d;",
              "MH__messages",
              _name.c_str(),
              cur
        );
    for (auto it = msg.results(); !it.done(); ++it)
        return;

    /* If we've made it here then there's no sequence number that
     * matches what we think should be selected.  That means we should
     * iterate through all the messages and try to find one that's
     * close to what was requested. */
    int ncur = -1;
    query fcur(_db, "SELECT (seq) FROM %s WHERE folder='%s';",
               "MH__messages",
               _name.c_str()
        );
    for (auto it = fcur.results(); !it.done(); ++it) {
        int check = atoi((*it).get("seq").c_str());

        if (ncur < 0)
            ncur = check;

        if (abs(ncur - cur) > abs(check - cur))
            ncur = check;
    }

    /* If no messages were found then we just need to give up now. */
    if (ncur < 0)
        return;

    /* Now we simply need to store the new message ID into the
     * database. */
    query set(_db, "REPLACE INTO %s (folder, seq) VALUES ('%s', %d);",
              TABLE,
              _name.c_str(),
              ncur
        );
    if (set.successful() == false) {
        fprintf(stderr, "Unable to update table %s\n", TABLE);
        abort();
    }
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

size_t folder::message_count(void) const
{
    size_t count = 0;

    db::query select(_db, "SELECT (uid) FROM %s WHERE folder='%s';",
                     "MH__messages", _name.c_str());

    for (auto it = select.results(); !it.done(); ++it) {
        count++;
    }

    return count;
}

void folder::seek_seq(int offset)
{
    if (_db->table_exists(TABLE) == false) {
        abort();
    }

    query seq(_db, "SELECT (seq) FROM %s WHERE folder='%s';",
              TABLE, _name.c_str());

    for (auto it = seq.results(); !it.done(); ++it) {
        int seq = atoi((*it).get("seq").c_str());
        seq += offset;

        /* Attempt to find the next message along this seek path that
         * actually exists in the target. */
        /* FIXME: This MIN should be in SQL. */
        query nseq(_db, "SELECT (seq) FROM %s WHERE folder='%s' AND seq>=%d ORDER BY seq DESC;",
                   "MH__messages", _name.c_str(), seq);
        for (auto nit = nseq.results(); !nit.done(); ++nit) {
            seq = atoi((*nit).get("seq").c_str());
        }

        query rseq(_db, "REPLACE INTO %s (folder, seq) VALUES ('%s', %d);",
                   TABLE, _name.c_str(), seq);

        if (rseq.successful() == false) {
            fprintf(stderr, "Unable to seek\n");
            abort();
        }
    }
}
