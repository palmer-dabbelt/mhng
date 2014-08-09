
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

#include "mailbox.h++"
#include "mailrc.h++"
#include "db/mh_messages.h++"
#include "db/mh_current.h++"
#include "db/mh_nextid.h++"
#include "db/imap_messages.h++"
#include <string.h>
#include <unistd.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

mailbox::mailbox(const std::string& path)
    : _path(path),
      _db(std::make_shared<sqlite::connection>(path + "/metadata.sqlite3")),
      _current_folder(this, _current_folder_func),
      _mailrc(this, _mailrc_func)
{
}

folder_ptr mailbox::open_folder(std::string folder_name) const
{
    /* Check to see that this folder exists before trying to return
     * it. */
    auto folder_path = _path + "/mail/" + folder_name;
    if (access(folder_path.c_str(), X_OK) != 0)
        return NULL;

    /* Now that we know we have access to the folder it's time to
     * simply return a handle for that folder. */
    return std::make_shared<folder>(_self_ptr.lock(), folder_name);
}

void mailbox::set_current_folder(const folder_ptr& folder)
{
    /* Check to see if the current folder matches the folder we've
     * requented to move to and if so just don't worry about issuing
     * the update.  Note that this is racy, but who cares: if you want
     * this to actually work then you'll have to wrap everything in a
     * transaction anyway, so it's not like it matters.  This has the
     * huge advantage of not doing writes all the time, so I think
     * it's worth it! */
    if (current_folder()->name() == folder->name())
        return;

    auto tr = _db->deferred_transaction();

    auto table = std::make_shared<db::mh_current>(_self_ptr.lock());
    table->clear_current(current_folder()->name());
    table->set_current(folder->name());
    _current_folder = folder;
}

message_ptr mailbox::insert(const std::string &folder_name,
                            const mime::message_ptr& mime)
{
    /* This whole thing needs to be locked... */
    auto trans = _db->deferred_transaction();

    /* Find a new unique MH id for this message. */
    auto idtbl = std::make_shared<db::mh_nextid>(_self_ptr.lock());
    auto uid = idtbl->select();
    idtbl->update(uid + 1);

    /* Fin a new sequence number for this message. */
    auto folder = open_folder(folder_name);
    unsigned seq = 1;
    for (const auto& message: folder->messages())
        if (message->seq()->to_uint() >= seq)
            seq = message->seq()->to_uint() + 1;

    /* Determine some other relevant information for this message. */
    std::shared_ptr<date> date = NULL;
    std::shared_ptr<address> from = NULL;
    std::shared_ptr<address> to = NULL;
    std::string subject = "";

    /* Walk through the headers to try and fill out some relevant
     * fields. */
    for (const auto& header: mime->header("Date"))
        date = std::make_shared<mhng::date>(header->utf8());
    for (const auto& header: mime->header("From"))
        from = address::parse_rfc(header->utf8(), false);
    for (const auto& header: mime->header("To"))
        to = address::parse_rfc(header->utf8(), false);
    for (const auto& header: mime->header("Subject"))
        subject = header->utf8();

    if (from == NULL)
        from = address::parse_rfc("", false);
    if (to == NULL)
        to = address::parse_rfc("", false);

    from = mrc()->email(from->email());
    to = mrc()->email(to->email());

    auto msg = std::make_shared<message>(_self_ptr.lock(),
                                         std::make_shared<sequence_number>(seq),
                                         folder,
                                         date,
                                         from,
                                         to,
                                         subject,
                                         std::to_string(uid),
                                         0
        );

    auto messages = std::make_shared<db::mh_messages>(_self_ptr.lock());
    messages->insert(seq,
                     folder->name(),
                     std::to_string(date->unix()),
                     from->email(),
                     to->email(),
                     subject,
                     uid
        );

    char filename[BUFFER_SIZE];
    snprintf(filename, BUFFER_SIZE, "%s/%s",
             folder->full_path().c_str(),
             std::to_string(uid).c_str());
             
    FILE *file = fopen(filename, "w");
    for (const auto& raw: mime->root()->raw())
        fprintf(file, "%s", raw.c_str());

    return msg;
}

message_ptr mailbox::insert(const folder_ptr& folder,
                            const mime::message_ptr& mime,
                            uint32_t imapid)
{
    auto trans = _db->deferred_transaction();

    auto message = insert(folder->name(), mime);

    uint64_t uid = atoi(message->uid().c_str());

    auto table = std::make_shared<db::imap_messages>(_self_ptr.lock());
    table->insert(message->folder()->name(), imapid, uid);

    return folder->open(uid);
}

void mailbox::did_purge(const folder_ptr& folder, uint32_t imapid)
{
    auto table = std::make_shared<db::imap_messages>(_self_ptr.lock());
    table->remove(folder->name(), imapid);
}

folder_ptr mailbox::_current_folder_impl(void)
{
    auto table = std::make_shared<db::mh_current>(_self_ptr.lock());
    return open_folder(table->select_current());
}

mailrc_ptr mailbox::_mailrc_impl(void)
{
    char path[BUFFER_SIZE];
    snprintf(path, BUFFER_SIZE, "%s/.local/share/pim/mailrc",
             getenv("HOME"));
    return std::make_shared<typename mhng::mailrc>(path);
}
