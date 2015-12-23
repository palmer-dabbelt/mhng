
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

#ifndef LIBMHNG__MIME__BASE64_HXX
#define LIBMHNG__MIME__BASE64_HXX

#include <string>
#include <vector>

/* This file provides safe C++ interfaces to the base64 encoding library. */

/* Decodes a base64-encoded string into a binary array. */
std::vector<uint8_t> base64_decode(const std::string& in);

/* Encodes a binary array as a base64-encoded string. */
std::string base64_encode(const std::vector<uint8_t>& in);
std::string base64_encode(const uint8_t *in, size_t in_count);

/* A utility function that should probably be killed -- this converts a base64
 * array to a string. */
std::string base64_array2string(const std::vector<uint8_t>& in);

/* Encodes a whole file as base64, returning each line as a string
 * without a newline. */
std::vector<std::string> base64_encode_file(const std::string& filename);

#endif
