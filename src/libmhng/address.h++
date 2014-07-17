
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

#ifndef MHNG__ADDRESS_HXX
#define MHNG__ADDRESS_HXX

#include <memory>

namespace mhng {
    class address;
    typedef std::shared_ptr<address> address_ptr;
}

#include "unknown.h++"
#include <string>

namespace mhng {
    /* This holds an address that's been properly parsed.  In other
     * words, it's not just a plain string but instead is actually a
     * wrapper that contains a whole bunch of strings! */
    class address {
    private:
        unknown<std::string> _email;
        unknown<std::string> _name;
        unknown<std::string> _alias;

    public:
        /* The only way to create an address is to have the entire
         * address parsed.  The only reason this is public because I
         * need to make shared pointers from this... */
        address(const unknown<std::string>& email,
                const unknown<std::string>& name,
                const unknown<std::string>& alias);

    public:
        /* Returns either the name or email address for this address.
         * Note that this always succeeds */
        std::string nom(void) const;

    public:
        static address_ptr from_email(const std::string email);
    };
}

#endif
