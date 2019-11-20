/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identified: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBOAUTH2__BASE64_HXX
#define LIBOAUTH2__BASE64_HXX

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <base64.h>

namespace libmhoauth {
    namespace base64 {
        inline std::string encode(std::vector<uint8_t> in) {
            auto length = base64_enclen(in.size()) + 1;
            auto buf = new char[length];
            if (base64_encode(in.data(), in.size(), buf, length) < 0) {
                std::cerr << "Unable to base64() encode\n";
                abort();
            }
            for (size_t i = 0; i < length; ++i)
                buf[i] = [&]() {
                    switch (buf[i]) {
                    case '+': return '-';
                    case '/': return '_';
                    case '=': return '\0';
                    default:  return buf[i];
                    }
                }();
            auto out = std::string(buf);
            delete[] buf;
            return out;
        }
    }
}

#endif
