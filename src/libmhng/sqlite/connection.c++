
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

#include "connection.h++"
using namespace mhng;

sqlite::connection::connection(const std::string& db_path)
    : _db(NULL)
{
    int err = sqlite3_open(db_path.c_str(), &_db);
    if (err != SQLITE_OK) {
        perror("Unable t open sqlite database");
        fprintf(stderr, "  database path:'%s'\n", db_path.c_str());
        fprintf(stderr, "  error: %d\n", err);
        abort();
    }
}
