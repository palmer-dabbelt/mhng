/* Copyright (C) 2015 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

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
