/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHIMAP__LOGGER_HXX
#define LIBMHIMAP__LOGGER_HXX

#ifndef SIZET_FORMAT
#if defined(__amd64__)
#define SIZET_FORMAT "%lu"
#define SSIZET_FORMAT "%ld"
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
        pid_t pid;

    public:
        logger(const char *fmt, ...) __attribute__(( format(printf, 2, 3) ));

        ~logger(void);

        void printf(const char *fmt, ...)
            __attribute__(( format(printf, 2, 3) ));
    };
}

#endif
