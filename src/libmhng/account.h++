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
#include "unknown.h++"
#include <string>

namespace mhng {
    /*
     * Stores an account, which may be either an oauth2 account or a user/pass
     * account.
     */
    class account {
    private:
        class oauth2 {
        public:
            const std::string client_id;
            libmhoauth::access_token access_token;

            oauth2(const decltype(client_id)& _client_id,
                   const decltype(access_token)& _access_token)
            : client_id(_client_id),
              access_token(_access_token)
            {}
        };

        class userpass {
            const std::string user;
            const std::string pass;
        };
    private:
        const mailbox_ptr _mbox;
        const std::string _name;

        unknown<oauth2> _oauth2;
        unknown<userpass> _userpass;

    public:
        account(const mailbox_ptr& mbox,
                std::string name,
                std::string client_id,
                const libmhoauth::access_token& access_token)
        : _mbox(mbox),
          _name(name),
          _oauth2(oauth2(client_id, access_token))
        {}
    
    public:
        const decltype(_name)& name(void) const { return _name; }
        const libmhoauth::access_token& access_token(void) const;
        libmhoauth::access_token refresh(void);
        std::string sasl(void) const;
    };
}

#endif
