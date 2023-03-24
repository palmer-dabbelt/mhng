/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <libmhng/gpg_sign.h++>
#include <regex>
#include <string.h>
#include <unistd.h>
#include <termcap.h>

static char termbuf[2048];

/* Reformats a vector of strings to fit within a box -- essentially
 * this is the "fold" command, but fixed so it can work with stuff
 * like ASCII escape codes. */
static std::vector<std::string>
make_box(const std::vector<std::string>& lines,
         size_t wrap_width, size_t trigger_width, size_t hard_width);

#ifdef HAVE_GPGME
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
#endif

int main(int argc, const char **argv)
{
#if defined(SHOW) || defined(BODY)
    auto args = mhng::args::parse_normal(argc, argv);
#elif (defined(NEXT) || defined(PREV))
    auto args = mhng::args::parse_fakecur(argc, argv);
#else
# error "You must define SHOW, NEXT, or PREV"
#endif

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

#if !defined(PIPE)
    /* Here we figure out what to do with this message: either we
     * attempt to move the current message pointer around based on
     * what was given on the commandline, or we move to the
     * next/previous message based on sequence numbers. */
# if defined(SHOW) || defined(BODY)
    if (folders.size() == 1 && messages.size() == 1) {
        args->mbox()->set_current_folder(folders[0]);
        folders[0]->set_current_message(messages[0]);
    }
# elif defined(NEXT) || defined(PREV)
#  if defined(NEXT)
    auto next = (args->fake_current() != nullptr)
                ? args->fake_current()->next_message( 1)
                : messages[0]->next_message( 1);
#  elif defined(PREV)
    auto next = (args->fake_current() != nullptr)
                ? args->fake_current()->next_message(-1)
                : messages[0]->next_message(-1);
#  endif
    if (next == NULL) {
        fprintf(stderr, "Unable to move message pointer\n");
        fprintf(stderr, "  Is there a message in that direction?\n");
        abort();
    }

    args->folders()[0]->set_current_message(next);
    messages = {args->folders()[0]->current_message()};
# else
#  error "Define some operation mode..."
# endif
#else
# if defined(NEXT) || defined(PREV)
#  error "PIPE means don't change the current pointer, but NEXT or PREV means move the pointer"
# endif
#endif

    /* Here's the command we want to run in order to produce some
     * formatted output. */
#if 0
    FILE *out = popen("less -FRSX", "w");
#else
    FILE *out = NULL;
    if ((isatty(STDOUT_FILENO) == 1) && (args->stdout() == false))
        out = popen("less", "w");
    else
        out = popen("cat", "w");
#endif

    size_t terminal_width = 80;
    char *termtype = getenv("TERM");
    if (tgetent(termbuf, termtype) >= 0) {
        terminal_width = tgetnum((char *)"co");
    }

    /* Allow a bit of extra slop when wrapping: wrap to 78 characters, but then
     * allow */
    size_t wrap_width = terminal_width;
    if (wrap_width > 78)
        wrap_width = 78;
    size_t trigger_width = terminal_width;
    if (trigger_width > 90)
        trigger_width = 90;

    /* Show every message out to the pager.  Note that this is
     * essentially the mbox format, but by default I don't bother
     * re-formatting From lines in the original messages, as this is
     * designed for human consumption. */
    for (const auto& msg: messages) {
#ifndef BODY
        if (msg->unread()) {
            msg->mark_read_and_unsynced();
            should_sync = true;
        }

        for (const auto& addr: msg->from())
            fprintf(out, "From:       %s\n", addr->rfc().c_str());
        for (const auto& addr: msg->to())
            fprintf(out, "To:         %s\n", addr->rfc().c_str());
        for (const auto& addr: msg->cc())
            fprintf(out, "CC:         %s\n", addr->rfc().c_str());
        for (const auto& addr: msg->bcc())
            fprintf(out, "BCC:        %s\n", addr->rfc().c_str());
        for (const auto& str: msg->subject())
            fprintf(out, "Subject:    %s\n", str.c_str());
        for (const auto& date: msg->date())
            fprintf(out, "Date:       %s\n", date->local().c_str());
        if (msg->imapid_known())
            fprintf(out, "Account:    %s\n", msg->imap_account_name().c_str());
        for (const auto& mid: msg->header_string("Message-ID"))
            fprintf(out, "Message-ID: %s\n", mid.c_str());

        fprintf(out, "\n");
#endif

        /* Checks to see if this sort of message should be treated as
         * pre-wrapped. */
        auto prewrapped = [&](){
            for (const auto& h: msg->header_string("X-Mailer"))
                if (strncmp("git-send-email ", h.c_str(), 15) == 0)
                    return true;

            return false;
        }();

        /* Check if there's a signature field, in which case we want
         * to verify the signutare -- otherwise just don't worry about
         * the verification at all! */
        auto body = msg->mime()->body();
#ifdef HAVE_GPGME
        auto sig = msg->mime()->signature();
        auto sigres = mhng::gpg_verification::ERROR;
        if (sig != NULL) {
            sigres = mhng::gpg_verify(body->raw(),
                                      sig->utf8(),
                                      msg->first_from()->email()
                );
        }

        if (sig != NULL) {
            write_in_box(out, body->utf8(), sigres, wrap_width, trigger_width, terminal_width);
        } else if (args->nowrap() == false && !prewrapped) {
            for (const auto& line: make_box(body->utf8(), wrap_width, trigger_width, terminal_width))
                fprintf(out, "%s\n", line.c_str());
        } else {
            for (const auto& line: body->utf8())
                fprintf(out, "%s\n", line.c_str());
        }
#else
        if (args->nowrap() == false && !prewrapped) {
            for (const auto& line: make_box(body->utf8(), wrap_width, trigger_width, terminal_width))
                fprintf(out, "%s\n", line.c_str());
        } else {
            for (const auto& line: body->utf8())
                fprintf(out, "%s\n", line.c_str());
        }
#endif
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
         size_t wrap_width, size_t trig_width, size_t hard_width)
{
    /* First split the mail up into the paragraphs that make up
     * this document. */
    std::vector<std::vector<std::string>> paragraphs;
    {
        std::vector<std::string> paragraph;

        for (const auto& line: lines) {
            if (strcmp(line.c_str(), "--") == 0)
                break;

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
    bool first_paragraph = true;
    for (const auto& paragraph: paragraphs) {
        /* All paragraphs (except the first, which isn't separated from
         * anything) must be separated by a newline. */
        if (first_paragraph == true)
            first_paragraph = false;
        else
            out.push_back("");

        /* First we check if this is a special sort of paragraph: it
         * might be one of those with a whole bunch of small,
         * already-broken lines or it might be one with a large shared
         * prefix. */
        bool all_small = true;
        char prefix_char = '\0';
        for (const auto& line: paragraph) {
            /* If we ever cross the hard wrapping width, then just force a
             * wrapping. */
            if (line.size() > hard_width)
                all_small = false;

            /* Check to see if this is a reply line, in which case we don't
             * want to trigger wrapping for the paragraph (unless it should
             * already be wrapped). */
            if (std::regex_match(line, std::regex(">* *On.*wrote:"))) {
                prefix_char = '<';
                continue;
            }

            if (prefix_char == '\0')
                prefix_char = line[0];

            /* Count the number of prefix characters. */
            size_t prefix_length;
            for (prefix_length = 0;
                line[prefix_length] == prefix_char || isspace(line[prefix_length]);
                ++prefix_length);

            /* Ignore the prefix characters when checking for */
            if ((line.size() - prefix_length) > trig_width)
                all_small = false;
        }

        /* If it's one of those special cases then just don't break it
         * at all. */
        if (all_small == true) {
            for (const auto& line: paragraph)
                out.push_back(line);

            continue;
        }

        /* Otherwise attempt to line break the paragraph at word
         * boundries, leaving very long words un-touched. */
        std::string remainder;
        for (const auto& line: paragraph) {
            remainder = remainder + line;

            while (remainder.size() > wrap_width) {
                auto f = remainder.rfind(" ", wrap_width);
                if (f == std::string::npos)
                    break;

                out.push_back(std::string(remainder, 0, f));
                remainder = std::string(remainder, f + 1);
            }

            if (line.size() == 0) {
                out.push_back(remainder);
                remainder = "";
                continue;
            }
        }

        if (remainder.size() > 0) {
            out.push_back(remainder);
        }
    }

    return out;
}

#ifdef HAVE_GPGME
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
#endif
