
/*
 * Copyright (C) 2014-2017 Palmer Dabbelt
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
#include <stdio.h>
#include <string.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

static bool strsta(const std::string haystack, const std::string needle);

mailrc::mailrc(const std::string& path)
    : _mail_map(),
      _alias_map(),
      _name_map()
{
    FILE *file = fopen(path.c_str(), "r");
    if (file == NULL) {
        perror("Unable to open mailrc");
        fprintf(stderr, "Attempted path: '%s'\n", path.c_str());
        return;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, file) != NULL) {
        while (strlen(line) > 0 && isspace(line[strlen(line)-1]))
            line[strlen(line)-1] = '\0';

        if (line[0] == '#')
            continue;
        if (strlen(line) == 0)
            continue;

        if (strsta(line, "local ") == true) {
            auto addr = address::parse_rfc(line + strlen("local "), true);
            add(addr);
            continue;
        }

        if (strsta(line, "address ") == true) {
            auto addr = address::parse_rfc(line + strlen("address "), false);
            if (email(addr->email())->local() == true)
                addr = address::parse_rfc(addr->email(), true);
            add(addr);
            continue;
        }

        if (strsta(line, "alias ") == true) {
            auto alias = line + strlen("alias ");
            if (strstr(alias, " ") == NULL) {
                fprintf(stderr, "Unable to parse mailrc line: '%s'\n", line);
                fprintf(stderr, "  Bad alias definition\n");
                abort();
            }

            auto address = strstr(alias, " ");
            address[0] = '\0';
            address++;

            auto addr = address::parse_alias(address, alias, false);
            if (email(addr->email())->local() == true)
                addr = address::parse_alias(address, alias, true);
            add(addr);
            continue;
        }

        if (strsta(line, "bcc ") == true) {
            auto lline = line + strlen("bcc ");
            while (isspace(*lline)) lline++;
            auto addr = address::parse_rfc(lline, false);
            if (email(addr->email())->local() == true)
                addr = address::parse_rfc(addr->email(), true);
            add_bcc(addr);
            continue;
        }

        fprintf(stderr, "Unable to parse mailrc line: '%s'\n", line);
        abort();
    }

    fclose(file);
}

address_ptr mailrc::email(const std::string& email)
{
    auto l = _mail_map.find(email);
    if (l == _mail_map.end())
        return address::from_email(email, false);

    return l->second;
}

address_ptr mailrc::emailias(const std::string& in)
{
    auto l = _alias_map.find(in);
    if (l != _alias_map.end())
        return l->second;
    return email(in);
}

bool strsta(const std::string haystack, const std::string needle)
{
    return strncmp(haystack.c_str(),
                   needle.c_str(),
                   strlen(needle.c_str())) == 0;
}

void mailrc::add(const address_ptr& addr)
{
    if (addr->email_known())
        _mail_map[addr->email()] = addr;

    if (addr->alias_known())
        _alias_map[addr->alias()] = addr;

    if (addr->name_known())
        _name_map[addr->name()] = addr;
}

void mailrc::add_bcc(const address_ptr& addr)
{
    _bcc.push_back(addr);
}
