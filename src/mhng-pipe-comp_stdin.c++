
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

#ifdef HAVE_UUID
#include <uuid.h>
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

int main(int argc, const char **argv)
{
    auto san_argv = new const char*[argc + 1];
    san_argv[0] = argv[0];
    int san_argc = 1;
    san_argv[san_argc] = NULL;

    auto args = mhng::args::parse_all_folders(san_argc, san_argv);

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
        char buffer[BUFFER_SIZE];
        while (fgets(buffer, BUFFER_SIZE, stdin) != NULL)
            fprintf(out, "%s", buffer);
        fclose(out);
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

        fclose(file);

        auto raw_mime = std::make_shared<mhng::mime::message>(raw);

        /* Now walk back through that MIME message and attempt to
         * perform any sort of necessary address book lookups. */
        std::vector<std::string> lookup;
        for (const auto& header: raw_mime->body()->headers()) {
            if (header->match({"From", "To", "CC", "BCC"})) {
                auto k = header->key();
                for (const auto v: header->split_commas()) {
                    auto a = args->mbox()->mrc()->emailias(v);
                    auto aa = a->rfc();
                    lookup.push_back(k + ": " + aa + "\n");
                }
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
    if (mime->header("Date").size() == 0) {
        auto date = mhng::date::now();
        mime->add_header("Date", date->local());
        mime->body()->add_header("Date", date->local());
    }

    /* Generate a unique identifier that cooresponds to this message,
     * which is used later for things like In-Reply-To. */
    if (mime->header("Message-ID").size() == 0) {
#ifdef HAVE_UUID
        uuid_t uuid;
        uuid_generate(uuid);

        char uuid_str[BUFFER_SIZE];
        uuid_unparse(uuid, uuid_str);
#else
        char uuid_str[BUFFER_SIZE];
        snprintf(uuid_str,
                 BUFFER_SIZE,
                 "%08llx%04x%04x",
                 mhng::date::now()->unix(),
                 arc4random(),
                 arc4random()
        );
#endif

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
