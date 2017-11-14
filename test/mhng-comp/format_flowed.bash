#include "harness_start.bash"

COMP_ARGS=""
ARGS="drafts 1"
export MHNG_COMP_DATE="Mon Nov 13 14:40:14 PST 2017"
export MHNG_COMP_MESSAGE_ID="<mhng-TEST@TEST>"

cat >"$MHNG_EDITOR" <<"EOF"
#!/bin/bash

cat in.test > $1
EOF

cat >in.test <<"EOF"
To: REDATCED2@example.com
From: REDACTOR3@example.com
Subject: Shipping Label for your Trade-In (TRN-REDACTED4)

Does this look any better on the Google systems?  I think the general idea here 
is that I should mark all my stuff as 'format=flowed'...
EOF

cat >out.gold <<"EOF"
Subject: Shipping Label for your Trade-In (TRN-REDACTED4)
From: REDACTOR3@example.com
To: REDATCED2@example.com
Date: Mon Nov 13 14:40:14 PST 2017
Message-ID: <mhng-TEST@TEST>
Mime-Version: 1.0 (MHng)
Content-Type: text/plain; charset=utf-8; format=flowed
Content-Transfer-Encoding: 8bit

Does this look any better on the Google systems?  I think the general idea here 
is that I should mark all my stuff as 'format=flowed'...
EOF

#include "harness_end.bash"
