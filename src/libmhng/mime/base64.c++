/*
 * Copyright (C) 2015 Palmer Dabbelt
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

#include "base64.h++"
#include <base64.h>

#ifndef BASE64_LINE_SIZE
#define BASE64_LINE_SIZE 10240
#endif

std::vector<uint8_t> base64_decode(const std::string& in)
{
    auto out_len = base64_declen(in.size()) + 1;
    auto out = std::vector<uint8_t>(0, out_len);
    
    if (base64_decode(in.c_str(), &out[0], out_len) != 0)
        abort();

    return out;
}

std::string base64_encode(const uint8_t *in, size_t in_count)
{
    auto out_len = base64_enclen(in_count) + 1;
    auto out_vec = std::vector<char>(0, out_len);

    if (base64_encode(in, in_count, &out_vec[0], out_len) != 0)
        abort();

    return std::string(&out_vec[0], out_vec.size());
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
