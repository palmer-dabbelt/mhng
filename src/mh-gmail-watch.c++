
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

#include <libmhimap/gmail_client.h++>
#include <libmh/options.h++>
#include <libmh/mhdir.h++>

int main(int argc, const char **argv)
{
    auto o = mh::options::create(argc, argv);

    mh::mhdir dir(o);

    /* We maintain this connection solely for issueing an IDLE on it,
     * as otherwise IMAP has race conditions. */
    mhimap::gmail_client idle_c(GMAIL_USERNAME, GMAIL_PASSWORD);

    /* Keep polling for new messages until we end up disconnected from
     * the server, at which point we just die. */
    while (idle_c.is_connected() == true) {
        /* This needs to come before we start polling for messages,
         * see the race condition below. */
        /* FIXME: We should probably let the user select which*/
        idle_c.send_idle("inbox");

        /* Create a new connection.  This deals with actually
         * transferring the mail around.  Note that this exists solely
         * to get rid of a race condition inherent to the IMAP
         * protocol: if a message comes in while you're polling the
         * server for new messages then you'll end up missing that
         * message. */
        mhimap::gmail_client c(GMAIL_USERNAME, GMAIL_PASSWORD);

        /* Check for new messages in every folder.  I think this is
         * probably safest -- this way we  */
        for (auto fit = c.folder_iter(); !fit.done(); ++fit) {
            /* Folders that don't already exist in our MH directory
             * just get ignored silently. */
            if (dir.folder_exists(*fit) == false)
                continue;

            /* For now just print out the folders that we would
             * synchronize. */
            fprintf(stderr, "folder to sync: '%s'\n", (*fit).c_str());
        }

        /* Waits for the server to say anything back to us. */
        mhimap::idle_response res = idle_c.wait_idle();
        switch (res) {
        case mhimap::idle_response::TIMEOUT:
        case mhimap::idle_response::DATA:
            break;

        case mhimap::idle_response::DISCONNECT:
            abort();
            break;
        }
    }

    return 0;
}
