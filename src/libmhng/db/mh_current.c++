/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "mh_current.h++"
using namespace mhng;

static psqlite::table::ptr generate_columns(void);

db::mh_current::mh_current(const mailbox_ptr& mbox)
    : _table(generate_columns()),
      _mbox(mbox)
{
}

std::vector<std::string> db::mh_current::select(void)
{
    auto resp = _mbox->db()->select(_table);

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        break;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }

    std::vector<std::string> out;
    for (const auto& row: resp->rows())
        out.push_back(row->get_str("folder"));
    return out;
}

unsigned db::mh_current::select(const std::string& folder)
{
    auto resp = _mbox->db()->select(_table, "folder='%s'",
                                    folder.c_str());

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
            break;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }

    if (resp->result_count() != 1)
        return -1;

    auto row = resp->rowi(0);
    return row->get_uint("seq");
}

void db::mh_current::update(const std::string& folder,
                            unsigned seq)
{
    auto map = std::map<std::string, std::string>();
    map["seq"] = std::to_string(seq);
    auto row = std::make_shared<psqlite::row>(map);

    auto resp = _mbox->db()->replace(_table, row, "folder = '%s'",
                                     folder.c_str());

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        return;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        return;
    }
}

void db::mh_current::clear_current(const std::string& folder_name)
{
    std::vector<std::string> map = {"cur"};

    auto resp = _mbox->db()->clear(_table, map, "folder = '%s'",
                                     folder_name.c_str());

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        return;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }
}

void db::mh_current::set_current(const std::string& folder_name)
{
    auto map = std::map<std::string, std::string>();
    map["cur"] = std::to_string(1);
    auto row = std::make_shared<psqlite::row>(map);

    auto resp = _mbox->db()->replace(_table, row, "folder = '%s'",
                                     folder_name.c_str());

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        return;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }
}

std::string db::mh_current::select_current(void)
{
    auto resp = _mbox->db()->select(_table, "cur=1");

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
            break;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }

    if (resp->result_count() != 1)
        abort();

    auto row = resp->rowi(0);
    return row->get_str("folder");
}

int64_t db::mh_current::uid_validity(const std::string& folder_name)
{
    auto resp = _mbox->db()->select(_table, "folder='%s'",
                                    folder_name.c_str());

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
            break;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }

    if (resp->result_count() != 1)
        return -1;

    auto row = resp->rowi(0);

    if (row->has("uid_validity") == false)
        return -1;

    return row->get_uint("uid_validity");
}

void db::mh_current::update_uid_validity(const std::string& folder_name,
                                         uint32_t uid_validity)
{
    auto map = std::map<std::string, std::string>();
    map["uid_validity"] = std::to_string(uid_validity);
    auto row = std::make_shared<psqlite::row>(map);

    auto resp = _mbox->db()->replace(_table, row, "folder = '%s'",
                                     folder_name.c_str());

    switch (resp->return_value()) {
    case psqlite::error_code::SUCCESS:
        return;
    case psqlite::error_code::FAILED_UNIQUE:
        abort();
        break;
    }
}

psqlite::table::ptr generate_columns(void)
{
    std::vector<psqlite::column::ptr> out;
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("folder"));
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("seq"));
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("cur"));
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("uid_validity"));
    return std::make_shared<psqlite::table>("MH__current", out);
}
