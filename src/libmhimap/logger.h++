
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

#ifndef LIBMHIMAP__LOGGER_HXX
#define LIBMHIMAP__LOGGER_HXX

#ifndef SIZET_FORMAT
#if defined(__amd64__)
#define SIZET_FORMAT "%llu"
#define SSIZET_FORMAT "%lld"
#elif defined(__arm__)
#define SIZET_FORMAT "%u"
#define SSIZET_FORMAT "%d"
#endif
#endif /* SIZET_FORMAT */

#include <string>

namespace mhimap {
    class logger {
    private:
        int depth;

    public:
        logger(const char *fmt, ...) __attribute__(( format(printf, 2, 3) ));

        ~logger(void);

        void printf(const char *fmt, ...)
            __attribute__(( format(printf, 2, 3) ));
    };
}

#endif
