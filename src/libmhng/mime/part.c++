
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

#include "part.h++"
#include <string.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

enum state {
    STATE_HEADERS,
    STATE_BODY,
    STATE_CHILD,
};

/* Returns TRUE if this seperates a MIME header boundary. */
static bool isterm(char c);

mime::part::part(const std::vector<std::string>& raw)
    : _raw(raw)
{
    enum state state = STATE_HEADERS;
    header_ptr last_header = NULL;
    std::vector<std::string> child_raw;

    /* Here's a helper function that allows the current header to be
     * committed to the MIME part under construction. */
    auto commit_header = [&](const header_ptr& header)
        {
            if (header->match("Content-Type")) {
                auto value = header->single_line();

                /* We have a boundary line, attempt to split things up
                 * accordingly. */
                if (strstr(value.c_str(), "boundary=") != NULL) {
                    char boundary[BUFFER_SIZE];
                    snprintf(boundary, BUFFER_SIZE, "%s",
                             strstr(value.c_str(), "boundary=")
                        );

                    char *bp = boundary + strlen("boundary=");
                    while (isterm(bp[0]))
                        bp++;

                    char *be = bp;
                    while (!isterm(be[0]))
                        be++;
                    be[0] = '\0';

                    char mime_boundary[BUFFER_SIZE];
                    snprintf(mime_boundary, BUFFER_SIZE, "--%s", bp);
                    _boundary = std::string(mime_boundary);

                    char end_boundary[BUFFER_SIZE];
                    snprintf(end_boundary, BUFFER_SIZE, "--%s--", bp);
                    _end_boundary = std::string(end_boundary);
                }

                /* Set the content-type of this message. */
                if (strstr(value.c_str(), ";") == NULL) {
                    _content_type = value;
                } else {
                    char strip[BUFFER_SIZE];
                    snprintf(strip, BUFFER_SIZE, "%s", value.c_str());
                    strstr(strip, ";")[0] = '\0';
                    _content_type = std::string(strip);
                }
            }

            _headers.push_back(header);
        };

    /* Walk through every line in this MIME part and try to do
     * something with it.  Lines can either by header lines, body
     * lines, or part of the children. */
    for (const auto& raw_line: raw) {
        char line[BUFFER_SIZE];
        snprintf(line, BUFFER_SIZE, "%s", raw_line.c_str());

        while (isspace(line[strlen(line)-1]))
            line[strlen(line)-1] = '\0';

        switch (state) {
        case STATE_HEADERS:
            /* First we want to check and see if this is the last line
             * of headers, which really means we want to skip on to
             * the body. */
            if (strlen(line) == 0) {
                if (last_header != NULL)
                    commit_header(last_header);
                last_header = NULL;

                state = STATE_BODY;

                continue;
            }

            /* Continuation headers are also easy because all we do
             * here is append this to the last header that was
             * added. */
            if (isspace(line[0])) {
                last_header->add_line(line);
                continue;
            }

            /* It's time to start a new header. */
            if (last_header != NULL)
                commit_header(last_header);
            last_header = std::make_shared<header>(line);
            break;

        case STATE_BODY:
            /* Check if this string matches the boundary seperator,
             * which means that we need to stop parsing the body and
             * instead move on to */
            if (matches_boundary(line)) {
                state = STATE_CHILD;
                continue;
            }

            /* The remainder of the body lines simply get pushed into
             * an array for later. */
            _body_raw.push_back(raw_line);

            break;

        case STATE_CHILD:
            /* If we hit another child boundary then it's time to
             * commit this to the list of children and start parsing a
             * new child list. */
            if (matches_boundary(line)) {
                state = STATE_CHILD;
                auto child = std::make_shared<part>(child_raw);
                _children.push_back(child);
                child_raw = {};
                continue;
            }

            /* If this matches the end boundary for MIME parsing then
             * go back to parsing the body. */
            if (matches_end_boundary(line)) {
                state = STATE_BODY;
                auto child = std::make_shared<part>(child_raw);
                _children.push_back(child);
                child_raw = {};
                continue;
            }

            /* The remainder of the child body lines justs get slurped
             * up into a big array for later use. */
            child_raw.push_back(raw_line);

            break;
        }
    }
}

bool mime::part::matches_boundary(const std::string& line)
{
    if (_boundary.known() == false)
        return false;

    auto boundary = _boundary.data();
    if (strcmp(line.c_str(), boundary.c_str()) == 0)
        return true;

    return false;
}

bool mime::part::matches_end_boundary(const std::string& line)
{
    if (_end_boundary.known() == false)
        return false;

    auto boundary = _end_boundary.data();
    if (strcmp(line.c_str(), boundary.c_str()) == 0)
        return true;

    return false;
}

bool isterm(char c)
{
    if (isspace(c))
        return true;

    if (c == '"')
        return true;

    if (c == ';')
        return true;

    return false;
}
