
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
#include "db/mh_current.h++"
#include "version.h"
#include <unordered_map>
#include <string.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

/* Obtains the default MHNG folder. */
static std::string default_mhng_folder_path(void);

args::args(const std::vector<message_ptr>& messages,
           const std::vector<folder_ptr>& folders,
           const std::vector<int>& numbers,
           const mailbox_ptr& mbox,
           const unknown<bool>& stdout,
           const unknown<bool>& nowrap,
           const unknown<bool>& nomailrc,
           const std::vector<std::string>& attach)
    : _messages(messages),
      _folders(folders),
      _numbers(numbers),
      _mbox(mbox),
      _stdout(stdout),
      _nowrap(nowrap),
      _nomailrc(nomailrc),
      _attach(attach)
{
}

args_ptr args::parse_normal(int argc, const char **argv)
{
    int flags = 0;
    flags |= pf_skipplus;
    flags |= pf_folders;
    flags |= pf_messages;

    return parse(argc, argv, flags);
}

args_ptr args::parse_all_messages(int argc, const char **argv)
{
    int flags = 0;
    flags |= pf_skipplus;
    flags |= pf_folders;
    flags |= pf_messages;
    flags |= pf_allm;

    return parse(argc, argv, flags);
}

args_ptr args::parse_all_messages(int argc, const char **argv, const std::vector<std::string>& more)
{
    int flags = 0;
    flags |= pf_skipplus;
    flags |= pf_folders;
    flags |= pf_messages;
    flags |= pf_allm;

    auto new_argv = new const char*[argc + more.size()];
    new_argv[0] = argv[0];
    for (size_t i = 0; i < more.size(); ++i)
        new_argv[i + 1] = more[i].c_str();
    for (size_t i = 1; i < (size_t)argc; ++i)
        new_argv[i + more.size()] = argv[i];

    auto out = parse(argc + more.size(), new_argv, flags);

    delete[] new_argv;
    return out;
}

args_ptr args::parse_all_folders(int argc, const char **argv)
{
    int flags = 0;
    flags |= pf_skipplus;
    flags |= pf_folders;
    flags |= pf_messages;
    flags |= pf_allf;
    flags |= pf_nom;

    return parse(argc, argv, flags);
}

args_ptr args::parse_numbers(int argc, const char **argv)
{
    int flags = 0;
    flags |= pf_skipplus;
    flags |= pf_folders;
    flags |= pf_numbers;

    return parse(argc, argv, flags);
}

args_ptr args::parse_noimplicit(int argc, const char **argv)
{
    int flags = 0;
    flags |= pf_nof;
    flags |= pf_nom;

    return parse(argc, argv, flags);
}

args_ptr args::parse(int argc, const char **argv, int flags)
{
    bool messages_written = false;
    std::vector<std::pair<std::string, int>> message_seqs;

    bool folders_written = false;
    std::vector<std::string> folder_names;
    std::string last_folder = "";

    std::vector<int> numbers;

    unknown<bool> stdout;
    unknown<bool> nowrap;
    unknown<bool> thread;
    unknown<bool> nomailrc;
    unknown<bool> noimplicit;

    std::vector<std::string> attach;

    auto mhng_folder = default_mhng_folder_path();

    for (int i = 1; i < argc; ++i) {
        /* FIXME: Much of this isn't implemented yet... */
        /* Here is my attempt to parse folder names, sequence numbers,
         * etc.  The rules are as follows:
         *  - Any pair of integers, "%d-%d" is an inclusive range
         *  - Any integer is a sequence number
         *  - Anything that starts with a "+" is a folder name
         *  - Anything that starts with a ":" is a sequence number
         *  - Anything that starts with a "@" is a UID
         *  - Anything that starts with a "-" is an argument
         *  - Anything else is a folder name
         */
        int r_start, r_end;
        if (sscanf(argv[i], "%d-%d", &r_start, &r_end) == 2) {
            if (flags & pf_numbers) {
                for (auto i = r_start; i <= r_end; ++i)
                    numbers.push_back(i);
            } else {
                messages_written = true;
                for (auto i = r_start; i <= r_end; ++i) {
                    message_seqs.push_back(
                        std::make_pair(last_folder, i)
                        );
                }
            }
        } else if (atoi(argv[i]) > 0) {
            if (flags & pf_numbers) {
                numbers.push_back(atoi(argv[i]));
            } else {
                messages_written = true;
                message_seqs.push_back(
                    std::make_pair(last_folder, atoi(argv[i]))
                    );
            }
        } else if (argv[i][0] == '+') {
            folders_written = true;
            folder_names.push_back(argv[i]+1);
            last_folder = argv[i]+1;
        } else if (strcmp(argv[i], "--stdout") == 0) {
            stdout = true;
        } else if (strcmp(argv[i], "--nowrap") == 0) {
            nowrap = true;
        } else if (strcmp(argv[i], "--version") == 0) {
            printf("%s\n", PCONFIGURE_VERSION);
            exit(0);
        } else if (strcmp(argv[i], "--attach") == 0) {
            attach.push_back(argv[i+1]);
            i++;
        } else if (strcmp(argv[i], "--thread") == 0) {
            thread = true;
        } else if (strcmp(argv[i], "--no-mailrc") == 0) {
            nomailrc = true;
        } else {
            folders_written = true;
            folder_names.push_back(argv[i]);
            last_folder = argv[i];
        }
    }

    auto dir = std::make_shared<mailbox>(
        mhng_folder,
        (nomailrc.known() == true) && (nomailrc.data() == true)
    );
    dir->set_self_pointer(dir);

    /* If the folder hasn't been written to then take a guess as to
     * which folders should be looked at. */
    if (folders_written == false) {
        if (flags & pf_allf) {
            auto current = std::make_shared<db::mh_current>(dir);
            folder_names = current->select();
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
    folder_ptr last_folder_ptr = NULL;
    for (const auto& folder_name: folder_names) {
        auto folder = dir->open_folder(folder_name);
        if (folder == NULL) {
            fprintf(stderr, "Unable to open folder '%s'\n",
                    folder_name.c_str());
            abort();
        }

        folders.push_back(folder);
        folder_map[folder_name] = folder;
        last_folder_ptr = folder;
    }

    /* If we haven't touched the message list then try and figure out
     * what it should be by default. */
    if (messages_written == false) {
        if (flags & pf_allm) {
            std::vector<message_ptr> messages;
            for (const auto& folder: folders)
                for (const auto& message: folder->messages())
                    messages.push_back(message);
            return std::make_shared<args>(messages,
                                          folders,
                                          numbers,
                                          dir,
                                          stdout,
                                          nowrap,
                                          nomailrc,
                                          attach);
        } else if (flags & pf_nom) {
            std::vector<message_ptr> messages;
            return std::make_shared<args>(messages,
                                          folders,
                                          numbers,
                                          dir,
                                          stdout,
                                          nowrap,
                                          nomailrc,
                                          attach);
        } else if (thread == true) {
            std::vector<message_ptr> messages;
            auto cur = last_folder_ptr->current_message();
            if (cur == NULL) {
                fprintf(stderr, "Unable to open current message\n");
                fprintf(stderr, "  Probably you just removed it\n");
                abort();
            }
            messages.push_back(cur);
            for (const auto& mit: cur->get_messages_in_thread())
                messages.push_back(mit);
            return std::make_shared<args>(messages,
                                          folders,
                                          numbers,
                                          dir,
                                          stdout,
                                          nowrap,
                                          nomailrc,
                                          attach);
        } else {
            std::vector<message_ptr> messages;
            auto cur = last_folder_ptr->current_message();
            if (cur == NULL) {
                fprintf(stderr, "Unable to open current message\n");
                fprintf(stderr, "  Probably you just removed it\n");
                abort();
            }
            messages.push_back(cur);
            return std::make_shared<args>(messages,
                                          folders,
                                          numbers,
                                          dir,
                                          stdout,
                                          nowrap,
                                          nomailrc,
                                          attach);
        }
    }

    /* Now that we've got the message IDs we can go ahead and
     * construct them all. */
    std::vector<message_ptr> messages;
    for (const auto& pair: message_seqs) {
        auto seq_int = pair.second;
        auto folder_name = pair.first;

        if (strcmp(folder_name.c_str(), "") == 0)
            folder_name = dir->current_folder()->name();

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
        if (message == NULL) {
            fprintf(stderr, "Unable to open '%d' in '%s'\n",
                    seq_int,
                    folder_name.c_str()
                );
            abort();
        }

        messages.push_back(message);
    }

    /* At this point everything should be fixed up so we can just go
     * ahead and construct the relevant argument structure. */
    return std::make_shared<args>(messages,
                                  folders,
                                  numbers,
                                  dir,
                                  stdout,
                                  nowrap,
                                  nomailrc,
                                  attach);
}

std::string default_mhng_folder_path(void)
{
    char buf[BUFFER_SIZE];

    if (getenv("MHNG_MAILDIR") != NULL) {
        snprintf(buf, BUFFER_SIZE, "%s", getenv("MHNG_MAILDIR"));
        return buf;
    } else {
        snprintf(buf, BUFFER_SIZE, "%s/%s", getenv("HOME"), ".mhng");
        return buf;
    }
}    
