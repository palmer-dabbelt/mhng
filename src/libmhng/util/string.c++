/* Copyright (C) 2019 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "string.h++"

std::string mhng::util::string::utf8_pad_to_cols(std::string base, size_t cols)
{
    if (utf8_cols(base) > cols)
        return utf8_crop_to_cols(base, cols);

    auto out = std::string(base);
    for (size_t i = 0; i < cols - utf8_cols(base); ++i)
        out = out + " ";
    return out;
}

std::string mhng::util::string::utf8_crop_to_cols(std::string base, size_t cols)
{
    if (utf8_cols(base) < cols)
        return base;

    auto prev = std::string("");
    auto next = prev + base[0];
    for (size_t i = 1; i < base.size(); ++i) {
        prev = next;
        next = next + base[i];
        if (utf8_cols(next) > cols)
            return prev;
    }

    abort();
}

size_t mhng::util::string::utf8_cols(std::string in)
{
    size_t cols = 0;
    for (unsigned char c : in)
        if ((c & 0xC0) != 0x80)
            cols++;
    return cols;
}
