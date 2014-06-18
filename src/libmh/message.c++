
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
#include "message_file.h++"
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

message message::link(uid id, options_ptr o, db::connection_ptr db)
{
    /* FIMXE: What else needs to be done here? */
    return message(id, o, db);
}

message message::insert(folder folder,
                        options_ptr o,
                        db::connection_ptr db,
                        temp_file &infile)
{
    /* This (at least attempts to) prevent anyone else from writing to
     * the file that was passed in. */
    infile.finish();

    /* Parses the message from the raw file on disk. */
    message_file mf(infile.path());

    /* Peeks inside the parsed file to find some information. */
    const std::string subject = mf.header("subject");
    const mh::date date = mf.header_date("date");
    const std::string from = mf.header_address("from");
    const std::string to = mf.header_address("to");

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

    uint64_t seq = 1;
    for (auto it = sns.results(); !it.done(); ++it) {
        uint64_t nseq = atol((*it).get("seq").c_str());

        if (nseq >= seq)
            seq = nseq + 1;
    }

    /* Insert a new row, needed for the primary key. */
    bool unread = true;
    query insert(db, "INSERT into %s "
                 "(seq, folder, unread, subject, date, fadr, tadr)"
                 " VALUES (%lu, '%s', %d, '%s', %lu, '%s', '%s');",
                 TABLE, seq, folder.name().c_str(), unread,
                 subject.c_str(), date.unix(), from.c_str(), to.c_str());

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

message message::folder_search(const std::string folder_name,
                               options_ptr o,
                               db::connection_ptr db,
                               int seq)
{
    query uidq(db, "SELECT (uid) from %s WHERE folder='%s' AND seq=%d;",
               TABLE, folder_name.c_str(), seq);

    for (auto it = uidq.results(); !it.done(); ++it) {
        return message(uid((*it).get("uid")), o, db);
    }

    fprintf(stderr, "Unable to find matching SEQ: %s/%d\n",
            folder_name.c_str(), seq);
    abort();
    return message(uid(""), o, db);
}

message_file message::read(void)
{
    return message_file(this->on_disk_path());
}

bool message::cur(void) const
{
    query fnameq(_db, "SELECT (folder) from %s WHERE uid='%s';",
                 TABLE, _id.string().c_str());

    for (auto nit = fnameq.results(); !nit.done(); ++nit) {
        auto folder = (*nit).get("folder");

        query fseqq(_db, "SELECT (seq) from MH__current where folder='%s';",
                    folder.c_str());

        for (auto sit = fseqq.results(); !sit.done(); ++sit) {
            int s = atoi((*sit).get("seq").c_str());
            return s == seq();
        }
    }

    return 1 == seq();
}

int message::seq(void) const
{
    query select(_db, "SELECT (seq) from %s WHERE uid='%s';",
                 TABLE, _id.string().c_str());
    for (auto it = select.results(); !it.done(); ++it) {
        return atoi((*it).get("seq").c_str());
    }

    return -1;
}

const std::string message::from(void) const
{
    query select(_db, "SELECT (fadr) from %s WHERE uid='%s';",
                 TABLE, _id.string().c_str());
    for (auto it = select.results(); !it.done(); ++it) {
        return (*it).get("fadr");
    }

    return "";
}

const std::string message::to(void) const
{
    query select(_db, "SELECT (tadr) from %s WHERE uid='%s';",
                 TABLE, _id.string().c_str());
    for (auto it = select.results(); !it.done(); ++it) {
        return (*it).get("tadr");
    }

    return "";
}

const std::string message::subject(void) const
{
    query select(_db, "SELECT (subject) from %s WHERE uid='%s';",
                 TABLE, _id.string().c_str());
    for (auto it = select.results(); !it.done(); ++it) {
        return (*it).get("subject");
    }

    return "";
}

const typename mh::date message::date(void) const
{
    query select(_db, "SELECT (date) from %s WHERE uid='%s';",
                 TABLE, _id.string().c_str());
    for (auto it = select.results(); !it.done(); ++it) {
        return mh::date((*it).get("date"));
    }

    return mh::date("");
}

bool message::exists(void) const
{
    query select(_db, "SELECT (uid) from %s WHERE uid='%s';",
                 TABLE, _id.string().c_str());
    for (auto it = select.results(); !it.done(); ++it) {
        return true;
    }

    return false;
}

const std::string message::on_disk_path(void) const
{
    query qf(_db, "SELECT (folder) from %s WHERE uid=%s;",
             TABLE, _id.string().c_str());

    for (auto it = qf.results(); !it.done(); ++it) {
        const std::string fn = (*it).get("folder");

        char target_fn[BUFFER_SIZE];
        snprintf(target_fn, BUFFER_SIZE, "%s/mail/%s/%s",
                 _o->mhdir().c_str(), fn.c_str(), _id.string().c_str());

        return target_fn;
    }

    fprintf(stderr, "No UID found for message '%s'\n", id().string().c_str());
    abort();
}

message::message(uid id, const options_ptr o, db::connection_ptr db)
    : _id(id),
      _o(o),
      _db(db)
{
}
