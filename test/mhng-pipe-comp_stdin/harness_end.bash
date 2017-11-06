sqlite3 $MHNG_MAILDIR/metadata.sqlite3 .dump

$PTEST_BINARY < in.test

$(dirname "$PTEST_BINARY")/mhng-pipe-show_stdout $ARGS > out.test

cat out.gold
cat out.test
diff -u out.gold out.test
