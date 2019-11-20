/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "gmail_client.h++"
#include <stdlib.h>

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
    add_folder_map("trash", "[Gmail]/All Mail");
    add_folder_map("important", "[Gmail]/Important");
    add_folder_map("starred", "[Gmail]/Starred");
    add_folder_map("rubbish", "[Gmail]/Trash");
    add_folder_map("lists", "Lists");
    add_folder_map("rss", "RSS");
    add_folder_map("promo", "Promo");
    add_folder_map("linux", "Linux");
    add_folder_map("upstream", "Upstream");
    add_folder_map("berkeley", "Berkeley");
    add_folder_map("patches", "Patches");
    add_folder_map("lkcl", "LKCL");
    add_folder_map("indef", "Indef");
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
    /* Look and see if this is a GMail folder name that we know about
     * -- in that case we can just select it directly. */
    if (_g2m.find(fn) != _g2m.end())
        return ssl_client::select(fn);

    /* If this isn't a GMail name that we know about, then convert it
     * into a GMail name so we can actually access the folder. */
    auto found = _m2g.find(fn);
    if (found == _m2g.end()) {
        fprintf(stderr, "SELECT on unmapped folder: '%s'\n", fn.c_str());
        abort();
    }

    return ssl_client::select(found->second);
}

void gmail_client::add_folder_map(const std::string m, const std::string g)
{
    _g2m[g] = m;
    _m2g[m] = g;
}
