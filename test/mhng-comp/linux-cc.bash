#include "harness_start.bash"

ARGS="drafts 1"

export TZ="America/Los_Angeles"

cat >"$MHNG_EDITOR" <<"EOF"
#!/bin/bash
cat >$1 <<FOE
Subject: Re: [PATCH 2/9] RISC-V: Atomic and Locking Code
In-Reply-To: <20170706103357.inu4hi52iogrwo77@tardis>
CC: peterz@infradead.org
CC: mingo@redhat.com
CC: mcgrof@kernel.org
CC: viro@zeniv.linux.org.uk
CC: sfr@canb.auug.org.au
CC: nicolas.dichtel@6wind.com
CC: rmk+kernel@armlinux.org.uk
CC: msalter@redhat.com
CC: tklauser@distanz.ch
CC: will.deacon@arm.com
CC: james.hogan@imgtec.com
CC: paul.gortmaker@windriver.com
CC: linux@roeck-us.net
CC: linux-kernel@vger.kernel.org
CC: linux-arch@vger.kernel.org
CC: albert@sifive.com
CC: patches@groups.riscv.org
From: Palmer Dabbelt <palmer@dabbelt.com>
To: boqun.feng@gmail.com

On Thu, 06 Jul 2017 03:33:57 PDT (-0700), boqun.feng@gmail.com wrote:
> On Tue, Jul 04, 2017 at 12:50:55PM -0700, Palmer Dabbelt wrote:
> [...]
FOE
EOF

#include "harness_end.bash"

grep ^CC out.test

if [[ "$(grep ^CC out.test)" != "CC: peterz@infradead.org, mingo@redhat.com, mcgrof@kernel.org," ]]
then
  cat out.test
  exit 1
fi
