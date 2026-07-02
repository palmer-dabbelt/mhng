#include "harness_start.bash"

unset TERM
ARGS="inbox"

export TZ="America/Los_Angeles"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (2, 1, "inbox", 1, "test", 1512183181, "fr$(printf '\xff')m@example.com", "to@example.com");
EOF

printf '*  1 12/01 fr\xffm@example.com          test\n' > out.gold

#include "harness_end.bash"
