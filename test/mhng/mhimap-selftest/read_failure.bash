#include "harness_start.bash"

# A mail server hanging up on us is a mail server being a mail server,
# and it happens constantly: the sync process would be ten messages
# into a fetch when GNUTLS started reporting that the session had been
# invalidated.  client::gets() called abort() over it, so every dropped
# connection cost a core file and a CrashReporter window on the way to
# the retry that was going to happen anyway.
#
# What the daemon needs is a non-zero exit it can retry.  Signal 6 here
# is the regression.
$PTEST_BINARY read-failure > out.test 2> err.test

cat >out.gold <<"EOF"
read failure: exited 1
EOF

cat >err.gold <<"EOF"
Read failure, giving up on this connection
EOF

#include "harness_end.bash"
