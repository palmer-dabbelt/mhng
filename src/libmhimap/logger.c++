
/*
 * Copyright (C) 2013 Palmer Dabbelt
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
