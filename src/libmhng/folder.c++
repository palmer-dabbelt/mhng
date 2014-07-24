
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
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

folder::folder(const mailbox_ptr& mbox,
               std::string name)
    : _mbox(mbox),
      _name(name),
      _current_message(this, _current_message_func),
      _messages(this, _messages_func)
{
}

message_ptr folder::open(const sequence_number_ptr& seq)
{
    auto messages = std::make_shared<db::mh_messages>(_mbox);
    return messages->select(this->name(), seq);
}

std::string folder::full_path(void) const
{
    char path[BUFFER_SIZE];
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
    auto out = std::make_shared<std::vector<message_ptr>>();
    auto table = std::make_shared<db::mh_messages>(_mbox);

    for (const auto& message: table->select(this->name()))
        out->push_back(message);

    return out;
}
