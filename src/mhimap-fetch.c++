
/*
 * Copyright (C) 2013,2014 Palmer Dabbelt
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
#include <libmhng/args.h++>

#ifndef MHIMAP_MAIN
#define MHIMAP_MAIN main
#endif

int MHIMAP_MAIN(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_folders(argc, argv);

    fprintf(stderr, "Logging In\n");

    /* Opens a connection to GMail. */
    mhimap::gmail_client client(GMAIL_USERNAME, GMAIL_PASSWORD);

    /* Look through the IMAP server and synchronize every folder. */
    for (auto fit = client.folder_iter(); !fit.done(); ++fit) {
        auto ifolder = *fit;
        const std::string folder_name(ifolder.name());

        /* Here we build up the set of messages that now exist on the
         * server but haven't yet been fetched to the client. */
        std::vector<mhimap::message> sonly;

        /* Here we build up th set of messages that still exist on the
         * local machine but no longer exist on the server. */
        std::vector<mhimap::message> conly;

        /* Check to see that the folder exists on the client, if it
         * doesn't then just silently skip it. */
        auto lfolder = args->mbox()->open_folder(folder_name);
        if (lfolder == NULL)
            continue;

        /* Check to see that the IMAP UIDs are still what we thought
         * they were for this mailbox.  If they've changed then there
         * isn't really anything we can do, so we'll just have to give
         * up right now. */
        if (lfolder->has_uid_validity() == false)
            lfolder->set_uid_validity(ifolder.uidvalidity());

        if (lfolder->uid_validity() != ifolder.uidvalidity()) {
            fprintf(stderr, "UIDVALIDITY changed for '%s'\n",
                    folder_name.c_str());
            abort();
        }

        /* Contains a [UID -> message] map that lets us easily look up
         * messages on the IMAP server.  This way we can determine
         * which messages still exist on the client that no longer
         * exist on the server. */
        std::map<uint32_t, mhimap::message> imessages;

        /* Walk every message that exists on the server both building
         * up a quick reference of UIDs on the server, and a list of
         * messages that exist on the server but not on our
         * machine. */
        for (auto mit = client.message_iter(*fit); !mit.done(); ++mit) {
            imessages.insert(
                std::pair<uint32_t, mhimap::message> (
                    (*mit).uid(), *mit
                    )
                );
        }

        /* Here we build the lists of things we've got to do in order
         * to get the client and server in sync. */
#if defined(PURGE) || defined(FETCH) || defined(FLAGS)
        std::vector<uint32_t> purge_messages;
        std::map<uint32_t, bool> should_purge;

        for (const auto& id: lfolder->purge()) {
            purge_messages.push_back(id);
            should_purge[id] = true;
        }
#endif

#if defined(FETCH)
        std::vector<mhimap::message> fetch_messages;
        for (const auto& pair: imessages) {
            auto imessage = pair.second;

            auto l = should_purge.find(imessage.uid());
            if (l != should_purge.end())
                continue;

            auto lmessage = lfolder->open_imap(imessage.uid());
            if (lmessage == NULL)
                fetch_messages.push_back(imessage);
        }
#endif

#if defined(DROP)
        std::vector<mhng::message_ptr> drop_messages;
        for (const auto& lmessage: lfolder->messages()) {
            if (lmessage->imapid_known() == false) {
                fprintf(stderr, "Warning: Skipping '%s'/%u\n",
                        lfolder->name().c_str(),
                        lmessage->seq()->to_uint()
                    );
                continue;
            }

            auto l = imessages.find(lmessage->imapid());
            if (l == imessages.end())
                drop_messages.push_back(lmessage);
        }
#endif

#if defined(FLAGS)
        std::vector<mhng::message_ptr> set_seen;
        for (const auto& lmessage: lfolder->messages()) {
            if (lmessage->imapid_known() == false) {
                fprintf(stderr, "Warning: Skipping '%s'/%u\n",
                        lfolder->name().c_str(),
                        lmessage->seq()->to_uint()
                    );
                continue;
            }

            auto l = should_purge.find(lmessage->imapid());
            if (l != should_purge.end())
                continue;

            if (lmessage->read_and_unsynced() == false)
                continue;

            set_seen.push_back(lmessage);
        }
#endif

        /* Now we actually go ahead and perform the necessary
         * transactions. */
#if defined(PURGE)
        for (const auto& imapid: purge_messages) {
#ifndef QUIET
            printf("Purging %s/%u\n",
                    folder_name.c_str(),
                    imapid
                );
#endif

            auto imessage = mhimap::message(ifolder, imapid);

            client.mark_as_read(imessage);
            client.mark_as_deleted(imessage);
            auto trans = args->mbox()->db()->exclusive_transaction();
            args->mbox()->did_purge(lfolder, imapid);
        }
#endif

#if defined(FETCH)
        for (const auto& imessage: fetch_messages) {
#ifndef QUIET
            printf("Fetching %s/%u\n",
                   imessage.folder_name().c_str(),
                   imessage.uid()
                );
#endif

            auto raw = client.fetch(imessage);
            auto mime = std::make_shared<mhng::mime::message>(raw);
            auto trans = args->mbox()->db()->exclusive_transaction();
            auto lmessage = args->mbox()->insert(lfolder,
                                                 mime,
                                                 imessage.uid()
                );

            printf("Done fetching\n");
        }
#endif

#if defined(DROP)
        for (const auto& message: drop_messages) {
#ifndef QUIET
            printf("Dropping %s/%u (uid: %s)\n",
                   message->folder()->name().c_str(),
                   message->seq()->to_uint(),
                   message->uid().c_str()
                );
#endif

            message->remove();
            auto trans = args->mbox()->db()->exclusive_transaction();
            args->mbox()->did_purge(lfolder, message->imapid());
        }
#endif

#if defined(FLAGS)
        for (const auto& message: set_seen) {
#ifndef QUIET
            printf("Setting read %s/%u (uid: %s)\n",
                   message->folder()->name().c_str(),
                   message->seq()->to_uint(),
                   message->uid().c_str()
                );
#endif

            auto imessage = mhimap::message(ifolder, message->imapid());

            client.mark_as_read(imessage);
            auto trans = args->mbox()->db()->exclusive_transaction();
            message->mark_read_and_synced();
        }
#endif
    }

    return 0;
}
