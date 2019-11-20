/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBOAUTH2__PROCESS_HXX
#define LIBOAUTH2__PROCESS_HXX

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

namespace libmhoauth {
    class process {
    private:
        const pid_t _pid;

    public:
        process(pid_t pid)
        : _pid(pid)
        {}

        ~process(void)
        {}
    };

    /* Launches a process.  Note that args[0] is exactly argv[0], as opposed to
     * the helper function below you're probably using. */
    inline process launch_process(std::string executable, std::vector<std::string> args)
    {
        auto pid = fork();
        if (pid > 0)
            return process(pid);

        auto argv = new char*[args.size() + 1];
        for (size_t i = 0; i < args.size(); ++i)
            argv[i] = (char *)args[i].c_str();
        argv[args.size()] = NULL;
        execvp(executable.c_str(), argv);

        perror("execvp() failed");
        abort();
    }

    /* A helper function that allows users to launch a process via just a list
     * of arguments. */
    template <typename... A>
    process launch_process(std::string executable, A... args)
    { return launch_process(executable, {executable, std::forward<A>(args)...}); }
}

#endif
