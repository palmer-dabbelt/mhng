
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

#include "row.h++"
#include <stdlib.h>
using namespace mhng;

template<class K, class V>
static std::vector<K> map_keys(const std::map<K, V>& m);

sqlite::row::row(const std::map<std::string, std::string>& m)
    : _columns(map_keys(m)),
      _m(m)
{
}

std::string sqlite::row::get_str(const std::string& col)
{
    auto l = _m.find(col);
    if (l == _m.end()) {
        fprintf(stderr, "Unable to find column '%s'\n", col.c_str());
        abort();
    }

    return l->second;
}

unsigned sqlite::row::get_uint(const std::string& col)
{
    auto str = get_str(col);
    auto uint = atoi(str.c_str());
    if (uint < 0) {
        fprintf(stderr, "Unable to parse '%s' as unsigned\n", str.c_str());
        abort();
    }
    return uint;
}

bool sqlite::row::get_bool(const std::string& col)
{
    auto uint = get_uint(col);
    switch (uint) {
    case 0:
        return false;

    case 1:
        return true;

    default:
        fprintf(stderr, "Unable to parse %d as bool\n", uint);
        abort();
    }
}

template<class K, class V>
std::vector<K> map_keys(const std::map<K, V>& m)
{
    std::vector<K> out;
    for (const auto& pair: m)
        out.push_back(pair.first);
    return out;
}
