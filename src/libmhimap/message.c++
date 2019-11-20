/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "message.h++"

using namespace mhimap;

message::message(const folder folder, const uint32_t uid)
    : _folder(folder),
      _uid(uid)
{
}
