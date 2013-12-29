
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

#ifndef LIBMHIMAP__IDLE_RESPONSE_HXX
#define LIBMHIMAP__IDLE_RESPONSE_HXX

namespace mhimap {
    /* Holds the different sort of responses that can come back from
     * an IDLE command. */
    enum class idle_response {
        /* FIXME: emacs doesn't know how to indent C++ enum
         * classes... :( */
        TIMEOUT,
        DISCONNECT,
        DATA,    
    };
}

#endif
