
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

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

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
    : _db(NULL),
      _tr(std::shared_ptr<transaction>(NULL))
{
    int err = sqlite3_open(db_path.c_str(), &_db);
    if (err != SQLITE_OK) {
        perror("Unable t open sqlite database");
        fprintf(stderr, "  database path:'%s'\n", db_path.c_str());
        fprintf(stderr, "  error: %d\n", err);
        abort();
    }

    sqlite3_busy_timeout(_db, 1000);
}

sqlite::result_ptr sqlite::connection::select(const table_ptr& table)
{
    return select(table, "'true'='true'");
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
    column_spec[0] = '\0';
    for (const auto& column: c) {
        strcat(column_spec, column->name().c_str());
        strcat(column_spec, ", ");
    }
    column_spec[strlen(column_spec)-2] = '\0';

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
#ifdef DEBUG_SQLITE_COMMANDS
    fprintf(stderr, "command: '%s'\n", command);
#endif

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
        if (error_string == NULL)
            error_string = (char *)"";
        out->set_error(error, error_string);
    }

    /* Finally we can clean up those allocated strings and return! */
    delete[] nformat;
    delete[] query;
    delete[] column_spec;
    delete[] command;
    return out;
}

sqlite::result_ptr sqlite::connection::insert(const table_ptr& table,
                                              const row_ptr& row)
{
    std::string command = "INSERT INTO " + table->name() + " (";
    for (const auto& column: row->columns()) {
        char format[BUFFER_SIZE];
        sqlite3_snprintf(BUFFER_SIZE, format, "%s",
                         column.c_str());
        command = command + format + ", ";
    }
    command.replace(strlen(command.c_str())-2, 1, "\0");
    command = command + ") VALUES (";
    for (const auto& column: row->columns()) {
        char format[BUFFER_SIZE];
        sqlite3_snprintf(BUFFER_SIZE, format, "'%q'",
                         row->value(column).c_str());
        command = command + format + ", ";
    }
    command.replace(strlen(command.c_str())-2, 1, "\0");
    command = command + ")";

#ifdef DEBUG_SQLITE_COMMANDS
    fprintf(stderr, "command: '%s'\n", command.c_str());
#endif

    /* SQLite fills out an argument pointer, so we need one
     * created. */
    auto out = std::make_shared<result>();

    /* At this point the SQL query can actually be run. */
    {
        struct sqlite3_exec_args args;
        char *error_string = NULL;
        args.result = out;
        int error = sqlite3_exec(_db,
                                 command.c_str(),
                                 &sqlite3_exec_func,
                                 &args,
                                 &error_string);
        if (error_string == NULL)
            error_string = (char *)"";
        out->set_error(error, error_string);
    }

    /* Finally we can clean up those allocated strings and return! */
    return out;
}

sqlite::result_ptr sqlite::connection::replace(const table_ptr& table,
                                               const row_ptr& row,
                                               const char *format,
                                               ...)
{
    va_list args; va_start(args, format);
    auto out = replace(table, row, format, args);
    va_end(args);
    return out;
}

sqlite::result_ptr sqlite::connection::replace(const table_ptr& table,
                                               const row_ptr& row,
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
    size_t column_length = 3;
    for (const auto& column: row->columns()) {
        column_length += strlen(column.c_str());
        column_length += 2;
        column_length += strlen(row->value(column).c_str());
        column_length += 3;
    }
    char *column_spec = new char[column_length];
    column_spec[0] = '\0';
    for (const auto& column: row->columns()) {
        strcat(column_spec, column.c_str());
        strcat(column_spec, "='");
        strcat(column_spec, row->value(column).c_str() );
        strcat(column_spec, "', ");
    }
    column_spec[strlen(column_spec)-2] = '\0';

    /* Now we can assemble the final SQL query string. */
    size_t command_length =
        strlen("UPDATE ")
        + strlen(table->name().c_str())
        + strlen(" SET ")
        + strlen(column_spec)
        + strlen(" WHERE ")
        + strlen(query)
        + 2;
    char *command = new char[command_length];
    sprintf(command, "UPDATE %s SET %s WHERE %s;",
            table->name().c_str(),
            column_spec,
            query);
#ifdef DEBUG_SQLITE_COMMANDS
    fprintf(stderr, "command: '%s'\n", command);
#endif

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
        if (error_string == NULL)
            error_string = (char *)"";
        out->set_error(error, error_string);
    }

    /* Finally we can clean up those allocated strings and return! */
    delete[] nformat;
    delete[] query;
    delete[] column_spec;
    delete[] command;
    return out;
}

sqlite::result_ptr sqlite::connection::remove(const table_ptr& table,
                                              const char *format,
                                              ...)
{
    va_list args; va_start(args, format);
    auto out = remove(table, format, args);
    va_end(args);
    return out;
}

sqlite::result_ptr sqlite::connection::remove(const table_ptr& table,
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

    /* Now we can assemble the final SQL query string. */
    size_t command_length =
        strlen("DELETE FROM ")
        + strlen(table->name().c_str())
        + strlen(" WHERE ")
        + strlen(query)
        + 2;
    char *command = new char[command_length];
    sprintf(command, "DELETE FROM %s WHERE %s;",
            table->name().c_str(),
            query);
#ifdef DEBUG_SQLITE_COMMANDS
    fprintf(stderr, "command: '%s'\n", command);
#endif

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
        if (error_string == NULL)
            error_string = (char *)"";
        out->set_error(error, error_string);
    }

    /* Finally we can clean up those allocated strings and return! */
    delete[] nformat;
    delete[] query;
    delete[] command;
    return out;
}

sqlite::result_ptr sqlite::connection::clear(const table_ptr& table,
                                             const std::vector<std::string>& cols,
                                             const char *format,
                                             ...)
{
    va_list args; va_start(args, format);
    auto out = clear(table, cols, format, args);
    va_end(args);
    return out;
}

sqlite::result_ptr sqlite::connection::clear(const table_ptr& table,
                                             const std::vector<std::string>& cols,
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
    size_t column_length = 3;
    for (const auto& column: cols) {
        column_length += strlen(column.c_str());
        column_length += 3;
        column_length += strlen("NULL");
        column_length += 1;
    }
    char *column_spec = new char[column_length];
    column_spec[0] = '\0';
    for (const auto& column: cols) {
        strcat(column_spec, column.c_str());
        strcat(column_spec, "=");
        strcat(column_spec, "NULL");
        strcat(column_spec, ", ");
    }
    column_spec[strlen(column_spec)-2] = '\0';

    /* Now we can assemble the final SQL query string. */
    size_t command_length =
        strlen("UPDATE ")
        + strlen(table->name().c_str())
        + strlen(" SET ")
        + strlen(column_spec)
        + strlen(" WHERE ")
        + strlen(query)
        + 2;
    char *command = new char[command_length];
    sprintf(command, "UPDATE %s SET %s WHERE %s;",
            table->name().c_str(),
            column_spec,
            query);
#ifdef DEBUG_SQLITE_COMMANDS
    fprintf(stderr, "command: '%s'\n", command);
#endif

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
        if (error_string == NULL)
            error_string = (char *)"";
        out->set_error(error, error_string);
    }

    /* Finally we can clean up those allocated strings and return! */
    delete[] nformat;
    delete[] query;
    delete[] column_spec;
    delete[] command;
    return out;
}

sqlite::exclusive_transaction_ptr
sqlite::connection::exclusive_transaction(void)
{
    /* Check to see if there's already an existing transaction. */
    auto tr = _tr.lock();
    if (tr != NULL) {
        auto cast = std::dynamic_pointer_cast<sqlite::exclusive_transaction>(tr);
        if (cast == NULL) {
            fprintf(stderr, "Attempted to grab an exclusive transaction\n");
            fprintf(stderr, "  Weaker transaction already in use\n");
            abort();
        }

        return cast;
    }

#ifdef DEBUG_SQLITE_COMMANDS
    fprintf(stderr, "command: '%s'\n", "BEGIN EXCLUSIVE TRANSACTION;");
#endif

    /* At this point the SQL query can actually be run. */
    {
        auto out = std::make_shared<result>();
        struct sqlite3_exec_args args;
        char *error_string = NULL;
        args.result = out;
        int error = sqlite3_exec(_db,
                                 "BEGIN EXCLUSIVE TRANSACTION;",
                                 &sqlite3_exec_func,
                                 &args,
                                 &error_string);
        if (error_string == NULL)
            error_string = (char *)"";
        out->set_error(error, error_string);

        switch (out->return_value()) {
        case sqlite::error_code::SUCCESS:
            break;
        case sqlite::error_code::LOCKED:
            return exclusive_transaction();
            break;
        }
    }

    auto ntr = std::make_shared<sqlite::exclusive_transaction>(this);
    _tr = ntr;
    return ntr;
}

sqlite::immediate_transaction_ptr
sqlite::connection::immediate_transaction(void)
{
    /* Check to see if there's already an existing transaction. */
    auto tr = _tr.lock();
    if (tr != NULL) {
        auto cast = std::dynamic_pointer_cast<sqlite::immediate_transaction>(tr);
        if (cast == NULL) {
            fprintf(stderr, "Attempted to grab an immediate transaction\n");
            fprintf(stderr, "  Weaker transaction already in use\n");
            abort();
        }

        return cast;
    }

#ifdef DEBUG_SQLITE_COMMANDS
    fprintf(stderr, "command: '%s'\n", "BEGIN IMMEDIATE TRANSACTION;");
#endif

    /* At this point the SQL query can actually be run. */
    {
        auto out = std::make_shared<result>();
        struct sqlite3_exec_args args;
        char *error_string = NULL;
        args.result = out;
        int error = sqlite3_exec(_db,
                                 "BEGIN IMMEDIATE TRANSACTION;",
                                 &sqlite3_exec_func,
                                 &args,
                                 &error_string);
        if (error_string == NULL)
            error_string = (char *)"";
        out->set_error(error, error_string);

        switch (out->return_value()) {
        case sqlite::error_code::SUCCESS:
            break;
        case sqlite::error_code::LOCKED:
            return immediate_transaction();
            break;
        }
    }

    auto ntr = std::make_shared<sqlite::immediate_transaction>(this);
    _tr = ntr;
    return ntr;
}

sqlite::deferred_transaction_ptr
sqlite::connection::deferred_transaction(void)
{
    /* Check to see if there's already an existing transaction. */
    auto tr = _tr.lock();
    if (tr != NULL) {
        auto cast = std::dynamic_pointer_cast<sqlite::deferred_transaction>(tr);
        if (cast == NULL) {
            fprintf(stderr, "Attempted to grab an deferred transaction\n");
            fprintf(stderr, "  Weaker transaction already in use\n");
            abort();
        }

        return cast;
    }

#ifdef DEBUG_SQLITE_COMMANDS
    fprintf(stderr, "command: '%s'\n", "BEGIN DEFERRED TRANSACTION;");
#endif

    /* At this point the SQL query can actually be run. */
    {
        auto out = std::make_shared<result>();
        struct sqlite3_exec_args args;
        char *error_string = NULL;
        args.result = out;
        int error = sqlite3_exec(_db,
                                 "BEGIN DEFERRED TRANSACTION;",
                                 &sqlite3_exec_func,
                                 &args,
                                 &error_string);
        if (error_string == NULL)
            error_string = (char *)"";
        out->set_error(error, error_string);

        switch (out->return_value()) {
        case sqlite::error_code::SUCCESS:
            break;
        case sqlite::error_code::LOCKED:
            return deferred_transaction();
            break;
        }
    }

    auto ntr = std::make_shared<sqlite::deferred_transaction>(this);
    _tr = ntr;
    return ntr;
}

sqlite::result_ptr sqlite::connection::commit_transaction(void)
{
#ifdef DEBUG_SQLITE_COMMANDS
    fprintf(stderr, "command: '%s'\n", "END TRANSACTION;");
#endif

    auto out = std::make_shared<result>();
    struct sqlite3_exec_args args;
    char *error_string = NULL;
    args.result = out;
    int error = sqlite3_exec(_db,
                             "END TRANSACTION;",
                             &sqlite3_exec_func,
                             &args,
                             &error_string);
    if (error_string == NULL)
        error_string = (char *)"";
    out->set_error(error, error_string);

    return out;
}

int sqlite3_exec_func(void *args_uncast,
                      int count,
                      char **data,
                      char **names)
{
    std::map<std::string, std::string> name2datum;
    for (int i = 0; i < count; ++i) {
        if (data[i] != NULL) {
            std::string name = names[i];
            std::string datum = data[i];
            name2datum[name] = datum;
        }
    }

    struct sqlite3_exec_args *args = (struct sqlite3_exec_args *)args_uncast;
    args->result->add_map(name2datum);

    return 0;
}
