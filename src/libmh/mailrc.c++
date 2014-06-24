
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

#include "mailrc.h++"
#include <string.h>
using namespace mh;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

static bool strsta(const std::string haystack, const std::string needle);
static char *skip_keyword(char *line);
static inline void parse_pair(char *line, std::string &name_o, std::string &addr_o);

mailrc::mailrc(const std::string path)
    : _local_name(),
      _local_mail(),
      _mail2long(),
      _mail2name()
{
    FILE *f = fopen(path.c_str(), "r");
    if (f == NULL) {
        fprintf(stderr, "Unable to open mailrc '%s'\n", path.c_str());
        abort();
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, f) != NULL) {
        while (isspace(buffer[strlen(buffer) - 1]))
            buffer[strlen(buffer) - 1] = '\0';

        /* Blank lines and comments don't do anything at all. */
        if (strlen(buffer) == 0)
            continue;
        if (strsta(buffer, "#"))
            continue;

        if (strsta(buffer, "local ")) {
            auto l = skip_keyword(buffer);

            std::string name, addr;
            parse_pair(l, name, addr);

            _mail2long[addr] = l;
            _mail2name[addr] = name;
            _local_name[name] = true;
            _local_mail[addr] = true;

            continue;
        }

        if (strsta(buffer, "alias ")) {
            auto alias = skip_keyword(buffer);
            char alias_str[BUFFER_SIZE];
            sscanf(alias, "%s", alias_str);

            auto l = skip_keyword(alias);

            std::string name, addr;
            parse_pair(l, name, addr);

            _alias2mail[alias_str] = addr;

            _mail2long[addr] = l;
            _mail2name[addr] = name;

            continue;
        }

        if (strsta(buffer, "address ")) {
            auto l = skip_keyword(buffer);

            std::string name, addr;
            parse_pair(l, name, addr);

            _mail2long[addr] = l;
            _mail2name[addr] = name;

            continue;
        }

        fprintf(stderr, "Unable to parse line: '%s'\n", buffer);
        abort();
    }

    fclose(f);
}

const std::string mailrc::mail2long(const std::string mail) const
{
    auto l = _mail2long.find(mail);
    if (l != _mail2long.end())
        return l->second;

    return mail;
}

const std::string mailrc::mail2name(const std::string mail) const
{
    auto l = _mail2name.find(mail);
    if (l != _mail2name.end())
        return l->second;

    return mail;
}

const std::string mailrc::alias2mail(const std::string alias) const
{
    auto l = _alias2mail.find(alias);
    if (l != _alias2mail.end())
        return l->second;

    return alias;
}

const std::string mailrc::mailias2long(const std::string mailias) const
{
    return mail2long(alias2mail(mailias));
}

bool mailrc::local_p(const std::string mail) const
{
    auto l = _local_mail.find(mail);
    if (l == _local_mail.end())
        return false;

    return l->second;
}

bool strsta(const std::string haystack, const std::string needle)
{
    return (strncmp(haystack.c_str(), needle.c_str(), needle.length()) == 0);
}

char *skip_keyword(char *line)
{
    while (!(isspace(*line)))
        line++;

    while (isspace(*line))
        line++;

    return line;
}

void parse_pair(char *line, std::string &name_o, std::string &addr_o)
{
    auto split = strstr(line, "<");
    if (split == NULL) {
        fprintf(stderr, "Unable to find '<' in '%s'\n", line);
        abort();
    }

    char name[BUFFER_SIZE];
    snprintf(name, BUFFER_SIZE, "%.*s", (int)(split - line), line);
    while (isspace(name[strlen(name) - 1]))
        name[strlen(name) - 1] = '\0';
    name_o = name;

    char addr[BUFFER_SIZE];
    snprintf(addr, BUFFER_SIZE, "%.*s", (int)(strlen(split) - 2), split + 1);
    addr_o = addr;
}
