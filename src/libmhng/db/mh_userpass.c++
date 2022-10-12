/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "mh_userpass.h++"
using namespace mhng;

static psqlite::table::ptr generate_columns(void);

db::mh_userpass::mh_userpass(const mailbox_ptr& mbox)
    : _table(generate_columns()),
      _mbox(mbox)
{
}

account_ptr db::mh_userpass::select(std::string name)
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
        row->get_str("pass")
    );
}


void db::mh_userpass::insert(std::string name,
                             std::string pass)
{
	auto map = std::map<std::string, std::string>();
    map["name"] = name;
    map["pass"] = pass;
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
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("pass"));
    return std::make_shared<psqlite::table>("MH__userpass", out);
}
