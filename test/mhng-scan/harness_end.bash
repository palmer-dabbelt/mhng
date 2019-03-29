sqlite3 $MHNG_MAILDIR/metadata.sqlite3 .dump

$PTEST_BINARY $ARGS > out.test

diff -u out.gold out.test
