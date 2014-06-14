
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

#ifndef LIBMH__MAILRC_HXX
#define LIBMH__MAILRC_HXX

#include <string>
#include <unordered_map>

namespace mh {
    /* Reads a mailrc configuration file from disk, producing an
     * in-memory representation that's designed to be useful... */
    class mailrc {
    private:
        /* Local addresses (which can be sent from) in two different
         * formats: the full name and just the email address. */
        std::unordered_map<std::string, bool> _local_name;
        std::unordered_map<std::string, bool> _local_mail;

        /* Converts an email address to a long name, which looks like
         * "First Last <mail@host>" */
        std::unordered_map<std::string, std::string> _mail2long;

        /* Converts an email address to a name. */
        std::unordered_map<std::string, std::string> _mail2name;

    public:
        /* The sanest way to create a mailrc file: pass the full path
         * to the file on disk. */
        mailrc(const std::string full_path);

    public:
        /* Looks up a long name for an address. */
        const std::string mail2long(const std::string mail) const;

        /* Looks up the user's name for an email address. */
        const std::string mail2name(const std::string mail) const;
    };
}

#endif
