/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

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

daemon::message_ptr daemon::message::response(uint64_t ticket) const
{
    auto out = std::make_shared<daemon::message>(wire.id);
    out->wire.response.event_ticket = ticket;

    return out;
}

daemon::message_ptr daemon::message::sync(int32_t seconds_since)
{
    auto out = std::make_shared<daemon::message>(daemon::message_type::SYNC);
    out->wire.sync.seconds_since = seconds_since;

    return out;
}

daemon::message_ptr daemon::message::new_message(uint64_t uid)
{
    auto out = std::make_shared<daemon::message>(daemon::message_type::NEW_MESSAGE);
    out->wire.new_message.uid = uid;

    return out;
}

daemon::message_ptr daemon::message::folder_event(uint32_t ticket)
{
    auto out = std::make_shared<daemon::message>(daemon::message_type::FOLDER_EVENT);
    out->wire.folder_event.ticket = ticket;

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
        fprintf(stderr, "  given    %llu\n", (long long unsigned)l);
        fprintf(stderr, "  required %llu\n", (long long unsigned)sizeof(struct message_on_wire));
        abort();
    }

    auto wire = (const struct message_on_wire *)b;
    auto type = uint2message_type(wire->type);
    auto out = std::make_shared<daemon::message>(type);
    bcopy(b, &out->wire, sizeof(*wire));
    return out;
}
