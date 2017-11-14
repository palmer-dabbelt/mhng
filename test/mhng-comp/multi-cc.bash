#include "harness_start.bash"

ARGS="drafts 1"

export TZ="America/Los_Angeles"
export MHNG_COMP_MESSAGE_ID="<mhng-TEST@TEST>"
export MHNG_COMP_DATE="Mon Nov 13 14:40:14 PST 2017"

cat >"$MHNG_EDITOR" <<"EOF"
#!/bin/bash
cat >$1 <<FOE
To: to@example.com
From: from@example.com
CC: cc1@example.com
CC: cc2@example.com
CC: cc3@example.com
CC: cc4@example.com
CC: cc5@example.com
CC: cc6@example.com
CC: cc7@example.com
CC: cc8@example.com
Subject: A Test Message

Some test content goes here.
FOE
EOF

cat > out.gold <<EOF
Subject: A Test Message
CC: cc1@example.com, cc2@example.com, cc3@example.com, cc4@example.com,
  cc5@example.com, cc6@example.com, cc7@example.com, cc8@example.com
From: from@example.com
To: to@example.com
Date: Mon Nov 13 14:40:14 PST 2017
Message-ID: <mhng-TEST@TEST>
Mime-Version: 1.0 (MHng)
Content-Type: text/plain; charset=utf-8; format=flowed
Content-Transfer-Encoding: 8bit

Some test content goes here.
EOF

#include "harness_end.bash"

if [[ "$(grep ^CC out.test)" != "CC: cc1@example.com, cc2@example.com, cc3@example.com, cc4@example.com," ]]
then
  cat out.test
  exit 1
fi

if [[ "$(grep "^  cc5" out.test)" != "  cc5@example.com, cc6@example.com, cc7@example.com, cc8@example.com" ]]
then
  cat out.test
  exit 1
fi
