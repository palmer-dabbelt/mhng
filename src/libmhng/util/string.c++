/* Copyright (C) 2019 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "string.h++"
#include <cstring>

std::string mhng::util::string::utf8_pad_to_length(std::string base, size_t length, char element)
{
    auto out = new char[length*4+1];

    memset(out, element, length*4+1);
    out[length*4] = '\0';
    strncpy(out, base.c_str(), strlen(base.c_str()));

    /* This is a pretty crude attempt at UTF8 decoding: it considers the number
     * of bytes in the input string, but assumes there is a single codepoint
     * for all the outputs.  It works well enough for the European-style two
     * byte accents, but that's going to be about it.
     *
     * I probably should have figured out if iconv can do this, but I'm stuck
     * in China and don't have real internet access so I didn't want to deal
     * with the headaches. */
    for (size_t i = 0; i < strlen(base.c_str()); ++i) {
        if ((base[i] & (0x80 + 0x40 + 0x20)) == (0x80 + 0x40)) {
            i += 1;
            length += 1;
        }
        else if ((base[i] & (0x80 + 0x40 + 0x20 + 0x10)) == (0x80 + 0x40 + 0x20)) {
            i += 2;
            length += 2;
        }
        else if ((base[i] & (0x80 + 0x40 + 0x20 + 0x10 + 0x08)) == (0x80 + 0x40 + 0x20 + 0x10)) {
            i += 3;
            length += 3;
        }
    }
        
    out[length] = '\0';

    auto ostr = std::string(out);
    delete out;
    return ostr;
}
