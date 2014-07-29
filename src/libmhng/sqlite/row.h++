
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

#ifndef MHNG__SQLITE__ROW_HXX
#define MHNG__SQLITE__ROW_HXX

#include <memory>

namespace mhng {
    namespace sqlite {
        class row;
        typedef std::shared_ptr<row> row_ptr;
    }
}

#include <map>
#include <string>
#include <vector>

namespace mhng {
    namespace sqlite {
        /* Holds a single row that was returned from a query. */
        class row {
        private:
            const std::vector<std::string> _columns;
            const std::map<std::string, std::string> _m;

        public:
            /* Creates a new row, given the data it contains. */
            row(const std::map<std::string, std::string>& m);

        public:
            /* These provide a mechanism for obtaining access to
             * particular fields within this row. */
            std::string get_str(const std::string& col);
            unsigned get_uint(const std::string& col);
            bool get_bool(const std::string& col);

            /* Returns the list of the columns that are known to this
             * row.  This is guarnteed */
            std::vector<std::string> columns(void) const
                { return _columns; }

            /* Returns the value associated with a column. */
            std::string value(const std::string& str) const
                { 
                    auto l = _m.find(str);
                    return l->second;
                }

            /* Returns TRUE if the value exists in the map, and FALSE
             * otherwise.  Note that this is FALSE for NULL SQL
             * values, as well as FALSE for values that aren't in the
             * SQL table at all! */
            bool has(const std::string& str) const
                {
                    auto l = _m.find(str);
                    if (l == _m.end())
                        return false;
                    else
                        return true;
                }
        };
    }
}

#endif
