#include "harness_start.bash"

ARGS="drafts 1"

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

#include "harness_end.bash"

if [[ "$(grep ^To out.test)" != "To: cc1@example.com, cc2@example.com, cc3@example.com, cc4@example.com" ]]
then
  cat out.test
  exit 1
fi

if [[ "$(grep "^  cc5" out.test)" != "  cc5@example.com, cc6@example.com, cc7@example.com, cc8@example.com" ]]
then
  cat out.test
  exit 1
fi
