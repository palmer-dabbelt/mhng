
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

#include "message.h++"
#include "promise.h++"
#include <string>

namespace mhng {
    /* Stores a single MHng folder.  */
    class folder {
    private:
        /* The folder name, which is essentially used as a key to
         * fetch everything folder-related from the database. */
        std::string _name;

        /* Allows access to the current message. */
        promise<folder, message> _current_message;

    public:
        /* Returns the name of this folder. */
        const std::string& name(void) const { return _name; }

        /* Returns the currently-selected message in this folder.
         * Note that you probably don't want to use this but instead
         * want to slurp the message list from an "args" class. */
        message_ptr current_message(void) const;

    private:
        static message_ptr _current_message_func(folder* f)
            { return f->_current_message_impl(); }
        message_ptr _current_message_impl(void);
    };
}

#endif
