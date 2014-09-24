
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
#include <libmhng/gpg_sign.h++>
#include <string.h>
#include <unistd.h>
#include <termcap.h>

static char termbuf[2048];

/* Reformats a vector of strings to fit within a box -- essentially
 * this is the "fold" command, but fixed so it can work with stuff
 * like ASCII escape codes. */
static std::vector<std::string>
make_box(const std::vector<std::string>& lines,
         size_t width);

/* Produces a ASCII terminal string that produces the correct color
 * for this verification code. */
static std::string color_on(mhng::gpg_verification ver);
static std::string color_off(mhng::gpg_verification ver);

/* Writes the given list of strings inside a box.*/
static void write_in_box(FILE *out,
                         const std::vector<std::string>& lines,
                         mhng::gpg_verification ver,
                         size_t width);

/* Writes a line of the given character, in the given color. */
static void write_line(FILE *o, char c,
                       const char *on, const char *off,
                       size_t n);

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_normal(argc, argv);

    auto folders = args->folders();
    auto messages = args->messages();

    /* To avoid having to trigger too many synchronization requests,
     * so instead we go ahead and batch them all up together. */
    bool should_sync = false;

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
    should_sync = true;
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
    FILE *out = popen("less -FRSX", "w");
#else
    FILE *out = NULL;
    if (isatty(STDOUT_FILENO) == 1)
        out = popen("less", "w");
    else
        out = popen("cat", "w");
#endif

    size_t terminal_width = 80;
    char *termtype = getenv("TERM");
    if (tgetent(termbuf, termtype) >= 0) {
        terminal_width = tgetnum("co");
    }
    if (terminal_width > 80)
        terminal_width = 80;

    /* Show every message out to the pager.  Note that this is
     * essentially the mbox format, but by default I don't bother
     * re-formatting From lines in the original messages, as this is
     * designed for human consumption. */
    for (const auto& msg: messages) {
        if (msg->unread()) {
            msg->mark_read_and_unsynced();
            should_sync = true;
        }

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

        /* Check if there's a signature field, in which case we want
         * to verify the signutare -- otherwise just don't worry about
         * the verification at all! */
        auto sig = msg->mime()->signature();
        auto body = msg->mime()->body();
        auto sigres = mhng::gpg_verification::ERROR;
        if (sig != NULL) {
            sigres = mhng::gpg_verify(body->raw(),
                                      sig->utf8(),
                                      msg->first_from()->email()
                );
        }

        if (sig != NULL) {
            write_in_box(out, body->utf8(), sigres, terminal_width);
        } else if (args->nowrap() == false) {
            for (const auto& line: make_box(body->utf8(), terminal_width))
                fprintf(out, "%s\n", line.c_str());
        } else {
            for (const auto& line: body->utf8())
                fprintf(out, "%s\n", line.c_str());
        }
    }

    /* Attempt to close the pager.  I have no idea how this can
     * possibly fail, but I figure it's best to check for it
     * anyway... */
    if (pclose(out) != 0) {
        fprintf(stderr, "Somehow 'less' failed... :(\n");
        abort();
    }

    /* If something has changed trigger a sync! */
    if (should_sync == true) {
        auto daemon = args->mbox()->daemon();
        auto message = mhng::daemon::message::sync();
        daemon->send(message);
    }

    return 0;
}

std::vector<std::string>
make_box(const std::vector<std::string>& lines,
         size_t width)
{
    /* First split the mail up into the paragraphs that make up
     * this document. */
    std::vector<std::vector<std::string>> paragraphs;
    {
        std::vector<std::string> paragraph;

        for (const auto& line: lines) {
            if (line.size() == 0) {
                paragraphs.push_back(paragraph);
                paragraph = std::vector<std::string>();
            } else {
                paragraph.push_back(line);
            }
        }

        paragraphs.push_back(paragraph);
    }

    /* Now that we're got a list of paragraphs, walk through each of
     * those, line-breaking them seperately in order to form the
     * desired output format. */
    std::vector<std::string> out;
    for (const auto& paragraph: paragraphs) {
        /* Empty paragraphs don't need to be processed at all! */
        if (paragraph.size() == 0) {
            out.push_back("");
            continue;
        }

        /* First we check if this is a special sort of paragraph: it
         * might be one of those with a whole bunch of small,
         * already-broken lines or it might be one with a large shared
         * prefix. */
        bool all_small = true;
        auto prefix = paragraph[0];

        for (const auto& line: paragraph) {
            if (line.size() > width)
                all_small = false;

            size_t i;
            for (i = 0; i < prefix.size() && i < line.size(); ++i)
                if (prefix[i] != line[i])
                    break;

            prefix = std::string(prefix, 0, i);
        }

        /* Paragraphs with only one line don't have prefix support at
         * all. */
        if (paragraph.size() == 1)
            prefix = "";

        /* If it's one of those special cases then just don't break it
         * at all. */
        if ((all_small == true) || (prefix.size() >= 1)) {
            for (const auto& line: paragraph)
                out.push_back(line);

            out.push_back("");

            continue;
        }

        /* Otherwise attempt to line break the paragraph at word
         * boundries, leaving very long words un-touched. */
        std::string remainder;
        for (const auto& line: paragraph) {
            auto line_start = line.c_str();
            while (isspace(*line_start))
                line_start++;
            remainder = remainder + line_start;

            if (strlen(line_start) > 0)
                remainder = remainder + " ";

            while (remainder.size() > width) {
                auto f = remainder.rfind(" ", width);
                if (f == std::string::npos)
                    break;

                out.push_back(std::string(remainder, 0, f));
                remainder = std::string(remainder, f + 1);
            }

            if (line.size() == 0) {
                out.push_back(remainder);
                out.push_back("");
                remainder = "";
                continue;
            }
        }

        out.push_back(remainder);
        out.push_back("");
    }

    return out;
}

std::string color_on(mhng::gpg_verification ver)
{
    switch (ver) {
    case mhng::gpg_verification::FAIL:
        return "\x1b[31;1m";
    case mhng::gpg_verification::ERROR:
        return "\x1b[33;1m";
    case mhng::gpg_verification::SUCCESS:
        return "\x1b[32;1m";
    }

    abort();
    return "";
}

std::string color_off(mhng::gpg_verification ver __attribute__((unused)))
{
    return "\x1b[30;0m";
}

void write_in_box(FILE *out,
                  const std::vector<std::string>& lines,
                  mhng::gpg_verification ver,
                  size_t width)
{
    write_line(out,
               '*',
               color_on(ver).c_str(),
               color_off(ver).c_str(),
               width);

    for (const auto& line: make_box(lines, width - 4)) {
        fprintf(out, "%s*%s %-*s %s*%s\n",
                color_on(ver).c_str(),
                color_off(ver).c_str(),
                (int)(width - 4),
                line.c_str(),
                color_on(ver).c_str(),
                color_off(ver).c_str()
            );
    }

    write_line(out,
               '*',
               color_on(ver).c_str(),
               color_off(ver).c_str(),
               width);
}

void write_line(FILE *o, char c,
                const char *on, const char *off,
                size_t n)
{
    fprintf(o, "%s", on);

    for (size_t i = 0; i < n; ++i)
        fprintf(o, "%c", c);

    fprintf(o, "%s", off);

    fprintf(o, "\n");
}
