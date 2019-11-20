/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBOAUTH2__HTTP_SERVER_HXX
#define LIBOAUTH2__HTTP_SERVER_HXX

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <queue>
#include <unistd.h>
#include <uv.h>

namespace libmhoauth {
    class http_server {
    private:
        struct uv {
            /* A single TCP connection, which interally handles the */
            struct connection {
                uv_tcp_t tcp;
                std::queue<uint8_t> read_buffer;
                std::queue<uv_write_t *> write_buffer;
                std::queue<uv_buf_t *> write_buffer_buf;
                bool open;

                connection(uv_tcp_t *server);
                ~connection(void);

                uint8_t deq(void) {
                    while (read_buffer.size() == 0) {
                        sleep(1);
                        pump();
                    }

                    auto out = read_buffer.front();
                    read_buffer.pop();
                    return out;
                }

                void write(uint8_t element);
                void pump(void) { uv_run(tcp.loop, UV_RUN_ONCE); }

                /* UV callbacks */
                static void alloc_callback(uv_handle_t *handle,
                                           size_t suggested_size,
                                           uv_buf_t *buf);
                static void read_callback(uv_stream_t *handle,
                                          ssize_t nread,
                                          const uv_buf_t *buf);
                static void write_callback(uv_write_t *write,
                                           int status);
                static void close_callback(uv_handle_t *handle);
            };

            /* This is a UV instance that listens on a single TCP port for
             * connections, converting all the callbacks from C-style callbacks
             * to C++-style callbacks. */
            const std::function<void(std::unique_ptr<connection>)> cb_connection;

            /* This state is all a bit messy in that it */
            uv_loop_t *loop;
            uv_tcp_t tcp;
            int port;

            /* Picks a random port and listens locally. */
            uv(decltype(cb_connection) _cb_connection);
            ~uv(void);

            /* Runs the UV event loop, waiting for something to happen. */
            void pump(void) { uv_run(loop, UV_RUN_ONCE); }

            /* This just shims each C callback into the cooresponding C++
             * callback. */
            static void connection_wrapper(uv_stream_t *server, int status);
        };

    public:
        class request {
        private:
            std::unique_ptr<uv::connection> _uvconn;
            bool _raw_valid;
            std::vector<uint8_t> _raw;

        public:
            /* Requests turn a generic TCP connection, via uvconn, into an HTTP
             * request.  There's no cooresponding response field, these can be
             * handled directly.  Destroying the request closes the connection
             * to the client. */
            request(std::unique_ptr<uv::connection> uvconn);

        public:
            /* The entire set of bytes in the HTTP request, which requires
             * draining the entire queue. */
            std::vector<uint8_t> raw(void);

            /* Writes the given string back out to the client, with no
             * implement newline added. */
            void operator<<(std::string in);
        };

    private:
        uv _uv;

        std::queue<std::unique_ptr<uv::connection>> _connections;

    public:
        http_server(void);

    public:
        /* Since we're picking a port, users probably want to see which one
         * they ended up with so they can actually talk to the server. */
        unsigned short port(void) const { return _uv.port; }

        /* Internally the HTTP server consist of a queue of requests.  This
         * call pops that queue, blocking until there's an actual request
         * availiable. */
        request get_request(void)
        {
            while (_connections.empty()) {
                sleep(1);
                _uv.pump();
            }
            auto out = request(std::move(_connections.front()));
            _connections.pop();
            return out;
        }
    };
}

#endif
