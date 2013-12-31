
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

#ifndef LIBMHIMAP__CLIENT_HXX
#define LIBMHIMAP__CLIENT_HXX

namespace mhimap {
    class client;
    class command;
    class done_command;
}

#include "command.h++"
#include "folder_iter.h++"
#include "idle_response.h++"
#include "line_buffer.h++"
#include "string_iter.h++"
#include <string>

namespace mhimap {
    /* An IMAP client, which represents a client-side connection to an
     * IMAP server. */
    class client {
    public:
        /* Command needs access to read/write. */
        friend class command;
        friend class done_command;

    private:
        /* The current command sequence number.  This is simply
         * incremented for every command. */
        uint32_t sequence;

        /* Here's how we convert from read()/write()-like input into
         * the buffered input used by gets()/puts(). */
        line_buffer linebuf;

        /* FALSE until logout() has been called, TRUE after that. */
        bool logged_out;

    public:
        /* Creates a new connection to an IMAP server, given the
         * connection object that will be used to create it.  You
         * should probably be using one of the myriad of other client
         * classes that extend this one rather than calling this
         * directly. */
        client(void);

        /* Returns an iterator that loops through all the folders in
         * this IMAP account.  Note that this probably isn't very
         * useful because IMAP has some names that differ from what MH
         * expects. */
        virtual typename mhimap::folder_iter folder_iter(void);

        /* Returns an iterator that loops through all the messages in
         * the given folder. */
        string_iter message_iter(const folder f);

        /* Returns TRUE when */
        virtual bool is_connected(void) const = 0;

        /* Sends an IDLE command and DOES NOT wait for any response
         * from the server.  The given folder will be SELECTed before
         * issuing the IDLE, and the first + response that comes back
         * from the server will be eaten before returning, which
         * ensures that the IDLE has already made it to the server. */
        void send_idle(const std::string folder_name);

        /* Waits for a response from the server.  As far as I know
         * this really only makes sense to issue right after a
         * send_idle(), but I don't think that's actually required by
         * IMAP. */
        virtual idle_response wait_idle(void);

    protected:
        /* A pair of functions that do raw readinig and writing
         * from/to the IMAP socket.  These need to be implemented by
         * subclasses of this because their exact implementation will
         * depend on what sort of crypto is being done.  These really
         * shouldn't be called by anything but from within puts() and
         * gets(), as those have some buffer state that ends up being
         * managed and will conflict with calls to read() and write()
         * here. */
        virtual ssize_t read(char *buffer, ssize_t buffer_size) = 0;
        virtual ssize_t write(char *buffer, ssize_t buffer_size) = 0;

        /* Eats the welcome message from the IMAP server, essentially
         * resetting the state machine.  It's expected that subclasses
         * call this from within their constructor, before
         * authenticate() is called but (potentially) before any
         * security is established. */
        int eat_hello(void);

        /* Performs a login, using read() and write() as implemeted by
         * the subclasses of this class.  It's expected that
         * subclasses call this as part of their constructor, after
         * read() and write() can communicate securely.  This must be
         * called after eat_hello(), but doesn't have to be called
         * _directly_ after eat_helly() (for instance, there may be
         * some TLS negotiation in the middle). */
        int authenticate(const std::string user, const std::string pass);

        /* This should be called directly before an instance of this
         * class is destroyed, and only from direct children of this
         * class.  This handles a depedency issue: I'd really like to
         * have this log out in a destructor, but by that time the TLS
         * state will have been torn down so I can't. */
        void logout(void);

    private:
        /* These deal with buffered (but not necessairially
         * IMAP-complient) lines being shipped to and from the
         * server. */
        ssize_t puts(const char *buffer);
        ssize_t gets(char *buffer, ssize_t buffer_size);
        ssize_t printf(const char *format, ...)
            __attribute__(( format(printf, 2, 3) ));

        /* Looks at a line that came back from the server and figures
         * out if it was an error code or not. */
        bool was_error(char *buffer);

        /* Obtains a new sequence number for a command. */
        inline uint32_t next_sequence(void) { return sequence++; }
    };
}

#endif
