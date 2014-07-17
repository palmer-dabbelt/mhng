
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

#include "args.h++"
#include "mailbox.h++"
#include "sequence_number.h++"
#include <unordered_map>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* Obtains the default MHNG folder. */
static std::string default_mhng_folder_path(void);

args::args(const std::vector<message_ptr>& messages,
           const std::vector<folder_ptr>& folders
    )
    : _messages(messages),
      _folders(folders)
{
}

args_ptr args::parse_all_messages(int argc, const char **argv)
{
    int flags = 0;
    flags |= pf_skipplus;
    flags |= pf_folders;
    flags |= pf_messages;
    flags |= pf_numbers;
    flags |= pf_allm;

    return parse(argc, argv, flags);
}

args_ptr args::parse(int argc, const char **argv __attribute__((unused)), int flags)
{
    bool messages_written = false;
    std::vector<std::pair<std::string, int>> message_seqs;

    bool folders_written = false;
    std::vector<std::string> folder_names;

    auto mhng_folder = default_mhng_folder_path();

    for (int i = 1; i < argc; ++i) {
    }

    auto dir = std::make_shared<mailbox>(mhng_folder);

    /* If the folder hasn't been written to then take a guess as to
     * which folders should be looked at. */
    if (folders_written == false) {
        if (flags & pf_allf) {
            fprintf(stderr, "Unimplemented\n");
            abort();
        } else {
            auto current_folder = dir->current_folder();
            if (current_folder == NULL) {
                fprintf(stderr, "Unable to obtain current folder\n");
                abort();
            }
            folder_names = { current_folder->name() };
        }
    }

    /* Now that we've got the folder names it's time to actually make
     * an array of folders. */
    std::vector<folder_ptr> folders;
    std::unordered_map<std::string, folder_ptr> folder_map;
    for (const auto& folder_name: folder_names) {
        auto folder = dir->open_folder(folder_name);
        if (folder == NULL) {
            fprintf(stderr, "Unable to open folder '%s'\n",
                    folder_name.c_str());
            abort();
        }

        folders.push_back(folder);
        folder_map[folder_name] = folder;
    }

    /* If we haven't touched the message list then try and figure out
     * what it should be by default. */
    if (messages_written == false) {
        if (flags & pf_allm) {
            for (const auto& folder: folders)
                for (const auto& message: folder->messages())
                    message_seqs.push_back(std::make_pair(folder->name(),
                                                          message->seq()));

        } else {
            fprintf(stderr, "Unimplemented\n");
            abort();
        }
    }

    /* Now that we've got the message IDs we can go ahead and
     * construct them all. */
    std::vector<message_ptr> messages;
    for (const auto& pair: message_seqs) {
        auto seq_int = pair.second;
        auto folder_name = pair.first;

        /* First figure out the folder we want to look into.  If this
         * fails it's some internal inconsistency, as the folder map
         * should always be properly populated. */
        auto l = folder_map.find(folder_name);
        if (l == folder_map.end()) {
            fprintf(stderr, "Unable to re-open folder '%s'\n",
                    folder_name.c_str());
            abort();
        }

        auto folder = l->second;
        auto seq = std::make_shared<sequence_number>(seq_int);
        auto message = folder->open(seq);
        messages.push_back(message);
    }

    /* At this point everything should be fixed up so we can just go
     * ahead and construct the relevant argument structure. */
    return std::make_shared<args>(messages, folders);
}

std::string default_mhng_folder_path(void)
{
    char buf[BUFFER_SIZE];
    snprintf(buf, BUFFER_SIZE, "%s/%s", getenv("HOME"), ".mhng");
    return buf;
}    
