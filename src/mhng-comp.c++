
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
#include <unistd.h>
#include <uuid.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#ifdef REPL
/* Formats a string as a reply string. */
static std::string format_reply(const std::string subject);
#endif

#ifdef FORW
/* Formats a string as a reply string. */
static std::string format_forw(const std::string subject);
#endif

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_normal(argc, argv);

    /* We need a temporary file to fire up an editor against.  This is
     * kind of unfortunate, but I guess that's just life... :(. */
    char *tempdir = strdup("/tmp/mhng-comp-XXXXXX");
    if (mkdtemp(tempdir) == NULL) {
        perror("Unable to create temporary directory\n");
        abort();
    }

    /* First we create the template that will be edited. */
    {
        char tmpl[BUFFER_SIZE];
        snprintf(tmpl, BUFFER_SIZE, "%s/template.msg", tempdir);
        FILE *out = fopen(tmpl, "w");

#if defined(COMP)
        /* This default template actually consists of pretty much
         * nothing, but that's OK! */
        fprintf(out, "From:    \n");
        fprintf(out, "To:      \n");
        fprintf(out, "Subject: \n");

        fprintf(out, "\n");

#elif defined(REPL)
        for (const auto& msg: args->messages()) {
            for (const auto& addr: msg->from())
                fprintf(out, "To:          %s\n", addr->rfc().c_str());
            for (const auto& addr: msg->to())
                fprintf(out, "CC:          %s\n", addr->rfc().c_str());
            for (const auto& addr: msg->cc())
                fprintf(out, "CC:          %s\n", addr->rfc().c_str());
            for (const auto& str: msg->subject())
                fprintf(out, "Subject:     %s\n", format_reply(str).c_str());
            for (const auto& date: msg->date())
                fprintf(out, "Date:        %s\n", date->local().c_str());
            for (const auto& mid: msg->header_string("Message-ID"))
                fprintf(out, "In-Reply-To: %s\n", mid.c_str());

            fprintf(out, "\n");

            fprintf(out, "On %s, %s wrote:\n",
                    msg->first_date()->local().c_str(),
                    msg->first_from()->nom().c_str()
                );

            for (const auto& line: msg->body_utf8())
                fprintf(out, "> %s\n", line.c_str());
        }
#elif defined(FORW)
        fprintf(out, "From:    \n");
        fprintf(out, "To:      \n");

        for (const auto& msg: args->messages()) {
            for (const auto& str: msg->subject())
                fprintf(out, "Subject:     %s\n", format_forw(str).c_str());
        }

        fprintf(out, "\n");

        fprintf(out, "---------- Forwarded message ----------\n");

        for (const auto& msg: args->messages()) {
            for (const auto& addr: msg->from())
                fprintf(out, "To:         %s\n", addr->rfc().c_str());
            for (const auto& addr: msg->to())
                fprintf(out, "CC:         %s\n", addr->rfc().c_str());
            for (const auto& addr: msg->cc())
                fprintf(out, "CC:         %s\n", addr->rfc().c_str());
            for (const auto& str: msg->subject())
                fprintf(out, "Subject:    %s\n", str.c_str());
            for (const auto& date: msg->date())
                fprintf(out, "Date:       %s\n", date->local().c_str());
            for (const auto& mid: msg->header_string("Message-ID"))
                fprintf(out, "Message-ID: %s\n", mid.c_str());

            fprintf(out, "\n");

            for (const auto& line: msg->body_utf8())
                fprintf(out, "%s\n", line.c_str());
        }
#endif

        fclose(out);
    }

    /* Now fire up an editor to actually edit the template. */
    {
        char cmd[BUFFER_SIZE];
        snprintf(cmd, BUFFER_SIZE, "emacs -nw %s/template.msg",
                 tempdir);
        if (system(cmd) != 0) {
            perror("Unable to open editor\n");
            abort();
        }
    }

    /* The user has written their entire message, so go ahead and
     * parse it right now. */
    std::shared_ptr<mhng::mime::message> mime;
    {
        char filename[BUFFER_SIZE];
        snprintf(filename, BUFFER_SIZE, "%s/template.msg", tempdir);
        FILE *file = fopen(filename, "r");

        std::vector<std::string> raw;

        char line[BUFFER_SIZE];
        while (fgets(line, BUFFER_SIZE, file) != NULL)
            raw.push_back(line);

        auto raw_mime = std::make_shared<mhng::mime::message>(raw);

        /* Now walk back through that MIME message and attempt to
         * perform any sort of necessary address book lookups. */
        std::vector<std::string> lookup;
        for (const auto& header: raw_mime->body()->headers()) {
            if (header->match({"From", "To", "CC", "BCC"})) {
                auto k = header->key();
                auto v = header->single_line();
                auto a = args->mbox()->mrc()->emailias(v);
                auto aa = a->rfc();
                lookup.push_back(k + ": " + aa + "\n");
            } else {
                for (const auto& hraw: header->raw())
                    lookup.push_back(hraw + "\n");
            }
        }
        lookup.push_back("\n");
        for (const auto& body: raw_mime->body()->body_raw())
            lookup.push_back(body);
        mime = std::make_shared<mhng::mime::message>(lookup);
    }

    /* Date-stamp the message with the current date. */
    {
        auto date = mhng::date::now();
        mime->add_header("Date", date->local());
        mime->body()->add_header("Date", date->local());
    }

    /* Generate a unique identifier that cooresponds to this message,
     * which is used later for things like In-Reply-To. */
    {
        uuid_t uuid;
        uuid_generate(uuid);

        char uuid_str[BUFFER_SIZE];
        uuid_unparse(uuid, uuid_str);

        char hostname[BUFFER_SIZE];
        gethostname(hostname, BUFFER_SIZE);

        char message_id[BUFFER_SIZE];
        snprintf(message_id, BUFFER_SIZE, "<mhng-%s@%s>", uuid_str, hostname);

        mime->add_header("Message-ID", message_id);
        mime->body()->add_header("Message-ID", message_id);
    }

    /* Go ahead and insert this message into the database. */
    auto message = args->mbox()->insert("drafts", mime);

    /* Clean up after ourselves... */
    {
        char cmd[BUFFER_SIZE];
        snprintf(cmd, BUFFER_SIZE, "rm -rf '%s'", tempdir);
        if (system(cmd) != 0) {
            perror("Unable to clean up\n");
            fprintf(stderr, "  There may be files in '%s'\n", tempdir);
            abort();
        }
    }

    return 0;
}

#ifdef REPL
std::string format_reply(const std::string subject)
{
    char buf[BUFFER_SIZE];
    snprintf(buf, BUFFER_SIZE, "Re: %s", subject.c_str());

    if (strncasecmp(buf, "Re: Re:", 7) == 0)
        return subject;

    return buf;
}
#endif

#ifdef FORW
std::string format_forw(const std::string subject)
{
    char buf[BUFFER_SIZE];
    snprintf(buf, BUFFER_SIZE, "FW: %s", subject.c_str());

    if (strncasecmp(buf, "FW: FW:", 7) == 0)
        return subject;

    if (strncasecmp(buf, "FW: Fwd:", 8) == 0)
        return subject;

    return buf;
}
#endif
