/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__SEQUENCE_NUMBER_HXX
#define MHNG__SEQUENCE_NUMBER_HXX

#include <memory>

namespace mhng {
    class sequence_number;
    typedef std::shared_ptr<sequence_number> sequence_number_ptr;
}

#include "folder.h++"
#include "promise.h++"
#include <psqlite/connection.h++>
#include <string>

namespace mhng {
    /* A thin wrapper around integers that allows them to represent
     * sequence numbers. */
    class sequence_number {
    private:
        unsigned _num;

    public:
        /* Allows a sequence number to be created from a plain-old
         * integer.  Note that you should probably only be using this
         * if you're formatting strings, otherwise it's a bit
         * unsafe. */
        sequence_number(unsigned num);

    public:
        /* Converts to an integer. */
        unsigned to_uint(void) const { return _num; }
        int to_int(void) const { return (int)_num; }
    };
}

#endif
