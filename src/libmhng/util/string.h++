/*
 * Copyright (C) 2019 Palmer Dabbelt
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
