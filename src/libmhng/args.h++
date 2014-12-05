
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

#ifndef MHNG__ARGS_HXX
#define MHNG__ARGS_HXX

#include <memory>

namespace mhng {
    class args;
    typedef std::shared_ptr<args> args_ptr;
}

#include "folder.h++"
#include "mailrc.h++"
#include "message.h++"
#include "promise.h++"
#include "sqlite/connection.h++"
#include <string>
#include <vector>

namespace mhng {
    /* The MHng command-line argument parser.  This is designed
     * specifically to work with all the command-line argument formats
     * that MHng supports, which is why it's a bit clunky to use. */
    class args {
    private:
        /* Contains a list of message sequence numbers that were
         * passed. */
        std::vector<message_ptr> _messages;

        /* Holds a list of folder names that were passed on as
         * arguments. */
        std::vector<folder_ptr> _folders;

        /* Holds a list of numbers that were passed on the
         * commandline.  This will only be filled if you parse with
         * the numbers flag. */
        std::vector<int> _numbers;

        /* Stores the mailbox associated with this run. */
        mailbox_ptr _mbox;

        /* Arguments that can be passed with "--" on the
         * command-line. */
        unknown<bool> _stdout;
        unknown<bool> _nowrap;
        std::vector<std::string> _attach;

    public:
        /* This constructor is private because you should be using one
         * of the public static methods below to ensure you parse your
         * arguments in the correct form. */
        args(const std::vector<message_ptr>& messages,
             const std::vector<folder_ptr>& folders,
             const std::vector<int>& numbers,
             const mailbox_ptr& mbox,
             const unknown<bool>& stdout,
             const unknown<bool>& nowrap,
             const std::vector<std::string>& attach);

    public:
        /* Accessor functions. */
        const std::vector<message_ptr>& messages(void) const
            { return _messages; }
        const std::vector<folder_ptr>& folders(void) const
            { return _folders; }
        const std::vector<int>& numbers(void) const
            { return _numbers; }
        const mailbox_ptr& mbox(void) const { return _mbox; }

        bool stdout(void) const
            { return _stdout.known() == true && _stdout.data() == true; }
        bool nowrap(void) const
            { return _nowrap.known() == true && _nowrap.data() == true; }

    public:
        /* Parses normal command-line arguments, which means arguments
         * that look like NMH but slightly extended.  Specifically,
         * arguments that start with a "+" are considered folder
         * names, arguments that don't start with a "--" and are text
         * are also folder names, and numeric arguments are considered
         * message IDs (with ranges supported). */
        static args_ptr parse_normal(int argc, const char **argv);

        /* Like a normal parse, but defaults to listing every message
         * in the default folder (or whatever folder is passed in)
         * rather than just the current message. */
        static args_ptr parse_all_messages(int argc, const char **argv);

        /* Like a normal parse, but defaults to listing every folder
         * in the mailbox, but no messages at all. */
        static args_ptr parse_all_folders(int argc, const char **argv);

        /* Parses command-line arguments, but doesn't do any implicit
         * folder or message ID parsing.  This will just bail out on
         * any other */
        static args_ptr parse_noimplicit(int argc, const char **argv);

        /* Parses everything like regular, but doesn't treat arbitrary
         * numbers as message IDs but instead just treates them as
         * arbitrary integers. */
        static args_ptr parse_numbers(int argc, const char **argv);

        /* A generic parsing method that allows flags to be passed in.
         * The idea here is that we want to allow users to specify
         * some arbitrary set of parsing options. */
        static args_ptr parse(int argc, const char **argv, int flags);

        /* A list of flags that can be passed to parse(). */
        static const int pf_skipplus = 0x01;
        static const int pf_folders  = 0x02;
        static const int pf_messages = 0x04;
        static const int pf_numbers  = 0x08;
        static const int pf_allf     = 0x10;
        static const int pf_allm     = 0x20;
        static const int pf_nof      = 0x40;
        static const int pf_nom      = 0x80;
    };
}

#endif
