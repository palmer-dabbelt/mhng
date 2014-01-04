
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

#ifndef LIBMH__MIME_HXX
#define LIBMH__MIME_HXX

namespace mh {
    class mime;
    class message_file;
}

#include "string_iter.h++"
#include <string>

namespace mh {
    /* Represents a single MIME part of a message. */
    class mime {
    public:
        friend class message_file;

    private:
        /* Stores the value that is stuck into MIME that represents
         * the different sorts of boundaries that need to be
         * detected. */
        std::string _boundary_child;
        std::string _boundary_sibling;
        std::string _boundary_parent;

        /* Set to TRUE when this is parsing MIME headers, and FALSE
         * when it is in the body. */
        bool _in_headers;

        /* Holds the content type of this part, as well as the
         * encoding of that value. */
        std::string _type;
        std::string _encoding;

        /* A pointer to the parent of this node.  Note that this is
         * unsafe, you really shouldn't go removing parts of the
         * tree... */
        mime *_parent;

        /* A vector of all the children of this node. */
        std::vector<mime *> _children;

        /* The list of all lines in the body of this message. */
        std::vector<std::string> _body;

        /* Yet another header parser... */
        std::string _hname, _hcont;
        bool _hfirst;

    public:
        /* Creates a new MIME message, which begins as initially
         * empty.  It's expected that you fill the message one at a
         * time. */
        mime(void);

        ~mime(void);

        /* Searches this MIME tree for something that matches the
         * given type, returning NULL if none exist.  No new memory is
         * allocated, we just return a pointer inside this message
         * tree. */
        const mime *search(const std::string type) const;

        /* Returns an iteration that lists the body of this part,
         * decoded into UTF-8 text. */
        string_iter body_utf8(void) const;

        /* Everything below here is used to build MIME objects one
         * line at a time by reading in a file.  They should really
         * only be used by "message_file", which is why they're listed
         * as protected. */
    protected:
        /* This is a special method used for initializing the root of
         * a MIME tree.  Essentially the idea is that the very first
         * MIME object that's created can't be properly built because
         * it needs to be somehow initialized before construction.
         * This here will end up being called with the "content-type"
         * header's value.  Note that you only really want to call
         * this for the root MIME object because this has a little
         * hack to throw away the pre-MIME message junk. */
        void set_root_content_type(const std::string value);

        /* Returns a pointer to a newly-created MIME object (which is
         * allocated as part of this object) if a new sibling is to be
         * created, and NULL otherwise. */
        mime *is_sibling(const std::string buffer);

        /* Returns a pointer to a newly-created MIME object (which is
         * allocated as part of this object) if a new child is to be
         * created, and NULL otherwise. */
        mime *is_child(const std::string buffer);

        /* Returns the parent pointer if this is a parent mime
         * boundary, otherwise NULL. */
        mime *is_parent(const std::string buffer);

        /* If none of the above is_*() methods did anything, then this
         * must be called to finalize the parse into this object. */
        void parse(const std::string buffer);
    };
}

#endif
