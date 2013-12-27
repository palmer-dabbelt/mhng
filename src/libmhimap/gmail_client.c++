
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
                 "NORMAL" VERS_RC4 VERS_SSLONLY)
{
}
