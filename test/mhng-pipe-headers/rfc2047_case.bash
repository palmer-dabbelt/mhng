#include "harness_start.bash"

ARGS="inbox 1"

export TZ="America/Los_Angeles"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (2, 1, "inbox", 0, "test", 1, "mb@example.com", "to@example.com");
EOF

# RFC 2047 section 2 says the encoding character is case insensitive, but
# git send-email emits a lower-case 'q' while most mailers emit an upper-case
# 'Q'.  The From: here is the upper-case form and decodes on its own; it's
# there to leave its latin-1 bytes behind in the decoder, so that a Cc: that
# takes an untested path through the decoder is caught reading them back as
# UTF-8 rather than quietly producing something that happens to look sane.
cat >$MHNG_MAILDIR/mail/inbox/2 <<"EOF"
Message-ID: <mhng-TEST@TEST>
Date: Thu, 20 Aug 2026 16:04:01 +0200
From: =?iso-8859-1?Q?Morten_Br=F8rup?= <mb@example.com>
To: to@example.com
Cc: =?UTF-8?q?Morten=20Br=C3=B8rup?= <mb@example.com>
X-Base64-Upper: =?UTF-8?B?TW9ydGVuIEJyw7hydXA=?= <mb@example.com>
X-Base64-Lower: =?UTF-8?b?TW9ydGVuIEJyw7hydXA=?= <mb@example.com>
X-Unknown-Encoding: =?UTF-8?x?Morten=20Br=C3=B8rup?= <mb@example.com>
Subject: test
Content-Type: text/plain; charset=UTF-8

body
EOF

cat >out.gold <<"EOF"
message-id: <mhng-TEST@TEST>
date: Thu, 20 Aug 2026 16:04:01 +0200
from: Morten Brørup<mb@example.com>
to: to@example.com
cc: Morten Brørup<mb@example.com>
x-base64-upper: Morten Brørup<mb@example.com>
x-base64-lower: Morten Brørup<mb@example.com>
x-unknown-encoding: =?UTF-8?x?Morten=20Br=C3=B8rup?= <mb@example.com>
subject: test
content-type: text/plain; charset=UTF-8
EOF

#include "harness_end.bash"
