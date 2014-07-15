
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

#include "sequence_number.h++"
#include <string>

namespace mhng {
    /* Stores a single MHng message.  */
    class message {
    private:

    public:

    public:
        /* Returns TRUE if this message is the current message in its
         * folder. */
        bool cur(void) const { return false; }

        /* Returns the sequence number associated with this
         * message. */
        int seq(void) const { return -1; }

        /* Returns the date this message was recieved. */
        std::string date(void) const { return "date"; }

        /* Returns the sender of this message. */
        std::string from(void) const { return "from"; }

        /* Returns the subject line of this message. */
        std::string subject(void) const { return "subject"; }
    };
}

#endif
