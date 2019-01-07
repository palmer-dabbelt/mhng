
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
