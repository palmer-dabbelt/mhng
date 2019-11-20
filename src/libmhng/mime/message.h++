/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__MIME__MESSAGE_HXX
#define MHNG__MIME__MESSAGE_HXX

#include <memory>

namespace mhng {
    namespace mime {
        class message;
        typedef std::shared_ptr<message> message_ptr;
    }
}

#include "header.h++"
#include "part.h++"
#include <map>
#include <string>
#include <vector>

namespace mhng {
    namespace mime {
        /* Stores a single MIME message, which itself is actually just
         * a container around a root MIME object that deals with the
         * fact that some email messages aren't actually in MIME
         * format. */
        class message {
        private:
            /* This is the root MIME part of this message. */
            part_ptr _root;

            /* A lookup table that allows us to easily seach the list
             * of headers for one that matches a string. */
            std::multimap<std::string, header_ptr> _headers;

        public:
            /* Creates a MIME message, given the raw text of the
             * messages as dumped by IMAP. */
            message(const std::vector<std::string>& raw);

        public:
            /* Returns a pointer to the root of the MIME message, in
             * case someone wants to walk the tree directly. */
            const part_ptr& root(void) const { return _root; }

            /* Returns all the header that match the given string in
             * the top-level MIME message.  Note that these seaches
             * are all case insensitive. */
            std::vector<header_ptr> header(const std::string name) const;

            /* Searches for a body for this message, returning what is
             * found. */
            part_ptr body(void) const;

            /* Searches for the signature attached to a message. */
            part_ptr signature(void) const;

            /* Adds a new header to the root of this message. */
            void add_header(const header_ptr& header);
            void add_header(const std::string& key,
                            const std::string& value);
        };
    }
}

#endif
