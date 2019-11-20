/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "command.h++"
#include "logger.h++"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

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

bool command::is_list(const char *line)
{
    if (strncmp(line, "* LIST ", strlen("* LIST ")) == 0)
        return true;

    return false;
}

/* FIXME: God, this is awful...  I'm not really sure how I should go
 * about parsing IMAP results as it appears that they've all got
 * different formats. */
char *command::list_get_folder(char *line)
{
    if (!is_list(line))
        return NULL;

    line += strlen("* LIST");

    while (*line != ')' && *line != '\0')
        line++;

    if (*line == '\0')
        return NULL;
    line++;

    while (*line == ' ' && *line != '\0')
        line++;

    if (*line == '\0')
        return NULL;

    if (*line != '"')
        return NULL;
    line++;

    while (*line != '"' && *line != '\0')
        line++;

    if (*line == '\0')
        return NULL;
    line++;

    if (*line != ' ')
        return NULL;
    line++;

    if (*line != '"')
        return NULL;
    line++;

    char *end = line;
    while (*end != '"' && *end != '\0')
        end++;

    *end = '\0';

    return line;
}

bool command::list_is_noselect(const char *line)
{
    if (!is_list(line))
        return false;

    line += strlen("* LIST");

    while (*line != '(' && *line != '\0')
        line++;
    if (*line == '\0')
        abort();
    line++;

    while (*line != ')' && *line != '\0') {
        while (*line == ' ')
            line++;

        if (*line == ')' || *line == '\0')
            return false;

        if (strncmp(line, "\\Noselect", strlen("\\Noselect")) == 0)
            return true;

        while (*line != ' ' && *line != '\0')
            line++;
    }

    return false;
}

command::command(const std::string buffer, uint32_t seq, client *cl)
    : c(cl),
      sequence(seq)
{
    c->puts(buffer.c_str());
}
