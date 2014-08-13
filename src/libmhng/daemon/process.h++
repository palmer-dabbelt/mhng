
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

            const char *_filename;
            const char *_name;

        public:
            /* Creates a new process -- note that this doesn't
             * actually start the process, it simply creates a
             * placeholder that _can_ start processes. */
            process(const char *filename, const char *name);

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
        };
    }
}

#endif

