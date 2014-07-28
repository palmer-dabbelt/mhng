
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

#include "message.h++"
#include <string.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

mime::header::header(const std::string& first_line)
    : _raw({first_line})
{
}

std::string mime::header::single_line(void) const
{
    std::string out = "";

    for (size_t i = 0; i < _raw.size(); ++i) {
        /* Usually we just want to strip spaces from the raw line, but
         * if it's the first line then we also need to strip the key
         * string. */
        auto cont = _raw[i];
        if (i == 0) {
            char buffer[BUFFER_SIZE];
            snprintf(buffer, BUFFER_SIZE, "%s", cont.c_str());
            cont = strstr(buffer, ":") + 1;
        }

        /* Now actually strip the spaces from the front. */
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "%s", cont.c_str());

        char *bufp = buffer;
        while (isspace(bufp[0]))
            bufp++;

        if (strlen(out.c_str()) == 0)
            out = bufp;
        else
            out = out + " " + bufp;
    }

    return out;
}

std::string mime::header::key(void) const
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s", _raw[0].c_str());
    strstr(buffer, ":")[0] = '\0';
    return buffer;
}

std::string mime::header::key_downcase(void) const
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s", _raw[0].c_str());
    strstr(buffer, ":")[0] = '\0';

    for (size_t i = 0; i < strlen(buffer); ++i)
        buffer[i] = tolower(buffer[i]);

    return buffer;
}

void mime::header::add_line(const std::string& line)
{
    _raw.push_back(line);
}

bool mime::header::match(const std::string& key)
{
    if (strncasecmp(key.c_str(), _raw[0].c_str(), strlen(key.c_str())) != 0)
        return false;

    if (_raw[0].c_str()[strlen(key.c_str())] != ':')
        return false;

    return true;
}

bool mime::header::match(const std::vector<std::string>& keys)
{
    for (const auto& key: keys)
        if (match(key))
            return true;

    return false;
}
