
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

#include "message.h++"
#include "db/create_table.h++"
#include "db/query.h++"

using namespace mh;
using namespace mh::db;

#ifndef TABLE
#define TABLE "MH__messages"
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

message message::insert(folder folder,
                        options_ptr o,
                        db::connection_ptr db,
                        temp_file &infile)
{
    uint64_t seq;
    bool unread = true;
    std::string subject;
    uint64_t date = 0;
    std::string from;
    std::string to;

    /* Read the temporary file passed in to fill out the variables
     * required by . */
    {
        infile.finish();
        FILE *f = fopen(infile.path().c_str(), "r");

        

        fclose(f);
    }

    /* We need to start a transaction here because the SELECT we use
     * to determine the new sequence number needs to be atomic WRT the
     * INSERT actually add the row in. */
    db->trans_up();

    /* Checks that the required table exists, if not it will be
     * created here. */
    if (db->table_exists(TABLE) == false) {
        fprintf(stderr, "Table '%s' doesn't exist\n", TABLE);

        create_table create(db, TABLE,
                            table_col("uid", col_type::INTEGER_PK),
                            table_col("seq", col_type::INTEGER, true),
                            table_col("folder", col_type::STRING, true),
                            table_col("unread", col_type::INTEGER),
                            table_col("subject", col_type::STRING),
                            table_col("date", col_type::INTEGER),
                            table_col("fadr", col_type::STRING),
                            table_col("tadr", col_type::STRING)
            );
            
        if (create.successful() == false) {
            fprintf(stderr, "Error creating table '%s'\n", TABLE);
            fprintf(stderr, "  %d: '%s'\n",
                    create.error_code(), create.error_string());
            abort();
        }
    }

    /* Figures out the sequence number for this message, which is
     * really just the next message. */
    query sns(db, "SELECT (seq) FROM %s WHERE folder='%s';",
              TABLE, folder.name().c_str());

    seq = 1;
    for (auto it = sns.results(); !it.done(); ++it) {
        uint64_t nseq = atol((*it).get("seq").c_str());

        if (nseq >= seq)
            seq = nseq + 1;
    }

    /* Insert a new row, needed for the primary key. */
    query insert(db, "INSERT into %s "
                 "(seq, folder, unread, subject, date, fadr, tadr)"
                 " VALUES (%lu, '%s', %d, '%s', %lu, '%s', '%s');",
                 TABLE, seq, folder.name().c_str(), unread,
                 subject.c_str(), date, from.c_str(), to.c_str());

    /* I believe this can't fail because everything is wrapped in a
     * transaction. */
    if (insert.successful() == false) {
        fprintf(stderr, "Unexpected INSERT failure\n");
        fprintf(stderr, "  %d: %s\n", insert.error_code(),
                insert.error_string());
        abort();
    }

    /* Now that we've actually inserted the row we can fetch the MH
     * UID, which will end up being the filename of the message.  We
     * need to do this by first figureing out the last INSERTed ROW ID
     * and then fetching it's primary key -- I think this is the
     * safest way to do this (the Internet suggests
     * list_insert_rowid() is the primary key, but I don't trust that
     * at all!).  */
    query uidq(db, "SELECT (uid) FROM %s WHERE _ROWID_=%lu;",
               TABLE, db->last_insert_rowid());

    for (auto it = uidq.results(); !it.done(); ++it) {
        uid uid((*it).get("uid"));

        char target_fn[BUFFER_SIZE];
        snprintf(target_fn, BUFFER_SIZE, "%s/%s",
                 folder.full_path().c_str(), uid.string().c_str());

        rename(infile.path().c_str(), target_fn);

        db->trans_down();
        return message(uid, o, db);
    }

    /* Making it here means that we didn't get back a UID response,
     * which shouldn't be possible... */
    db->trans_down();
    abort();
    return message(uid(""), o, db);
}

message::message(uid id, const options_ptr o, db::connection_ptr db)
    : _id(id),
      _o(o),
      _db(db)
{
}
