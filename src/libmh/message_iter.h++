
/*
 * Copyright (C) 2013 Palmer Dabbelt
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

#ifndef LIBMH__MESSAGE_ITER_HXX
#define LIBMH__MESSAGE_ITER_HXX

namespace mh {
    class message_iter;
}

#include "folder.h++"
#include "message.h++"
#include <string>
#include <vector>

namespace mh {
    /* This deals with iterating through messages.  You're almost
     * certainly not going to want to create one of these yourself,
     * but instead are going to want get one from a folder. */
    class message_iter {
        friend class folder;

    private:
        std::vector<message> _messages;

    protected:
        message_iter(std::vector<message> messages);
    };
}

#endif
