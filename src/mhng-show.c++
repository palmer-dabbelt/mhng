
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
#include <string.h>

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_normal(argc, argv);

    auto folders = args->folders();
    auto messages = args->messages();

    /* It's possible we need to remove the current message.  Note that
     * this really only makes any form of sense when given a
     * NEXT/PREV, as otherwise it would be impossible to show the
     * message. */
#if defined(RMM)
#if defined(NEXT) || defined(PREV)
    if (messages.size() != 1) {
        fprintf(stderr, "Attempted to mhng-{mtn,mtp} with many messages\n");
        abort();
    }

    messages[0]->remove();
#else
#error "Can't RMM without NEXT || PREV"
#endif
#endif

    /* Here we figure out what to do with this message: either we
     * attempt to move the current message pointer around based on
     * what was given on the commandline, or we move to the
     * next/previous message based on sequence numbers. */
#if defined(SHOW)
    if (folders.size() == 1 && messages.size() == 1) {
        args->mbox()->set_current_folder(folders[0]);
        folders[0]->set_current_message(messages[0]);
    }
#elif defined(NEXT) || defined(PREV)
#if defined(NEXT)
    auto next = messages[0]->next_message( 1);
#elif defined(PREV)
    auto next = messages[0]->next_message(-1);
#endif
    if (next == NULL) {
        fprintf(stderr, "Unable to move message pointer\n");
        fprintf(stderr, "  Is there a message in that direction?\n");
        abort();
    }

    args->folders()[0]->set_current_message(next);
    messages = {args->folders()[0]->current_message()};
#else
#error "Define some operation mode..."
#endif

    /* Here's the command we want to run in order to produce some
     * formatted output. */
#if 0
    FILE *out = popen("fold -s | less -FRSX", "w");
#else
    FILE *out = popen("fold -s | less", "w");
#endif

    /* Show every message out to the pager.  Note that this is
     * essentially the mbox format, but by default I don't bother
     * re-formatting From lines in the original messages, as this is
     * designed for human consumption. */
    for (const auto& msg: messages) {
        for (const auto& addr: msg->from())
            fprintf(out, "From:    %s\n", addr->rfc().c_str());
        for (const auto& addr: msg->to())
            fprintf(out, "To:      %s\n", addr->rfc().c_str());
        for (const auto& addr: msg->cc())
            fprintf(out, "CC:      %s\n", addr->rfc().c_str());
        for (const auto& addr: msg->bcc())
            fprintf(out, "BCC:     %s\n", addr->rfc().c_str());
        for (const auto& str: msg->subject())
            fprintf(out, "Subject: %s\n", str.c_str());
        for (const auto& date: msg->date())
            fprintf(out, "Date:    %s\n", date->local().c_str());

        fprintf(out, "\n");

        for (const auto& line: msg->body_utf8())
            fprintf(out, "%s\n", line.c_str());
    }

    /* Attempt to close the pager.  I have no idea how this can
     * possibly fail, but I figure it's best to check for it
     * anyway... */
    if (pclose(out) != 0) {
        fprintf(stderr, "Somehow 'less' failed... :(\n");
        abort();
    }

    return 0;
}
