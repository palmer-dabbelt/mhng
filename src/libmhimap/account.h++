/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHIMAP__ACCOUNT_HXX
#define LIBMHIMAP__ACCOUNT_HXX

#include <string>

namespace mhimap {
    class account {
    private:
        const std::string _name;

    public:
        account(const std::string& name)
        : _name(name)
        {}

        const std::string& name(void) const { return _name; }
    };
}

#endif
