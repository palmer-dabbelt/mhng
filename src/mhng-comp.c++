
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
#include <libmhng/mime/base64.h++>
#include <string.h>
#include <unistd.h>
#include <sstream>

#ifdef HAVE_UUID
#include <uuid.h>
#endif

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

/* Joins a vector of strings, seperating elements with sep1.  When the string
 * length is over max, seperate them with sep2 instead. */
static std::string joinmax(const std::vector<std::string>& v, std::string sep1, size_t max, std::string sep2);

int main(int argc, const char **argv)
{
#if defined(COMP)
    auto args = mhng::args::parse_all_folders(argc, argv);
#else
    auto args = mhng::args::parse_normal(argc, argv);
#endif

    /* Before doing anything, check to make sure that the listed
     * attachments actually exist. */
    for (const auto& file: args->attach()) {
        if (access(file.c_str(), R_OK) != 0) {
            fprintf(stderr, "Unable to open attachment %s\n", file.c_str());
            abort();
        }
    }

    /* We need a temporary file to fire up an editor against.  This is
     * kind of unfortunate, but I guess that's just life... :(. */
    auto tempdir = [&](){
        char *dup = NULL;
        if (asprintf(&dup, "%s/tmp/mhng-comp-XXXXXX", args->mbox()->path().c_str()) == -1) {
            fprintf(stderr, "Unable to asprintf\n");
            abort();
        }
        if (mkdtemp(dup) == NULL) {
            perror("Unable to create temporary directory");
            fprintf(stderr, "    tmpdir: %s\n", dup);
            abort();
        }
        auto out = std::string(dup);
        free(dup);
        return out;
    }();

    /* First we create the template that will be edited. */
    {
        char tmpl[BUFFER_SIZE];
        snprintf(tmpl, BUFFER_SIZE, "%s/template.msg", tempdir.c_str());
        FILE *out = fopen(tmpl, "w");

#if defined(COMP)
        /* This default template actually consists of pretty much
         * nothing, but that's OK! */
        fprintf(out, "From:    \n");
        fprintf(out, "To:      \n");
        fprintf(out, "Subject: \n");

        fprintf(out, "\n");

#elif defined(REPL)
        mhng::address_ptr local = NULL;
        bool should_sync = false;
        for (const auto& msg: args->messages()) {
            for (const auto& addr: msg->bcc()) {
                if (addr->local() == true)
                    local = addr;
            }
            for (const auto& addr: msg->cc()) {
                if (addr->local() == true)
                    local = addr;
            }
            for (const auto& addr: msg->to()) {
                if (addr->local() == true)
                    local = addr;
            }

            if (msg->unread()) {
                msg->mark_read_and_unsynced();
                should_sync = true;
            }
        }

        if (should_sync == true) {
            auto daemon = args->mbox()->daemon();
            auto message = mhng::daemon::message::sync();
            daemon->send(message);
        }

        if (local == NULL)
            fprintf(out, "From:        \n");
        else
            fprintf(out, "From:        %s\n", local->rfc().c_str());

        for (const auto& msg: args->messages()) {
            for (const auto& addr: msg->from())
                if (addr->local() == false)
                    fprintf(out, "To:          %s\n", addr->rfc().c_str());
            for (const auto& addr: msg->to())
                if (addr->local() == false)
                    fprintf(out, "CC:          %s\n", addr->rfc().c_str());
            for (const auto& addr: msg->cc())
                if (addr->local() == false)
                    fprintf(out, "CC:          %s\n", addr->rfc().c_str());
            for (const auto& str: msg->subject())
                fprintf(out, "Subject:     %s\n", format_reply(str).c_str());
            for (const auto& mid: msg->header_string("Message-ID"))
                fprintf(out, "In-Reply-To: %s\n", mid.c_str());

            fprintf(out, "\n");

            fprintf(out, "On %s, %s wrote:\n",
                    msg->first_date()->local().c_str(),
                    msg->first_from()->nom().c_str()
                );

            int trailing_newlines = 0;
            for (const auto& line: msg->body_utf8()) {
                if (strcmp(line.c_str(), "--") == 0)
                    break;

                if (strcmp(line.c_str(), "") == 0) {
                    trailing_newlines++;
                    continue;
                }

                for (int i = 0; i < trailing_newlines; ++i)
                    fprintf(out, ">\n");
                trailing_newlines = 0;

                if (line.c_str()[0] == '>')
                    fprintf(out, ">%s\n", line.c_str());
                else
                    fprintf(out, "> %s\n", line.c_str());
            }
        }
#elif defined(FORW)
        fprintf(out, "From:    \n");
        fprintf(out, "To:      \n");

        for (const auto& msg: args->messages()) {
            for (const auto& str: msg->subject())
                fprintf(out, "Subject: %s\n", format_forw(str).c_str());
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
        auto editor = [](void) -> const char *
            {
                if (getenv("MHNG_COMP_EDITOR") != NULL)
                    return getenv("MHNG_COMP_EDITOR");
                if (getenv("MHNG_EDITOR") != NULL)
                    return getenv("MHNG_EDITOR");
                if (getenv("COMP_EDITOR") != NULL)
                    return getenv("COMP_EDITOR");
                if (getenv("MH_EDITOR") != NULL)
                    return getenv("MH_EDITOR");
                if (getenv("EDITOR") != NULL)
                    return getenv("EDITOR");
                return "vi";
            }();

        char cmd[BUFFER_SIZE];
        snprintf(cmd, BUFFER_SIZE, "%s %s/template.msg",
                 editor, tempdir.c_str());
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
        snprintf(filename, BUFFER_SIZE, "%s/template.msg", tempdir.c_str());
        FILE *file = fopen(filename, "r");

        std::vector<std::string> raw;

        char line[BUFFER_SIZE*2];
        while (fgets(line, BUFFER_SIZE, file) != NULL) {
            if (args->nowrap() == true) {
                while ((strlen(line) > 0) && isspace(line[strlen(line)-1]))
                    line[strlen(line)-1] = '\0';
                strncat(line, "\n", BUFFER_SIZE);
            }
            raw.push_back(line);
        }

        fclose(file);

        auto raw_mime = std::make_shared<mhng::mime::message>(raw);

        /* Now walk back through that MIME message and attempt to
         * perform any sort of necessary address book lookups. */
        std::vector<std::string> lookup;
        std::string from;
        auto oheaders = std::map<std::string, std::vector<std::string>>();
        for (const auto& header: raw_mime->body()->headers()) {
            if (header->match({"From", "To", "CC", "BCC"})) {
                auto k = header->key();
                for (const auto v: header->split_commas()) {
                    auto a = args->mbox()->mrc()->emailias(v);
                    auto aa = a->rfc();
                    auto o = oheaders.find(k);
                    if (o == oheaders.end()) {
                        oheaders.insert(std::make_pair(k, std::vector<std::string>()));
                        o = oheaders.find(k);
                    }
                    o->second.push_back(aa);

                    if (strcasecmp(k.c_str(), "from") == 0)
                        from = a->email();
                }
            } else {
                for (const auto& hraw: header->raw())
                    lookup.push_back(hraw + "\n");
            }
        }

        for (const auto& pair: oheaders) {
            auto key = pair.first;
            auto val = pair.second;
            lookup.push_back(key + ": " + joinmax(val, ", ", 80, ",\n  ") + "\n");
        }

        /* Date-stamp the message with the current date. */
        {
            auto date = mhng::date::now();
            auto env_date = getenv("MHNG_COMP_DATE");
            auto date_str = (env_date == NULL) ? date->local() : std::string(env_date);
            lookup.push_back(std::string("Date: ") + date_str + "\n");
        }

        /* Generate a unique identifier that cooresponds to this
         * message, which is used later for things like
         * In-Reply-To. */
        {
#ifdef HAVE_UUID
            uuid_t uuid;
            uuid_generate(uuid);

            char uuid_str[BUFFER_SIZE];
            uuid_unparse(uuid, uuid_str);
#elif defined(__APPLE__)
            char uuid_str[BUFFER_SIZE];
            snprintf(uuid_str,
                     BUFFER_SIZE,
                     "%08llx%04x%04x",
                     mhng::date::now()->unix(),
                     arc4random(),
                     arc4random()
            );
#else
            char uuid_str[BUFFER_SIZE];
            snprintf(uuid_str,
                     BUFFER_SIZE,
                     "%08lx%04lx%04lx",
                     mhng::date::now()->unix(),
                     random(),
                     random()
            );
#endif

            char hostname[BUFFER_SIZE];
            gethostname(hostname, BUFFER_SIZE);

            char message_id[BUFFER_SIZE*3];
            snprintf(message_id, BUFFER_SIZE*3, "<mhng-%s@%s>", uuid_str, hostname);

            auto message_id_env = getenv("MHNG_COMP_MESSAGE_ID");
            auto message_id_str = (message_id_env == NULL) ? std::string(message_id) : std::string(message_id_env);

            lookup.push_back(std::string("Message-ID: ") + message_id_str + "\n");
        }

        for (const auto& bcc: args->mbox()->mrc()->bcc())
            lookup.push_back(std::string("BCC: ") + bcc->nom() + "\n");

        /* Make this a MIME message. */
        lookup.push_back("Mime-Version: 1.0 (MHng)\n");
        if (args->attach().size() == 0) {
            if (args->nowrap())
                lookup.push_back("Content-Type: text/plain; charset=utf-8\n");
            else
                lookup.push_back("Content-Type: text/plain; charset=utf-8; format=flowed\n");
            lookup.push_back("Content-Transfer-Encoding: 8bit\n");
        }
        lookup.push_back("\n");

        /* Copy the whole body of the message into the output. */
        for (const auto& body: raw_mime->body()->body_raw())
            lookup.push_back(body);

        /* Now actually re-parse the message. */
        mime = std::make_shared<mhng::mime::message>(lookup);
    }

    /* Check to see if we need to re-parse everything again in order
     * to insert the attachments. */
    if (args->attach().size() > 0) {
        std::vector<std::string> raw;

        /* Copy the headers over. */
        for (const auto& header: mime->body()->headers())
            for (const auto& hraw: header->raw())
                raw.push_back(hraw + "\n");

        /* Add some additional headers to make this a MIME multipart
         * message. */
        raw.push_back("Content-Type: multipart/mixed;\n");
        raw.push_back(" boundary=MHngMIME-attachments\n");
        raw.push_back("\n");
        raw.push_back("--MHngMIME-attachments\n");
        if (args->nowrap())
            raw.push_back("Content-Type: text/plain; charset=utf-8\n");
        else
            raw.push_back("Content-Type: text/plain; charset=utf-8; format=flowed\n");
        raw.push_back("Content-Transfer-Encoding: 8bit\n");
        raw.push_back("Content-Disposition: inline\n");
        raw.push_back("\n");

        /* Copy the message body, inserting a trailing space after every line
         * so it gets wrapped. */
        for (const auto& body: mime->body()->body_raw()) {
            raw.push_back(body);
        }
        raw.push_back("\n");

        /* Add any necessary attachments after the text. */
        for (const auto& filename: args->attach()) {
            auto content_type = "text/plain";

            raw.push_back("--MHngMIME-attachments\n");
            raw.push_back("Content-Transfer-Encoding: base64\n");
            raw.push_back(std::string("Content-Type: ") + content_type + ";\n");
            raw.push_back(std::string(" name=") + filename + "\n");
            raw.push_back("Content-Disposition: attachment;\n");
            raw.push_back(std::string(" filename=") + filename + "\n");
            raw.push_back("\n");

            for (const auto& line: base64_encode_file(filename))
                raw.push_back(line + "\n");
            raw.push_back("\n");
        }

        raw.push_back("--MHngMIME-attachments--\n");

        /* Now actually re-parse the message. */
        mime = std::make_shared<mhng::mime::message>(raw);
    }

    /* Go ahead and insert this message into the database. */
    auto message = args->mbox()->insert("drafts", mime);

    /* Clean up after ourselves... */
    {
        char cmd[BUFFER_SIZE];
        snprintf(cmd, BUFFER_SIZE, "rm -rf '%s'", tempdir.c_str());
        if (system(cmd) != 0) {
            perror("Unable to clean up\n");
            fprintf(stderr, "  There may be files in '%s'\n", tempdir.c_str());
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

static std::string joinmax(const std::vector<std::string>& v, std::string sep1, size_t max, std::string sep2)
{
    bool first = true;
    size_t len = 0;
    std::ostringstream out;
    for (const auto& s: v) {
        size_t nlen = len + s.size() + sep1.size();
        if (nlen > max) {
            out << sep2 << s;
            len = 0;
        } else if (first) {
            out << s;
            len = s.size();
            first = false;
        } else {
            out << sep1 << s;
            len = nlen;
        }
    }
    return out.str();
}
