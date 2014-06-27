
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

#include <libmh/global_mailrc.h++>
#include <libmh/message.h++>
#include <libmh/message_file.h++>
#include <libmh/mhdir.h++>
#include <libmh/options.h++>
#include <algorithm>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <uuid.h>

#ifndef COMMAND_LENGTH
#define COMMAND_LENGTH 1024
#endif

#ifndef DATE_LENGTH
#define DATE_LENGTH 128
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
static std::string format_forward(const std::string subject);
#endif

int main(int argc, const char **argv)
{
    auto o = mh::options::create(argc, argv);
    mh::mhdir dir(o);
    auto mailrc = mh::global_mailrc();

#if defined(REPL) || defined(FORW)
    /* Here we read the source message, which is the message that
     * we're templating the reply based on. */
    mh::folder source_folder = dir.open_folder(true);
    mh::message source_message = source_folder.open_current();
    mh::message_file source_mf = source_message.read();
#endif

    /* Opens up a temporary file that we can use to edit inside
     * emacs. */
    auto temp_file_path = dir.get_tmp();
    FILE *temp_file = temp_file_path.fp();

#if defined(REPL)
    /* First we must figure out the local address */
    std::string local = "";
    for (auto it = source_mf.headers_address({"From", "To", "CC"}); !it.done(); ++it) {
        auto address = *it;

        if (mailrc->local_p(address) == false)
            continue;

        local = address;
    }

    /* Write out the relevant headers to the file, somewhat modified
     * because this is a reply. */
    fprintf(temp_file, "From:        %s\n", mailrc->mail2long(local).c_str());
    for (auto it = source_mf.headers_address("From"); !it.done(); ++it) {
        if (mailrc->local_p(*it) == true) continue;
        fprintf(temp_file, "To:          %s\n", mailrc->mail2long(*it).c_str());
    }
    for (auto it = source_mf.headers_address("To"); !it.done(); ++it) {
        if (mailrc->local_p(*it) == true) continue;
        fprintf(temp_file, "CC:          %s\n", mailrc->mail2long(*it).c_str());
    }
    for (auto it = source_mf.headers_address("CC"); !it.done(); ++it) {
        if (mailrc->local_p(*it) == true) continue;
        fprintf(temp_file, "CC:          %s\n", mailrc->mail2long(*it).c_str());
    }
    for (auto it = source_mf.headers("Subject"); !it.done(); ++it) {
        if (mailrc->local_p(*it) == true) continue;
        fprintf(temp_file, "Subject:     %s\n", format_reply(*it).c_str());
    }
    for (auto it = source_mf.headers("Message-ID"); !it.done(); ++it) {
        if (mailrc->local_p(*it) == true) continue;
        fprintf(temp_file, "In-Reply-To: %s\n", (*it).c_str());
    }
#elif defined(FORW)
    fprintf(temp_file,
            "From:    \n"
            "To:      \n"
        );
    for (auto it = source_mf.headers("Subject"); !it.done(); ++it) {
        if (mailrc->local_p(*it) == true) continue;
        fprintf(temp_file, "Subject: %s\n", format_forward(*it).c_str());
    }
#elif defined(COMP)
    fprintf(temp_file,
            "From:    \n"
            "To:      \n"
            "Subject: \n"
        );
#endif

    fprintf(temp_file, "\n");

#if defined(REPL)
    /* Write out a little header that makes it apparent what's
     * happening with the message. */
    fprintf(temp_file, "On %s, %s wrote:\n",
            source_mf.header_date("Date").local().c_str(),
            mailrc->mail2name(source_mf.header_address("From")).c_str()
        );
#endif

#if defined(FORW)
    /* Write out a little header that makes it apparent what's
     * happening with the message. */
    fprintf(temp_file, "---------- Forwarded message ----------\n");

    for (auto it = source_mf.headers_address("From"); !it.done(); ++it) {
        if (mailrc->local_p(*it) == true) continue;
        fprintf(temp_file, "From:    %s\n", mailrc->mail2long(*it).c_str());
    }
    for (auto it = source_mf.headers_address("To"); !it.done(); ++it) {
        if (mailrc->local_p(*it) == true) continue;
        fprintf(temp_file, "To:      %s\n", mailrc->mail2long(*it).c_str());
    }
    for (auto it = source_mf.headers_address("CC"); !it.done(); ++it) {
        if (mailrc->local_p(*it) == true) continue;
        fprintf(temp_file, "CC:      %s\n", mailrc->mail2long(*it).c_str());
    }
    for (auto it = source_mf.headers("Subject"); !it.done(); ++it) {
        if (mailrc->local_p(*it) == true) continue;
        fprintf(temp_file, "Subject: %s\n", (*it).c_str());
    }
#endif

#if defined(REPL) || defined(FORW)
    /* Write out the body of the message, prefixed by a "> " to
     * indicate it's part of a reply. */
    size_t trailing_whitespace = 0;
    for (auto it = source_mf.body(); !it.done(); ++it) {
        /* Be sure to skip signatures, they're stupid. */
        if (strcmp((*it).c_str(), "--") == 0)
            break;

        /* Skip any trailing whitespace. */
        if (strcmp((*it).c_str(), "") == 0) {
            trailing_whitespace++;
            continue;
        }

        /* Re-include trailing whitespace whenever it's actually
         * important. */
        for (size_t i = 0; i < trailing_whitespace; ++i)
#if defined(REPL)
            fprintf(temp_file, "> \n");
#elif defined(FORW)
            fprintf(temp_file, "\n");
#else
#error "Define REPL or FORW"
#endif
        trailing_whitespace = 0;

        /* Here we finally output the text of the message! */
#if defined(REPL)
        fprintf(temp_file, "> %s\n", (*it).c_str());
#elif defined(FORW)
        fprintf(temp_file, "%s\n", (*it).c_str());
#else
#error "Define REPL or FORW"
#endif

    }
#endif

    temp_file_path.finish();

    /* Open up $EDITOR to allow the user to edit the message. */
    char cmd[COMMAND_LENGTH];
    snprintf(cmd, COMMAND_LENGTH, "emacs -nw \"%s\"", 
             temp_file_path.path().c_str());
    system(cmd);

    /* At this point we need to re-parse the input file as a message
     * header, modifying it to actually be usable by the rest of the
     * system. */
    mh::message_file edited_file(temp_file_path.path());

    /* Create a second temporary file and re-write that just parsed
     * file out to it. */
    auto filtered_file_path = dir.get_tmp();
    FILE *filtered_file = filtered_file_path.fp();

    /* The headers that have addresses in them should be looked up in
     * the address book */
    std::vector<std::string> address_headers({"from", "to", "cc", "bcc"});
    for (const auto& key: address_headers) {
        for (auto it = edited_file.headers_address(key); !it.done(); ++it) {
            fprintf(filtered_file, "%s: %s\n",
                    key.c_str(),
                    mailrc->mailias2long(*it).c_str()
                );
        }
    }

    /* The non-address headers are just copied directly over. */
    for (const auto& h: edited_file.headers()) {
        auto f = std::find(address_headers.begin(),
                           address_headers.end(),
                           h.key()
            );
        if (f != address_headers.end())
            continue;

        fprintf(filtered_file, "%s: %s\n",
                h.key().c_str(),
                h.value().c_str()
            );
    }

    /* Obtains an RFC-822 complient date string and attaches it to the
     * message. */
    {
        time_t tod;
        time(&tod);

        struct tm tm;
        localtime_r(&tod, &tm);

        char rfc_2822_date[DATE_LENGTH];
        strftime(rfc_2822_date, DATE_LENGTH, "%a, %d %b %Y %T %z", &tm);

        fprintf(filtered_file, "Date: %s\n", rfc_2822_date);
    }

    /* Builds a new message ID and attaches it to the current
     * message. */
    {
        uuid_t uuid;
        uuid_generate(uuid);

        char uuid_str[BUFFER_SIZE];
        uuid_unparse(uuid, uuid_str);

        char hostname[BUFFER_SIZE];
        gethostname(hostname, BUFFER_SIZE);

        char message_id[BUFFER_SIZE];
        snprintf(message_id, BUFFER_SIZE, "mhng-%s@%s", uuid_str, hostname);

        fprintf(filtered_file, "Message-ID: %s\n", message_id);
    }

    /* Drop in the message body right here. */
    fprintf(filtered_file, "\n");
    for (auto it = edited_file.body(); !it.done(); ++it)
        fprintf(filtered_file, "%s\n", (*it).c_str());

    /* Open up the drafts folder, which is where we will eventually
     * jam that message. */
    dir.insert("drafts", filtered_file_path);

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
std::string format_forward(const std::string subject)
{
    char buf[BUFFER_SIZE];
    snprintf(buf, BUFFER_SIZE, "FW: %s", subject.c_str());

    if (strncasecmp(buf, "FW: FW:", 7) == 0)
        return subject;

    return buf;
}
#endif
