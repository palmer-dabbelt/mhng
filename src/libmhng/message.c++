
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

#include "message.h++"
#include "db/mh_current.h++"
using namespace mhng;

/* Returns TRUE if the given message is the current message and FALSE
 * otherwise. */
static bool check_for_current(const mailbox_ptr& mbox,
                              const std::string& table,
                              const unsigned& seq);

message::message(const mailbox_ptr& mbox,
                 const sequence_number_ptr& seq,
                 const std::string& folder,
                 const date_ptr& date,
                 const address_ptr& from,
                 const std::string& subject,
                 const std::string& uid)
    : _mbox(mbox),
      _cur(check_for_current(mbox, folder, seq->to_uint())),
      _seq(seq),
      _folder(folder),
      _date(date),
      _from(from),
      _subject(subject),
      _uid(uid)
{
}

bool check_for_current(const mailbox_ptr& mbox,
                       const std::string& folder,
                       const unsigned& seq)
{
    auto table = std::make_shared<db::mh_current>(mbox);
    return table->select(folder) == seq;
}
