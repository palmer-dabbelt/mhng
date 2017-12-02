#include "harness_start.bash"

ARGS=""

export TZ="America/Los_Angeles"
export MHNG_HFIPIP_DATE="WHENEVER"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (2, 1, "inbox", 0, "test", 1512183181, "from@example.com", "to@example.com");
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (3, 2, "inbox", 0, "test", 1512183181, "from@example.com", "to@example.com");
BEGIN TRANSACTION;
UPDATE MH__current SET cur=NULL WHERE folder="inbox";
UPDATE MH__current SET cur=1 WHERE folder="drafts";
COMMIT TRANSACTION;
EOF

cat > out.gold <<EOF
<HTML> <HEAD> <TITLE> HFIPI? </TITLE> </HEAD> <BODY>
This page computes the DJB hash of the subjects of all the messages in my inbox every minute (when my computer is on), so you can see if I've lost your message or not.  The message I most recently viewed is marked with a "*", and unread messages are marked with a "U".<br/>
<p/>
<tt>*&nbspU&nbsp12/01&nbsp0xf84b2772&nbsp0x7c9e6865</tt><br/>
<tt>&nbsp&nbspU&nbsp12/01&nbsp0xf84b2772&nbsp0x7c9e6865</tt><br/>
<p/>
This message was computed on WHENEVER
</BODY> </HTML>
EOF

#include "harness_end.bash"
