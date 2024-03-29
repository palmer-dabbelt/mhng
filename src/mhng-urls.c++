/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhng/args.h++>
#include <libmhng/mime/part.h++>
#include <functional>
#include <string.h>
#include <stdio.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* Returns a list of all the URLs in a single line. */
static std::vector<std::string> find_urls_in(const std::string line);

/* Returns TRUE when the given character terminates a URL. */
static bool is_url_terminator(char c);

int main(int argc, const char **argv)
{
    auto args = mhng::args::parse_numbers(argc, argv);
#ifndef PIPE
    args->mbox()->set_current_folder(args->folders()[0]);
    args->folders()[0]->set_current_message(args->messages()[0]);
#endif

    /* The first thing we want to do is build up a vector of the
     * URLs found when walking this message. */
    std::vector<std::pair<std::string, std::string>> urls;
    std::map<mhng::mime::part_ptr, size_t> part2depth;
    for (const auto& msg: args->messages())
        for (const auto& line: msg->body_utf8())
            for (const auto& url: find_urls_in(line))
                urls.push_back({msg->imap_account_name(), url});

    /* If we were given any arbitrary numbers on the commandline then
     * go ahead and open the cooresponding URLs in a web browser. */
    for (auto num: args->numbers()) {
        if (num > (int)(urls.size())) {
            fprintf(stderr, "Requested URL %d, but only %d URLs\n",
                    num, (int)(urls.size())
                );
            abort();
        }

	auto acc = urls[num - 1].first;
        auto url = urls[num - 1].second;

        if (args->stdout() == true) {
            printf("%s\n", url.c_str());
        } else {
            char command[BUFFER_SIZE];
            snprintf(command, BUFFER_SIZE, "%s --user \"%s\" -- \"%s\"",
                     getenv("BROWSER"),
		     acc.c_str(),
                     url.c_str()
                );
	    fprintf(stderr, "command: %s\n", command);
            if (system(command) != 0)
                fprintf(stderr, "command '%s' failed\n",
                        command);
        }
    }
    if (args->numbers().size() != 0)
        return 0;

    /* If we weren't given any numbers on the commandline then print
     * out every URL as it comes in. */
    for (size_t i = 0; i < urls.size(); ++i) {
        printf("%llu %s\n",
	       (long long unsigned)(i+1),
	       urls[i].second.c_str()
	       );
    }

    return 0;
}

std::vector<std::string> find_urls_in(const std::string line_cxx)
{
    /* Make a copy of that string for our personal use. */
    char line[BUFFER_SIZE];
    snprintf(line, BUFFER_SIZE, "%s", line_cxx.c_str());

    /* Here's the list of URLs that we're going to return, it's built
     * up during parsing. */
    std::vector<std::string> out;

    /* This function adds URLs that start with the given prefix to the
     * list. */
    auto add_url = [&](const char *prefix) -> void
        {
            char *cur = line;
            while ((cur = strcasestr(cur, prefix)) != NULL) {
                char url[BUFFER_SIZE];
                snprintf(url, BUFFER_SIZE, "%s", cur);

                char *end = url;
                while (*end != '\0' && !is_url_terminator(*end))
                    end++;
                *end = '\0';

                out.push_back(url);
                cur++;
            }
        };

    add_url("http://");
    add_url("https://");

    return out;
}

bool is_url_terminator(char c)
{
    if (isspace(c))
        return true;

    if (c == '>')
        return true;

    if (c == ')')
        return true;

    if (c == '"')
        return true;

    return false;
}
