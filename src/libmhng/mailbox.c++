
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
using namespace mhng;

mailbox::mailbox(const std::string& path)
    : _db(std::make_shared<sqlite::connection>(path + "/metadata.sqlite")),
      _current_folder(this, _current_folder_func)
{
    
}

folder_ptr mailbox::open_folder(std::string folder_name) const
{
    auto mh_messages = db::mh_messages();

    fprintf(stderr, "UNIMPLEMENTED mailbox::open_folder('%s')\n",
            folder_name.c_str()
        );
    abort();
    return NULL;
}

folder_ptr mailbox::_current_folder_impl(void)
{
    /* FIXME: This needs to actually be implemented. */
    return open_folder("inbox");
}
