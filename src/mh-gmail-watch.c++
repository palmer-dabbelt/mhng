
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

    /* IMAP synchronization requires a special table to store the map
     * of of known UIDs for each folder along with their mapping to MH
     * UIDs. */
    mh::imap_store imap_store = dir.get_imap_store();

    /* Keep polling for new messages until we end up disconnected from
     * the server, at which point we just die. */
    while (idle_c.is_connected() == true) {
        /* This needs to come before we start polling for messages,
         * see the race condition below. */
        /* FIXME: We should probably let the user select which mailbox
         * to IDLE on. */
        idle_c.send_idle("inbox");

        /* Create a new connection.  This deals with actually
         * transferring the mail around.  Note that this exists solely
         * to get rid of a race condition inherent to the IMAP
         * protocol: if a message comes in while you're polling the
         * server for new messages then you'll end up missing that
         * message. */
        mhimap::gmail_client c(GMAIL_USERNAME, GMAIL_PASSWORD);

        /* Here we build up th set of messages that still exist on the
         * local machine but no longer exist on the server. */
        std::vector<mhimap::message> client_only;

        /* Here we build up the set of messages that now exist on the
         * server but haven't yet been fetched to the client. */
        std::vector<mhimap::message> server_only;

        /* Check for new messages in every folder.  This way we'll be
         * sure to fetch all new messages before removing any, which
         * means that nothing should get lost even if things
         * disconnect in the middle. */
        for (auto fit = c.folder_iter(); !fit.done(); ++fit) {
            const std::string fname((*fit).name());

            /* Folders that don't already exist in our MH directory
             * just get ignored, but not silently anymore! */
            if (dir.folder_exists(fname.c_str()) == false) {
                printf("skipping '%s', not in mhdir\n", fname.c_str());
                continue;
            }

            /* Checks to see if this is a new folder, which means that
             * we've never seen anything about it before and we need
             * to update the cache. */
            imap_store.try_insert_folder((*fit).name(),
                                         (*fit).uidvalidity());

            /* Check to see that UIDVALIDITY hasn't changed -- note
             * that there's a whole bunch more checks in here, but
             * they are all in memory and not against the database. */
            if (imap_store.get_uidvalidity(fname) != (*fit).uidvalidity()) {
                /* FIXME: Do something other than bail here. */
                fprintf(stderr, "UIDVALIDITY changed for '%s'\n",
                        (*fit).name().c_str());
                abort();
            }

            /* Contains a [UID -> message] map that lets us easily
             * look up messages on the IMAP server.  This way we can
             * determine which messages still exist on the client that
             * no longer exist on the server. */
            std::map<uint32_t, const mhimap::message> uidmap;

            /* Walk every message that exists on the server both
             * building up a quick reference of UIDs on the server,
             * and a list of messages that exist on the server but not
             * on our machine. */
            for (auto mit = c.message_iter(*fit); !mit.done(); ++mit) {
                /* FIXME: Is this really the best way to insert
                 * something into a map?  That can't possibly be
                 * true... */
                uidmap.insert(
                    std::pair<uint32_t, const mhimap::message> (
                        (*mit).uid(), *mit
                        )
                    );

                /* If the message doesn't exist on the client then we
                 * want to add it to the list of messages to fetch. */
                if (imap_store.has((*fit).name(), (*mit).uid()) == false)
                    server_only.push_back(*mit);
            }

            /* Walk the list of messages that exist on the local
             * machine, trying to figure out which are no longer on
             * the server. */
            {
                auto uids = imap_store.uids((*fit).name());

                for (auto it = uids.begin(); it != uids.end(); ++it) {
                    auto s = uidmap.find(*it);
                    if (s == uidmap.end())
                        client_only.push_back(mhimap::message(*fit, *it));
                }
            }
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
