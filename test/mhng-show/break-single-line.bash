#include "harness_start.bash"

ARGS="inbox 1"

export TZ="America/Los_Angeles"
export TERM="xterm"
export COLUMNS="120"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (2, 1, "inbox", 0, "test", 1, "from@example.com", "to@example.com");
EOF

cat >$MHNG_MAILDIR/mail/inbox/2 <<"EOF"
From: test1@example.com
To: test2@example.com
Subject: An Example Subject
Date: Mon Nov 13 16:51:48 PST 2017
Message-ID: <mhng-test-case@example>

Here is a line that's longer than eighty characters and should therefor be broken by make_box.

For paragraphs where the input has already been broken, make_box shouldn't go
and mess with what's already OK.

>>>>>>>>>>>>>>> Additionally, in order to allow for some forgiveness when constructing replies,
>>>>>>>>>>>>>>> we don't counting the leading '>' characters that get pre-pended.
EOF

cat >out.gold <<"EOF"
From:       test1@example.com
To:         test2@example.com
Subject:    An Example Subject
Date:       Mon, 13 Nov 2017 16:51:48 PST (-0800)
Message-ID: <mhng-test-case@example>

Here is a line that's longer than eighty characters and should therefor be
broken by make_box.

For paragraphs where the input has already been broken, make_box shouldn't go
and mess with what's already OK.

>>>>>>>>>>>>>>> Additionally, in order to allow for some forgiveness when constructing replies,
>>>>>>>>>>>>>>> we don't counting the leading '>' characters that get pre-pended.
EOF

#include "harness_end.bash"
