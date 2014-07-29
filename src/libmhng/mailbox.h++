
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

#ifndef MHNG__MAILBOX_HXX
#define MHNG__MAILBOX_HXX

#include <memory>

namespace mhng {
    class mailbox;
    typedef std::shared_ptr<mailbox> mailbox_ptr;
}

#include "folder.h++"
#include "mailrc.h++"
#include "message.h++"
#include "promise.h++"
#include "mime/message.h++"
#include "sqlite/connection.h++"
#include <string>

namespace mhng {
    /* Stores a single MHng mailbox.  This is almost certainly the
     * class you want to create, but it's really only possible to
     * create one from an "args" class in a sane manner. */
    class mailbox {
    private:
        /* Holds a path to the MH directory, in case anyone needs
         * access to it later. */
        const std::string _path;

        /* Holds a connection to the SQLite database. */
        sqlite::connection_ptr _db;

        /* Contains the current folder, which we need to look up if
         * asked for it. */
        promise<mailbox, folder> _current_folder;

        /* Contains a pointer to a mailrc, which is only filled out if
         * someone asks for it. */
        promise<mailbox, mailrc> _mailrc;

        /* Sometimes we need a back-pointer to ourselves. */
        std::weak_ptr<mailbox> _self_ptr;

    public:
        /* Creates a new mailbox, given the folder that contaians the
         * MHng information. */
        mailbox(const std::string& path);
        void set_self_pointer(const mailbox_ptr& self)
            { _self_ptr = self; }

    public:
        /* Returns the current folder, which involves a database
         * lookup. */
        folder_ptr current_folder(void) { return _current_folder; }

        /* Sets the current folder to be something else. */
        void set_current_folder(const folder_ptr& folder);

        /* Opens a folder by name. */
        folder_ptr open_folder(const std::string name) const;

        /* Returns the database connection that relates to this
         * mailbox. */
        sqlite::connection_ptr db(void) const
            { return _db; }

        /* Returns a copy of the mailrc, which is used for all sorts
         * of stuff! */
        mailrc_ptr mrc(void) { return _mailrc; }

        /* Inserts a new message into this mailbox. */
        message_ptr insert(const std::string& folder_name,
                           const mime::message_ptr& mime);

        message_ptr insert(const folder_ptr& folder,
                           const mime::message_ptr& mime,
                           uint32_t imapid);

        /* Clears the purge status of a message. */
        void did_purge(const folder_ptr& folder, uint32_t imapid);

    private:
        static folder_ptr _current_folder_func(mailbox *mbox)
            { return mbox->_current_folder_impl(); }
        folder_ptr _current_folder_impl(void);

        static mailrc_ptr _mailrc_func(mailbox *mbox)
            { return mbox->_mailrc_impl(); }
        mailrc_ptr _mailrc_impl(void);
    };
}

#endif
