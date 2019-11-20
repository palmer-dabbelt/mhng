/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhoauth/http_server.h++>
#include <iostream>
#include <random>
using namespace libmhoauth;

static const int PORT_RETRIES = 1024;
static const int KERNEL_CONNECTIONS = 10;

template <int depth, typename Element> class fir {
private:
    Element _history[depth];
    int _index;
    std::function<bool(int index, Element datum)> _func;

public:
    fir(std::function<bool(int index, Element datum)> func)
    : _index(0),
      _func(func)
    {}

    Element operator<<(Element in)
    {
        auto out = _history[_index];
        _history[_index] = in;
        _index = (_index + 1) % depth;
        return out;
    }

    operator bool() const {
        bool match = true;
        for (int i = 0; i < depth; ++i)
            match &= _func(i, _history[(_index + i) % depth]);
        return match;
    }
};

http_server::uv::connection::connection(uv_tcp_t *server)
: open(true)
{
    uv_tcp_init(server->loop, &tcp);
    ((uv_handle_t *)&tcp)->data = this;
    uv_accept((uv_stream_t *)server, (uv_stream_t *)&tcp);
    uv_read_start((uv_stream_t *)&tcp, &alloc_callback, &read_callback);
}

http_server::uv::connection::~connection(void)
{
    while (write_buffer.size() > 0) {
        sleep(1);
        pump();
    }

    uv_close((uv_handle_t *)&tcp, close_callback);
    while (open) {
        sleep(1);
        pump();
    }
}

void http_server::uv::connection::write(uint8_t element)
{
    auto buf = new uv_buf_t;
    buf->len = 1;
    buf->base = new char[1];
    buf->base[0] = (char)element;
    write_buffer_buf.push(buf);

    auto write = new uv_write_t;
    ((uv_handle_t *)write)->data = this;
    write_buffer.push(write);

    uv_write(write, (uv_stream_t *)&tcp, buf, 1, write_callback);
}

void http_server::uv::connection::alloc_callback(uv_handle_t *handle __attribute__((unused)),
                                                 size_t suggested_size,
                                                 uv_buf_t *buf)
{
    buf->base = new char[suggested_size];
    buf->len  = suggested_size;
}

void http_server::uv::connection::read_callback(uv_stream_t *handle,
                                                ssize_t nread,
                                                const uv_buf_t *buf)
{
    auto that = (struct connection *)(((uv_handle_t *)handle)->data);
    for (ssize_t i = 0; i < nread; ++i)
        that->read_buffer.push((uint8_t)buf->base[i]);
    delete[] buf->base;
}

void http_server::uv::connection::write_callback(uv_write_t *write,
                                                 int status __attribute__((unused)))
{
    auto that = (connection *)(((uv_handle_t *)write)->data);

    auto stored_write = that->write_buffer.front();
    if (stored_write != write) {
        std::cerr << "Detected out of order write, blowing up\n";
        abort();
    }
    that->write_buffer.pop();
    delete write;

    auto buf = that->write_buffer_buf.front();
    that->write_buffer_buf.pop();
    delete[] buf->base;
    delete buf;
}

void http_server::uv::connection::close_callback(uv_handle_t *handle)
{
    auto that = (struct connection *)(((uv_handle_t *)handle)->data);
    that->open = false;
}

http_server::uv::uv(decltype(cb_connection) _cb_connection)
: cb_connection(_cb_connection)
{
    loop = uv_default_loop();
    uv_tcp_init(loop, &tcp);

    port = [&](){
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> ports(1024, 49151);
        for (int i = 0; i < PORT_RETRIES; ++i) {
            auto port = ports(rng);
            struct sockaddr_in addr;
            uv_ip4_addr("127.0.0.1", port, &addr);
            if (uv_tcp_bind(&tcp, (struct sockaddr *)&addr, 0) == 0)
                return port;
        }

        abort();
    }();

    ((uv_handle_t *)&tcp)->data = this;
    uv_listen((uv_stream_t *)&tcp, KERNEL_CONNECTIONS, &connection_wrapper);
    std::cerr << "Listening on http://localhost:" << std::to_string(port) << "\n";
}

http_server::uv::~uv(void)
{
}

void http_server::uv::connection_wrapper(uv_stream_t *server, int status __attribute__((unused)))
{
    auto that = (struct uv *)(((uv_handle_t *)server)->data);
    that->cb_connection(std::move(std::make_unique<connection>(&that->tcp)));
}

http_server::request::request(std::unique_ptr<uv::connection> uvconn)
: _uvconn(std::move(uvconn)),
  _raw_valid(false)
{
}

std::vector<uint8_t> http_server::request::raw(void)
{
    if (_raw_valid)
        return _raw;

    auto empty_line = fir<4, uint8_t>( [](int index __attribute__((unused)), uint8_t ch) { return ch == '\r' || ch == '\n'; } );
    while (!empty_line) {
        auto byte = _uvconn->deq();
        empty_line << byte;
        _raw.push_back(byte);
    }

    _raw_valid = true;
    return _raw;
}

void http_server::request::operator<<(std::string in)
{
    for (const auto& byte: in)
        _uvconn->write(byte);
}

http_server::http_server(void)
: _uv([this](auto conn){ _connections.push(std::move(conn)); })
{
}
