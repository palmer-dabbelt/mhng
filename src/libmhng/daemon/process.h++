/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__DAEMON__PROCESS_HXX
#define MHNG__DAEMON__PROCESS_HXX

#include <memory>

namespace mhng {
    namespace daemon {
        class process;
        typedef std::shared_ptr<process> process_ptr;
    }
}

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace mhng {
    namespace daemon {
        /* This handles running a process in the background along with
         * allowing it to be joined and killed from a number of
         * different places. */
        class process {
        private:
            /* This is set to TRUE when this process is running, and
             * FALSE otherwise.  Note that this lock also implicitly
             * protects _pid and _status: when _running is TRUE then
             * _pid is valid and _status isn't, when _running is FALSE
             * then _status is valid and _pid isn't. */
            bool _running;
            std::mutex _running_lock;
            std::condition_variable _running_signal;
            int _pid;
            int _status;

            std::string _filename;
            std::vector<std::string> _args;

            const int _timeout;

        public:
            /* Creates a new process -- note that this doesn't
             * actually start the process, it simply creates a
             * placeholder that _can_ start processes. */
            process(std::string filename, std::vector<std::string> args);

            /* Creates a process with a timeout. */
            process(std::string filename, std::vector<std::string> args, int timeout);

        public:
            /* Forks off the stored process, allowing for it to be
             * joined later. */
            void fork(void);

            /* Waits for this process to complete, returning a code
             * when it does. */
            int join(void);

            /* Sends a KILL signal to this process, which will cause
             * anyone who is waiting on the join to eventually end up
             * terminating. */
            void kill(void);

        private:
            static void do_fork(process *that) { return that->_do_fork(); }
            void _do_fork(void);

            static void do_timeout(process *that) { return that->_do_timeout(); }
            void _do_timeout(void);
        };
    }
}

#endif

