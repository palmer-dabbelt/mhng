#include "harness_start.bash"

ARGS="drafts 1"

export MHNG_COMP_DATE="Mon Nov 13 14:40:14 PST 2017"
export MHNG_COMP_MESSAGE_ID="<mhng-TEST@TEST>"
export MHNG_MAILRC="$(pwd)/mailrc"
export TZ="America/Los_Angeles"

cat > $MHNG_MAILRC <<EOF
local Palmer Dabbelt <palmer@dabbelt.com>
local Palmer Dabbelt <palmer@sifive.com>
alias palmer    Palmer Dabbelt <palmer@dabbelt.com>
alias si        Palmer Dabbelt <palmer@sifive.com>
alias olof      Olof Johansson <olof@lixom.net>
bcc   palmer@dabbelt.com
EOF

cat >"$MHNG_EDITOR" <<"EOF"
#!/bin/bash

cat in.test > $1
EOF

cat >in.test <<"EOF"
From:    si
To:      olof
Subject: VC707 Beta Program

IIRC you said you had a VC707 you were interested in playing around with?  Do
you want to be on the beta program mailing list?
EOF

cat >out.gold <<"EOF"
Subject: VC707 Beta Program
From: Palmer Dabbelt <palmer@sifive.com>
To: Olof Johansson <olof@lixom.net>
Date: Mon Nov 13 14:40:14 PST 2017
Message-ID: <mhng-TEST@TEST>
BCC: palmer@dabbelt.com
Mime-Version: 1.0 (MHng)
Content-Type: text/plain; charset=utf-8; format=flowed
Content-Transfer-Encoding: 8bit

IIRC you said you had a VC707 you were interested in playing around with?  Do
you want to be on the beta program mailing list?
EOF

#include "harness_end.bash"
