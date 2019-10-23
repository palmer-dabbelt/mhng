
/*
 * Copyright (C) 2019 Palmer Dabbelt
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

#include "db/mh_messages.h++"
#include "fake_message.h++"
using namespace mhng;

/* This is exactly the same code as message::next_message, which further
 * indicates I have some sort of class heirarchy issue.  I don't care enough to
 * fix it. */
message_ptr fake_message::next_message(int offset)
{
    auto table = std::make_shared<db::mh_messages>(_mbox);
    return table->select(_folder->name(), _seq, offset);
}