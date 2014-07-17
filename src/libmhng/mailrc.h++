
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

#ifndef MHNG__MAILRC_HXX
#define MHNG__MAILRC_HXX

#include <memory>

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

    public:
        /* Creates a new mailrc given the full path to the file that
         * represents that mailrc. */
        mailrc(const std::string& path);

    public:
        /* Returns an address */
        address_ptr email(const std::string& email);

    private:
        /* Adds a new address to this database.  This will probably
         * only be called from the constructor, so be careful with
         * it! */
        void add(const address_ptr& addr);
    };
}

#endif
