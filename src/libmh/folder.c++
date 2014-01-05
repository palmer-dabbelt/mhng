
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

using namespace mh;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
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
