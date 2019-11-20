/* Copyright (C) 2013-2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMH__DATE_HXX
#define LIBMH__DATE_HXX

#include <memory>

namespace mhng {
    class date;
    typedef std::shared_ptr<date> date_ptr;
}

#include <libputil/chrono/datetime.h++>

namespace mhng {
    /* Stores a date, along with the ability to store it as a number
     * of formats. */
    class date: public putil::chrono::datetime {
    public:
        /* Parses the given string into a date. */
        date(const std::string to_parse)
            : putil::chrono::datetime(to_parse)
            {
            }

        date(const putil::chrono::datetime& dt)
            : putil::chrono::datetime(dt)
            {
            }

    public:
        uint64_t unix(void) const
            { return this->unix_seconds(); }
        std::string unix_str(void) const
            { return std::to_string(this->unix()); }

    public:
        /* Creates a new date that represents the time right now. */
        static date_ptr now(void)
            {
                auto n = putil::chrono::datetime::now();
                return std::make_shared<date>(n);
            }
    };
}

#endif
