#include "harness_start.bash"

ARGS="drafts 1"

export MHNG_COMP_DATE="Mon Nov 13 14:40:14 PST 2017"
export MHNG_COMP_MESSAGE_ID="<mhng-TEST@TEST>"
export MHNG_MAILRC="$(pwd)/mailrc"
export TZ="America/Los_Angeles"

cat > $MHNG_MAILRC <<EOF
local Palmer Dabbelt <palmer@dabbelt.com>
local Palmer Dabbelt <palmer@gnu.org>
local Palmer Dabbelt <palmer@gnupg.net>
local Palmer Dabbelt <palmer@sifive.com>
local Palmer Dabbelt <palmerandlulu@dabbelt.com>

address         BoA Bill Pay <billpay@billpay.bankofamerica.com>

# Personal
alias palmer    Palmer Dabbelt <palmer@dabbelt.com>
alias lulu      Lulu Li <lilulu6@gmail.com>
alias cma       Charlie Ma <cma1681@gmail.com>
alias eric      Eric Love <ericlove@gmail.com>

# Family
alias dad       Dan Dabbelt <dan@dabbelt.com>
alias mom       Mary Jane Kennedy <mjk@dabbelt.com>
alias jwk       John Kennedy <johnwkennedy009@comcast.net>

# SiFive
alias si        Palmer Dabbelt <palmer@sifive.com>
alias andrew    Andrew Waterman <andrew@sifive.com>
alias david     David Johnson <davidj@sifive.com>
alias yunsup    Yunsup Lee <yunsup@sifive.com>
alias megan     Megan Wachs <megan@sifive.com>
alias renxin    Renxin Xia <renxin@sifive.com>
alias wes       Wesley Terpstra <wesley@sifive.com>
alias jack      Jack Kang <jack@sifive.com>
alias drew      Drew Barbier <drew@sifive.com>
alias krste     Krste Asanovic <krste@sifive.com>
alias jim       Jim Wilson <jimw@sifive.com>

# Upstream
alias arnd      Arnd Bergmann <arnd@arndb.de>
alias olof      Olof Johansson <olof@lixom.net>
alias hjl       H.J. Lu <hjl.tools@gmail.com>
alias nickc     Nick Clifton <nickc@redhat.com>
alias sfr       Stephen Rothwell <sfr@canb.auug.org.au>
alias linus     Linus Torvalds <torvalds@linux-foundation.org>
alias will      Will Deacon <will.deacon@arm.com>

# RISC-V
alias arun      Arun Thomas <arun.thomas@gmail.com>
alias asb       Alex Bradbury <asb@asbradbury.org>
alias darius    Darius Rad <darius@bluespec.com>
alias khem      Khem Raj <raj.khem@gmail.com>
alias mjc       Michael Clark <michaeljclark@mac.com>
alias hisen     Hisen Woo <hcwoo@me.com>
alias davidlt   David Abdurachmanov <David.Abdurachmanov@cern.ch>
alias kito      Kito Cheng <kito.cheng@gmail.com>
alias lustig    Daniel Lustig <dlustig@nvidia.com>

# Mailing Lists
alias binutils-patches binutils@sourceware.org
alias gcc-patches      gcc-patches@gcc.gnu.org
alias libc-patches     libc-alpha@sourceware.org
alias linux-patches    linux-kernel@vger.kernel.org
alias riscv-patches    patches@groups.riscv.org
alias riscv-sw         sw-dev@groups.riscv.org

alias patch-bin    binutils@sourceware.org
alias patch-gcc    gcc-patches@gcc.gnu.org
alias patch-libc   libc-alpha@sourceware.org
alias patch-linux  linux-kernel@vger.kernel.org
EOF

cat >"$MHNG_EDITOR" <<"EOF"
#!/bin/bash

cat in.test > $1
EOF

cat >in.test <<"EOF"
From:    si
To:      riscv-sw
Subject: I've reorganized the riscv-linux GitHub repo

I've been making a habit of going through and cleaning up our github repos 
after we get things upstream, as we tend to accumulate lots of old branches and 
such.  I spent this morning doing this for our Linux repo, where there was a 
lot of cruft.  No commits are actually gone, they're just all hidden behind the 
__archive__ branch now.

So far we've only upstreamed the core architecture part of the kernel, we 
haven't gotten the drivers in yet.  While master should be able to build and 
nominally boot a kernel, you can't do a whole lot -- for example there's no 
console, no interrupts, no timers, etc.  In other words: the RISC-V Linux port 
isn't done, it's just reached a point where we're not all serialized on one big 
chunk of code any more.

I've opened a dozen or so pull requests on our Linux GitHub page

  https://github.com/riscv/riscv-linux/pulls

to help track all the in progress work we have going on right now.  The vast 
majority of these have been reviewed to some degree on various mailing lists, 
but none of them are actually ready to go yet.
EOF

cat >out.gold <<"EOF"
Subject: I've reorganized the riscv-linux GitHub repo
From: Palmer Dabbelt <palmer@sifive.com>
To:         sw-dev@groups.riscv.org
Date: Mon Nov 13 14:40:14 PST 2017
Message-ID: <mhng-TEST@TEST>
Mime-Version: 1.0 (MHng)
Content-Type: text/plain; charset=utf-8; format=flowed
Content-Transfer-Encoding: 8bit

I've been making a habit of going through and cleaning up our github repos 
after we get things upstream, as we tend to accumulate lots of old branches and 
such.  I spent this morning doing this for our Linux repo, where there was a 
lot of cruft.  No commits are actually gone, they're just all hidden behind the 
__archive__ branch now.

So far we've only upstreamed the core architecture part of the kernel, we 
haven't gotten the drivers in yet.  While master should be able to build and 
nominally boot a kernel, you can't do a whole lot -- for example there's no 
console, no interrupts, no timers, etc.  In other words: the RISC-V Linux port 
isn't done, it's just reached a point where we're not all serialized on one big 
chunk of code any more.

I've opened a dozen or so pull requests on our Linux GitHub page

  https://github.com/riscv/riscv-linux/pulls

to help track all the in progress work we have going on right now.  The vast 
majority of these have been reviewed to some degree on various mailing lists, 
but none of them are actually ready to go yet.
EOF

#include "harness_end.bash"
