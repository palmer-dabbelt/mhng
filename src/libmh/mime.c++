
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

#include "mime.h++"
#include <string.h>

using namespace mh;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

mime::mime(void)
    : _boundary_child(""),
      _boundary_sibling(""),
      _boundary_parent(""),
      _in_headers(true),
      _type("UNKNOWN"),
      _encoding("UNKNOWN"),
      _parent(NULL),
      _children(),
      _body(),
      _hname(""),
      _hcont(""),
      _hfirst(true)
{
}

mime::~mime(void)
{
    for (auto it = _children.begin(); it != _children.end(); ++it) {
        delete *it;
    }
}

const mime *mime::search(const std::string type) const
{
    if (strcmp(type.c_str(), _type.c_str()) == 0)
        return this;

    for (auto it = _children.begin(); it != _children.end(); ++it) {
        auto out = (*it)->search(type);
        if (out != NULL)
            return out;
    }

    return NULL;
}

string_iter mime::body_utf8(void) const
{
    return string_iter(_body);
}

void mime::set_root_content_type(const std::string value)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s", value.c_str());

    char *boundary = strstr(buffer, "boundary=");
    if (boundary == NULL) {
        fprintf(stderr, "Content-Type without boundary\n");
        abort();
    }

    boundary += strlen("boundary=");

    if (*boundary == '"')
        boundary++;

    {
        char *bend = boundary;

        while (*bend != '"' && *bend != '\0' && !isspace(*bend) && *bend != ';')
            bend++;

        *bend = '\0';
    }

    char bbuf[BUFFER_SIZE];

    snprintf(bbuf, BUFFER_SIZE, "--%s", boundary);
    _boundary_child = bbuf;

    _in_headers = false;
}

mime *mime::is_sibling(const std::string buffer)
{
    if (strcmp(_boundary_sibling.c_str(), "") == 0)
        return NULL;

    if (strcmp(_boundary_sibling.c_str(), buffer.c_str()) != 0)
        return NULL;

    auto nn = new mime();
    nn->_parent = this->_parent;
    nn->_boundary_sibling = this->_boundary_sibling;
    nn->_boundary_parent = this->_boundary_parent;
    this->_parent->_children.push_back(nn);
    return nn;
}

mime *mime::is_child(const std::string buffer)
{
    if (strcmp(_boundary_child.c_str(), "") == 0)
        return NULL;

    if (strcmp(_boundary_child.c_str(), buffer.c_str()) != 0)
        return NULL;

    auto nn = new mime();
    nn->_parent = this;
    nn->_boundary_sibling = this->_boundary_child;
    nn->_boundary_parent = this->_boundary_child + "--";
    this->_children.push_back(nn);
    return nn;
}

mime *mime::is_parent(const std::string buffer)
{
    if (strcmp(_boundary_parent.c_str(), "") == 0)
        return NULL;

    if (strcmp(_boundary_parent.c_str(), buffer.c_str()) != 0)
        return NULL;

    return this->_parent;
}

void mime::parse(const std::string buffer)
{
    if (strcmp(_boundary_sibling.c_str(), "") != 0) {
        if (strcmp(_boundary_sibling.c_str(), buffer.c_str()) == 0)
            return;
    }
    if (strcmp(_boundary_child.c_str(), "") != 0) {
        if (strcmp(_boundary_child.c_str(), buffer.c_str()) == 0)
            return;
    }
    if (strcmp(_boundary_parent.c_str(), "") != 0) {
        if (strcmp(_boundary_parent.c_str(), buffer.c_str()) == 0)
            return;
    }

    if (!_in_headers) {
        _body.push_back(buffer);
        return;
    }

    if (strcmp(buffer.c_str(), "") == 0) {
        _in_headers = false;
    }

    if (!_hfirst && !isspace(buffer.c_str()[0])) {
        _hfirst = true;

        if (strcasecmp(_hname.c_str(), "Content-Type") == 0) {
            {
                char type[BUFFER_SIZE];
                snprintf(type, BUFFER_SIZE, "%s", _hcont.c_str());

                char *nbuf = type;
                while (*nbuf != '\0' && *nbuf != ';' && !isspace(*nbuf))
                    nbuf++;
                *nbuf = '\0';

                _type = type;
            }

            {
                char type[BUFFER_SIZE];
                snprintf(type, BUFFER_SIZE, "%s", _hcont.c_str());

                char *encoding = strstr(type, "charset=");
                if (encoding == NULL)
                    goto no_charset;

                encoding += strlen("charset=");
                if (*encoding == '"')
                    encoding++;

                char *n = encoding;
                while (*n != '\0' && !isspace(*n) && *n != ';' && *n != '"')
                    n++;
                *n = '\0';

                _encoding = encoding;
            }

        no_charset:
            {
                char type[BUFFER_SIZE];
                snprintf(type, BUFFER_SIZE, "%s", _hcont.c_str());

                char *boundary = strstr(type, "boundary=");
                if (boundary == NULL)
                    goto no_boundary;

                boundary += strlen("boundary=");
                if (*boundary == '"')
                    boundary++;

                char *n = boundary;
                while (*n != '\0' && !isspace(*n) && *n != ';' && *n != '"')
                    n++;
                *n = '\0';

                char b[BUFFER_SIZE];
                snprintf(b, BUFFER_SIZE, "--%s", boundary);
                _boundary_child = b;
            }

        no_boundary:
            {
            }
        }

        if (strcasecmp(_hname.c_str(), "Content-Transfer-Encoding") == 0) {
            _encoding = _hcont;
        }
    }

    if (!_hfirst) {
    }

    if (!_in_headers) {
        return;
    }

    if (_hfirst) {
        char nbuf[BUFFER_SIZE];
        snprintf(nbuf, BUFFER_SIZE, "%s", buffer.c_str());

        char *colon = strstr(nbuf, ":");
        if (colon == NULL) {
            fprintf(stderr, "mh::mime -- Header without ':'\n");
            abort();
        }

        *colon = '\0';
        colon++;

        while (isspace(*colon))
            colon++;

        _hname = nbuf;
        _hcont = colon;
        _hfirst = false;
    }
}
