/* Copyright (C) 2013-2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMH__GPG_SIGN_HXX
#define LIBMH__GPG_SIGN_HXX

#ifdef HAVE_GPGME
#include <string>
#include <vector>

namespace mhng {
    std::vector<std::string> gpg_sign(const std::vector<std::string>& in,
                                      std::string email);

    enum class gpg_verification {
        FAIL,
        ERROR,
        SUCCESS,
    };

    gpg_verification gpg_verify(const std::vector<std::string>& msg,
                                const std::vector<std::string>& sig,
                                std::string email);
}
#endif

#endif
