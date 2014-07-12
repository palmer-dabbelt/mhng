
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

#ifndef MHNG__SQLITE__RESULT_HXX
#define MHNG__SQLITE__RESULT_HXX

#include <memory>

namespace mhng {
    namespace sqlite {
        class result;
        typedef std::shared_ptr<result> result_ptr;
    }
}

#include <sqlite3.h>
#include <string>

namespace mhng {
    namespace sqlite {
        /* Holds a single result. */
        class result {
        private:
            
        };

        /* Results are paramaterized by */
        template<class T> class result_t: public result {
        };
    }
}

#endif
