
/*
 * Copyright (C) 2014 Palmer Dabbelt
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

#ifndef MHNG__SQLITE__TABLE_HXX
#define MHNG__SQLITE__TABLE_HXX

#include <memory>

namespace mhng {
    namespace sqlite {
        class table;
        typedef std::shared_ptr<table> table_ptr;
    }
}

#include "column.h++"
#include <string>
#include <vector>

namespace mhng {
    namespace sqlite {
        /* This represents a SQL table, which pretty much just
         * consists of a table name and a bunch of columns in some
         * order. */
        class table {
        private:
            std::vector<column_ptr> _cols;

        public:
            /* Creates a new column from a list of tables. */
            table(const std::vector<column_ptr>& cols);
        };
    }
}

#endif
