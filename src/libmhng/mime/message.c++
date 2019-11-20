/* Copyright (C) 2014 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "message.h++"
#include <string.h>
using namespace mhng;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

static std::string to_lower(const std::string uppers);

mime::message::message(const std::vector<std::string>& raw)
    : _root(std::make_shared<part>(raw))
{
    for (const auto& header: _root->headers())
        add_header(header);
}

std::vector<mime::header_ptr>
mime::message::header(const std::string name) const
{
    std::vector<mime::header_ptr> out;

    auto lower = to_lower(name);
    auto begin = _headers.lower_bound(lower);
    auto end = _headers.upper_bound(lower);
    for (auto it = begin; it != end; ++it)
        out.push_back(it->second);

    return out;
}

mime::part_ptr mime::message::body(void) const
{
    auto root_body = _root->body();
    if (root_body == NULL)
        return _root;
    return root_body;
}

mime::part_ptr mime::message::signature(void) const
{
    return _root->signature();
}

void mime::message::add_header(const header_ptr& header)
{
    auto key = header->key_downcase();
    _headers.insert(std::make_pair(key, header));
}

void mime::message::add_header(const std::string& key,
                               const std::string& value)
{
    auto header = std::make_shared<mime::header>(key + ": " + value);
    add_header(header);
}

std::string to_lower(const std::string uppers)
{
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%s", uppers.c_str());
    for (size_t i = 0; i < strlen(buffer); ++i)
        buffer[i] = tolower(buffer[i]);
    return buffer;
}
