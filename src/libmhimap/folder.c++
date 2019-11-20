/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "command.h++"
#include "folder.h++"
#include "logger.h++"

using namespace mhimap;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

folder::folder(const std::string name, client *c)
    : _name(name),
      _uidvalidity(c->select(name))
{
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
