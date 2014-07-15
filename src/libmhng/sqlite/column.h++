
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

#ifndef MHNG__SQLITE__COLUMN_HXX
#define MHNG__SQLITE__COLUMN_HXX

#include <memory>

namespace mhng {
    namespace sqlite {
        class column;
        typedef std::shared_ptr<column> column_ptr;
    }
}

#include <string>

namespace mhng {
    namespace sqlite {
        /* Represents a single SQLite column.  */
        class column {
        private:
            const std::string _name;

        public:
            column(const std::string& name)
                : _name(name)
                {
                }

        public:
            const std::string& name(void) const
                { return _name; }
        };

        /* Represents a singel SQLite column type, which is actually
         * capable of casting things around. */
        template<class T> class column_t: public column {
        public:
            column_t(const std::string& name)
                : column(name)
                {
                }

        public:
        };
    }
}

#endif
