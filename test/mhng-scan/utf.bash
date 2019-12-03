#include "harness_start.bash"

unset TERM
ARGS="inbox"

export TZ="America/Los_Angeles"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (2, 1, "inbox", 0, "test", 1512183181, "fröm@example.com", "to@example.com");
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (3, 2, "inbox", 0, "test", 1512183181, "from@example.com", "to@example.com");
BEGIN TRANSACTION;
UPDATE MH__current SET cur=NULL WHERE folder="inbox";
UPDATE MH__current SET cur=1 WHERE folder="drafts";
COMMIT TRANSACTION;
EOF

cat > out.gold <<EOF
*  1 12/01 fröm@example.com          test
   2 12/01 from@example.com          test
EOF

#include "harness_end.bash"
