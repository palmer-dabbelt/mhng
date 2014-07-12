
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
#include "promise.h++"
#include "sqlite/connection.h++"
#include <string>

namespace mhng {
    /* Stores a single MHng mailbox.  This is almost certainly the
     * class you want to create, but it's really only possible to
     * create one from an "args" class in a sane manner. */
    class mailbox {
    private:
        /* Holds a connection to the SQLite database. */
        sqlite::connection_ptr _db;

        /* Contains the current. */
        promise<mailbox, folder> _current_folder;

    public:
        /* Creates a new mailbox, given the folder that contaians the
         * MHng information. */
        mailbox(const std::string& path);

    public:
        /* Returns the current folder, which involves a database
         * lookup. */
        folder_ptr current_folder(void) { return _current_folder; }

        /* Opens a folder by name. */
        folder_ptr open_folder(const std::string name) const;

    private:
        static folder_ptr _current_folder_func(mailbox *mbox)
            { return mbox->_current_folder_impl(); }
        folder_ptr _current_folder_impl(void);
    };
}

#endif
