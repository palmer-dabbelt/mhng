
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
#include "db/mh_messages.h++"
#include <unistd.h>
using namespace mhng;

mailbox::mailbox(const std::string& path)
    : _path(path),
      _db(std::make_shared<sqlite::connection>(path + "/metadata.sqlite")),
      _current_folder(this, _current_folder_func)
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
    return std::make_shared<folder>(_db, folder_name);
}

folder_ptr mailbox::_current_folder_impl(void)
{
    /* FIXME: This needs to actually be implemented. */
    return open_folder("inbox");
}
