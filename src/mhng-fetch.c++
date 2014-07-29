
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

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_normal(argc, argv);

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
        std::vector<uint32_t> purge_messages;
        std::map<uint32_t, bool> should_purge;
        std::vector<mhimap::message> fetch_messages;
        std::vector<mhng::message_ptr> drop_messages;

        for (const auto& id: lfolder->purge()) {
            purge_messages.push_back(id);
            should_purge[id] = true;
        }

        for (const auto& pair: imessages) {
            auto imessage = pair.second;

            auto l = should_purge.find(imessage.uid());
            if (l != should_purge.end())
                continue;

            auto lmessage = lfolder->open_imap(imessage.uid());
            if (lmessage == NULL)
                fetch_messages.push_back(imessage);
        }

        for (const auto& lmessage: lfolder->messages()) {
            auto l = imessages.find(lmessage->imapid());
            if (l == imessages.end())
                drop_messages.push_back(lmessage);
        }

        /* Now we actually go ahead and perform the necessary
         * transactions. */
        for (const auto& imapid: purge_messages) {
            fprintf(stderr, "Purging %s/%u\n",
                    folder_name.c_str(),
                    imapid
                );

            auto imessage = mhimap::message(ifolder, imapid);

            auto trans = args->mbox()->db()->exclusive_transaction();
            client.mark_as_deleted(imessage);
            args->mbox()->did_purge(lfolder, imapid);
        }

        for (const auto& imessage: fetch_messages) {
            fprintf(stderr, "Fetching %s/%u\n",
                    imessage.folder_name().c_str(),
                    imessage.uid()
                );

            auto raw = client.fetch(imessage);
            for (const auto& line: raw)
                fprintf(stderr, "line: %s", line.c_str());

            auto mime = std::make_shared<mhng::mime::message>(raw);
            auto lmessage = args->mbox()->insert(lfolder,
                                                 mime,
                                                 imessage.uid()
                );
        }

        for (const auto& message: drop_messages) {
            fprintf(stderr, "Dropping %s (%s/%u)\n",
                    message->uid().c_str(),
                    message->folder()->name().c_str(),
                    message->seq()->to_uint()
                );
        }
    }
}
