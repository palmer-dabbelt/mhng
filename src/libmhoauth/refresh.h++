/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBOAUTH2__REFRESH_HXX
#define LIBOAUTH2__REFRESH_HXX

#include <libmhoauth/access_token.h++>
#include <string>

namespace libmhoauth {
    /* Obtains another "access + refresh" token pair from a refresh token. */
    access_token refresh(
        std::string client_id,    /* The client ID, which is semi-secret. */
        std::string token_uri,    /* The URI that can be automatically accessed. */
        std::string token         /* The refresh token. */
    );
}

#endif
