#include "harness_start.bash"

ARGS="3"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (2, 1, "inbox", 0, "test", 1, "from@example.com", "to@example.com");
EOF

cat >$MHNG_MAILDIR/mail/inbox/2 <<"EOF"
To: REDATCED2@example.com
Message-ID: <REDACTED@email.amazonses.com>
Subject: Shipping Label for your Trade-In (TRN-REDACTED4)
MIME-Version: 1.0
Content-Type: multipart/alternative; 
	boundary="----=_Part_REDATCED3"
Date: Sat, 10 Dec 2016 20:03:44 +0000

------=_Part_REDATCED3
Content-Type: text/plain; charset=utf-8
Content-Transfer-Encoding: 7bit

Look, content!
------=_Part_REDATCED3
Content-Type: text/html; charset=utf-8
Content-Transfer-Encoding: base64

aGVsbG8Kd29ybGQ=
------=_Part_REDATCED3--

EOF

touch out.gold

echo -en "hello\nworld" > mhng-attachment.gold

#include "harness_end.bash"
