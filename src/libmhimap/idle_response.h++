/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#ifndef LIBMHIMAP__IDLE_RESPONSE_HXX
#define LIBMHIMAP__IDLE_RESPONSE_HXX

namespace mhimap {
    /* Holds the different sort of responses that can come back from
     * an IDLE command. */
    enum class idle_response {
        /* FIXME: emacs doesn't know how to indent C++ enum
         * classes... :( */
        TIMEOUT,
        DISCONNECT,
        DATA,    
    };
}

#endif
