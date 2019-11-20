/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__MIME__HEADER_HXX
#define MHNG__MIME__HEADER_HXX

#include <memory>

namespace mhng {
    namespace mime {
        class header;
        typedef std::shared_ptr<header> header_ptr;
    }
}

#include <vector>
#include <string>

namespace mhng {
    namespace mime {
        /* Stores a single MIME header in its original format (a block
         * of lines) along with mechanisms for formatting those
         * headers in a sane way. */
        class header {
        private:
            std::vector<std::string> _raw;

        public:
            /* Creates a new, empty header.  Note that it's pretty
             * much required that you end up adding information to
             * this header in order to make it actually valid... */
            header(const std::string& first_line);

        public:
            /* Adds a new line of header to this header. */
            void add_line(const std::string& line);

            /* This is probably what you want to use to parse a
             * header: it merges the whole header onto a single line
             * and decodes any quoted-printable sort of stuff to
             * UTF-8. */
            std::string utf8(void) const;

            /* Forms the header (only the value part, not the key) as
             * a single line, stripping all but one space when the
             * line changes over. */
            std::string single_line(void) const;

            /* Splits up this header based on commas. */
            std::vector<std::string> split_commas(void) const;

            /* Returns the key of this header, which is the stuff
             * before the colon. */
            std::string key(void) const;
            std::string key_downcase(void) const;

            /* Checks to see if the key of this header matches the
             * given key. */
            bool match(const std::string& key) const;
            bool match(const std::vector<std::string>& key) const;

            /* Returns the raw bytes of this header. */
            const std::vector<std::string>& raw(void) const
                { return _raw; }
        };
    }
}

#endif
