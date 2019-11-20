/* Copyright (C) 2017 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

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
