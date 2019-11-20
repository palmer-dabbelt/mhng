/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#ifdef HAVE_LIBNOTIFY
#include <libnotify/notify.h>
#endif
#include <cstring>
#include <iostream>
#include <regex>
#include <sstream>

#ifdef HAVE_LIBNOTIFY
/* Returns a HTML-escaped version of a string. */
static std::string html(const std::string& str);
#endif

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_all_folders(argc, argv);
    auto daemon = args->mbox()->daemon();

    /* The last message that we've seen. */
    auto last_seen = args->mbox()->largest_uid();

    while (true) {
        std::cout << "Last Seen: " << last_seen << "\n";
        std::cout << std::flush;

        auto message = mhng::daemon::message::new_message(last_seen);
        auto resp = daemon->send(message);
        resp->wait();

        auto highest = args->mbox()->largest_uid();

        for (auto uid = last_seen+1; uid <= highest; ++uid) {
            auto msg = args->mbox()->open(uid);
            if (msg == NULL)
                continue;

            if (strcmp(msg->folder()->name().c_str(), "inbox") != 0)
                continue;

#ifdef HAVE_LIBNOTIFY
            notify_init("MHng");

            {
                std::stringstream title;
                title << msg->folder()->name() << ":" << msg->seq()->to_uint();

                std::stringstream body;

                for (const auto& addr: msg->from())
                    body << "<tt>From:    " << html(addr->rfc()) << "</tt>\n";
                for (const auto& addr: msg->to())
                    body << "<tt>To:      " << html(addr->rfc()) << "</tt>\n";
                for (const auto& str: msg->subject())
                    body << "<tt>Subject: " << html(str) << "</tt>\n";

                body.str(body.str().erase(body.str().length()-1));

                auto n = notify_notification_new(
                    title.str().c_str(),
                    body.str().c_str(),
                    NULL
                    );

                notify_notification_show (n, NULL);

                g_object_unref(G_OBJECT(n));
            }

            notify_uninit();
#endif

            {
                std::cout << msg->folder()->name()
                          << ":"
                          << msg->seq()->to_uint()
                          << "\n";

                for (const auto& addr: msg->from())
                    std::cout << "From:    " << addr->rfc() << "\n";
                for (const auto& addr: msg->to())
                    std::cout << "To:      " << addr->rfc() << "\n";
                for (const auto& str: msg->subject())
                    std::cout << "Subject: " << str << "\n";

                std::cout << std::flush;
            }
        }

        last_seen = highest;
    }
        
    return 0;
}

#ifdef HAVE_LIBNOTIFY
std::string html(const std::string& str)
{
    std::stringstream os;

    for (const auto c: str) {
        if (c == '<')
            os << "&lt;";
        else if (c == '>')
            os << "&gt;";
        else if (c == '&')
            os << "&amp;";
        else
            os << c;
    }

    return os.str();
}
#endif
