
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
using namespace mhng;

static sqlite::table_ptr generate_columns(void);

db::mh_messages::mh_messages(const sqlite::connection_ptr& db)
    : _db(db),
      _table(generate_columns())
{
}

message_ptr db::mh_messages::select(const std::string& folder_name,
                                    const sequence_number_ptr& seq)
{
    auto resp = _db->select(_table, "WHERE folder='%s' AND seq='%d';",
                            folder_name.c_str(), seq->to_int());

    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
        return NULL;
        break;
    }

    return NULL;
}

sqlite::table_ptr generate_columns(void)
{
    std::vector<sqlite::column_ptr> out;
    out.push_back(std::make_shared<sqlite::column_t<int>>("seq"));
    return std::make_shared<sqlite::table>("MH__messages", out);
}
