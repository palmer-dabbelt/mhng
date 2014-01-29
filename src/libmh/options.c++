
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

#include "options.h++"
#include <stdio.h>

using namespace mh;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

options_ptr options::create(int argc, const char **argv)
{
    return options_ptr(new options(argc, argv));
}

const std::string options::mhdir(void) const
{
    if (getenv("HOME") == NULL) {
        fprintf(stderr, "No HOME env defined\n");
        abort();
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s/.mhng", getenv("HOME"));
    return buffer;
}

const std::string options::dbfile(void) const
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s/metadata.sqlite3", mhdir().c_str());
    return buffer;
}

options::options(int argc, const char **argv)
    : _argc(argc),
      _argv(argv),
      _folder_valid(false)
{
}
