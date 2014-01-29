
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

#include "gmail_client.h++"

#ifndef GMAIL_HOSTNAME
#define GMAIL_HOSTNAME "imap.gmail.com"
#endif

#ifndef GMAIL_PORT
#define GMAIL_PORT 993
#endif

#ifdef GMAIL_FORCE_SSL_NOT_TLS
#define VERS_SSLONLY ":+VERS-SSL3.0:-VERS-TLS1.2:-VERS-TLS1.1:-VERS-TLS1.0"
#else
#define VERS_SSLONLY ""
#endif

#ifdef GMAIL_ALLOW_RC4128
#define VERS_RC4 ""
#else
#define VERS_RC4 ":-ARCFOUR-128"
#endif

using namespace mhimap;

gmail_client::gmail_client(const std::string username,
                           const std::string password)
    : ssl_client(GMAIL_HOSTNAME, GMAIL_PORT, username, password,
                 "NORMAL" VERS_RC4 VERS_SSLONLY),
      _g2m(),
      _m2g()
{
    /* FIXME: I'm pretty sure some of these folder names change when
     * your gmail language isn't English... */

    /* FIXME: Should there be some sort of case-insensitive matching
     * here? */

    add_folder_map("inbox", "INBOX");
    add_folder_map("drafts", "[Gmail]/Drafts");
    add_folder_map("queue", "Queue");
    add_folder_map("sent", "[Gmail]/Sent Mail");
    add_folder_map("spam", "[Gmail]/Spam");
    add_folder_map("trash", "[Gmail]/Trash");
}

typename mhimap::folder_iter gmail_client::folder_iter(void)
{
    std::vector<folder> folders;

    for (auto it = ssl_client::folder_iter(); !it.done(); ++it) {
        std::string folder_name = (*it).name();

        if (_g2m.find(folder_name) == _g2m.end())
            continue;

        folders.push_back((*it).rename(_g2m.find(folder_name)->second));
    }

    return mhimap::folder_iter(folders);
}

uint32_t gmail_client::select(const std::string fn)
{
    if (_m2g.find(fn) == _m2g.end()) {
        fprintf(stderr, "SELECT on unmapped folder: '%s'\n", fn.c_str());
        abort();
    }

    return ssl_client::select(fn);
}

void gmail_client::add_folder_map(const std::string m, const std::string g)
{
    _g2m[g] = m;
    _m2g[m] = g;
}
