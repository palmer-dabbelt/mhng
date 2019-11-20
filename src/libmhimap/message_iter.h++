/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHIMAP__MESSAGE_ITER_HXX
#define LIBMHIMAP__MESSAGE_ITER_HXX

namespace mhimap {
    class message_iter;
}

#include "message.h++"
#include "vector_iter.h++"

namespace mhimap {
    class message_iter: public vector_iter<message> {
    public:
        message_iter(const std::vector<message> items)
            : vector_iter<message>(items)
            {
            }
    };
}

#endif

