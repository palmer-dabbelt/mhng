
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
#include <map>
#include <stdarg.h>
#include <string.h>
using namespace mhng;

/* SQLite uses a callback for enumerating results, but unfortunately
 * this requires that you use C function pointers instead of C++
 * std::function.  Thus I have to do the whole struct/wrapper thing
 * here... :(. */
struct sqlite3_exec_args {
    sqlite::result_ptr result;
};
static int sqlite3_exec_func(void *args,
                             int count,
                             char **data,
                             char **names);

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

sqlite::result_ptr sqlite::connection::select(const table_ptr& table,
                                              const char * format, ...)
{
    va_list args; va_start(args, format);
    auto out = select(table, format, args);
    va_end(args);
    return out;
}

sqlite::result_ptr sqlite::connection::select(const table_ptr& table,
                                              const char * format,
                                              va_list args)
{
    return select(table, table->columns(), format, args);
}

sqlite::result_ptr sqlite::connection::select(const table_ptr& table,
                                              const std::vector<column_ptr>& c,
                                              const char *format,
                                              va_list args)
{
    /* It turns out that SQLite provides a mechanism for eliminating
     * SQL injection attacks, but it conflicts with GCC's printf-like
     * format string checker.  Thus I work around the problem by
     * simply converting everything to injection-proof right here. */
    char *nformat = new char[strlen(format) + 1];
    strcpy(nformat, format);
    for (size_t i = 0; i < strlen(nformat); ++i)
        if (strncmp(nformat + i, "%s",  2) == 0)
            nformat[i+1] = 'q';

    /* Here we format the string to avoid injection attacks. */
    size_t query_length = strlen(nformat) * 10 + 1;
    char *query = new char[query_length];
    sqlite3_vsnprintf(query_length, query, nformat, args);

    /* That's not the whole SQL command, we also need the "SELECT (...)
     * FROM ..." part.  */
    size_t column_length = 2;
    for (const auto& column: c)
        column_length += strlen(column->name().c_str()) + 2;
    char *column_spec = new char[column_length];
    strcpy(column_spec, "(");
    for (const auto& column: c) {
        strcat(column_spec, column->name().c_str());
        strcat(column_spec, ", ");
    }
    strcat(column_spec, ")");

    /* Now we can assemble the final SQL query string. */
    size_t command_length =
        strlen("SELECT ")
        + strlen(column_spec)
        + strlen(" FROM ")
        + strlen(table->name().c_str())
        + strlen(" WHERE ")
        + strlen(query)
        + 2;
    char *command = new char[command_length];
    sprintf(command, "SELECT %s FROM %s WHERE %s;",
            column_spec,
            table->name().c_str(),
            query);

    /* SQLite fills out an argument pointer, so we need one
     * created. */
    auto out = std::make_shared<result>();

    /* At this point the SQL query can actually be run. */
    {
        struct sqlite3_exec_args args;
        char *error_string = NULL;
        args.result = out;
        int error = sqlite3_exec(_db,
                                 command,
                                 &sqlite3_exec_func,
                                 &args,
                                 &error_string);
        out->set_error(error, error_string);
    }

    /* Finally we can clean up those allocated strings and return! */
    delete[] nformat;
    delete[] query;
    delete[] column_spec;
    delete[] command;
    return out;
}

int sqlite3_exec_func(void *args_uncast,
                      int count,
                      char **data,
                      char **names)
{
    std::map<std::string, std::string> name2datum;
    for (int i = 0; i < count; ++i) {
        auto name = names[i];
        auto datum = data[i];
        name2datum[name] = datum;
    }

    struct sqlite3_exec_args *args = (struct sqlite3_exec_args *)args_uncast;
    args->result->add_map(name2datum);

    return 0;
}
