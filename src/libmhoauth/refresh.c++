/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identified: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhoauth/refresh_request.h++>
#include <libmhoauth/authorization_code.h++>
#include <libmhoauth/authorization_request.h++>
#include <libmhoauth/http_server.h++>
#include <libmhoauth/http_request.h++>
#include <libmhoauth/process.h++>
#include <libmhoauth/refresh.h++>
#include <pson/parser.h++>
#include <pson/emitter.h++>
#include <sstream>
#include <regex>
using namespace libmhoauth;

access_token libmhoauth::refresh(std::string client_id,
                                std::string token_uri,
                                std::string token)
{
    /* At this point we have the authorization code, which must be exchanged
     * for an access token by going back to the server.  This time we don't
     * actually need to do the whole browser+webserver song and dance because
     * the user has already given us access, we just need to prove that the
     * wasn't some MITM intercepting that redirection to localhost. */
    auto acc_req = refresh_request(client_id, token);
    http_request http(http_request::method::POST,
                      token_uri,
                      acc_req.headers(),
                      acc_req.body());

    auto json = std::dynamic_pointer_cast<pson::tree_object>(
        pson::parse_json_string(http.response().body())
    );

    if (json->get<int>("expires_in").valid() == false) {
        std::cerr << "Malformed OAUTH refresh response:\n";
        std::cerr << std::to_string(json) << "\n";
        abort();
    }

    return access_token(json->get<std::string>("access_token").data(),
                        token,
                        json->get<int>("expires_in").data());
}
