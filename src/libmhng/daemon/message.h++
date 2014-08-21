
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

#ifndef LIBMHNG__DAEMON__MESSAGE_HXX
#define LIBMHNG__DAEMON__MESSAGE_HXX

#include <memory>

namespace mhng {
    namespace daemon {
        class message;
        typedef std::shared_ptr<message> message_ptr;
    }
}

#include "message_type.h++"
#include <stdint.h>

namespace mhng {
    namespace daemon {
        struct message_on_wire {
            uint32_t id;

            /* FIXME: This should be a union... */
            uint32_t type;
            struct {
                int32_t seconds_since;
            } sync;
        };

        /* Holds a single message to/from the daemon. */
        class message {
        private:
            struct message_on_wire wire;

        public:
            /* Initializes a new message.  Note that this itself isn't
             * particularly useful, it's just public because
             * std::shared_ptr wants it.  You should instead use the
             * nice static creation functions below which will allow
             * you to actually have a full message. */
            message(message_type type);

            /* Initializes a new response message, which just takes
             * the ID. */
            message(uint32_t id);

        public:
            /* Obtains the unique ID for this message. */
            uint32_t id(void) const { return wire.id; }

            /* Returns the type of this message. */
            message_type type(void) const
                { return uint2message_type(wire.type); }

            /* Returns a message that is the respones for this current
             * message. */
            message_ptr response(void) const;

        public:
            /* Creates a new message that requests a full inbox
             * synchronization.  The server will return a message that
             * indicates what happened, but you're welcome to ignore
             * it if you don't want to block.  The "seconds_since"
             * argument will force a sync if none has been performed
             * recently (defined as the argument), and by default
             * always forces a sync. */ 
            static message_ptr sync(int32_t seconds_since = -1);

            /* Creates a new message that informs the server that the
             * network has turned either off or on.  Sending these
             * messages to the server will cause it to kill whatever
             * subprocesses may have hung due to this condition. */
            static message_ptr net_up(void);
            static message_ptr net_down(void);

        public:
            /* Converts between raw buffers and parsed messages. */
            static size_t serialize(const message_ptr& msg,
                                    char *buf, size_t len);
            static message_ptr deserialize(const char *buf, size_t len);
        };
    }
}

#endif
