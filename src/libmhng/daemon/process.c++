/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "process.h++"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
using namespace mhng;

daemon::process::process(const char *filename, const char *name)
    : _running(false),
      _running_lock(),
      _running_signal(),
      _pid(),
      _status(),
      _filename(filename),
      _name(name),
      _timeout(0)
{
}

daemon::process::process(const char *filename, const char *name, int timeout)
    : _running(false),
      _running_lock(),
      _running_signal(),
      _pid(),
      _status(),
      _filename(filename),
      _name(name),
      _timeout(timeout)
{
}

void daemon::process::fork(void)
{
    std::unique_lock<std::mutex> lock(_running_lock);

    if (_running == true) {
        fprintf(stderr, "fork() while running!\n");
        abort();
    }

    std::thread forker(do_fork, this);
    forker.detach();

    if (_timeout > 0) {
        std::thread timeout(do_timeout, this);
        timeout.detach();
    }

    _running_signal.wait(lock, [&]{ return _running == true; });
}

int daemon::process::join(void)
{
    std::unique_lock<std::mutex> lock(_running_lock);
    _running_signal.wait(lock, [&]{ return _running == false; });
    return _status;
}

void daemon::process::kill(void)
{
    {
        std::unique_lock<std::mutex> lock(_running_lock);
        if (_running == false)
            return;

        ::kill(_pid, SIGTERM);
    }

    sleep(10);

    {
        std::unique_lock<std::mutex> lock(_running_lock);
        if (_running == false)
            return;

        ::kill(_pid, SIGKILL);
    }
}

void daemon::process::_do_fork(void)
{
    _pid = ::fork();
    if (_pid == 0) {
        execl(_filename, _name, NULL);
        perror("Unable to exec\n");
        abort();
    }

    {
        std::unique_lock<std::mutex> lock(_running_lock);
        _running = true;
        _running_signal.notify_all();
    }

    int status;
    if (waitpid(_pid, &status, 0) < 0) {
        perror("Unable to waitpid()");
        abort();
    }

    {
        std::unique_lock<std::mutex> lock(_running_lock);
        _running = false;
        _status = status;
        _pid = 0;
        _running_signal.notify_all();
    }
}

void daemon::process::_do_timeout(void)
{
    sleep(_timeout);
    kill();
}
