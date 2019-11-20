/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBOAUTH2__AUTHORIZATION_CODE_HXX
#define LIBOAUTH2__AUTHORIZATION_CODE_HXX

#include <libmhoauth/code_verifier.h++>
#include <string>

namespace libmhoauth {
    class authorization_code {
    private:
        const std::string _string;
        const code_verifier _verifier;
        const std::string _redirect_uri;

    public:
        authorization_code(std::string string,
                           code_verifier verifier,
                           std::string redirect_uri)
        : _string(string),
          _verifier(verifier),
          _redirect_uri(redirect_uri)
        {}

    public:
        const std::string value(void) const { return _string; }
        const code_verifier& verifier(void) const { return _verifier; }
        const std::string redirect_uri(void) const { return _redirect_uri; }
    };
}

#endif
