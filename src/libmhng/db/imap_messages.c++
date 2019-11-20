/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "imap_messages.h++"
using namespace mhng;

static psqlite::table::ptr generate_columns(void);

db::imap_messages::imap_messages(const mailbox_ptr& mbox)
    : _table(generate_columns()),
      _mbox(mbox)
{
}

int64_t db::imap_messages::select(uint64_t uid)
{
    auto resp = _mbox->db()->select(_table, "mhid='%s'",
                                    std::to_string(uid).c_str());

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

    if (resp->result_count() != 1)
        return -1;

    auto row = resp->rowi(0);
    return row->get_uint("uid");
}

uint64_t db::imap_messages::select(std::string folder,
                                   uint32_t imapid,
                                   std::string account)
{
    auto resp = _mbox->db()->select(_table, "uid='%s' AND folder='%s' AND account='%s'",
                                    std::to_string(imapid).c_str(),
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

    if (resp->result_count() != 1)
        return -1;

    auto row = resp->rowi(0);
    return row->get_uint("mhid");
}

void db::imap_messages::remove(uint64_t mhid)
{
    auto resp = _mbox->db()->remove(_table, "mhid='%llu'",
                                    (long long unsigned)mhid);

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        break;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }
}

void db::imap_messages::remove(std::string folder, uint32_t imapid, std::string account)
{
    auto resp = _mbox->db()->remove(_table, "folder='%s' AND uid='%u' AND account='%s'",
                                    folder.c_str(),
                                    imapid,
                                    account.c_str());

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        break;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }
}

void db::imap_messages::update_purge(uint64_t uid, bool purge)
{
    auto map = std::map<std::string, std::string>();
    map["purge"] = purge ? "1" : "0";
    auto row = std::make_shared<psqlite::row>(map);

    auto resp = _mbox->db()->replace(_table, row, "mhid = %llu",
				     (long long unsigned)uid);

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        return;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }
}

std::vector<uint32_t> db::imap_messages::select_purge(std::string folder, std::string account)
{
    auto resp = _mbox->db()->select(_table, "folder='%s' AND purge=1 AND account='%s'",
                                    folder.c_str(),
                                    account.c_str());

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        break;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }

    std::vector<uint32_t> out;
    for (const auto& row : resp->rows())
        out.push_back(row->get_uint("uid"));
    return out;
}

void db::imap_messages::insert(std::string folder,
                               uint32_t imapid,
                               uint64_t mhid,
                               std::string account)
{
    auto map = std::map<std::string, std::string>();
    map["folder"] = folder;
    map["uid"] = std::to_string(imapid);
    map["purge"] = "0";
    map["mhid"] = std::to_string(mhid);
    map["account"] = account;
    auto row = std::make_shared<psqlite::row>(map);

    auto resp = _mbox->db()->insert(_table, row);

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        return;
    case psqlite::error_code::FAILED_UNIQUE:
        fprintf(stderr, "FAILED_UNIQUE when inserting %u %llu\n",
                imapid, (long long unsigned)mhid);
        abort();
        break;
    }
}

std::string db::imap_messages::select_account(uint64_t uid)
{
    auto resp = _mbox->db()->select(_table, "mhid='%llu'", (long long unsigned)uid);

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

    if (resp->result_count() != 1) {
        fprintf(stderr, "select_account() with no account\n");
        abort();
    }

    return resp->rowi(0)->get_str("account");
}

psqlite::table::ptr generate_columns(void)
{
    std::vector<psqlite::column::ptr> out;
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("mhid"));
    out.push_back(std::make_shared<psqlite::column_t<bool>>("purge"));
    out.push_back(std::make_shared<psqlite::column_t<uint32_t>>("uid"));
    out.push_back(std::make_shared<psqlite::column_t<uint32_t>>("folder"));
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("account"));
    return std::make_shared<psqlite::table>("IMAP__messages", out);
}
