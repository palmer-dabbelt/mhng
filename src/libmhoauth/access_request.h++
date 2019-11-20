/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBOAUT2__ACCESS_REQUEST_HXX
#define LIBOAUT2__ACCESS_REQUEST_HXX

#include <libmhoauth/authorization_code.h++>
#include <map>
#include <string>
#include <curl/curl.h>

namespace libmhoauth {
    class access_request {
    private:
        const std::map<std::string, std::string> _headers;
        const std::map<std::string, std::string> _body;

    private:
        static decltype(_headers) mkmap(void) { return {}; }

        template<typename... A>
        static decltype(_headers) mkmap(decltype(_headers)::key_type key,
                                        decltype(_headers)::mapped_type value,
                                        A... args) {
            auto out = mkmap(std::forward<A>(args)...);
            out[key] = value;
            return out;
        }

    public:
        access_request(std::string client_id,
                       authorization_code auth_code)
        : _headers(mkmap("Content-type", "application/x-www-form-urlencoded")),
          _body(mkmap("grant_type", "authorization_code",
                      "client_id", client_id,
                      "code_verifier", auth_code.verifier().plain(),
                      "code", auth_code.value(),
                      "redirect_uri", auth_code.redirect_uri()
                      ))
        {}

    public:
        /* These format the access request as a HTTP request, which is
         * essentially the whole point of doing this. */
        const decltype(_headers)& headers(void) const { return _headers; }

        /* Uses libcurl to convert this to a URL-encoded string, which can then
         * be passed to the browser directly. */
        std::string body(void) const {
            auto curl = curl_easy_init();
            auto merge = std::string();
            for (const auto& pair: _body) {
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

