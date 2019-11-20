/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identified: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBOAUTH2__AUTHORIZATION_REQUEST_HXX
#define LIBOAUTH2__AUTHORIZATION_REQUEST_HXX

#include <libmhoauth/code_verifier.h++>
#include <libmhoauth/base64.h++>
#include <map>
#include <curl/curl.h>

namespace libmhoauth {
    class authorization_request {
    private:
        const std::string _auth_uri;
        const code_verifier _verifier;
        const std::map<std::string, std::string> _info;

    private:
        static decltype(_info) mkmap(void) { return {}; }

        template<typename... A>
        static decltype(_info) mkmap(decltype(_info)::key_type key,
                                     decltype(_info)::mapped_type value,
                                     A... args) {
            auto out = mkmap(std::forward<A>(args)...);
            out[key] = value;
            return out;
        }

    public:
        authorization_request(std::string auth_uri,
                              std::string client_id,
                              std::string scope,
                              std::string redirect_uri,
                              std::string login_hint)
        : _auth_uri(auth_uri),
          _verifier(),
          _info(mkmap("response_type", "code",
                      "access_type", "offline",
                      "client_id", client_id,
                      "scope", scope,
                      "redirect_uri", redirect_uri,
                      "code_challenge", _verifier.s256(),
                      "code_challenge_method", "S256",
                      "login_hint", login_hint))
        {}

        const decltype(_verifier)& verifier(void) const { return _verifier; }

        /* Uses libcurl to convert this to a URL-encoded string, which can then
         * be passed to the browser directly. */
        std::string urlencode(void) const {
            auto curl = curl_easy_init();
            auto merge = _auth_uri + "?";
            for (const auto& pair: _info) {
                auto key = curl_easy_escape(curl, pair.first.c_str(), 0);
                auto val = curl_easy_escape(curl, pair.second.c_str(), 0);
                merge = merge + "&" + key + "=" + val;
                curl_free(key);
                curl_free(val);
            }
            curl_easy_cleanup(curl);
            return merge;
        }
    };
}

#endif
