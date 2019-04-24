#include "harness_start.bash"

ARGS="inbox 1"

export TZ="America/Los_Angeles"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (11, 1, "inbox", 0, "test", 1, "from@example.com", "to@example.com");
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (12, 2, "inbox", 0, "test", 1, "from@example.com", "to@example.com");
EOF

cat >$MHNG_MAILDIR/mail/inbox/11 <<"EOF"
Message-ID: <REDACTED0@example.com>
Date: Tue, 2 Dec 2014 23:15:26 -0800
From: REDACTED1@example.com
To: REDACTED2@example.com
Subject: Soft Machines "VISC"?
In-Reply-To: <REDACTED3@example.com>
References: <REDACTED4@example.com>
X-Mailer: VM 8.2.0b under 24.3.1 (x86_64-apple-darwin12.4.0)
Mime-Version: 1.0
Content-Type: multipart/mixed;
	boundary="++----------20141202231523-1330795279022344039----------++"
Content-Transfer-Encoding: 8bit
Cc: REDACTED5@example.com
X-BeenThere: REDACTED5@example.com
X-Mailman-Version: 2.1.9
Precedence: list

This is a multi-part message in MIME format.
--++----------20141202231523-1330795279022344039----------++
Content-Type: text/plain; charset=us-ascii
Content-Transfer-Encoding: 8bit

UID 11

--++----------20141202231523-1330795279022344039----------++--
EOF

cat >$MHNG_MAILDIR/mail/inbox/12 <<"EOF"
Message-ID: <REDACTED0@example.com>
Date: Tue, 2 Dec 2014 23:15:26 -0800
From: REDACTED1@example.com
To: REDACTED2@example.com
Subject: Soft Machines "VISC"?
In-Reply-To: <REDACTED3@example.com>
References: <REDACTED4@example.com>
X-Mailer: VM 8.2.0b under 24.3.1 (x86_64-apple-darwin12.4.0)
Mime-Version: 1.0
Content-Type: multipart/mixed;
	boundary="++----------20141202231523-1330795279022344039----------++"
Content-Transfer-Encoding: 8bit
Cc: REDACTED5@example.com
X-BeenThere: REDACTED5@example.com
X-Mailman-Version: 2.1.9
Precedence: list

This is a multi-part message in MIME format.
--++----------20141202231523-1330795279022344039----------++
Content-Type: text/plain; charset=us-ascii
Content-Transfer-Encoding: 8bit

UID 12

--++----------20141202231523-1330795279022344039----------++--
EOF

cat >out.gold <<"EOF"
From:       REDACTED1@example.com
To:         REDACTED2@example.com
CC:         REDACTED5@example.com
Subject:    Soft Machines "VISC"?
Date:       Tue, 02 Dec 2014 23:15:26 PST (-0800)
Message-ID: <REDACTED0@example.com>

UID 12
EOF

#include "harness_end.bash"
