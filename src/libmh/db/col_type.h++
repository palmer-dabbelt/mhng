
/*
 * Copyright (C) 2013 Palmer Dabbelt
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

#ifndef LIBMH__DB__COL_TYPE_HXX
#define LIBMH__DB__COL_TYPE_HXX

#include <stdlib.h>

namespace mh {
    namespace db {
        /* This contains a list of the types that a column can
         * contain. */
        enum class col_type {
            STRING,
            INTEGER,
            UNIQ_STRING,
        };

        /* Converts a col_type to a constant string. */
        static inline const char *to_string(col_type type)
        {
            switch (type) {
            case col_type::STRING:
                return "STRING";
            case col_type::INTEGER:
                return "INTEGER";
            case col_type::UNIQ_STRING:
                return "UNIQUE STRING";
            }

            return NULL;
        }
    }
}

#endif
