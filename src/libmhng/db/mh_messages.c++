
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

#include "mh_messages.h++"
#include <vector>
using namespace mhng;

static sqlite::table_ptr _mh_messages;
static void _mh_messages_func(void) __attribute__((constructor));

const sqlite::table_ptr& db::mh_messages(void)
{
    return _mh_messages;
}

void _mh_messages_func(void)
{
    std::vector<sqlite::column_ptr> cols
        ({
            std::make_shared<sqlite::column_t<int>>("seq")
        });

    _mh_messages = std::make_shared<sqlite::table>(cols);
}
