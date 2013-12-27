
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

#include "command.h++"
#include "logger.h++"
#include <stdarg.h>
#include <string.h>

using namespace mhimap;

#ifndef LINE_SIZE
#define LINE_SIZE 1024
#endif

command::command(client *c, const char *fmt, ...)
    : c(c),
      sequence(c->next_sequence())
{
    logger l("command::command(...)");

    char buffer[LINE_SIZE];

    snprintf(buffer, LINE_SIZE, "%u ", sequence);

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer + strlen(buffer), LINE_SIZE - strlen(buffer), fmt, args);
    va_end(args);

    l.printf("sending '%s'", buffer);
    c->puts(buffer);
}

bool command::is_end(const char *line)
{
    logger l("command::is_end('%s')", line);
    
    if (strncmp(line, "* ", strlen("* ")) == 0) {
        l.printf("==> false");
        return false;
    }

    char buffer[LINE_SIZE];
    snprintf(buffer, LINE_SIZE, "%u ", sequence);
    if (strncmp(line, buffer, strlen(buffer)) == 0) {
        l.printf("==> true");
        return true;
    }

    /* We got some sort of response from the IMAP server that I don't
     * understand... just abort here! */
    fprintf(stderr, "Unknown IMAP response: '%s'\n", line);
    abort();
    return true;
}

bool command::is_error_end(const char *line)
{
    char buffer[LINE_SIZE];
    snprintf(buffer, LINE_SIZE, "%u OK ", sequence);
    if (strncmp(line, buffer, strlen(buffer)) == 0)
        return false;

    return true;
}
