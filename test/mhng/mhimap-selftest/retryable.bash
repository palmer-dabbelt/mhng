#include "harness_start.bash"

# read() used to retry every GNUTLS error ten times.  Only two of them
# can be retried: the rest invalidate the session on their way past, so
# every call after the first returns GNUTLS_E_INVALID_SESSION without
# going near the network.  That turned one real error into ten copies
# of a useless one, spent as fast as the CPU could get through them:
#
#   GNUTLS exception thrown during read(), retrying
#     The TLS connection was non-properly terminated.
#   GNUTLS exception thrown during read(), retrying
#     The specified session has been invalidated for some reason.
#   [... eight more ...]
#   Too many GNUTLS exceptions thrown
#
# GNUTLS_E_AGAIN really does show up on a healthy connection -- it
# turns up on the first read of most of them -- so it has to stay
# retryable, which is what makes this worth pinning down in both
# directions.
$PTEST_BINARY retryable > out.test

cat >out.gold <<"EOF"
GNUTLS_E_AGAIN: retry
GNUTLS_E_INTERRUPTED: retry
GNUTLS_E_INVALID_SESSION: give up
GNUTLS_E_PREMATURE_TERMINATION: give up
GNUTLS_E_PULL_ERROR: give up
GNUTLS_E_PUSH_ERROR: give up
GNUTLS_E_DECRYPTION_FAILED: give up
GNUTLS_E_SUCCESS: give up
EOF

#include "harness_end.bash"
