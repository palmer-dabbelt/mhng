
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

#include "mh_current.h++"
using namespace mhng;

static sqlite::table_ptr generate_columns(void);

db::mh_current::mh_current(const sqlite::connection_ptr& db)
    : _db(db),
      _table(generate_columns())
{
}

unsigned db::mh_current::select(const std::string& folder)
{
    auto resp = _db->select(_table, "folder='%s'",
                            folder.c_str());

    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
            break;

    default:
        return -1;
        break;
    }

    if (resp->result_count() != 1)
        return -1;

    auto row = resp->row(0);
    return row->get_uint("seq");
}

sqlite::table_ptr generate_columns(void)
{
    std::vector<sqlite::column_ptr> out;
    out.push_back(std::make_shared<sqlite::column_t<std::string>>("folder"));
    out.push_back(std::make_shared<sqlite::column_t<std::string>>("seq"));
    return std::make_shared<sqlite::table>("MH__current", out);
}
