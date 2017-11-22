#include "harness_start.bash"

ARGS="2"

export TZ="America/Los_Angeles"
export MHNG_COMP_DATE="Mon Nov 13 14:40:14 PST 2017"
export MHNG_COMP_MESSAGE_ID="<mhng-TEST@TEST>"

cat >message <<EOF
To: to@example.com
From: from@example.com
CC: cc1@example.com
CC: cc2@example.com
Subject: A Test Message

Some test content goes here.
EOF
cat message | $(dirname "$PTEST_BINARY")/mhng-pipe-comp_stdin

cat >message <<EOF
To: to@example.com
From: from@example.com
CC: cc1@example.com
CC: cc2@example.com
Subject: Another Test Message

Content #2!
EOF
cat message | $(dirname "$PTEST_BINARY")/mhng-pipe-comp_stdin

cat > out.gold <<EOF
POST
Subject: Another Test Message
CC: cc1@example.com, cc2@example.com
From: from@example.com
To: to@example.com
Date: Mon Nov 13 14:40:14 PST 2017
Message-ID: <mhng-TEST@TEST>

Content #2!
EOF

#include "harness_end.bash"
