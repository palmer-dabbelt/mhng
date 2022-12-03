/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <string.h>
#include <unistd.h>
#include <sstream>

#ifdef HAVE_UUID
#include <uuid.h>
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* Joins a vector of strings, seperating elements with sep1.  When the string
 * length is over max, seperate them with sep2 instead. */
static std::string joinmax(const std::vector<std::string>& v, std::string sep1, size_t max, std::string sep2);

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
        std::map<std::string, std::vector<std::string>> oheaders;
        for (const auto& header: raw_mime->root()->headers()) {
            if (header->match({"From", "To", "CC", "BCC"})) {
                auto k = header->key();
                for (const auto& v: header->split_commas()) {
                    auto a = args->mbox()->mrc()->emailias(v);
                    auto aa = a->rfc();
                    auto o = oheaders.find(k);
                    if (o == oheaders.end()) {
                        oheaders.insert(std::make_pair(k, std::vector<std::string>()));
                        o = oheaders.find(k);
                    }
                    o->second.push_back(aa);
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

        lookup.push_back("\n");
        bool in_headers = true;
        for (const auto& body: raw_mime->root()->raw()) {
            if (in_headers) {
                if (strcmp(body.c_str(), "") == 0)
                    in_headers = false;
                if (strcmp(body.c_str(), "\r") == 0)
                    in_headers = false;
                if (strcmp(body.c_str(), "\n") == 0)
                    in_headers = false;
                if (strcmp(body.c_str(), "\r\n") == 0)
                    in_headers = false;
                if (strcmp(body.c_str(), "\n\r") == 0)
                    in_headers = false;
            } else {
                lookup.push_back(body);
            }
        }
        mime = std::make_shared<mhng::mime::message>(lookup);
    }

    /* Date-stamp the message with the current date. */
    if (mime->header("Date").size() == 0) {
        auto date = mhng::date::now();
        auto env_date = getenv("MHNG_COMP_DATE");
        auto date_str = (env_date == NULL) ? date->local() : std::string(env_date);
        mime->add_header("Date", date_str);
        mime->body()->add_header("Date", date_str);
    }

    /* Generate a unique identifier that cooresponds to this message,
     * which is used later for things like In-Reply-To. */
    if (mime->header("Message-ID").size() == 0) {
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

        mime->add_header("Message-ID", message_id_str);
        mime->body()->add_header("Message-ID", message_id_str);
    }

    /* Go ahead and insert this message into the database. */
    auto message = args->mbox()->insert(FOLDER, mime);

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
