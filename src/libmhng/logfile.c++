/* Copyright (C) 2026 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "logfile.h++"
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace mhng;

/* Point both stdout and stderr at "fd".  We deliberately keep going if
 * one of the dup2()s fails: a half-redirected log still beats aborting a
 * mail daemon over a logging hiccup. */
static void dup_onto_std(int fd)
{
    if (dup2(fd, STDOUT_FILENO) < 0)
        perror("mhng::logfile: unable to redirect stdout");
    if (dup2(fd, STDERR_FILENO) < 0)
        perror("mhng::logfile: unable to redirect stderr");
}

logfile::logfile(const std::string& path, size_t max_bytes, unsigned keep)
: _path(path),
  _max_bytes(max_bytes),
  _keep(keep),
  _fd(-1),
  _rotator_running(false)
{
    /* Logs can carry subjects and addresses, so keep them private. */
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0600);
    if (fd < 0) {
        perror(("mhng::logfile: unable to open " + path).c_str());
        return;
    }

    dup_onto_std(fd);
    _fd = fd;
}

logfile::~logfile(void)
{
    /* The rotation thread is detached and only ever touches "this", so we
     * can't be destructed while it runs in practice (the daemons that use
     * a rotation thread never destruct their logfile).  Just release our
     * private handle; fd 1 and 2 stay redirected, which is what we want
     * for whatever is still writing to them. */
    if (_fd >= 0)
        close(_fd);
}

void logfile::rotate_if_needed(void)
{
    std::unique_lock<std::mutex> lock(_lock);

    if (_fd < 0)
        return;

    struct stat st;
    if (fstat(_fd, &st) < 0) {
        perror("mhng::logfile: unable to stat log");
        return;
    }

    if ((size_t)st.st_size < _max_bytes)
        return;

    /* Flush anything still buffered in stdio (stdout is block-buffered
     * once it points at a regular file) so it lands in the outgoing
     * generation rather than bleeding into the fresh one. */
    fflush(NULL);

    /* With no history requested just reset the live file in place. */
    if (_keep == 0) {
        if (ftruncate(_fd, 0) < 0)
            perror("mhng::logfile: unable to truncate log");
        return;
    }

    /* Shift the generations down: drop the oldest, then rename
     * path.(keep-1) -> path.keep, ..., path -> path.1.  Missing files
     * just fail the rename() harmlessly. */
    std::string oldest = _path + "." + std::to_string(_keep);
    unlink(oldest.c_str());

    for (unsigned i = _keep; i > 1; i--) {
        std::string dst = _path + "." + std::to_string(i);
        std::string src = _path + "." + std::to_string(i - 1);
        rename(src.c_str(), dst.c_str());
    }
    rename(_path.c_str(), (_path + ".1").c_str());

    /* Reopen the (now missing) live path and swing fd 1 and 2 over to it.
     * Children that were fork()ed before this keep writing into the
     * renamed generation until they exit, which is harmless. */
    int fd = open(_path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0600);
    if (fd < 0) {
        perror("mhng::logfile: unable to reopen log after rotation");
        return;
    }

    dup_onto_std(fd);
    close(_fd);
    _fd = fd;
}

void logfile::start_rotation_thread(unsigned seconds)
{
    std::unique_lock<std::mutex> lock(_lock);

    if (_rotator_running)
        return;
    _rotator_running = true;

    std::thread rotator([this, seconds] {
        while (true) {
            sleep(seconds);
            rotate_if_needed();
        }
    });
    rotator.detach();
}

logfile_ptr mhng::redirect_log(const std::string& path,
                               size_t max_bytes,
                               unsigned keep)
{
    if (const char *env = getenv("MHNG_LOG_MAX_BYTES")) {
        char *end = NULL;
        unsigned long long v = strtoull(env, &end, 10);
        if (end != env && *end == '\0' && v > 0)
            max_bytes = (size_t)v;
    }

    if (const char *env = getenv("MHNG_LOG_KEEP")) {
        char *end = NULL;
        unsigned long v = strtoul(env, &end, 10);
        if (end != env && *end == '\0')
            keep = (unsigned)v;
    }

    return std::make_shared<logfile>(path, max_bytes, keep);
}
