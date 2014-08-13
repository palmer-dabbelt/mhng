
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

#include "process.h++"
#include <unistd.h>
#include <sys/wait.h>
using namespace mhng;

daemon::process::process(const char *filename, const char *name)
    : _running(false),
      _running_lock(),
      _running_signal(),
      _pid(),
      _status(),
      _filename(filename),
      _name(name)
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

    _running_signal.wait(lock, [&]{ return _running == true; });
}

int daemon::process::join(void)
{
    std::unique_lock<std::mutex> lock(_running_lock);
    _running_signal.wait(lock, [&]{ return _running == false; });
    return _status;
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
