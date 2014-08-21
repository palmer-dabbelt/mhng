
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

#include "message.h++"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
using namespace mhng;

static uint32_t message_id = 0;

daemon::message::message(message_type type)
{
    wire.id = message_id++;
    wire.type = message_type2uint(type);
}

daemon::message::message(uint32_t id)
{
    wire.id = id;
    wire.type = message_type2uint(daemon::message_type::RESPONSE);
}

daemon::message_ptr daemon::message::response(void) const
{
    auto out = std::make_shared<daemon::message>(wire.id);
    return out;
}

daemon::message_ptr daemon::message::sync(int32_t seconds_since)
{
    auto out = std::make_shared<daemon::message>(daemon::message_type::SYNC);
    out->wire.sync.seconds_since = seconds_since;

    return out;
}

daemon::message_ptr daemon::message::net_up(void)
{
    auto out = std::make_shared<daemon::message>(daemon::message_type::NET_UP);

    return out;
}

daemon::message_ptr daemon::message::net_down(void)
{
    auto out = std::make_shared<daemon::message>(daemon::message_type::NET_DOWN);

    return out;
}

size_t daemon::message::serialize(const message_ptr& m, char *b, size_t l)
{
    if (sizeof(m->wire) > l) {
        fprintf(stderr, "Message too small!\n");
        abort();
    }

    auto size = sizeof(m->wire);
    bcopy(&m->wire, b, size);
    return size;
}

daemon::message_ptr daemon::message::deserialize(const char *b, size_t l)
{
    if (sizeof(struct message_on_wire) > l) {
        fprintf(stderr, "Message too small!\n");
        fprintf(stderr, "  given    %lu\n", l);
        fprintf(stderr, "  required %lu\n", sizeof(struct message_on_wire));
        abort();
    }

    auto wire = (const struct message_on_wire *)b;
    auto type = uint2message_type(wire->type);
    auto out = std::make_shared<daemon::message>(type);
    bcopy(b, &out->wire, sizeof(*wire));
    return out;
}
