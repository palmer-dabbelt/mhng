/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBOAUTH2__PKCE_HXX
#define LIBOAUTH2__PKCE_HXX

#include <libmhoauth/access_token.h++>
#include <string>

namespace libmhoauth {
    /* Fundametally, PKCE is a 4-step process:
     *   - The client sends an authorization request to the server.
     *   - The server responds with an authorization code.
     *   - The client sends an access token request.
     *   - The server responds with an access token.
     * These states are all rolled up into the single pkce() function, which is
     * all you should really need to know about as a user.  Interally this uses
     * the user's browser command and an embedded HTTP server.
     */
    access_token pkce(
        std::string client_id,    /* The client ID, which is semi-secret. */
        std::string auth_uri,     /* The URI the browser goes to. */
        std::string token_uri,    /* The URI that can be automatically accessed. */
        std::string browser,      /* The command to launch a browser. */
        std::string scope,        /* The scope of this access request. */
        std::string hint          /* Tells the server which account to select. */
    );
}

#endif
