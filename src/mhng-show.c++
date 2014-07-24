
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
    args->mbox()->set_current_folder(args->folders()[0]);
//    args->mbox()->set_current_message(args->messages()[0]);

    /* Here's the command we want to run in order to produce some
     * formatted output. */
    FILE *out = popen("fold -s | less +g", "w");

    /* Show every message out to the pager.  Note that this is
     * essentially the mbox format, but by default I don't bother
     * re-formatting From lines in the original messages, as this is
     * designed for human consumption. */
    for (const auto& msg: args->messages()) {
        for (const auto& addr: msg->from())
            fprintf(out, "From:    %s\n", addr->rfc().c_str());
        for (const auto& addr: msg->to())
            fprintf(out, "To:      %s\n", addr->nom().c_str());
        for (const auto& addr: msg->cc())
            fprintf(out, "CC:      %s\n", addr->nom().c_str());
        for (const auto& addr: msg->bcc())
            fprintf(out, "BCC:     %s\n", addr->nom().c_str());
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
