/* Copyright 2019 Palmer Dabbelt <palmerdabbelt@google.com> */
/* SPDX-License-Identified: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "picosha2.h"
#include <libmhoauth/code_verifier.h++>
#include <libmhoauth/base64.h++>
#include <random>
using namespace libmhoauth;

static std::random_device random_device;
static std::mt19937 random_generator(random_device());
static std::uniform_int_distribution<std::mt19937::result_type> byte_range(0, 255);

static std::vector<uint8_t> generate_code_verifier(void)
{
    std::vector<uint8_t> out;

    for (size_t i = 0; i < 32; ++i)
        out.push_back(byte_range(random_generator));

    return out;
}

code_verifier::code_verifier(void)
: _plain(base64::encode(generate_code_verifier()))
{
}

std::string code_verifier::s256(void) const
{
    /* This is super small, but it's stuck in here because I don't want
     * PicoSHA2 to become part of the API. */
    std::vector<uint8_t> hash(picosha2::k_digest_size);
    picosha2::hash256(_plain.begin(), _plain.end(), hash.begin(), hash.end());
    return base64::encode(hash);
}
