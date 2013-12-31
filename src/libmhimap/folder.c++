
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

#include "command.h++"
#include "folder.h++"
#include "logger.h++"

using namespace mhimap;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

folder::folder(const std::string name, client *c)
    : _name(name)
{
    char buffer[BUFFER_SIZE];
    bool uidvalidity_valid = false;

    logger l("folder::folder('%s', ...)", name.c_str());

    command select(c, "SELECT \"%s\"", name.c_str());

    while (c->gets(buffer, BUFFER_SIZE) > 0) {
        if (select.is_end(buffer))
            break;

        if (sscanf(buffer, "* OK [UIDVALIDITY %u] ", &_uidvalidity) == 1)
            uidvalidity_valid = true;
    }

    if (uidvalidity_valid == false) {
        fprintf(stderr, "No UIDVALIDITY response obtained\n");
        abort();
    }
}

folder folder::rename(const std::string new_name) const
{
    return folder(new_name,
                  _uidvalidity);
}

folder::folder(const std::string name,
               uint32_t uidvalidity)
    : _name(name),
      _uidvalidity(uidvalidity)
{
}
