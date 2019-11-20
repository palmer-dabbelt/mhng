/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include <libmhoauth/http_request.h++>
#include <curl/curl.h>
#include <functional>
#include <sstream>
#include <vector>
using namespace libmhoauth;

static size_t curl_write_wrapper(void *ptr, size_t size, size_t nmemb, void *priv)
{
    auto out = (std::stringstream *)priv;
    auto buf = (char *)ptr;
    for (size_t i = 0; i < size * nmemb; ++i)
        out->put(buf[i]);
    return size * nmemb;
}

std::string http_request::do_curl(method m,
                                  std::string uri,
                                  std::map<std::string, std::string> headers,
                                  std::string body)
{
    auto curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());

    switch (m) {
    case method::POST:
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    
        struct curl_slist *header_slist = NULL;
        std::vector<std::string> merged_headers;
        for (const auto& header: headers)
            merged_headers.push_back(header.first + ": " + header.second);
        for (const auto& header: merged_headers)
            header_slist = curl_slist_append(header_slist, header.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_slist);
    
        std::stringstream out;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_wrapper);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    
        auto res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            return "";
    
        curl_slist_free_all(header_slist);
        curl_easy_cleanup(curl);
    
        return out.str();
    }

    abort();
}
