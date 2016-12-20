sqlite3 $MHNG_MAILDIR/metadata.sqlite3 .dump

$PTEST_BINARY $ARGS > out

find

find -iname "*.gold" | while read name
do
    diff -a -u "$(echo "$name" | sed 's/[.]gold//g')" "$name"
done
