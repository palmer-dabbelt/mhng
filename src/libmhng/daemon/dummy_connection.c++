
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

#include "dummy_connection.h++"
#include "response.h++"
using namespace mhng;

daemon::dummy_connection::dummy_connection(void)
{
}

daemon::dummy_connection::~dummy_connection(void)
{
}

daemon::response_ptr daemon::dummy_connection::send(const message_ptr& msg)
{
    auto resp = std::make_shared<response>(msg->id());
    resp->fill(0);
    return resp;
}
