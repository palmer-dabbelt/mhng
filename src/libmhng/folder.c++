
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
#include "db/mh_messages.h++"
using namespace mhng;

folder::folder(const sqlite::connection_ptr& db,
               std::string name)
    : _db(db),
      _name(name),
      _current_message(this, _current_message_func),
      _messages(this, _messages_func)
{
}

message_ptr folder::open(const sequence_number_ptr& seq)
{
    auto messages = std::make_shared<db::mh_messages>(_db);
    return messages->select(this->name(), seq);
}

message_ptr folder::_current_message_impl(void)
{
    return NULL;
}

std::shared_ptr<std::vector<message_ptr>> folder::_messages_impl(void)
{
    auto out = std::make_shared<std::vector<message_ptr>>();
    auto table = std::make_shared<db::mh_messages>(_db);

    for (const auto& message: table->select(this->name()))
        out->push_back(message);

    return out;
}