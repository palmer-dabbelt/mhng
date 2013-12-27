
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

    public:
        options(int argc, const char **argv);
    };
}

#endif
