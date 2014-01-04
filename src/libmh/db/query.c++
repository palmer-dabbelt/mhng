
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

#include "query.h++"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

using namespace mh;
using namespace mh::db;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* Here's the helper that grabs all the exec results and converts them
 * into an array of result objects. */
struct exec_args {
    std::vector<result> *results;
};
static int exec_wrapper(void *auc, int cnt, char **cols, char **names);

query::query(connection_ptr db, const char *format, ...)
    : _db(db),
      _err_string(NULL)
{
    char buffer[BUFFER_SIZE];

    char sformat[BUFFER_SIZE];
    strncpy(sformat, format, BUFFER_SIZE);
    for (size_t i = 0; i < strlen(sformat); i++)
        if (strncmp(sformat + i, "%s", 2) == 0)
            sformat[i+1] = 'q';

    va_list args;
    va_start(args, format);
    sqlite3_vsnprintf(BUFFER_SIZE, buffer, sformat, args);
    va_end(args);

    run(buffer);
}

query::query(connection_ptr db)
    : _db(db),
      _err_string(NULL)
{
}

void query::run(const std::string sql)
{
    struct exec_args a;

#ifdef SQLITE_DEBUG
    fprintf(stderr, "SQLITE: '%s'\n", sql.c_str());
#endif

    a.results = &_results;
    _err = sqlite3_exec(_db->_s, sql.c_str(), &exec_wrapper, &a, &_err_string);
}

int exec_wrapper(void *auc, int cnt, char **cols, char **names)
{
    struct exec_args *a = (struct exec_args *)auc;

    a->results->push_back(result(cnt, cols, names));
    return 0;
}
