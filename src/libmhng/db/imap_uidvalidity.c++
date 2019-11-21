/* Copyright (C) 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "imap_uidvalidity.h++"
using namespace mhng;

static psqlite::table::ptr generate_columns(void);

db::imap_uidvalidity::imap_uidvalidity(const mailbox_ptr& mbox)
    : _table(generate_columns()),
      _mbox(mbox)
{
}

unknown<uint32_t> db::imap_uidvalidity::select(std::string folder,
                                               std::string account)
{
    auto resp = _mbox->db()->select(_table, "folder='%s' AND account='%s'",
                                    folder.c_str(),
                                    account.c_str()
        );

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        break;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }

    if (resp->result_count() > 1) {
        fprintf(stderr, "UNIQUE not respected\n");
        abort();
    }

    if (resp->result_count() == 0)
        return unknown<uint32_t>();

    auto row = resp->rowi(0);
    return row->get_uint("uidvalidity");
}

void db::imap_uidvalidity::update(std::string folder,
                                  uint32_t uidv,
                                  std::string account)
{
    auto map = std::map<std::string, std::string>();
    map["uidvalidity"] = std::to_string(uidv);
    auto row = std::make_shared<psqlite::row>(map);

    auto resp = _mbox->db()->replace(_table, row, "folder='%s' AND account='%s'",
                                     folder.c_str(),
                                     account.c_str());

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        return;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }
}

void db::imap_uidvalidity::insert(std::string folder,
                                  uint32_t uidv,
                                  std::string account)
{
    auto map = std::map<std::string, std::string>();
    map["folder"] = folder;
    map["uidvalidity"] = std::to_string(uidv);
    map["account"] = account;
    auto row = std::make_shared<psqlite::row>(map);

    auto resp = _mbox->db()->insert(_table, row);

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        return;
    case psqlite::error_code::FAILED_UNIQUE:
        fprintf(stderr, "FAILED_UNIQUE when inserting %s %u %s\n",
                folder.c_str(), uidv, account.c_str());
        abort();
        break;
    }
}

psqlite::table::ptr generate_columns(void)
{
    std::vector<psqlite::column::ptr> out;
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("folder"));
    out.push_back(std::make_shared<psqlite::column_t<uint32_t>>("uidvalidity"));
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("account"));
    return std::make_shared<psqlite::table>("IMAP__uidvalidity", out);
}
