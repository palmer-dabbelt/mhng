/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHIMAP__DONE_COMMAND_HXX
#define LIBMHIMAP__DONE_COMMAND_HXX

namespace mhimap {
    class done_command;
}

#include "command.h++"

namespace mhimap {
    /* The DONE half of the IDLE command is special because it doesn't
     * get tagged with a number (and therefor doesn't increment the
     * number. */
    class done_command: public command {
    public:
        /* The DONE command doesn't take any arguments. */
        done_command(client *c);
    };
}

#endif
