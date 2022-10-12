/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "mh_accounts.h++"
#include "mh_oauth2cred.h++"
#include "mh_userpass.h++"
#include <cstring>
using namespace mhng;

static psqlite::table::ptr generate_columns(void);

db::mh_accounts::mh_accounts(const mailbox_ptr& mbox)
    : _table(generate_columns()),
      _mbox(mbox)
{
}

std::vector<account_ptr> db::mh_accounts::select(void)
{
    auto resp = _mbox->db()->select(_table);

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        break;
    case psqlite::error_code::FAILED_UNIQUE:
        return {};
    }

    auto out = std::vector<account_ptr>();
    for (const auto& row: resp->rows()) {
        if (strcmp(row->get_str("auth").c_str(), "userpass") == 0) {
            auto t = std::make_shared<db::mh_userpass>(_mbox);
            out.push_back(t->select(row->get_str("name")));
        } else if (strcmp(row->get_str("auth").c_str(), "oauth2") == 0) {
            auto t = std::make_shared<db::mh_oauth2cred>(_mbox);
            out.push_back(t->select(row->get_str("name")));
        } else
            abort();
    }
    return out;
}

account_ptr db::mh_accounts::select(std::string name)
{
    auto resp = _mbox->db()->select(_table, "name='%s'", name.c_str());

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        break;
    case psqlite::error_code::FAILED_UNIQUE:
        return {};
    }

    if (resp->rows().size() != 1)
        return nullptr;
    auto row = resp->rowi(0);

    if (strcmp(row->get_str("auth").c_str(), "userpass") == 0) {
        auto t = std::make_shared<db::mh_userpass>(_mbox);
        return t->select(row->get_str("name"));
    } else if (strcmp(row->get_str("auth").c_str(), "oauth2") == 0) {
        auto t = std::make_shared<db::mh_oauth2cred>(_mbox);
        return t->select(row->get_str("name"));
    } else
        abort();
}

void db::mh_accounts::insert(std::string name,
                             std::string auth)
{
    auto map = std::map<std::string, std::string>();
    map["name"] = name;
    map["auth"] = auth;
    auto row = std::make_shared<psqlite::row>(map);

    auto resp = _mbox->db()->insert(_table, row);

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        return;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        return;
    }
}

psqlite::table::ptr generate_columns(void)
{
    std::vector<psqlite::column::ptr> out;
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("name"));
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("auth"));
    return std::make_shared<psqlite::table>("MH__accounts", out);
}
