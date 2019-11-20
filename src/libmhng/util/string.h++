/* Copyright (C) 2019 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHNG__UTIL__STRING_HXX
#define LIBMHNG__UTIL__STRING_HXX

#include <algorithm>
#include <string>

namespace mhng {
    namespace util {
        namespace string {
            /* Performs a UTF8-aware padding. */
            std::string utf8_pad_to_length(std::string base, size_t length, char element = ' ');

            /* Converts a string to lower case. */
            static inline std::string tolower(std::string in)
            {
                std::string out = in;
                std::transform(out.begin(), out.end(), out.begin(), ::tolower);
                return out;
            }
        }
    }
}

#endif
