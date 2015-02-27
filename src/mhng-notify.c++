
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

#include <libmhng/args.h++>
#ifdef HAVE_LIBNOTIFY
#include <libnotify/notify.h>
#endif
#include <sstream>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_folders(argc, argv);
    auto daemon = args->mbox()->daemon();

    /* The last message that we've seen. */
    auto last_seen = args->mbox()->largest_uid();

#ifdef HAVE_LIBNOTIFY
    notify_init("MHng");
#endif

    while (true) {
        auto message = mhng::daemon::message::idle(last_seen);
        auto resp = daemon->send(message);
        resp->wait();

        auto highest = args->mbox()->largest_uid();

        for (auto uid = last_seen+1; uid <= highest; ++uid) {
            auto msg = args->mbox()->open(uid);
            if (msg == NULL)
                continue;

            std::stringstream title;
            title << msg->folder()->name() << ":" << msg->seq()->to_uint();

            std::stringstream body;
            for (const auto& addr: msg->from())
                body << "From:    " << addr->rfc() << "\n";
            for (const auto& addr: msg->to())
                body << "To:      " << addr->rfc() << "\n";
            for (const auto& str: msg->subject())
                body << "Subject: " << str << "\n";

#ifdef HAVE_LIBNOTIFY
            auto n = notify_notification_new(
                title.str().c_str(),
                body.str().erase(body.str().length()-1).c_str(),
                NULL
                );

            notify_notification_show (n, NULL);

            g_object_unref(G_OBJECT(n));
#else
            printf("%s\n%s", title.str().c_str(), body.str().c_str());
#endif
        }

        last_seen = highest;
    }
        
    return 0;
}
