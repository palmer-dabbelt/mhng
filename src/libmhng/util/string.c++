/*
 * Copyright (C) 2019 Palmer Dabbelt
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
