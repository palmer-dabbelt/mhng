
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

#ifndef MHNG__UNKNOWN_HXX
#define MHNG__UNKNOWN_HXX

#include <stdio.h>
#include <stdlib.h>

namespace mhng {
    /* Holds a value that might not be valid. */
    template<class T> class unknown {
    private:
        T _data;
        bool _known;

    public:
        unknown(void)
            : _known(false)
            {
            }

        unknown(const T& data)
            : _data(data),
              _known(true)
            {
            }

    public:
        bool known(void) const
            { return _known; }

        const T& data(void) const
            {
                if (_known == false) {
                    fprintf(stderr, "unknown not known\n");
                    abort();
                }

                return _data;
            }

        T operator=(const T& data)
            {
                _data = data;
                _known = true;
                return data;
            }
    };
}

#endif
