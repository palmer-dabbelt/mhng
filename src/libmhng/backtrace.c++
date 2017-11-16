/*
 * Copyright (C) 2017 Palmer Dabbelt
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

#include "backtrace.h++"
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
using namespace mhng;

#define MAX_STACK_DEPTH 128

static void handler(int signal)
{
    void *array[MAX_STACK_DEPTH];

    fprintf(stderr, "MHng caught signal %d:\n", signal);
    backtrace_symbols_fd(array, backtrace(array, MAX_STACK_DEPTH), STDERR_FILENO);
    exit(1);
}

void mhng::register_backtrace_printer(void)
{
    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);
}

static void init(void) __attribute__((constructor));
void init(void)
{
    mhng::register_backtrace_printer();
}
