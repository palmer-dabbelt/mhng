
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

#include "temp_file.h++"

using namespace mh;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

temp_file temp_file::open(const std::string folder)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s/temp_file-XXXXXX", folder.c_str());

    int fd = mkstemp(buffer);
    return temp_file(fdopen(fd, "w"), buffer);
}

temp_file::~temp_file(void)
{
    if (_fp == NULL)
        return;

    fclose(_fp);
}

void temp_file::finish(void)
{
    if (_fp == NULL)
        return;

    fclose(_fp);
    _fp = NULL;
}

temp_file::temp_file(FILE *fp, const std::string path)
    : _fp(fp),
      _path(path)
{
}
