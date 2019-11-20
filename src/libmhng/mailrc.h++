/* Copyright (C) 2014-2017 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__MAILRC_HXX
#define MHNG__MAILRC_HXX

#include <memory>
#include <vector>

namespace mhng {
    class mailrc;
    typedef std::shared_ptr<mailrc> mailrc_ptr;
}

#include "address.h++"
#include <unordered_map>
#include <string>

namespace mhng {
    /* Deals with parsing a mailrc, which contains address-related
     * configuration information. */
    class mailrc {
    private:
        std::unordered_map<std::string, address_ptr> _mail_map;
        std::unordered_map<std::string, address_ptr> _alias_map;
        std::unordered_map<std::string, address_ptr> _name_map;
        std::vector<address_ptr> _bcc;

    public:
        /* Creates a new mailrc given the full path to the file that
         * represents that mailrc. */
        mailrc(const std::string& path);

    public:
        /* Returns an address, looked up by an email. */
        address_ptr email(const std::string& email);

        /* Returns an address, looking up either an email or an
         * alias. */
        address_ptr emailias(const std::string& in);

        /* Returns the list of all BCCs that should be automatically added to
         * _all_ messages. */
        const std::vector<address_ptr>& bcc(void) const { return _bcc; }

    private:
        /* Adds a new address to this database.  This will probably
         * only be called from the constructor, so be careful with
         * it! */
        void add(const address_ptr& addr);

        /* Adds a new BCC to this database. */
        void add_bcc(const address_ptr& addr);
    };
}

#endif
