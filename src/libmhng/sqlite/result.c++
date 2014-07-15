
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

#include "result.h++"
using namespace mhng;

sqlite::result::result(void)
    : _return_set(false)
{
}

sqlite::error_code sqlite::result::return_value(void) const
{
    if (_return_set == false) {
        fprintf(stderr, "return_value() called before return_set()\n");
        abort();
    }

    return _return_value;
}

void sqlite::result::set_error(int code, std::string str)
{
    if (_return_set == true) {
        fprintf(stderr, "set_error() called twice!\n");
        abort();
    }

    switch (code) {
    default:
        fprintf(stderr, "Unknown SQLite error code %d\n", code);
        abort();
        break;
    }

    _return_string = str;
    _return_set = true;
}

void sqlite::result::add_map(const std::map<std::string, std::string>& m)
{
    if (_return_set == true) {
        fprintf(stderr, "add_map() called after set_return()\n");
        abort();
    }

    _data.push_back(m);
}

