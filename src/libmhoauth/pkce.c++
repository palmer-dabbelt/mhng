/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identified: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhoauth/access_request.h++>
#include <libmhoauth/authorization_code.h++>
#include <libmhoauth/authorization_request.h++>
#include <libmhoauth/http_server.h++>
#include <libmhoauth/http_request.h++>
#include <libmhoauth/process.h++>
#include <libmhoauth/pkce.h++>
#include <pson/parser.h++>
#include <sstream>
#include <regex>
using namespace libmhoauth;

access_token libmhoauth::pkce(std::string client_id,
                             std::string auth_uri,
                             std::string token_uri,
                             std::string browser,
                             std::string scope,
                             std::string hint)
{
    /* The first phase of the PKCE exchange involves firing up a web browser
     * local to the client and pointing it at an OAUTH2 endpoint server.  The
     * user deals with a web interface to accept the credential request, the
     * final step of which has the server redirect the user's browser back to
     * localhost with a GET request that contains an authorization code. */
    auto auth_code = [&]() {
        /* First, we must start up a web server running locally so the forwarded
         * credentials can be recieved on this end.  This server will listen on a
         * random port, but we give it the FQDN. */
        http_server http;

        /* Now that we know the port that webserver is listening on, we can
         * construct the local URI that the request will be forwarded to and launch
         * the user's browser to perform the first stage of the request. */
        auto local_uri = "http://127.0.0.1:" + std::to_string(http.port());
        auto auth_req = authorization_request(auth_uri, client_id, scope, local_uri, hint);
        auto browser_process = launch_process(browser, auth_req.urlencode());

        /* A request came in.  There's a bunch of tags, which will be eaten by
         * the raw() call, but all we car about is the first line as that
         * contains the entire code that we're interested in. */
        auto forward = http.get_request();
        auto first_line = [&](){
            auto buf = std::stringstream();
            for (size_t i = 0; i < forward.raw().size(); ++i) {
                buf << forward.raw()[i];
                if (forward.raw()[i] == '\r' && forward.raw()[i+1] == '\n')
                    return buf.str();
            }
            std::cerr << "No first line in HTTP request\n";
            abort();
        }();

        /* This uses the standard HTTP encoding scheme for URL parameters.  I'm
         * not going to bother to parse it, but am instead just going to use a
         * regex to get out the "code" field. */
        auto code_string = [&](){
            auto r = std::regex("code=([A-Za-z0-9_/-]*)[& ]");
            auto m = std::smatch();
            if (std::regex_search(first_line, m, r))
                return m[1];
            std::cerr << "No code field in response\n";
            abort();
        }();

        /* Reply with a canned HTTP response, at least for now. */
        forward << "HTTP/1.1 200 OK\n";
        forward << "Content-Type: text/html; charset=utf-8\n";
        forward << "\n";
        forward << "<!DOCTYPE html>\n";
        forward << "<html><body>\n";
        forward << "Success!<br/>\n";
        forward << "You may close this window now.<br/>";
        forward << "</body></html>\n";
        forward << "\n";

        return authorization_code(code_string, auth_req.verifier(), local_uri);
    }();

    /* At this point we have the authorization code, which must be exchanged
     * for an access token by going back to the server.  This time we don't
     * actually need to do the whole browser+webserver song and dance because
     * the user has already given us access, we just need to prove that the
     * wasn't some MITM intercepting that redirection to localhost. */
    auto acc_req = access_request(client_id, auth_code);
    http_request http(http_request::method::POST,
                      token_uri,
                      acc_req.headers(),
                      acc_req.body());

    auto json = std::dynamic_pointer_cast<pson::tree_object>(
        pson::parse_json_string(http.response().body())
    );

    auto _access_token = json->get<std::string>("access_token");
    auto refresh_token = json->get<std::string>("refresh_token");
    auto expires_in = json->get<int>("expires_in");

    if (!(_access_token.valid() && refresh_token.valid() && expires_in.valid())) {
        for (const auto& byte: http.response().body()) {
            char str[] = {byte, '\0'};
            std::cerr << str;
        }
        abort();
    }

    return access_token(_access_token.data(), refresh_token.data(), expires_in.data());
}
