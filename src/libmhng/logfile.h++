/* Copyright (C) 2026 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef MHNG__LOGFILE_HXX
#define MHNG__LOGFILE_HXX

#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace mhng {
    class logfile;
    typedef std::shared_ptr<logfile> logfile_ptr;

    /* Redirects this process' stdout and stderr into a file on disk and
     * keeps that file from growing without bound by rotating it once it
     * gets too large.
     *
     * This exists so the long-running MHng daemons (mhng-daemon,
     * mhng-notify, ...) can own their own logs instead of leaning on a
     * shell "&>>" redirect or a LaunchAgent StandardErrorPath.  Neither
     * of those ever rotates, so the log grows forever -- which is exactly
     * what happens now that the daemon logs on every sync/idle/event.
     *
     * The redirect is done by dup2()-ing onto fd 1 and 2, so it needs no
     * changes to any of the existing fprintf(stderr, ...) / std::cerr /
     * perror() call sites, and -- importantly -- every child process the
     * daemon fork()+exec()s (mhimap-idle, mhimap-sync, mhoauth-refresh_loop,
     * ...) inherits the same log for free. */
    class logfile {
    private:
        const std::string _path;
        const size_t _max_bytes;
        const unsigned _keep;

        /* Guards _fd and the rotation itself so two callers (or the
         * rotation thread and an explicit caller) can't rotate at once. */
        std::mutex _lock;

        /* Our own handle to the live log, kept open purely so we can
         * fstat() its size cheaply.  -1 if the redirect failed. */
        int _fd;

        bool _rotator_running;

    public:
        /* Opens "path" (creating it, appending to it if it exists) and
         * redirects fd 1 and 2 into it.  "max_bytes" is the size at which
         * the live log is rotated; "keep" is how many rotated generations
         * (path.1 ... path.<keep>) to retain.  A "keep" of 0 truncates in
         * place rather than keeping any history. */
        logfile(const std::string& path, size_t max_bytes, unsigned keep);
        ~logfile(void);

        /* Rotate the log now, but only if it has grown past max_bytes.
         * Cheap (a single fstat()) and thread-safe when no rotation is
         * needed, which is the common case. */
        void rotate_if_needed(void);

        /* Spins up a detached background thread that calls
         * rotate_if_needed() every "seconds".  Use this from a daemon that
         * logs continuously; a one-shot tool can just let the size cap
         * apply the next time it starts. */
        void start_rotation_thread(unsigned seconds);
    };

    /* Builds a logfile and immediately redirects into it.  The defaults
     * (8 MiB per generation, 4 generations kept) can be overridden at
     * runtime with the MHNG_LOG_MAX_BYTES and MHNG_LOG_KEEP environment
     * variables, so the size policy can be tuned without a recompile. */
    logfile_ptr redirect_log(const std::string& path,
                             size_t max_bytes = 8 * 1024 * 1024,
                             unsigned keep = 4);
}

#endif
