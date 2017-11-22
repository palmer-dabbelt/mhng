sqlite3 $MHNG_MAILDIR/metadata.sqlite3 .dump

$PTEST_BINARY $ARGS

diff -u out.test out.gold
