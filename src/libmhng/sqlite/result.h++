
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

#include "error_code.h++"
#include "row.h++"
#include <map>
#include <string>
#include <vector>

namespace mhng {
    namespace sqlite {
        /* Holds the result of a SQL command. */
        class result {
        private:
            /* Here we handle the return code from SQLite.*/
            bool _return_set;
            enum error_code _return_value;
            std::string _return_string;

            /* This contains the actual return data, which is the
             * whole point of this object. */
            std::vector<row_ptr> _data;

        public:
            /* Creates a new result set that hasn't yet been
             * finalized. */
            result(void);

        public:
            /* Returns the error code (which can be "Success") that
             * cooresponds to this command.  Note that you can only
             * call this _after_ set_error()! */
            enum error_code return_value(void) const;

            /* This actually finalizes the construction of the result
             * and makes it usable. */
            void set_error(int code, std::string str);

            /* Adds an entry to the list of results. */
            void add_map(const std::map<std::string, std::string>& m);

            /* Returns the number of results that exist in this
             * entry. */
            size_t result_count(void) const
                { return _data.size(); }

            /* Returns a single row from the listing. */
            const row_ptr& row(size_t i) const
                { return _data[i]; }
            const std::vector<row_ptr>& rows(void) const
                { return _data; }
        };
    }
}

#endif
