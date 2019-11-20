/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "mh_accounts.h++"
using namespace mhng;

static psqlite::table::ptr generate_columns(void);

static auto get_chrono(std::string time)
{
    auto mhng = mhng::date(time);
    return std::chrono::system_clock::from_time_t(mhng.unix());
}

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
        out.push_back(
            std::make_shared<account>(
                _mbox,
                row->get_str("name"),
                row->get_str("client_id"),
                libmhoauth::access_token(
                    row->get_str("access_token"),
                    row->get_str("refresh_token"),
                    get_chrono(row->get_str("access_token_expires"))
                )
            )
        );
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

    return std::make_shared<account>(
        _mbox,
        row->get_str("name"),
        row->get_str("client_id"),
        libmhoauth::access_token(
            row->get_str("access_token"),
            row->get_str("refresh_token"),
            get_chrono(row->get_str("access_token_expires"))
        )
    );
}

void db::mh_accounts::insert(std::string name,
                             std::string client_id,
                             std::string access_token,
                             std::string refresh_token,
                             putil::chrono::datetime expires_at)
{
	auto map = std::map<std::string, std::string>();
    map["name"] = name;
    map["client_id"] = client_id;
    map["access_token"] = access_token;
    map["access_token_expires"] = expires_at.gm();
    map["refresh_token"] = refresh_token;
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

void db::mh_accounts::update(std::string name,
                             std::string access_token,
                             std::string refresh_token,
                             putil::chrono::datetime expires_at)
{
    auto map = std::map<std::string, std::string>();
    map["access_token"] = access_token;
    map["refresh_token"] = refresh_token;
    map["access_token_expires"] = expires_at.gm();
    auto row = std::make_shared<psqlite::row>(map);

    auto resp = _mbox->db()->replace(_table, row, "name='%s'",
                                     name.c_str());

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
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("client_id"));
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("access_token"));
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("refresh_token"));
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("access_token_expires"));
    return std::make_shared<psqlite::table>("MH__accounts", out);
}
