
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

#include "mailrc.h++"
#include <stdio.h>
using namespace mhng;

mailrc::mailrc(const std::string& path)
    : _mailmap()
{
    FILE *file = fopen(path.c_str(), "r");
    if (file == NULL) {
        perror("Unable to open mailrc");
        fprintf(stderr, "Attempted path: '%s'\n", path.c_str());
        abort();
    }

    

    fclose(file);
}

address_ptr mailrc::email(const std::string& email)
{
    auto l = _mailmap.find(email);
    if (l == _mailmap.end())
        return address::from_email(email);
    return l->second;
}

