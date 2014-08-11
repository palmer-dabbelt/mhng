
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

#include "connection.h++"
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

daemon::connection::connection(const std::string& path)
{
    _socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (_socket < 0) {
        perror("Unable to create UNIX socket\n");
        abort();
    }

    struct sockaddr_un local;
    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, path.c_str());
    socklen_t len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (connect(_socket, (struct sockaddr *)&local, len) < 0) {
        perror("Unable to connect to MHng daemon");
        fprintf(stderr, "  path: '%s'\n", path.c_str());
        abort();
    }
}

daemon::connection::~connection(void)
{
    close(_socket);
}

daemon::response_ptr daemon::connection::send(const message_ptr& msg)
{
    {
        auto l = _outstanding.find(msg->id());
        if (l != _outstanding.end()) {
            fprintf(stderr, "Re-use of message!\n");
            abort();
        }
    }

    char buf[BUFFER_SIZE];
    size_t size = daemon::message::serialize(msg, buf, BUFFER_SIZE);
    if (::send(_socket, buf, size, 0) < 0) {
        perror("Unable to send");
        abort();
    }

    auto resp = std::make_shared<response>(msg->id());
    _outstanding[msg->id()] = resp;
    return resp;
}
