
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

#define _BSD_SOURCE

#include "mhdir.h++"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

using namespace mh;

mhdir::mhdir(const options_ptr o)
    : _o(o),
      _db(db::connection::create( (*o).dbfile() ))
{
}

bool mhdir::folder_exists(const std::string folder_name)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s/mail/%s",
             (*_o).mhdir().c_str(), folder_name.c_str());

    struct stat sbuf;
    if (lstat(buffer, &sbuf) != 0)
        return false;

    if (!S_ISDIR(sbuf.st_mode))
        return false;

    return true;
}

folder mhdir::open_folder(const std::string folder_name)
{
    if (!folder_exists(folder_name)) {
        fprintf(stderr, "Opened a folder that doesn't exist!");
        abort();
    }

    return folder(folder_name, _o, _db);
}

imap_store mhdir::get_imap_store(void)
{
    return imap_store(_o, _db);
}

temp_file mhdir::get_tmp(void)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s/tmp", (*_o).mhdir().c_str());
    return temp_file::open(buffer);
}

void mhdir::trans_up(void)
{
    _db->trans_up();
}

void mhdir::trans_down(void)
{
    _db->trans_down();
}

message mhdir::insert(const std::string folder_name, temp_file &file)
{
    return message::insert(open_folder(folder_name), _o, _db, file);
}
