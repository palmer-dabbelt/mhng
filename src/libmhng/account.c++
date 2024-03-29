/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "account.h++"
#include "db/mh_oauth2cred.h++"
#include "mime/base64.h++"
using namespace mhng;

libmhoauth::access_token account::refresh(void)
{
    if (!_oauth2.has_value())
        abort();

    auto ref = libmhoauth::refresh(
        (*_oauth2).client_id,
        "https://oauth2.googleapis.com/token",
        access_token().refresh_token()
    );

    auto ts = [&](){
        auto tp = ref.expires_at();
        auto ss = std::chrono::time_point_cast<std::chrono::seconds>(tp);
        auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(tp)
                  - std::chrono::time_point_cast<std::chrono::nanoseconds>(ss);
        timespec out;
        out.tv_sec = ss.time_since_epoch().count();
        out.tv_nsec = ns.count();
        return out;
    }(); 

    auto accounts = std::make_shared<db::mh_oauth2cred>(_mbox);
    accounts->update(
        name(),
        ref.value(),
        ref.refresh_token(),
        putil::chrono::datetime(ts)
    );

    (*_oauth2).access_token = ref;

    return ref;
}

const libmhoauth::access_token& account::access_token(void) const
{
    return _oauth2.value().access_token;
}

std::string account::password(void) const
{
    return _userpass.value().pass;
}
