/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHIMAP__FOLDER_ITER_HXX
#define LIBMHIMAP__FOLDER_ITER_HXX

#include "folder.h++"
#include "vector_iter.h++"

namespace mhimap {
    class folder_iter: public vector_iter<folder> {
    public:
        folder_iter(const std::vector<folder> items)
            : vector_iter<folder>(items)
            {
            }
    };
}

#endif
