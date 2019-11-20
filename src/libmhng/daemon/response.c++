/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "response.h++"
#include <unistd.h>
using namespace mhng;

daemon::response::response(uint32_t id)
    : _id(id),
      _finished(false)
{
}

void daemon::response::wait(void)
{
    std::unique_lock<std::mutex> lock(_lock);
    _signal.wait(lock, [this]{ return _finished; });
}

void daemon::response::fill(uint64_t ticket)
{
    std::lock_guard<std::mutex> lock(_lock);
    _ticket = ticket;
    _finished = true;
    _signal.notify_all();
}
