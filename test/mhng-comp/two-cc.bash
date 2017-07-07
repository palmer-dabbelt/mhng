#include "harness_start.bash"

ARGS="drafts 1"

export TZ="America/Los_Angeles"

cat >"$MHNG_EDITOR" <<"EOF"
#!/bin/bash
cat >$1 <<FOE
To: to@example.com
From: from@example.com
CC: cc1@example.com
CC: cc2@example.com
Subject: A Test Message

Some test content goes here.
FOE
EOF

#include "harness_end.bash"

if [[ "$(grep ^CC out.test)" != "CC: cc1@example.com, cc2@example.com" ]]
then
  cat out.test
  exit 1
fi
