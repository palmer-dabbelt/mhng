
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

#ifndef LIBMHIMAP__GMAIL_CLIENT_HXX
#define LIBMHIMAP__GMAIL_CLIENT_HXX

#include "ssl_client.h++"
#include <map>
#include <string>

namespace mhimap {
    /* A GMail client, which is pretty much an IMAP client but with a
     * special set of rules to prevent it from causing trouble when
     * connecting to a GMail server. */
    class gmail_client: public ssl_client {
    private:
        /* These two maps convert between GMail and MH folder names --
         * the general idea is that GMail has some magic associated
         * with different folder names and therefor we have to match
         * their names EXACTLY, lest all hell break loose.  You should
         * edit these using the helper function below. */
        /* FIXME: These should really be const, but it appears that
         * C++ barfs here... */
        std::map<std::string, std::string> _g2m;
        std::map<std::string, std::string> _m2g;

    public:
        /* Opens a new connection to a GMail server, given just the
         * username and password.  The set of supported authentication
         * mechanisms and such is hard-coded into this class, which is
         * really why it exists. */
        gmail_client(const std::string username,
                     const std::string password
            );

        /* This overrides the default IMAP folder listing with one
         * that closely matches GMail's hard-coded folder expectations
         * with MH's hard-coded folder expectations. */
        string_iter folder_iter(void);

        /* Again, anything that has to do with folders needs to be
         * overridden when it comes to GMail. */
        void send_idle(const std::string folder_name);

    private:
        /* Installs a folder map entry in both directions -- this
         * should really be used instead of directly touching the two
         * maps above. */
        void add_folder_map(std::string mh, std::string gmail);
    };
}

#endif
