
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

#ifndef MHNG__FOLDER_HXX
#define MHNG__FOLDER_HXX

#include <memory>

namespace mhng {
    class folder;
    typedef std::shared_ptr<folder> folder_ptr;
}

#include "mailbox.h++"
#include "message.h++"
#include "promise.h++"
#include "sequence_number.h++"
#include "sqlite/connection.h++"
#include <string>

namespace mhng {
    /* Stores a single MHng folder.  */
    class folder {
    private:
        mailbox_ptr _mbox;

        /* The folder name, which is essentially used as a key to
         * fetch everything folder-related from the database. */
        std::string _name;

        /* Allows access to the current message. */
        promise<folder, message> _current_message;

        /* Allows access to all messages in this folder. */
        promise<folder, std::vector<message_ptr>> _messages;

    public:
        /* Creates a new folder handle.  Note that this database
         * handle will end up filling out information as it's
         * requested, so there aren't any synchronization guarantees
         * here.  Specifically that means you need to create a brand
         * new folder handle within a transaction if you want some
         * sort of synchronization here! */
        folder(const mailbox_ptr& mbox,
               const std::string name);

    public:
        /* Returns the name of this folder. */
        const std::string& name(void) const { return _name; }

        /* Returns the currently-selected message in this folder.
         * Note that you probably don't want to use this but instead
         * want to slurp the message list from an "args" class. */
        message_ptr current_message(void)
            { return _current_message; }

        /* Sets the current messagae in this folder. */
        void set_current_message(const message_ptr& message);

        /* Lists every message in the current folder. */
        /* FIXME: There must ba a better way to deal with this than
         * just copying the entire vector... */
        std::vector<message_ptr> messages(void)
            {
                std::shared_ptr<std::vector<message_ptr>> m = _messages;
                std::vector<message_ptr> out;
                for (auto it = m->begin(); it != m->end(); ++it)
                    out.push_back(*it);
                return out;
            }

        /* Opens a message, given the sequence number for that
         * message. */
        message_ptr open(const sequence_number_ptr& seq);

        /* Returns the path that's associated with this folder. */
        std::string full_path(void) const;

    private:
        static message_ptr _current_message_func(folder* f)
            { return f->_current_message_impl(); }
        message_ptr _current_message_impl(void);

        static std::shared_ptr<std::vector<message_ptr>>
        _messages_func(folder* f) { return f->_messages_impl(); }
        std::shared_ptr<std::vector<message_ptr>> _messages_impl(void);
    };
}

#endif
