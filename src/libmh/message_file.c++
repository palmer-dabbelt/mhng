
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

#include "message_file.h++"
#include <algorithm>
#include <string.h>
#include <ctype.h>

using namespace mh;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

enum state {
    STATE_HEADERS,
    STATE_HCONT,
    STATE_PLAIN,
    STATE_MIME,
};

message_file::message_file(const std::string full_path)
    : _plain_text(true)
{
    FILE *file = fopen(full_path.c_str(), "r");

    enum state state = STATE_HEADERS;

    char buffer[BUFFER_SIZE];

    std::string hname = "";
    std::string hcont = "";

    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        /* Strip newlines from the buffer in question. */
        while (isspace(buffer[strlen(buffer) - 1]))
            buffer[strlen(buffer) - 1] = '\0';

        switch (state) {
        case STATE_HCONT:
            /* An empty line means the headers are over. */
            if (strlen(buffer) == 0) {
                state = _plain_text ? STATE_PLAIN : STATE_MIME;
                continue;
            }

            /* If the first character isn't a space then the header
             * continuation is over. */
            if (buffer[0] != ' ') {
                state = STATE_HEADERS;
                add_header(hname, hcont);

                /* Here we _purpousely_ fall through to STATE_HEADERS
                 * -- effectively we want to take the state transition
                 * _now_, not later. */
            } else {
                /* This continue exists so we don't fall through and
                 * end up in the HEADERS section. */
                continue;
            }

        case STATE_HEADERS:
            /* An empty line means the headers are over. */
            if (strlen(buffer) == 0) {
                state = _plain_text ? STATE_PLAIN : STATE_MIME;
                continue;
            }

            state = STATE_HCONT;

            if (strstr(buffer, ":") == NULL) {
                hname = "";
                hcont = "";
            } else {
                char *cptr = strstr(buffer, ":");
                *cptr = '\0';
                cptr++;

                while (isspace(*cptr) && *cptr != '\0')
                    cptr++;

                hname = buffer;
                hcont = cptr;

                std::transform(hname.begin(),
                               hname.end(),
                               hname.begin(),
                               ::tolower);
            }

            break;

        case STATE_PLAIN:
        case STATE_MIME:
            _body.push_back(buffer);
            break;
        }

        /* Make sure there's nothing important here, there are a whole
         * bunch of "continue"s above. */
    }

    fclose(file);
}

string_iter message_file::headers(const std::string header_name) const
{
    std::string lower(header_name.c_str());
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    auto lookup = _headers.find(lower);
    if (lookup == _headers.end())
        return string_iter(std::vector<std::string>());

    return string_iter(lookup->second);
}

string_iter message_file::body(void) const
{
    return string_iter(_body);
}

void message_file::add_header(std::string h, std::string v)
{
    if (strcmp(h.c_str(), "") == 0)
        return;

    if (strcmp(h.c_str(), "mime-version"))
        _plain_text = false;

    auto found = _headers.find(h);
    if (found == _headers.end()) {
        _headers[h] = std::vector<std::string>();
        found = _headers.find(h);
    }

    found->second.push_back(v);
}
