/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "real_connection.h++"
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

daemon::real_connection::real_connection(const std::string& path)
{
#ifdef __APPLE__
    _socket = socket(AF_UNIX, SOCK_STREAM, 0);
#else
    _socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
#endif
    if (_socket < 0) {
        perror("Unable to create UNIX socket\n");
        abort();
    }

    struct sockaddr_un local;
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, path.c_str());
    socklen_t len = sizeof(local);
    if (connect(_socket, (struct sockaddr *)&local, len) < 0) {
        perror("Unable to connect to MHng daemon");
        fprintf(stderr, "  path: '%s'\n", path.c_str());
        abort();
    }

    _recv_thread = std::thread(recv_thread_wrapper, this);
}

daemon::real_connection::~real_connection(void)
{
    close(_socket);
    _recv_thread.detach();
}

daemon::response_ptr daemon::real_connection::send(const message_ptr& msg)
{
    char buf[BUFFER_SIZE];
    size_t size = daemon::message::serialize(msg, buf, BUFFER_SIZE);

    auto resp = std::make_shared<response>(msg->id());

    /* We need a lock here to ensure that that insert into the
     * _outstanding map is atomic. */
    {
        std::unique_lock<std::mutex> lock(_outstanding_lock);

        auto l = _outstanding.find(msg->id());
        if (l != _outstanding.end()) {
            fprintf(stderr, "Re-use of message!\n");
            abort();
        }

        _outstanding[msg->id()] = resp;
    }

    if (::send(_socket, buf, size, 0) < 0) {
        perror("Unable to send");
        abort();
    }

    return resp;
}

void daemon::real_connection::recv_thread_main(void)
{
    while (true) {
        char buf[BUFFER_SIZE];
        ssize_t size = ::recv(_socket, buf, BUFFER_SIZE, 0);
        if (size <= 0)
            return;

        auto resp = daemon::message::deserialize(buf, size);

        {
            std::unique_lock<std::mutex> lock(_outstanding_lock);

            auto l = _outstanding.find(resp->id());
            if (l == _outstanding.end())
                continue;

            auto w = l->second.lock();
            if (w == NULL)
                continue;

            w->fill(resp->response_event_ticket());
        }
    }
}
