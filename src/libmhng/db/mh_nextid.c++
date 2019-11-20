/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "mh_nextid.h++"
using namespace mhng;

static psqlite::table::ptr generate_columns(void);

db::mh_nextid::mh_nextid(const mailbox_ptr& mbox)
    : _table(generate_columns()),
      _mbox(mbox)
{
}

uint64_t db::mh_nextid::select(void)
{
    auto resp = _mbox->db()->select(_table);

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
    return row->get_uint("uid");
}

void db::mh_nextid::update(uint64_t id)
{
    auto map = std::map<std::string, std::string>();
    map["uid"] = std::to_string(id);
    auto row = std::make_shared<psqlite::row>(map);

    auto resp = _mbox->db()->replace(_table, row, "single=1");

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
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("uid"));
    return std::make_shared<psqlite::table>("MH__nextid", out);
}
