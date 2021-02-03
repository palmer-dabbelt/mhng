/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBOAUT2__HTTP_REQUEST_HXX
#define LIBOAUT2__HTTP_REQUEST_HXX

#include <map>
#include <string>

namespace libmhoauth {
    /* A HTTP request, which comes with a cooresponding response.  Simply
     * constructing this will send a message to the HTTP server. */
    class http_request {
    public:
        enum class method {
            POST,
        };

        class response {
        public:
            const std::string _body;

        public:
            response(const std::string& body)
            : _body(body)
            {}

            /* Returns the body of the response, as a list of bytes. */
            std::string body(void) const { return _body; }
        };

    private:
        const response _response;

    public:
        http_request(method m,
                     std::string uri,
                     std::map<std::string, std::string> headers,
                     std::string body)
        : _response{do_curl(m, uri, headers, body)}
        {}

    public:
        /* Gets the response back from the server. */
        const response& response(void) { return _response; }

    private:
        /* Returns an empty string if the request fails. */
        static std::string do_curl(method m,
                                std::string uri,
                                std::map<std::string, std::string> headers,
                                std::string body);
    };
}

#endif
