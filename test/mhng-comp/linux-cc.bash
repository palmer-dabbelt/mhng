#include "harness_start.bash"

ARGS="drafts 1"

export TZ="America/Los_Angeles"
export MHNG_COMP_DATE="Mon Nov 13 14:40:14 PST 2017"
export MHNG_COMP_MESSAGE_ID="<mhng-TEST@TEST>"

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

cat > out.gold <<EOF
Subject: Re: [PATCH 2/9] RISC-V: Atomic and Locking Code
In-Reply-To: <20170706103357.inu4hi52iogrwo77@tardis>
CC: peterz@infradead.org, mingo@redhat.com, mcgrof@kernel.org,
  viro@zeniv.linux.org.uk, sfr@canb.auug.org.au, nicolas.dichtel@6wind.com, rmk+kernel@armlinux.org.uk,
  msalter@redhat.com, tklauser@distanz.ch, will.deacon@arm.com, james.hogan@imgtec.com,
  paul.gortmaker@windriver.com, linux@roeck-us.net, linux-kernel@vger.kernel.org, linux-arch@vger.kernel.org,
  albert@sifive.com, patches@groups.riscv.org
From: Palmer Dabbelt <palmer@dabbelt.com>
To: boqun.feng@gmail.com
Date: Mon Nov 13 14:40:14 PST 2017
Message-ID: <mhng-TEST@TEST>
Mime-Version: 1.0 (MHng)
Content-Type: text/plain; charset=utf-8; format=flowed
Content-Transfer-Encoding: 8bit

On Thu, 06 Jul 2017 03:33:57 PDT (-0700), boqun.feng@gmail.com wrote:
> On Tue, Jul 04, 2017 at 12:50:55PM -0700, Palmer Dabbelt wrote:
> [...]
EOF

#include "harness_end.bash"

grep ^CC out.test

if [[ "$(grep ^CC out.test)" != "CC: peterz@infradead.org, mingo@redhat.com, mcgrof@kernel.org," ]]
then
  cat out.test
  exit 1
fi
