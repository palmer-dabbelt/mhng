/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBOAUTH2__ACCESS_TOKEN_HXX
#define LIBOAUTH2__ACCESS_TOKEN_HXX

#include <chrono>
#include <string>

namespace libmhoauth {
    class access_token {
    private:
        std::string _access_token;
        std::string _refresh_token;
        decltype(std::chrono::system_clock::now()) _expires_at;

    public:
        access_token(const std::string &a, const std::string &r, int delta)
        : _access_token(a),
          _refresh_token(r),
          _expires_at(std::chrono::system_clock::now() + std::chrono::seconds(delta))
        {}

        access_token(const std::string &a, const std::string &r, const decltype(_expires_at)& e)
        : _access_token(a),
          _refresh_token(r),
          _expires_at(e)
        {}

        access_token(const access_token& that)
        : _access_token(that._access_token),
          _refresh_token(that._refresh_token),
          _expires_at(that._expires_at)
        {}

    public:
        const auto& value(void) const { return _access_token; }
        const auto& refresh_token(void) const { return _refresh_token; }
        const auto& expires_at(void) const { return _expires_at; }

    public:
        access_token operator=(const access_token& that)
        {
            _access_token = that._access_token;
            _refresh_token = that._refresh_token;
            _expires_at = that._expires_at;
            return *this;
        }
    };
}

#endif
