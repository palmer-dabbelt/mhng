
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
#include <string.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* Returns a list of all the URLs in a single line. */
static std::vector<std::string> find_urls_in(const std::string line);

/* Returns TRUE when the given character terminates a URL. */
static bool is_url_terminator(char c);

int main(int argc, const char **argv)
{
    auto o = mh::options::create(0, NULL);
    mh::mhdir dir(o);

    /* Opens the default folder. */
    mh::folder folder = dir.open_folder(true);

    /* Reads the current message from the given folder. */
    mh::message message = folder.open_current();

    /* Reads the full message contents from disk. */
    mh::message_file mf = message.read();

    /* Build a list of all the URLs in the message. */
    std::vector<std::string> urls;
    for (auto it = mf.body(); !it.done(); ++it) {
        for (const auto& url: find_urls_in(*it))
            urls.push_back(url);
    }

    /* If we're not given any arguments then just print out all the
     * URLs in the message, prefixed by a number that you can use to
     * open them. */
    if (argc == 1) {
        for (size_t i = 1; i <= urls.size(); ++i)
            printf("%lu %s\n", i, urls[i-1].c_str());

        return 0;
    } 

    if (argc == 2) {
        int index = atoi(argv[1]);

        if (index <= 0) {
            perror("Unable to parse URL index");
            abort();
        }

        if ((size_t)index > urls.size()) {
            fprintf(stderr, "Index %d, but only %lu URLs\n",
                    index,
                    urls.size()
                );
            abort();
        }

        char command[BUFFER_SIZE];
        snprintf(command, BUFFER_SIZE, "%s %s",
                 "firefox",
                 urls[index-1].c_str()
            );
        system(command);

        return 0;
    }

    fprintf(stderr, "Bad command-line arguments\n");
    abort();
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

    return false;
}
