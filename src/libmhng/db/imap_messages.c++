
/*
 * Copyright (C) 2014 Palmer Dabbelt
 *   <palmer@dabbelt.com>
 *
 * This file is part of mhng.
 *
 * mhng is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mhng is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with mhng.  If not, see <http://www.gnu.org/licenses/>.
 */

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
                                   uint32_t imapid)
{
    auto resp = _mbox->db()->select(_table, "uid='%s' AND folder='%s'",
                                    std::to_string(imapid).c_str(),
                                    folder.c_str()
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

void db::imap_messages::remove(std::string folder, uint32_t imapid)
{
    auto resp = _mbox->db()->remove(_table, "folder='%s' AND uid=%u",
                                    folder.c_str(),
                                    imapid);

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

std::vector<uint32_t> db::imap_messages::select_purge(std::string folder)
{
    auto resp = _mbox->db()->select(_table, "folder='%s' AND purge=1",
                                    folder.c_str());

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
                               uint64_t mhid)
{
    auto map = std::map<std::string, std::string>();
    map["folder"] = folder;
    map["uid"] = std::to_string(imapid);
    map["purge"] = "0";
    map["mhid"] = std::to_string(mhid);
    auto row = std::make_shared<psqlite::row>(map);

    auto resp = _mbox->db()->insert(_table, row);

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
    out.push_back(std::make_shared<psqlite::column_t<std::string>>("mhid"));
    out.push_back(std::make_shared<psqlite::column_t<bool>>("purge"));
    out.push_back(std::make_shared<psqlite::column_t<uint32_t>>("uid"));
    out.push_back(std::make_shared<psqlite::column_t<uint32_t>>("folder"));
    return std::make_shared<psqlite::table>("IMAP__messages", out);
}
