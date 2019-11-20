/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__MAILBOX_HXX
#define MHNG__MAILBOX_HXX

#include <memory>

namespace mhng {
    class mailbox;
    typedef std::shared_ptr<mailbox> mailbox_ptr;
}

#include "account.h++"
#include "folder.h++"
#include "mailrc.h++"
#include "message.h++"
#include "promise.h++"
#include "daemon/connection.h++"
#include "mime/message.h++"
#include <psqlite/connection.h++>
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

        /* This is TRUE if we shouldn't be building a mailrc. */
        const bool _nomailrc;

        /* Holds a connection to the SQLite database. */
        psqlite::connection::ptr _db;

        /* Contains the current folder, which we need to look up if
         * asked for it. */
        promise<mailbox, folder> _current_folder;

        /* Contains a pointer to a mailrc, which is only filled out if
         * someone asks for it. */
        promise<mailbox, mailrc> _mailrc;

        /* Contains a pointer to the daemon connection.  Note that
         * this must only be filled out when someone asks for it,
         * because the daemon itself can use this class!  That means
         * you CANNOT use this anywhere from the daemon... */
        promise<mailbox, daemon::connection> _daemon;

        /* Sometimes we need a back-pointer to ourselves. */
        std::weak_ptr<mailbox> _self_ptr;

        /* All the accounts. */
        promise<mailbox, std::vector<account_ptr>> _accounts;

    public:
        /* Creates a new mailbox, given the folder that contaians the
         * MHng information. */
        mailbox(const std::string& path, bool nomailrc);
        void set_self_pointer(const mailbox_ptr& self)
            { _self_ptr = self; }

    public:
        /* Returns the path to the directory that contains this
         * maildir.  You probably shouldn't use this... */
        const std::string& path(void) { return _path; }

        /* Returns the current folder, which involves a database
         * lookup. */
        folder_ptr current_folder(void) { return _current_folder; }

        /* Returns every account that's currently supported. */
        std::vector<account_ptr> accounts(void);

        /* Sets the current folder to be something else. */
        void set_current_folder(const folder_ptr& folder);

        /* Opens a folder by name. */
        folder_ptr open_folder(const std::string name) const;

        /* Returns the database connection that relates to this
         * mailbox. */
        psqlite::connection::ptr db(void) const
            { return _db; }

        /* Returns a copy of the mailrc, which is used for all sorts
         * of stuff! */
        mailrc_ptr mrc(void) { return _mailrc; }

        /* Inserts a new message into this mailbox. */
        message_ptr insert(const std::string& folder_name,
                           const mime::message_ptr& mime);

        message_ptr insert(const folder_ptr& folder,
                           const mime::message_ptr& mime,
                           uint32_t imapid,
                           const std::string account);

        /* Clears the purge status of a message. */
        void did_purge(const folder_ptr& folder, uint32_t imapid);

        /* Returns the single daemon connection that everything is
         * multiplexed over. */
        daemon::connection_ptr daemon(void) { return _daemon; }

        /* Returns the largest UID in this entire mailbox. */
        uint64_t largest_uid(void) const;
        uint64_t largest_uid(const std::string folder) const;

        /* Opens a new message by UID, returning NULL if it doesn't
         * exist. */
        message_ptr open(uint64_t uid) const;

        /* Adds a new account.  For now this is only gmail.  In theory you can
         * do other stuff after calling this, but nothing is going to test
         * that. */
        void add_account(const std::string& name) const;

        /* Obtains the account with the given name. */
        account_ptr account(const std::string& name);

    private:
        static folder_ptr _current_folder_func(mailbox *mbox)
            { return mbox->_current_folder_impl(); }
        folder_ptr _current_folder_impl(void);

        static mailrc_ptr _mailrc_func(mailbox *mbox)
            { return mbox->_mailrc_impl(); }
        mailrc_ptr _mailrc_impl(void);

        static daemon::connection_ptr _daemon_func(mailbox *mbox)
            { return mbox->_daemon_impl(); }
        daemon::connection_ptr _daemon_impl(void);

        static std::shared_ptr<std::vector<account_ptr>> _accounts_func(mailbox *mbox)
            { return mbox->_accounts_impl(); }
        std::shared_ptr<std::vector<account_ptr>> _accounts_impl(void);
    };
}

#endif
