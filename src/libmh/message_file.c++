
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
    STATE_PLAIN_EAT,
    STATE_PLAIN, /* Don't directly transition here, use
                  * "STATE_PLAIN_EAT" instead. */
    STATE_MIME,
};

/* Strips everything but the email address from a mail header. */
static void strip_address(std::vector<std::string> &vec, std::string buf);

message_file::message_file(const std::string full_path)
    : _plain_text(true),
      _mime(new mime())
{
    FILE *file = fopen(full_path.c_str(), "r");

    enum state state = STATE_HEADERS;

    char buffer[BUFFER_SIZE];

    std::string hname = "";
    std::string hcont = "";

    auto mstack = _mime;

    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        /* Strip newlines from the buffer in question. */
        while (isspace(buffer[strlen(buffer) - 1]))
            buffer[strlen(buffer) - 1] = '\0';

        switch (state) {
        case STATE_HCONT:
            /* An empty line means the headers are over. */
            if (strlen(buffer) == 0) {
                add_header(hname, hcont);
                state = _plain_text ? STATE_PLAIN_EAT : STATE_MIME;
                for (auto it = headers("content-type"); !it.done(); ++it) {
                    _mime->set_root_content_type(*it);
                }

                continue;
            }

            /* If the first character isn't a space then the header
             * continuation is over. */
            if (!isspace(buffer[0])) {
                state = STATE_HEADERS;
                add_header(hname, hcont);

                /* Here we _purpousely_ fall through to STATE_HEADERS
                 * -- effectively we want to take the state transition
                 * _now_, not later. */
            } else {
                char *cbuf = buffer;
                while (isspace(*cbuf))
                    cbuf++;

                hcont = hcont + " " + cbuf;

                /* This continue exists so we don't fall through and
                 * end up in the HEADERS section. */
                continue;
            }

        case STATE_HEADERS:
            /* An empty line means the headers are over. */
            if (strlen(buffer) == 0) {
                add_header(hname, hcont);
                state = _plain_text ? STATE_PLAIN_EAT : STATE_MIME;
                for (auto it = headers("content-type"); !it.done(); ++it) {
                    _mime->set_root_content_type(*it);
                }

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

        case STATE_PLAIN_EAT:
            /* Some people (I'm looking at you, Krste) start all their
             * messages with a gratuitous newline.  This fixes the
             * problem by eating blank lines until we end up with one
             * that's not blank. */
            if (strcmp(buffer, "") == 0)
                continue;

            /* As soon as we get a non-blank line then we should just
             * move to parsing this like a regular plain-text mail.
             * Note that this should be the only state transition to
             * "STATE_PLAIN", the rest should come through
             * "STATE_PLAIN_EAT". */
            state = STATE_PLAIN;

            /* There's an explicit fall-through to STATE_PLAIN here in
             * order to actually add this line, as we don't want to
             * drop the first non-empty line. */

        case STATE_PLAIN:
            _body.push_back(buffer);
            break;

        case STATE_MIME:
            auto sibling = mstack->is_sibling(buffer);
            if (sibling != NULL) {
                mstack = sibling;
                continue;
            }

            auto child = mstack->is_child(buffer);
            if (child != NULL) {
                mstack = child;
                continue;
            }

            auto parent = mstack->is_parent(buffer);
            if (parent != NULL) {
                mstack = parent;
                continue;
            }

            mstack->parse(buffer);
            break;
        }

        /* Make sure there's nothing important here, there are a whole
         * bunch of "continue"s above. */
    }

    fclose(file);
}

message_file::~message_file(void)
{
    delete _mime;
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

string_iter message_file::headers_address(const std::string hn) const
{
    std::vector<std::string> values;

    for (auto it = headers(hn); !it.done(); ++it) {
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "%s", (*it).c_str());

        char *start, *end, *cur;
        start = cur = buffer;
        end = start + strlen(start);
        bool in_quotes = false;
        while (*cur != '\0') {
            if (*cur == '"')
                in_quotes = !in_quotes;

            if (in_quotes)
                goto is_in_quotes;

            if (*cur == ',') {
                end = cur;

                char addr[BUFFER_SIZE];
                snprintf(addr, BUFFER_SIZE, "%s", start);
                addr[(size_t)(end - start)] = '\0';
                strip_address(values, addr);

                start = end + 1;

                while (isspace(*start))
                    start++;
            }

        is_in_quotes:
            cur++;
        }

        strip_address(values, start);
    }

    return string_iter(values);
}

date_iter message_file::headers_date(const std::string hn) const
{
    std::vector<date> values;

    for (auto it = headers(hn); !it.done(); ++it) {
        values.push_back(date(*it));
    }

    return date_iter(values);
}

string_iter message_file::body(void) const
{
    if (_plain_text == true)
        return string_iter(_body);

    auto text = _mime->search("text/plain");
    if (text != NULL)
        return text->body_utf8();

    auto html = _mime->search("text/html");
    if (html != NULL)
        return html->body_utf8();

    return string_iter(std::vector<std::string>());
}

void message_file::add_header(std::string h, std::string v)
{
    if (strcmp(h.c_str(), "") == 0)
        return;

    if (strcmp(h.c_str(), "mime-version") == 0)
        _plain_text = false;

    auto found = _headers.find(h);
    if (found == _headers.end()) {
        _headers[h] = std::vector<std::string>();
        found = _headers.find(h);
    }

    found->second.push_back(v);
}

void strip_address(std::vector<std::string> &vec, std::string buf)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s", buf.c_str());

    bool in_quote = false;

    char *start, *end, *cur;
    cur = start = buffer;
    end = buffer + strlen(buffer);
    while (*cur != '\0') {
        if (*cur == '"')
            in_quote = !in_quote;

        if (!in_quote) {
            if (*cur == '<')
                start = cur + 1;
            if (*cur == '>')
                end = cur;
        }

        cur++;
    }

    char addr[BUFFER_SIZE];
    snprintf(addr, BUFFER_SIZE, "%s", start);
    addr[(size_t)(end - start)] = '\0';
    vec.push_back(addr);
}
