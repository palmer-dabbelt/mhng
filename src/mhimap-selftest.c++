/* Copyright (C) 2026 Palmer Dabbelt <palmer@dabbelt.com> */
/* SPDX-License-Identifier: GPL-2.0+ OR Apache-2.0 OR BSD-3-Clause */

/* Drives the parts of libmhimap that only run when something has gone
 * wrong: a CA bundle that isn't there, a TLS session the server has
 * hung up on, a line longer than the buffer it's being read into.
 * They're all reachable from a real IMAP session, but only on a bad
 * day, and not on one anybody can arrange on purpose -- so this pokes
 * at them directly.  Every mode prints a line per check, and the
 * harness diffs the lot against a gold file. */

#include <libmhimap/client.h++>
#include <libmhimap/line_buffer.h++>
#include <libmhimap/logger.h++>
#include <libmhimap/ssl_client.h++>
#include <gnutls/gnutls.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/* Fills 'buf' with a line of 'length' 'x's followed by CRLF. */
static void fill_line(mhimap::line_buffer &buf, ssize_t length)
{
    std::string line(length, 'x');
    line += "\r\n";

    ssize_t space = buf.recharge_size();
    if (space < (ssize_t)line.size()) {
        fprintf(stderr, "line_buffer only offered " SSIZET_FORMAT " bytes\n",
                space);
        exit(1);
    }

    memcpy(buf.recharge_buffer(), line.data(), line.size());
    buf.recharge_commit(line.size());
}

/* line_buffer::get() terminates what it copies, so a line that fills
 * the caller's buffer exactly used to put that '\0' one byte past the
 * end of it -- and the callers in client.c++ all hand it a
 * BUFFER_SIZE-byte stack array, which IMAP lines do reach.  Hand it a
 * buffer with a known byte sitting immediately after the part it's
 * allowed to touch and report whether that byte survives. */
static int test_line_buffer(void)
{
    static const ssize_t buffer_size = 64;
    static const char canary = 0x5a;
    static const ssize_t lengths[] = { 1, 63, 64, 65, 200 };

    for (const auto &length: lengths) {
        mhimap::line_buffer buf;
        fill_line(buf, length);

        char out[buffer_size + 1];
        memset(out, canary, sizeof(out));

        ssize_t ret = buf.get(out, buffer_size);

        printf("line of " SSIZET_FORMAT ": returned " SSIZET_FORMAT
               ", copied " SIZET_FORMAT ", canary %s\n",
               length, ret, strnlen(out, buffer_size),
               (out[buffer_size] == canary) ? "intact" : "CLOBBERED");
    }

    return 0;
}

/* An IMAP connection that has already died: every read reports the
 * failure that ssl_client::read() now returns when GNUTLS hands back
 * something fatal. */
namespace {
    class dead_client: public mhimap::client {
    public:
        dead_client(void)
        : mhimap::client(mhimap::account("selftest@example.com"))
        {}

        bool is_connected(void) const { return true; }

    protected:
        ssize_t read(char *buffer __attribute__((unused)),
                     ssize_t buffer_size __attribute__((unused)))
        {
            return -1;
        }

        ssize_t write(char *buffer __attribute__((unused)),
                      ssize_t buffer_size)
        {
            return buffer_size;
        }
    };
}

/* A connection dropping is a mail server being a mail server, so it
 * has to come out as a non-zero exit that the daemon can retry -- not
 * as the SIGABRT that a core file and a CrashReporter window get made
 * out of.  Run it in a child so this can report which one happened. */
static int test_read_failure(void)
{
    fflush(NULL);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork() failed");
        return 1;
    }

    if (pid == 0) {
        dead_client client;
        client.wait_idle();

        /* wait_idle() reads before it does anything else, so getting
         * here at all means the read failure went unnoticed. */
        fprintf(stderr, "wait_idle() returned on a dead connection\n");
        exit(0);
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid() failed");
        return 1;
    }

    if (WIFSIGNALED(status))
        printf("read failure: killed by signal %d\n", WTERMSIG(status));
    else
        printf("read failure: exited %d\n", WEXITSTATUS(status));

    return 0;
}

/* GNUTLS only has two errors that a retry can do anything about.  A
 * retry on any of the rest spins until the retry count runs out,
 * because they've all invalidated the session on the way past. */
static int test_retryable(void)
{
    static const struct {
        const char *name;
        int code;
    } codes[] = {
        { "GNUTLS_E_AGAIN",                  GNUTLS_E_AGAIN },
        { "GNUTLS_E_INTERRUPTED",            GNUTLS_E_INTERRUPTED },
        { "GNUTLS_E_INVALID_SESSION",        GNUTLS_E_INVALID_SESSION },
        { "GNUTLS_E_PREMATURE_TERMINATION",  GNUTLS_E_PREMATURE_TERMINATION },
        { "GNUTLS_E_PULL_ERROR",             GNUTLS_E_PULL_ERROR },
        { "GNUTLS_E_PUSH_ERROR",             GNUTLS_E_PUSH_ERROR },
        { "GNUTLS_E_DECRYPTION_FAILED",      GNUTLS_E_DECRYPTION_FAILED },
        { "GNUTLS_E_SUCCESS",                GNUTLS_E_SUCCESS },
    };

    for (const auto &c: codes)
        printf("%s: %s\n", c.name,
               mhimap::tls_error_is_retryable(c.code) ? "retry" : "give up");

    return 0;
}

/* The CA bundle search: loads from the first path that has any CAs in
 * it, so that a bundle going missing -- which is what a Homebrew
 * upgrade does to CAFILE for a moment -- doesn't take every IMAP
 * connection down with it. */
static int test_ca_bundle(int argc, const char **argv)
{
    mhimap::trust_credentials credentials;
    int certs = credentials.load_trust_files(argv, argc);

    printf("loaded %d CAs\n", certs);
    return 0;
}

int main(int argc, const char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <line-buffer|read-failure|retryable"
                        "|ca-bundle <path>...>\n", argv[0]);
        return 1;
    }

    if (getenv("MHIMAP_SELFTEST_VERBOSE") != NULL)
        mhimap::logger::set_debug(true);

    if (strcmp(argv[1], "line-buffer") == 0)
        return test_line_buffer();
    if (strcmp(argv[1], "read-failure") == 0)
        return test_read_failure();
    if (strcmp(argv[1], "retryable") == 0)
        return test_retryable();
    if (strcmp(argv[1], "ca-bundle") == 0)
        return test_ca_bundle(argc - 2, argv + 2);

    fprintf(stderr, "Unknown selftest '%s'\n", argv[1]);
    return 1;
}
