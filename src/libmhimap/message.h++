/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHIMAP__MESSAGE_HXX
#define LIBMHIMAP__MESSAGE_HXX

namespace mhimap {
    class message;
}

#include "folder.h++"
#include <stdint.h>
#include <string>

namespace mhimap {
    /* References a single IMAP message. */
    class message {
    private:
        /* Each message lives within a single folder. */
        const folder _folder;

        /* Each message contains a single immutable identifier. */
        const uint32_t _uid;

    public:
        /* Copy constructor... */
        message(const message &that)
            : _folder(that._folder), _uid(that._uid)
            {
            }

        /* Creates a new message given the two options that define it
         * -- essentially a message is just a container for this
         * pair. */
        message(const folder folder, const uint32_t uid);

        /* Accessor functions. */
        uint32_t uid(void) const { return _uid; }
        const std::string folder_name(void) const { return _folder.name(); }

        /* Returns the folder. */
        const folder& owning_folder(void) const { return _folder; }

        /* Creates a new message that is exactly the same as this
         * message but in a different folder.  This is useful for IMAP
         * servers that name their messages wrong. */
        message to_folder(folder f) const { return message(f, _uid); }
    };
}

#endif
