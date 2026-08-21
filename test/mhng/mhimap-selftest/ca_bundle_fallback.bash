#include "harness_start.bash"

# CAFILE on macOS is a Homebrew path, and it's the only trust store the
# IMAP client used to look at.  While ca-certificates was mid-upgrade
# nothing could connect at all, every five seconds, with nothing in the
# log naming the file:
#
#   GNUTLS exception thrown during session.set_credentials
#   GNUTLS exception thrown
#     Error while reading file.
#
# Now it walks a list and takes the first bundle that has CAs in it.
openssl req -x509 -newkey rsa:2048 -nodes -days 1 \
        -subj "/CN=mhng selftest CA" -keyout key.pem -out ca.pem 2>/dev/null

# A bundle that's readable but has nothing in it can't verify anything
# either, so it has to count as a miss rather than as a trust store.
touch empty.pem

{
    echo "no bundle exists:"
    $PTEST_BINARY ca-bundle missing-one.pem missing-two.pem

    echo "past a missing bundle:"
    $PTEST_BINARY ca-bundle missing-one.pem ca.pem

    echo "past an empty bundle:"
    $PTEST_BINARY ca-bundle empty.pem ca.pem

    echo "first bundle that has CAs wins:"
    $PTEST_BINARY ca-bundle ca.pem missing-two.pem
} > out.test

cat >out.gold <<"EOF"
no bundle exists:
loaded 0 CAs
past a missing bundle:
loaded 1 CAs
past an empty bundle:
loaded 1 CAs
first bundle that has CAs wins:
loaded 1 CAs
EOF

#include "harness_end.bash"
