/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identified: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBOAUTH2__CODE_VERIFIER_HXX
#define LIBOAUTH2__CODE_VERIFIER_HXX

#include <libmhoauth/base64.h++>
#include <string>
#include <vector>

namespace libmhoauth {
    class code_verifier {
    private:
        std::string _plain;

    public:
        code_verifier(void);

    public:
        const decltype(_plain)& plain(void) const { return _plain; }
        std::string s256(void) const;
    };
}

#endif
