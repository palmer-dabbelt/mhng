
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

#include "folder.h++"
#include "db/mh_current.h++"
#include "db/mh_messages.h++"
#include "db/imap_messages.h++"
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

folder::folder(const mailbox_ptr& mbox,
               std::string name)
    : _mbox(mbox),
      _name(name),
      _current_message(this, _current_message_func),
      _messages(this, _messages_func),
      _uid_validity(this, _uid_validity_func),
      _purge(this, _purge_func)
{
}

size_t folder::message_count(void)
{
    auto messages = std::make_shared<db::mh_messages>(_mbox);
    return messages->count(this->name());
}

message_ptr folder::open(uint64_t uid)
{
    auto messages = std::make_shared<db::mh_messages>(_mbox);
    return messages->select(uid);
}

message_ptr folder::open(const sequence_number_ptr& seq)
{
    auto messages = std::make_shared<db::mh_messages>(_mbox);
    return messages->select(this->name(), seq);
}

std::string folder::full_path(void) const
{
    char path[BUFFER_SIZE];

    if (getenv("MHNG_MAILDIR") != NULL) {
        snprintf(path, BUFFER_SIZE, "%s/mail/%s",
                 getenv("MHNG_MAILDIR"),
                 _name.c_str()
            );
        return path;
    }

    snprintf(path, BUFFER_SIZE, "%s/.mhng/mail/%s",
             getenv("HOME"),
             _name.c_str()
        );
    return path;
}

void folder::set_current_message(const message_ptr& message)
{
    auto cur = std::make_shared<db::mh_current>(_mbox);
    cur->update(message->folder()->name(), message->seq()->to_uint());
    _current_message = message;
}

message_ptr folder::open_imap(uint32_t imapid)
{
    auto imap = std::make_shared<db::imap_messages>(_mbox);
    auto uid = imap->select(this->name(), imapid);

    auto mh = std::make_shared<db::mh_messages>(_mbox);
    auto message = mh->select(uid);

    if (message != NULL)
        message->set_imapid(imapid);

    return message;
}

void folder::set_uid_validity(uint32_t uidv)
{
    auto table = std::make_shared<db::mh_current>(_mbox);
    table->update_uid_validity(this->_name, uidv);
    _uid_validity = std::make_shared<int64_t>(uidv);
}

message_ptr folder::_current_message_impl(void)
{
    auto cur = std::make_shared<db::mh_current>(_mbox);
    auto seq_uint = cur->select(_name);
    auto seq = std::make_shared<sequence_number>(seq_uint);

    auto msgs = std::make_shared<db::mh_messages>(_mbox);
    auto msg = msgs->select(_name, seq);

    return msg;
}

std::shared_ptr<std::vector<message_ptr>> folder::_messages_impl(void)
{
    auto tr = _mbox->db()->deferred_transaction();

    auto out = std::make_shared<std::vector<message_ptr>>();
    auto table = std::make_shared<db::mh_messages>(_mbox);

    for (const auto& message: table->select(this->name()))
        out->push_back(message);

    return out;
}

std::shared_ptr<int64_t> folder::_uid_validity_impl(void)
{
    auto table = std::make_shared<db::mh_current>(_mbox);
    auto uidv = table->uid_validity(this->_name);
    return std::make_shared<int64_t>(uidv);
}

std::shared_ptr<std::vector<uint32_t>> folder::_purge_impl(void)
{
    auto table = std::make_shared<db::imap_messages>(_mbox);
    auto out = std::make_shared<std::vector<uint32_t>>();
    for (const auto& uid: table->select_purge(_name))
        out->push_back(uid);
    return out;
}
