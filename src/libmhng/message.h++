
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

#include "address.h++"
#include "date.h++"
#include "mailbox.h++"
#include "promise.h++"
#include "sequence_number.h++"
#include "mime/message.h++"
#include "sqlite/connection.h++"
#include <string>
#include <vector>

namespace mhng {
    /* Stores a single MHng message.  */
    class message {
    private:
        mailbox_ptr _mbox;

        /* These are all provided by SQLite, so it doesn't really cost
         * anything to just read them out right away. */
        const bool _cur;
        const sequence_number_ptr _seq;
        const folder_ptr _folder;
        const date_ptr _date;
        const address_ptr _from;
        const address_ptr _to;
        const std::string _subject;
        const std::string _uid;

        /* Contains the raw bytes of the message, parsed as strings
         * (one per line). */
        promise<message, std::vector<std::string>> _raw;

        /* This contains the message formatted as a MIME message. */
        promise<message, mime::message> _mime;

    public:
        /* Here's the sole way of creating a new message: with all the
         * data that's necessary in order to show it to a user.  This
         * means that messages can only ever be valid! */
        message(const mailbox_ptr& mbox,
                const sequence_number_ptr& seq,
                const folder_ptr& folder,
                const date_ptr& date,
                const address_ptr& from,
                const address_ptr& to,
                const std::string& subject,
                const std::string& uid);

    public:
        /* Accessors for the various database fields, these are all
         * fast. */
        bool cur(void) const { return _cur; }
        const sequence_number_ptr& seq(void) const { return _seq; }
        const folder_ptr& folder(void) const { return _folder; }
        const date_ptr& first_date(void) const { return _date; }
        const address_ptr& first_from(void) const { return _from; }
        const address_ptr& first_to(void) const { return _to; }
        const std::string& first_subject(void) const { return _subject; }
        const std::string& uid(void) const { return _uid; }

        /* Accessors for slower fields. */
        std::shared_ptr<std::vector<std::string>> raw_pointer(void)
            { return _raw; }
        std::vector<std::string> raw(void) { return *(raw_pointer()); }
        mime::message_ptr mime(void) { return _mime; }

        /* Removes a message from the store.  Note that this is a
         * somewhat dangerous operation: there could be a whole bunch
         * of pointers still sitting around (both in this process and
         * others).  Essentially this means that the fact that you've
         * got a pointer to a message doesn't really mean anything. */
        void remove(void);

        /* Allows access to _all_ the headers of particular types in a
         * message, not just the first one.  Note that this requires
         * reading and parsing the entire message, so it's fairly
         * slow!  If you're just going to throw out everything but the
         * first header then go ahead and use the fast accessors
         * above, but this is significantly safer... */
        std::vector<address_ptr> header_addr  (const std::string hdr);
        std::vector<std::string> header_string(const std::string hdr);
        std::vector<date_ptr>    header_date  (const std::string hdr);

        std::vector<address_ptr> from(void)
            { return header_addr("From"); }
        std::vector<address_ptr> to(void)
            { return header_addr("To"); }
        std::vector<address_ptr> cc(void)
            { return header_addr("CC"); }
        std::vector<address_ptr> bcc(void)
            { return header_addr("BCC"); }

        std::vector<date_ptr> date(void)
            { return header_date("Date"); }

        std::vector<std::string> subject(void)
            { return header_string("Subject"); }

        /* You shouldn't be using this directly but should instead be
         * using the other functions that parse these headers into a
         * format you actually care about. */
        std::vector<mime::header_ptr> header(const std::string hdr);

        /* Returns the body of the message, parsed as UTF-8 and
         * suitable for writing to stdout. */
        std::vector<std::string> body_utf8(void)
            { return mime()->body()->utf8(); }

    private:
        std::string full_path(void) const;

    private:
        static std::shared_ptr<std::vector<std::string>> _raw_func(message *m)
            { return m->_raw_impl(); }
        std::shared_ptr<std::vector<std::string>> _raw_impl(void);

        static std::shared_ptr<mime::message> _mime_func(message *m)
            { return m->_mime_impl(); }
        std::shared_ptr<mime::message> _mime_impl(void);
    };
}

#endif
