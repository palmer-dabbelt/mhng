chmod +x "$MHNG_EDITOR"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 .dump

$PTEST_BINARY $COMP_ARGS

$(dirname "$PTEST_BINARY")/mhng-pipe-show_stdout $ARGS > out.test

diff -u out.test out.gold
