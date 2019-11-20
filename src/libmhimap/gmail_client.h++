/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

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

        /* mh::client overrides. */
        typename mhimap::folder_iter folder_iter(void);
        virtual uint32_t select(const std::string name);

    private:
        /* Installs a folder map entry in both directions -- this
         * should really be used instead of directly touching the two
         * maps above. */
        void add_folder_map(std::string mh, std::string gmail);
    };
}

#endif
