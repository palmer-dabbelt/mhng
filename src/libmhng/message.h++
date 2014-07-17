
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

#ifndef MHNG__MESSAGE_HXX
#define MHNG__MESSAGE_HXX

#include <memory>

namespace mhng {
    class message;
    typedef std::shared_ptr<message> message_ptr;
}

#include "date.h++"
#include "sequence_number.h++"
#include "sqlite/connection.h++"
#include <string>

namespace mhng {
    /* Stores a single MHng message.  */
    class message {
    private:
        std::shared_ptr<sqlite::connection> _db;
        const bool _cur;
        const sequence_number_ptr _seq;
        const std::string _folder;
        const date_ptr _date;
        const std::string _from;
        const std::string _subject;
        const std::string _uid;

    public:
        /* Here's the sole way of creating a new message: with all the
         * data that's necessary in order to show it to a user.  This
         * means that messages can only ever be valid! */
        message(const sqlite::connection_ptr& _db,
                const sequence_number_ptr& seq,
                const std::string& folder,
                const date_ptr& date,
                const std::string& from,
                const std::string& subject,
                const std::string& uid);

    public:
        /* Accessors for the various database fields, these are all
         * fast. */
        bool cur(void) const { return _cur; }
        const sequence_number_ptr& seq(void) const { return _seq; }
        const std::string& folder(void) const { return _folder; }
        const date_ptr& date(void) const { return _date; }
        const std::string& from(void) const { return _from; }
        const std::string& subject(void) const { return _subject; }
        const std::string& uid(void) const { return _uid; }
    };
}

#endif
