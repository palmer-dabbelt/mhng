/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "done_command.h++"

using namespace mhimap;

done_command::done_command(client *c)
    : command("DONE", c->sequence - 1, c)
{
    
}
