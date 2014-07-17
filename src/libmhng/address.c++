
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

#include "address.h++"
using namespace mhng;

address::address(const unknown<std::string>& email,
                 const unknown<std::string>& name,
                 const unknown<std::string>& alias)
    : _email(email),
      _name(name),
      _alias(alias)
{
}

std::string address::nom(void) const
{
    if (_name.known() == true)
        return _name.data();
    return _email.data();
}

address_ptr address::from_email(const std::string email)
{
    return std::make_shared<address>(
        unknown<std::string>(email),
        unknown<std::string>(),
        unknown<std::string>()
        );
}
