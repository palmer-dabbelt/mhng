
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
#include "db/mh_messages.h++"
#include <unistd.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* Returns TRUE if the given message is the current message and FALSE
 * otherwise. */
static bool check_for_current(const mailbox_ptr& mbox,
                              const folder_ptr& folder,
                              const unsigned& seq);

message::message(const mailbox_ptr& mbox,
                 const sequence_number_ptr& seq,
                 const folder_ptr& folder,
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
      _uid(uid),
      _raw(this, _raw_func)
{
}

void message::remove(void)
{
    auto messages = std::make_shared<db::mh_messages>(_mbox);
    messages->remove(atoi(_uid.c_str()));

    int err = unlink(full_path().c_str());
    if (err < 0) {
        perror("Unable to remove message file");
        abort();
    }
}

std::string message::full_path(void) const
{
    char path[BUFFER_SIZE];
    snprintf(path, BUFFER_SIZE, "%s/%s",
             _folder->full_path().c_str(),
             _uid.c_str()
        );
    return path;
}

std::shared_ptr<std::vector<std::string>> message::_raw_impl(void)
{
    FILE *file = fopen(full_path().c_str(), "r");
    if (file == NULL) {
        fprintf(stderr, "Unable to open message: '%s'\n",
                full_path().c_str());
        abort();
    }

    auto out = std::make_shared<std::vector<std::string>>();
    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, file) != NULL)
        out->push_back(line);

    fclose(file);
    return out;
}

bool check_for_current(const mailbox_ptr& mbox,
                       const folder_ptr& folder,
                       const unsigned& seq)
{
    auto table = std::make_shared<db::mh_current>(mbox);
    return table->select(folder->name()) == seq;
}
