
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

#include "mh_messages.h++"
using namespace mhng;

static sqlite::table_ptr generate_columns(void);

db::mh_messages::mh_messages(const mailbox_ptr& mbox)
    : _table(generate_columns()),
      _mbox(mbox)
{
}

message_ptr db::mh_messages::select(const std::string& folder_name,
                                    const sequence_number_ptr& seq)
{
    auto resp = _mbox->db()->select(_table, "folder='%s' AND seq='%u'",
                                    folder_name.c_str(), seq->to_uint());

    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
        break;

    default:
        return NULL;
        break;
    }

    if (resp->result_count() > 1) {
        fprintf(stderr, "UNIQUE not respected\n");
        abort();
    }

    if (resp->result_count() == 0)
        return NULL;

    auto row = resp->row(0);
    return std::make_shared<message>(
        _mbox,
        std::make_shared<sequence_number>(row->get_uint("seq")),
        _mbox->open_folder(folder_name),
        std::make_shared<date>(row->get_str("date")),
        _mbox->mrc()->email(row->get_str("fadr")),
        _mbox->mrc()->email(row->get_str("tadr")),
        row->get_str("subject"),
        row->get_str("uid")
        );
}

std::vector<message_ptr> db::mh_messages::select(const std::string& folder)
{
    auto resp = _mbox->db()->select(_table, "folder='%s'",
                                    folder.c_str());

    auto out = std::vector<message_ptr>();
    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
            break;
    }

    for (const auto& row: resp->rows()) {
        auto m =  std::make_shared<message>(
            _mbox,
            std::make_shared<sequence_number>(row->get_uint("seq")),
            _mbox->open_folder(folder),
            std::make_shared<date>(row->get_str("date")),
            _mbox->mrc()->email(row->get_str("fadr")),
            _mbox->mrc()->email(row->get_str("tadr")),
            row->get_str("subject"),
            row->get_str("uid")
            );
        out.push_back(m);
    }

    return out;
}

message_ptr db::mh_messages::select(const std::string& folder,
                                    const sequence_number_ptr& seq,
                                    int offset)
{
    auto resp = _mbox->db()->select(_table,
                                    "folder='%s' AND seq%s%u ORDER BY seq %s",
                                    folder.c_str(),
                                    (offset >= 0) ? ">=" : "<=",
                                    (int)(seq->to_uint()) + offset,
                                    (offset >= 0) ? "ASC" : "DESC"
        );

    
    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
        break;

    default:
        return NULL;
        break;
    }

    if (resp->result_count() == 0)
        return NULL;

    auto row = resp->row(0);
    return std::make_shared<message>(
        _mbox,
        std::make_shared<sequence_number>(row->get_uint("seq")),
        _mbox->open_folder(folder),
        std::make_shared<date>(row->get_str("date")),
        _mbox->mrc()->email(row->get_str("fadr")),
        _mbox->mrc()->email(row->get_str("tadr")),
        row->get_str("subject"),
        row->get_str("uid")
        );
}

void db::mh_messages::update(uint64_t uid,
                             const sequence_number_ptr& seq)
{
    auto map = std::map<std::string, std::string>();
    map["seq"] = std::to_string(seq->to_uint());
    auto row = std::make_shared<sqlite::row>(map);

    auto resp = _mbox->db()->replace(_table, row, "uid='%s'",
                                     std::to_string(uid).c_str());

    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
        return;
    }
}

void db::mh_messages::insert(unsigned seq,
                             std::string folder,
                             std::string date,
                             std::string from,
                             std::string to,
                             std::string subject,
                             uint64_t uid)
{
    auto map = std::map<std::string, std::string>();
    map["seq"] = std::to_string(seq);
    map["folder"] = folder;
    map["date"] = date;
    map["fadr"] = from;
    map["tadr"] = to;
    map["subject"] = subject;
    map["uid"] = std::to_string(uid);
    auto row = std::make_shared<sqlite::row>(map);

    auto resp = _mbox->db()->insert(_table, row);

    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
        return;
    }
}

void db::mh_messages::remove(uint64_t uid)
{
    auto resp = _mbox->db()->remove(_table, "uid='%lu'",
                                    uid);

    switch (resp->return_value()) {
    case sqlite::error_code::SUCCESS:
        break;
    }
}

sqlite::table_ptr generate_columns(void)
{
    std::vector<sqlite::column_ptr> out;
    out.push_back(std::make_shared<sqlite::column_t<std::string>>("uid"));
#if 0
    /* FIXME: Fix the database format! */
    out.push_back(std::make_shared<sqlite::column_t<bool>>("cur"));
#endif
    out.push_back(std::make_shared<sqlite::column_t<int>>("seq"));
    out.push_back(std::make_shared<sqlite::column_t<std::string>>("date"));
    out.push_back(std::make_shared<sqlite::column_t<std::string>>("fadr"));
    out.push_back(std::make_shared<sqlite::column_t<std::string>>("tadr"));
    out.push_back(std::make_shared<sqlite::column_t<std::string>>("subject"));
    return std::make_shared<sqlite::table>("MH__messages", out);
}
