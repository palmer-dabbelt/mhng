
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

#ifndef LIBMH__DB__TABLE_COL_HXX
#define LIBMH__DB__TABLE_COL_HXX

#include "col_type.h++"
#include <string>

namespace mh {
    namespace db {
        /* This is really just a pair that represent the type that a
         * table's column can take on. */
        class table_col {
        public:
            const std::string name;
            const col_type type;
            const bool unique;

            table_col(const std::string _name, col_type _type)
                : name(_name), type(_type), unique(false)
                {
                }

            table_col(const std::string _n, col_type _t, bool _u)
                : name(_n), type(_t), unique(_u)
                {
                }
        };
    }
}

#endif
