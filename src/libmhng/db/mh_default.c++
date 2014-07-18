
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

#include "mh_default.h++"
using namespace mhng;

static sqlite::table_ptr generate_columns(void);

db::mh_default::mh_default(const mailbox_ptr& mbox)
    : _table(generate_columns()),
      _mbox(mbox)
{
}

folder_ptr db::mh_default::select(void)
{
    auto resp = _mbox->db()->select(_table, "folder != ''");

    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
            break;
    }

    if (resp->result_count() != 1)
        return NULL;

    auto row = resp->row(0);
    auto folder_name = row->get_str("folder");
    return std::make_shared<folder>(_mbox, folder_name);
}

void db::mh_default::replace(const folder_ptr& folder)
{
    auto map = std::map<std::string, std::string>();
    map["folder"] = folder->name();
    auto row = std::make_shared<sqlite::row>(map);

    auto resp = _mbox->db()->replace(_table, row, "folder != ''");

    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
        return;
    }
}

sqlite::table_ptr generate_columns(void)
{
    std::vector<sqlite::column_ptr> out;
    out.push_back(std::make_shared<sqlite::column_t<std::string>>("folder"));
    return std::make_shared<sqlite::table>("MH__default", out);
}
