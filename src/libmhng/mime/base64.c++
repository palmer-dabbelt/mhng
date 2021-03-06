/* Copyright (C) 2015 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "base64.h++"
#include <base64.h>

#ifndef BASE64_LINE_SIZE
#define BASE64_LINE_SIZE 10240
#endif

std::vector<uint8_t> base64_decode(const std::string& in)
{
    if (in[in.size() - 1] != '\n') {
        fprintf(stderr, "base64_decode('%s'): trailing newline expected\n", in.c_str());
        abort();
    }

    auto out_len = base64_declen(in.size()) + 5;
    auto out = std::vector<uint8_t>(out_len);
    auto out_unused = 5 + (
        in[in.size() - 3] == '=' ? 2
        : in[in.size() - 2] == '=' ? 1
        : 0);
    
    int err;
    if ((err = base64_decode(in.c_str(), &out[0], out_len)) < 0) {
        fprintf(stderr, "base64_decode('%s', out, %lu) => %d\n", in.c_str(), out_len, err);
        fprintf(stderr, "    in.size(): %lu\n", in.size());
        fprintf(stderr, "    in[in.size() - 4]: %c\n", in[in.size() - 4]);
        fprintf(stderr, "    in[in.size() - 3]: %c\n", in[in.size() - 3]);
        fprintf(stderr, "    in[in.size() - 2]: %c\n", in[in.size() - 2]);
        fprintf(stderr, "    in[in.size() - 1]: %c\n", in[in.size() - 1]);
        abort();
    }

    return std::vector<uint8_t>(out.begin(), out.begin() + out_len - out_unused);
}

std::string base64_encode(const uint8_t *in, size_t in_count)
{
    auto out_len = base64_enclen(in_count) + 1;
    auto out_vec = std::vector<char>(out_len);

    if (base64_encode(in, in_count, out_vec.data(), out_len) < 0)
        abort();

    return std::string(&out_vec[0], out_vec.size());
}

std::string base64_encode(std::string in)
{
    return base64_encode((const uint8_t *)(in.c_str()), in.length());
}

std::string base64_array2string(const std::vector<uint8_t>& in)
{
    auto tmp = std::vector<char>(in.size() + 1);
    for (size_t i = 0; i < in.size(); ++i)
        tmp[i] = in[i];
    tmp[in.size()] = '\0';

    return std::string(&tmp[0]);
}

std::vector<std::string> base64_encode_file(const std::string& filename)
{
    std::vector<std::string> out;
    unsigned char buffer[BASE64_LINE_SIZE];

    auto file = fopen(filename.c_str(), "r");
    if (file == NULL)
        abort();

    ssize_t n;
    while ((n = fread(&buffer[0], 1, BASE64_LINE_SIZE, file)) > 0)
        out.push_back(base64_encode(buffer, n));

    return out;
}
