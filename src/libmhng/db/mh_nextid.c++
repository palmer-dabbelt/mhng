
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

#include "mh_nextid.h++"
using namespace mhng;

static sqlite::table_ptr generate_columns(void);

db::mh_nextid::mh_nextid(const mailbox_ptr& mbox)
    : _table(generate_columns()),
      _mbox(mbox)
{
}

uint64_t db::mh_nextid::select(void)
{
    auto resp = _mbox->db()->select(_table);

    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
            break;
    }

    if (resp->result_count() != 1)
        return -1;

    auto row = resp->row(0);
    return row->get_uint("uid");
}

void db::mh_nextid::update(uint64_t id)
{
    auto map = std::map<std::string, std::string>();
    map["uid"] = std::to_string(id);
    auto row = std::make_shared<sqlite::row>(map);

    auto resp = _mbox->db()->replace(_table, row, "single=1");

    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
        return;
    }
}

sqlite::table_ptr generate_columns(void)
{
    std::vector<sqlite::column_ptr> out;
    out.push_back(std::make_shared<sqlite::column_t<std::string>>("uid"));
    return std::make_shared<sqlite::table>("MH__nextid", out);
}
