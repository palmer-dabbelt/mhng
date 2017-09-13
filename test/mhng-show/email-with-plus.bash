#include "harness_start.bash"

ARGS="inbox 1"

export TZ="America/Los_Angeles"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (2, 1, "inbox", 0, "test", 1, "from@example.com", "to@example.com");
EOF

cat >$MHNG_MAILDIR/mail/inbox/2 <<"EOF"
Delivered-To: palmer@dabbelt.com
Received: by 10.100.149.78 with SMTP id y14csp1211566pja;
        Wed, 13 Sep 2017 08:20:43 -0700 (PDT)
X-Received: by 10.202.212.199 with SMTP id l190mr20818317oig.126.1505316043716;
        Wed, 13 Sep 2017 08:20:43 -0700 (PDT)
ARC-Seal: i=1; a=rsa-sha256; t=1505316043; cv=none;
        d=google.com; s=arc-20160816;
        b=XOdKie+S9s/PB+xs3SvREvs1SjyLNvF1zdBfFFKE+9pxpnbygqNtvvb5yVhRxCLI3y
         /xQMe6002VXM9TQURggNwIUrj39TrC9UlmajukdlaAQnzxIZxAwqdu2XTIisORelTVqA
         KHNBnWyZciM0zzvzFWVapeZuOEom75rGqUWvcYxtSSiTe/wPMgef5tv7urrc0aWzqGFs
         CdtG3r9lwskTsMD+s6EehUAW3y9zJaWtJCctB6Q/LTh0m+WL/dcGEiPIr3+C2IyEyqLR
         fWs9/eokVa3CM6hmYuP5Ayak5yWkJDqETQQHY0m/R8W8BaXEY2rCj+wIDJvdNRQhb2pi
         OCsA==
ARC-Message-Signature: i=1; a=rsa-sha256; c=relaxed/relaxed; d=google.com; s=arc-20160816;
        h=cc:to:subject:message-id:date:from:references:in-reply-to:sender
         :mime-version:dkim-signature:arc-authentication-results;
        bh=walLDO05NjOtFxzFrq/36e4tMRXRjeiJERCEdIt6z1c=;
        b=uOdxNBiiXtcqsU4wsQozMhTrL+2yMzFc05aZB0irxkWx8c1gzk5hEUI7hprd0Nnj/C
         ffKxnLgR2YF/D/8sBg3Vx/Dyqr5vQ5v0CkbtwmwamaFF+OYbfr0ggWasqGFyENQ/Nwsh
         U+ceF8AB4Pk/ycnDMXDgNoPNEoso40gbxy4p80uCw+MeDPaC4wz8nQhEEPVzcANLUbJK
         4XD5dY2aKvrdVI4KFtaFXdUQJsAudfxzq6FvYNZOYP645rCDt5hZyWBbV/MPaLj7j0p+
         zxFeZQPknfVAAy6hJIU/1o7L8urypt/DckMNwnTA4t3UYjChoEvxUWphX4Wm31B4fH+T
         BJ2Q==
ARC-Authentication-Results: i=1; mx.google.com;
       dkim=pass header.i=@gmail.com header.s=20161025 header.b=IVFgAlxP;
       spf=pass (google.com: domain of arndbergmann@gmail.com designates 209.85.220.65 as permitted sender) smtp.mailfrom=arndbergmann@gmail.com
Return-Path: <arndbergmann@gmail.com>
Received: from mail-sor-f65.google.com (mail-sor-f65.google.com. [209.85.220.65])
        by mx.google.com with SMTPS id r81sor3509605oie.100.2017.09.13.08.20.43
        for <palmer@dabbelt.com>
        (Google Transport Security);
        Wed, 13 Sep 2017 08:20:43 -0700 (PDT)
Received-SPF: pass (google.com: domain of arndbergmann@gmail.com designates 209.85.220.65 as permitted sender) client-ip=209.85.220.65;
Authentication-Results: mx.google.com;
       dkim=pass header.i=@gmail.com header.s=20161025 header.b=IVFgAlxP;
       spf=pass (google.com: domain of arndbergmann@gmail.com designates 209.85.220.65 as permitted sender) smtp.mailfrom=arndbergmann@gmail.com
DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed;
        d=gmail.com; s=20161025;
        h=mime-version:sender:in-reply-to:references:from:date:message-id
         :subject:to:cc;
        bh=walLDO05NjOtFxzFrq/36e4tMRXRjeiJERCEdIt6z1c=;
        b=IVFgAlxPoQl07HJKsLK0No7Cko6hTdLrQiTQB35k+Ot4wLwwDWywdnuyeXSfxesXOd
         gddsdYUmU23fCuT7ciNKxmhR6wXRC8oON6ngZECAb86jnMDBuVHTTglqGo1FU9nhACbg
         hGaSXXq4ZuKhjLMooNrPD4aQqUbNNl6cTsEYGdzMnOH71KYJDEHTTMz61olqTxRtS4L9
         0Hn6wTJeUnGipKiQO8F5BkIxGELHGe5JDWog4N+65czbJf5qcdtyAPQ16UNQZ74EA1nZ
         s5rWhx+kNz9Qjod65s+moKqt0yTF/dS/BaVkKbNwCxQRLJ4Mz0k0qt/maaFGbtXEn9xo
         lXvg==
X-Google-DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed;
        d=1e100.net; s=20161025;
        h=x-gm-message-state:mime-version:sender:in-reply-to:references:from
         :date:message-id:subject:to:cc;
        bh=walLDO05NjOtFxzFrq/36e4tMRXRjeiJERCEdIt6z1c=;
        b=nn+D38foBJwLSrEMccVOieyrWPBhFbVTi8slansYwykwtTRHbw2c05UNpRPF3jwUFG
         j04Y7Ox9Zui9msxfhNnINJradNq0I5f60p5/+GqJdhOYut6lF2cmI+NY6NPXc8wGH3yS
         MbuTEzvAG86lG/EOYwdCcE1hHRHIS8IynI3TPdwxXnSmrpaA3L2ZbI2MBFFNfjQm7uwG
         v06Hf0ngr8sO1XGtf7qawk6nGF1n0iX2up/kBo8ZkE5wpsBC02RtVdiVqYAZZdXoAcIp
         tcLPsz6LegUXGxh3BZE+efh6fwnvKcyaCHUClelLZ6r/8AYYdhXhfYNXatUxL5LprnlP
         9Ehw==
X-Gm-Message-State: AHPjjUhBJFoXV7FHmPOC8a9KpepDqQfK5I1tB6sOE3TmaDVH4jNNYNul
	vL4+8bk4OqnIQSRprmcIjse0gKG1W2ifuAWb68+FGw==
X-Google-Smtp-Source: AOwi7QCmupQPN0Zx1zo3o+PzDdCRjZWjxnKMujT4T0yTHhJhDcuyIcDOKD2qov4zv96vkOlrPUHS7Ct0vrCHbdx49lo=
X-Received: by 10.202.179.137 with SMTP id c131mr9527511oif.175.1505316042886;
 Wed, 13 Sep 2017 08:20:42 -0700 (PDT)
MIME-Version: 1.0
Sender: arndbergmann@gmail.com
Received: by 10.157.29.197 with HTTP; Wed, 13 Sep 2017 08:20:42 -0700 (PDT)
In-Reply-To: <20170912215715.4186-11-palmer@dabbelt.com>
References: <20170912215715.4186-1-palmer@dabbelt.com> <20170912215715.4186-11-palmer@dabbelt.com>
From: Arnd Bergmann <arnd@arndb.de>
Date: Wed, 13 Sep 2017 17:20:42 +0200
X-Google-Sender-Auth: qqq2NdqjtTrCWTPXVZ0veyTElu0
Message-ID: <CAK8P3a3fxTkRjXfHfW=8OoME0DNyU=DKNcaRPWwnaKqF-w1ccg@mail.gmail.com>
Subject: Re: [PATCH v8 10/18] RISC-V: Init and Halt Code
To: Palmer Dabbelt <palmer@dabbelt.com>
Cc: Peter Zijlstra <peterz@infradead.org>, Thomas Gleixner <tglx@linutronix.de>,
	Jason Cooper <jason@lakedaemon.net>, Marc Zyngier <marc.zyngier@arm.com>, dmitriy@oss-tech.org,
	Masahiro Yamada <yamada.masahiro@socionext.com>, Michal Marek <mmarek@suse.com>, albert@sifive.com,
	Will Deacon <will.deacon@arm.com>, Boqun Feng <boqun.feng@gmail.com>,
	Oleg Nesterov <oleg@redhat.com>, Ingo Molnar <mingo@redhat.com>,
	Daniel Lezcano <daniel.lezcano@linaro.org>, gregkh <gregkh@linuxfoundation.org>,
	Jiri Slaby <jslaby@suse.com>, David Miller <davem@davemloft.net>,
	Mauro Carvalho Chehab <mchehab@kernel.org>, Hans Verkuil <hverkuil@xs4all.nl>,
	Randy Dunlap <rdunlap@infradead.org>, Al Viro <viro@zeniv.linux.org.uk>,
	Masami Hiramatsu <mhiramat@kernel.org>, =?UTF-8?B?RnLDqWTDqXJpYyBXZWlzYmVja2Vy?= <fweisbec@gmail.com>,
	"Luis R. Rodriguez" <mcgrof@kernel.org>, Doug Ledford <dledford@redhat.com>,
	Bart Van Assche <bart.vanassche@sandisk.com>, sstabellini@kernel.org,
	Michael Ellerman <mpe@ellerman.id.au>, Russell King <rmk+kernel@armlinux.org.uk>,
	Paul Gortmaker <paul.gortmaker@windriver.com>, Nicolas Dichtel <nicolas.dichtel@6wind.com>,
	Guenter Roeck <linux@roeck-us.net>, Heiko Carstens <heiko.carstens@de.ibm.com>,
	Martin Schwidefsky <schwidefsky@de.ibm.com>, Geert Uytterhoeven <geert@linux-m68k.org>,
	Andrew Morton <akpm@linux-foundation.org>,
	Andy Shevchenko <andriy.shevchenko@linux.intel.com>, Jiri Pirko <jiri@mellanox.com>,
	Vineet Gupta <vgupta@synopsys.com>, Dave Airlie <airlied@redhat.com>, jk@ozlabs.org,
	Chris Wilson <chris@chris-wilson.co.uk>, "Jason A. Donenfeld" <Jason@zx2c4.com>,
	"Paul E. McKenney" <paulmck@linux.vnet.ibm.com>, ncardwell@google.com,
	Linux Kernel Mailing List <linux-kernel@vger.kernel.org>,
	Linux Kbuild mailing list <linux-kbuild@vger.kernel.org>, patches@groups.riscv.org
Content-Type: text/plain; charset="UTF-8"

On Tue, Sep 12, 2017 at 11:57 PM, Palmer Dabbelt <palmer@dabbelt.com> wrote:
> This contains the various __init C functions, the initial assembly
> kernel entry point, and the code to reset the system.  When a file was
> init-related this patch contains the entire file.

One minor comment:

> +       /*
> +        * This hart didn't win the lottery, so we wait for the winning hart to
> +        * get far enough along the boot process that it should continue.
> +        */
> +.Lwait_for_cpu_up:
> +       REG_L sp, (a1)
> +       REG_L tp, (a2)
> +       beqz sp, .Lwait_for_cpu_up
> +       beqz tp, .Lwait_for_cpu_up
> +       fence

We usually discourage having the CPUs spin in a busy-loop while
waiting to be started up, at least on ARM platforms. It seems that
you could however just have them wait for an interrupt before checking
the sp/tp values. Would that guarantee to put the CPUs in a low
power state?

       Arnd
EOF

cat >out.gold <<"EOF"
From:       arnd@arndb.de
To:         palmer@dabbelt.com
CC:         peterz@infradead.org
CC:         tglx@linutronix.de
CC:         jason@lakedaemon.net
CC:         marc.zyngier@arm.com
CC:         dmitriy@oss-tech.org
CC:         yamada.masahiro@socionext.com
CC:         mmarek@suse.com
CC:         albert@sifive.com
CC:         will.deacon@arm.com
CC:         boqun.feng@gmail.com
CC:         oleg@redhat.com
CC:         mingo@redhat.com
CC:         daniel.lezcano@linaro.org
CC:         gregkh@linuxfoundation.org
CC:         jslaby@suse.com
CC:         davem@davemloft.net
CC:         mchehab@kernel.org
CC:         hverkuil@xs4all.nl
CC:         rdunlap@infradead.org
CC:         viro@zeniv.linux.org.uk
CC:         mhiramat@kernel.org
CC:         fweisbec@gmail.com
CC:         mcgrof@kernel.org
CC:         dledford@redhat.com
CC:         bart.vanassche@sandisk.com
CC:         sstabellini@kernel.org
CC:         mpe@ellerman.id.au
CC:         rmk+kernel@armlinux.org.uk
CC:         paul.gortmaker@windriver.com
CC:         nicolas.dichtel@6wind.com
CC:         linux@roeck-us.net
CC:         heiko.carstens@de.ibm.com
CC:         schwidefsky@de.ibm.com
CC:         geert@linux-m68k.org
CC:         akpm@linux-foundation.org
CC:         andriy.shevchenko@linux.intel.com
CC:         jiri@mellanox.com
CC:         vgupta@synopsys.com
CC:         airlied@redhat.com
CC:         jk@ozlabs.org
CC:         chris@chris-wilson.co.uk
CC:         Jason@zx2c4.com
CC:         paulmck@linux.vnet.ibm.com
CC:         ncardwell@google.com
CC:         linux-kernel@vger.kernel.org
CC:         linux-kbuild@vger.kernel.org
CC:         patches@groups.riscv.org
Subject:    Re: [PATCH v8 10/18] RISC-V: Init and Halt Code
Date:       Wed, 13 Sep 2017 08:20:42 PDT (-0700)
Message-ID: <CAK8P3a3fxTkRjXfHfW=8OoME0DNyU=DKNcaRPWwnaKqF-w1ccg@mail.gmail.com>

On Tue, Sep 12, 2017 at 11:57 PM, Palmer Dabbelt <palmer@dabbelt.com> wrote:
> This contains the various __init C functions, the initial assembly
> kernel entry point, and the code to reset the system.  When a file was
> init-related this patch contains the entire file.

One minor comment:

> +       /*
> +        * This hart didn't win the lottery, so we wait for the winning hart to
> +        * get far enough along the boot process that it should continue.
> +        */
> +.Lwait_for_cpu_up:
> +       REG_L sp, (a1)
> +       REG_L tp, (a2)
> +       beqz sp, .Lwait_for_cpu_up
> +       beqz tp, .Lwait_for_cpu_up
> +       fence

We usually discourage having the CPUs spin in a busy-loop while
waiting to be started up, at least on ARM platforms. It seems that
you could however just have them wait for an interrupt before checking
the sp/tp values. Would that guarantee to put the CPUs in a low
power state?

       Arnd

EOF

#include "harness_end.bash"
