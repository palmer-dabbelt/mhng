/* Copyright (C) 2013 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

#include "logger.h++"
#include <stdarg.h>
#include <stdio.h>

using namespace mhimap;

static int global_depth = 0;

#ifdef IMAP_DEBUG
static bool global_debug = true;
#else
static bool global_debug = false;
#endif

logger::logger(const char *fmt, ...)
    : depth(global_depth++)
{
    if (!global_debug)
        return;
    
    for (int i = 0; i < this->depth; i++)
        fprintf(stderr, "  ");

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}

logger::~logger(void)
{
    global_depth--;
    if (!global_debug)
        return;
    
    for (int i = 0; i < this->depth; i++)
        fprintf(stderr, "  ");

    fprintf(stderr, "DONE\n");
}

void logger::printf(const char *fmt, ...)
{
    if (!global_debug)
        return;

    for (int i = 0; i <= this->depth; i++)
        fprintf(stderr, "  ");

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}
