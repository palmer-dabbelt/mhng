/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHIMAP__FOLDER_HXX
#define LIBMHIMAP__FOLDER_HXX

namespace mhimap {
    class folder;
    class client;
}

#include <string>

namespace mhimap {
    /* References a single IMAP folder (called mailboxes in the proper
     * IMAP spec).  This wraps the response of an IMAP SELECT command
     * up so code outside the library doesn't have to be fully exposed
     * to that.  */
    class folder {
    private:
        /* Stores the full name (starting from the mailbox root) of
         * this folder. */
        const std::string _name;

        /* Stores the UIDVALIDITY as returned from the IMAP server
         * during SELECT. */
        uint32_t _uidvalidity;

    public:
        /* Creates a new folder by communicating with the IMAP server
         * and fetching some values from it.  This does a SELECT. */
        folder(const std::string name, client *c);

        /* Produces a new folder, which is a copy of this folder but
         * with the name changed. */
        folder rename(const std::string new_name) const;

        /* Accessor functions. */
        const std::string name(void) const { return _name; }
        uint32_t uidvalidity(void) const { return _uidvalidity; }

    private:
        /* Creates a new folder given all the information necessary
         * directly.  This DOES NOT contact the IMAP server at all. */
        folder(const std::string name,
               uint32_t uidvalidity);
    };
}

#endif
