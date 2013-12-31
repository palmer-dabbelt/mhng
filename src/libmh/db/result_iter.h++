
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

#ifndef LIBMH__DB__RESULT_ITER_HXX
#define LIBMH__DB__RESULT_ITER_HXX

namespace mh {
    namespace db {
        class result_iter;
    }
}

#include "result.h++"
#include <string>
#include <sqlite3.h>

namespace mh {
    namespace db {
        class result_iter {
        private:
            std::vector<result> _results;
            std::vector<result>::const_iterator _it;

        public:
            result_iter(const std::vector<result> results)
                : _results(results),
                  _it(_results.begin())
                {
                }

            bool done(void) const { return _it == _results.end(); }
            void operator++(void) { _it++; }
            result operator*(void) { return *_it; }
        };
    }
}

#endif
