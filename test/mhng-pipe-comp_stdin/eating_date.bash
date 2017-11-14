#include "harness_start.bash"

ARGS="drafts 1"
export MHNG_COMP_DATE="Mon Nov 13 14:40:14 PST 2017"
export MHNG_COMP_MESSAGE_ID="<mhng-TEST@TEST>"

cat >in.test <<"EOF"
To: REDATCED2@example.com
From: REDACTOR3@example.com
Subject: Shipping Label for your Trade-In (TRN-REDACTED4)

Test Line 1
Test Line 2
EOF

cat >out.gold <<"EOF"
Subject: Shipping Label for your Trade-In (TRN-REDACTED4)
From: REDACTOR3@example.com
To: REDATCED2@example.com
Date: Mon Nov 13 14:40:14 PST 2017
Message-ID: <mhng-TEST@TEST>

Test Line 1
Test Line 2
EOF

#include "harness_end.bash"
