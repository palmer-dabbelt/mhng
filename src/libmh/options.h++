
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

#ifndef LIBMH__OPTIONS_HXX
#define LIBMH__OPTIONS_HXX

namespace mh {
    class options;
}

#include <memory>
#include <string>

namespace mh {
    typedef std::shared_ptr<options> options_ptr;

    /* Handles the myriad of options that may be passed to MHng.
     * Options can be passed in many ways (commandline, enviornment
     * variables, config files), this deals with merging them all
     * together and providing a consistant interface to the remainder
     * of the code. */
    class options {
    private:
        /* The exact argument string that created this, saved for
         * later use.  Note that it shouldn't be necessary to use this
         * during the course of regular operation because everything
         * will be parsed by the constructor. */
        const int _argc;
        const char **_argv;

        /* Holds the folder name if one was passed on the command
         * line. */
        bool _folder_valid;
        const std::string _folder;

        /* Holds the sequence number if one was passed on the command
         * line. */
        bool _seq_valid;
        int _seq;

    public:
        /* Calls the cooresponding constructor but returns a shared
         * pointer. */
        static options_ptr create(int argc, const char **argv);

        /* Prints the full path to the MH directory.  You probably
         * want to use one of the helpers below... */
        const std::string mhdir(void) const;

        /* Prints the database file, usually within the MH
         * directory. */
        const std::string dbfile(void) const;

        /* Returns the folder name if one was passed on the
         * commandline. */
        bool folder_valid(void) const { return _folder_valid; }
        const std::string folder(void) const { return _folder; }

        /* Returns the message sequence if one was passed on the
         * commandline. */
        bool seq_valid(void) const { return _seq_valid; }
        int seq(void) const { return _seq; }

    private:
        /* Parses a set of options, given the commandline arguments
         * that should be used.  Note that this will implicitly look
         * at all the other sources of options, you don't have to list
         * those anywhere. */
        options(int argc, const char **argv);

    };
}

#endif
