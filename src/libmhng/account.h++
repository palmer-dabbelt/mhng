/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__ACCOUNT_HXX
#define MHNG__ACCOUNT_HXX

#include <libmhoauth/refresh.h++>
#include <memory>

namespace mhng {
	class account;
	typedef std::shared_ptr<account> account_ptr;
}

#include "mailbox.h++"
#include <string>

namespace mhng {
    /* Stores an account, which is implicitly an oauth2 account. */
	class account {
    private:
        const mailbox_ptr _mbox;
        const std::string _name;
        const std::string _client_id;
        libmhoauth::access_token _access_token;

    public:
        account(const mailbox_ptr& mbox,
                std::string name,
                std::string client_id,
                const libmhoauth::access_token& access_token)
        : _mbox(mbox),
          _name(name),
          _client_id(client_id),
          _access_token(access_token)
        {}
    
    public:
        const decltype(_name)& name(void) const { return _name; }
        const decltype(_access_token)& access_token(void) const { return _access_token; }
        libmhoauth::access_token refresh(void);
        std::string sasl(void) const;
    };
}

#endif
