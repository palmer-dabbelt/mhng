#include "harness_start.bash"

ARGS="drafts 1"

export MHNG_COMP_DATE="Mon Nov 13 14:40:14 PST 2017"
export MHNG_COMP_MESSAGE_ID="<mhng-TEST@TEST>"
export TZ="America/Los_Angeles"

cat >"$MHNG_EDITOR" <<"EOF"
#!/bin/bash
cat >$1 <<FOE
CC: to@example.com
From: from@example.com
To: cc1@example.com
To: cc2@example.com
To: cc3@example.com
To: cc4@example.com
To: cc5@example.com
To: cc6@example.com
To: cc7@example.com
To: cc8@example.com
Subject: A Test Message

Some test content goes here.
FOE
EOF

cat > out.gold <<EOF
Subject: A Test Message
CC: to@example.com
From: from@example.com
To: cc1@example.com, cc2@example.com, cc3@example.com, cc4@example.com,
  cc5@example.com, cc6@example.com, cc7@example.com, cc8@example.com
Date: Mon Nov 13 14:40:14 PST 2017
Message-ID: <mhng-TEST@TEST>
Mime-Version: 1.0 (MHng)
Content-Type: text/plain; charset=utf-8; format=flowed
Content-Transfer-Encoding: 8bit

Some test content goes here.
EOF

#include "harness_end.bash"

if [[ "$(grep ^To out.test)" != "To: cc1@example.com, cc2@example.com, cc3@example.com, cc4@example.com," ]]
then
  cat out.test
  exit 1
fi

if [[ "$(grep "^  cc5" out.test)" != "  cc5@example.com, cc6@example.com, cc7@example.com, cc8@example.com" ]]
then
  cat out.test
  exit 1
fi
