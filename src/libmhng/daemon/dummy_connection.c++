/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

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
