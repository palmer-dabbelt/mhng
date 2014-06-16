
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
        std::vector<mhimap::message> conly;

        /* Check for new messages in every folder.  This way we'll be
         * sure to fetch all new messages before removing any, which
         * means that nothing should get lost even if things
         * disconnect in the middle. */
        for (auto fit = c.folder_iter(); !fit.done(); ++fit) {
            const std::string fname((*fit).name());

            printf("folder '%s'\n", fname.c_str());

            /* Here we build up the set of messages that now exist on
             * the server but haven't yet been fetched to the
             * client. */
            std::vector<mhimap::message> sonly;

            /* Folders that don't already exist in our MH directory
             * just get ignored, but not silently anymore! */
            if (dir.folder_exists(fname.c_str()) == false) {
                printf("  skipping not in mhdir\n");
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
            printf("  Lising New Messages\n");
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
                if (imap_store.has((*fit).name(), (*mit).uid()) == false) {
                    printf("    New Message '%s':%u\n",
                           (*fit).name().c_str(),
                           (*mit).uid()
                        );
                    sonly.push_back(*mit);
                }
            }

            printf("  End of new messages\n");

            /* Walk the list of messages that exist on the local
             * machine, trying to figure out which are no longer on
             * the server. */
            {
                auto uids = imap_store.uids((*fit).name());

                for (auto it = uids.begin(); it != uids.end(); ++it) {
                    auto s = uidmap.find(*it);
                    if (s == uidmap.end())
                        conly.push_back(mhimap::message(*fit, *it));
                }
            }

            /* Fetches every message from the server down to the local
             * machine.  I do this first in order to avoid losing any
             * messages in a move (in other words, duplicate messages
             * are better than lost messages). */
            printf("  Fetching Messages\n");
            for (auto it = sonly.begin(); it != sonly.end(); ++it) {
                printf("    Fetching Message: '%s':%u\n",
                       (*it).folder_name().c_str(),
                       (*it).uid()
                    );
                auto tmp = dir.get_tmp();
                c.fetch_to_file(*it, tmp.fp());

                dir.trans_up();
                auto m = dir.insert((*it).folder_name(), tmp);
                imap_store.insert(*it, m.id());
                dir.trans_down();
            }
            printf("  Done fetching messages\n");

            /* Remove messages that are only on the client. */
            /* FIXME: There is a race condition here because the
             * filesystem is not synchronized with respect to the SQL
             * database.  I guess I should probably be storing blobs
             * in SQL, but I'm not really ready to commit to that
             * yet... */
            for (const auto& im: conly) {
                auto m = imap_store.lookup(im.folder_name(), im.uid());
                dir.trans_up();
                dir.remove(m);
                imap_store.remove(im);
                dir.trans_down();
            }

            /* Remove all the messages that have been marked as
             * needing a purge. */
            printf("  Purging Messages\n");
            for (auto mit = c.message_iter(*fit); !mit.done(); ++mit) {
                if (imap_store.needs_purge(*mit) == false)
                    continue;

                printf("    Purging Message: '%s':%u\n",
                       (*mit).folder_name().c_str(),
                       (*mit).uid()
                    );

                c.mark_as_deleted(*mit);
                imap_store.remove(*mit);
            }
            printf("  End Purging Messages\n");
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
