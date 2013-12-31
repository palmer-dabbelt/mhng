
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

#ifndef LIBMH__DB__RESULT_LIST_HXX
#define LIBMH__DB__RESULT_LIST_HXX

namespace mh {
    namespace db {
        class result_list;
    }
}

#include "result.h++"
#include <string>
#include <vector>

namespace mh {
    namespace db {
        /* Holds the list of results returned by a query along with
         * the return code from that query. */
        class result_list {
        private:
            std::vector<result> _results;
            int return_code;

        public:
            result_list(int return_code, std::vector<result> results);
        };
    }
}

#endif
