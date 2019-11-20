/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHIMAP__STRING_ITER_HXX
#define LIBMHIMAP__STRING_ITER_HXX

#include "vector_iter.h++"
#include <string>
#include <vector>

namespace mhimap {
    /* Iterates through an arbitrary list of strings.  This isn't
     * really type-safe, but I think it's going to be as good as it
     * gets thanks to IMAP.  Note that this is a const_iterator
     * because we can'tt change anything about what's on the IMAP
     * server this way. */
    class string_iter : public vector_iter<std::string> {
    public:
        string_iter(const std::vector<std::string> items)
            : vector_iter<std::string>(items)
            {
            }
    };
}

#endif

