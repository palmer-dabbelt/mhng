
/*
 * Copyright (C) 2013,2014 Palmer Dabbelt
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

#ifndef LIBMH__HEADER_HXX
#define LIBMH__HEADER_HXX

#include <string>

namespace mh {
    class header_kv {
    private:
        const std::string _key;
        const std::string _value;

    public:
        header_kv(const std::string key, const std::string value)
            : _key(key),
              _value(value)
            {
            }

        const std::string& key(void) const { return _key; }
        const std::string& value(void) const { return _value; }
    };
}

#endif
