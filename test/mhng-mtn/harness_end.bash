sqlite3 $MHNG_MAILDIR/metadata.sqlite3 .dump

$PTEST_BINARY --stdout --no-mailrc $ARGS > out.test

diff -u out.gold out.test
