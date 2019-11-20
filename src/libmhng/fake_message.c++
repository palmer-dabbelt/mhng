/* Copyright (C) 2019 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "db/mh_messages.h++"
#include "fake_message.h++"
using namespace mhng;

/* This is exactly the same code as message::next_message, which further
 * indicates I have some sort of class heirarchy issue.  I don't care enough to
 * fix it. */
message_ptr fake_message::next_message(int offset)
{
    auto table = std::make_shared<db::mh_messages>(_mbox);
    return table->select(_folder->name(), _seq, offset);
}
