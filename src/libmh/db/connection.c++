
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

#include "connection.h++"
#include <sqlite3.h>

using namespace mh;
using namespace mh::db;

connection_ptr connection::create(const std::string path)
{
    return connection_ptr(new connection(path));
}

connection::~connection(void)
{
    /* sqlite3_close(NULL) is a no-op, according to the manual. */
    int err = sqlite3_close(_s);
    if (err != SQLITE_OK) {
        fprintf(stderr, "Unable to close sqlite3 connection\n");
        fprintf(stderr, "  error %d: '%s'\n", err, sqlite3_errstr(err));
        abort();
    }
}

connection::connection(const std::string path)
{
    /* Attempt to open a connection to SQLite, bailing out if it can't
     * be opened. */
    _s = NULL;
    {
        int err = sqlite3_open(path.c_str(), &_s);
        if (err != SQLITE_OK) {
            fprintf(stderr, "Unable to open sqlite3 '%s'\n", path.c_str());
            fprintf(stderr, "  error %d: '%s'\n", err, sqlite3_errstr(err));

            if (err == 14) {
                fprintf(stderr, "Maybe the directory doesn't exist?\n");
            }

            abort();
        }
    }
}
