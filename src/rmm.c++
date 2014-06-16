
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

#include <libmh/message.h++>
#include <libmh/mhdir.h++>
#include <libmh/options.h++>
#include <libmhimap/message.h++>

int main(const int argc, const char **argv)
{
    auto o = mh::options::create(argc, argv);
    mh::mhdir dir(o);

    /* Opens the default folder. */
    mh::folder folder = dir.open_folder();

    /* Reads the current message from the given folder. */
    mh::message message = folder.open_current();

    dir.trans_up();

    /* Removes the current message from the MH directory and from the
     * IMAP cache. */
    dir.remove(message);

    /* Mark the message for removal on the IMAP server, which will
     * cause it to be removed next time IMAP is synchronized. */
    dir.get_imap_store().mark_for_removal(message);

    dir.trans_down();

    return 0;
}
