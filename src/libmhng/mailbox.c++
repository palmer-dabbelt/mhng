
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
#include "db/mh_default.h++"
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
    auto table = std::make_shared<db::mh_default>(_self_ptr.lock());
    table->replace(folder);
    _current_folder = folder;
}

folder_ptr mailbox::_current_folder_impl(void)
{
    auto table = std::make_shared<db::mh_default>(_self_ptr.lock());
    return table->select();
}

mailrc_ptr mailbox::_mailrc_impl(void)
{
    char path[BUFFER_SIZE];
    snprintf(path, BUFFER_SIZE, "%s/.local/share/pim/mailrc",
             getenv("HOME"));
    return std::make_shared<typename mhng::mailrc>(path);
}
