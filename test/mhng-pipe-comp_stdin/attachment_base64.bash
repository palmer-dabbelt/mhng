#include "harness_start.bash"

ARGS="drafts 1"

cat >in.test <<"EOF"
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

aGVsbG8K
------=_Part_REDATCED3--

EOF

cat >out.gold <<"EOF"
Message-ID: <REDACTED@email.amazonses.com>
Subject: Shipping Label for your Trade-In (TRN-REDACTED4)
MIME-Version: 1.0
Content-Type: multipart/alternative;
	boundary="----=_Part_REDATCED3"
Date: Sat, 10 Dec 2016 20:03:44 +0000
To: REDATCED2@example.com

------=_Part_REDATCED3
Content-Type: text/plain; charset=utf-8
Content-Transfer-Encoding: 7bit

Look, content!
------=_Part_REDATCED3
Content-Type: text/html; charset=utf-8
Content-Transfer-Encoding: base64

aGVsbG8K
------=_Part_REDATCED3--

EOF


#include "harness_end.bash"
