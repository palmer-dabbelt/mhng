#include "harness_start.bash"

ARGS="inbox 1"

export TZ="America/Los_Angeles"
export TERM="xterm"
export COLUMNS="90"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (2, 1, "inbox", 0, "test", 1, "from@example.com", "to@example.com");
EOF

cat >$MHNG_MAILDIR/mail/inbox/2 <<"EOF"
Delivered-To: palmer@dabbelt.com
Received: by 2002:aed:2963:0:0:0:0:0 with SMTP id s90csp894571qtd;
        Fri, 4 Oct 2019 08:09:12 -0700 (PDT)
X-Received: by 2002:a2e:442:: with SMTP id 63mr9976138lje.66.1570201752636;
        Fri, 04 Oct 2019 08:09:12 -0700 (PDT)
ARC-Seal: i=2; a=rsa-sha256; t=1570201752; cv=pass;
        d=google.com; s=arc-20160816;
        b=O6UqQpvJOw5w8GyEMJrgn+7DgBihfMw2Kst4wi99qDpJe1Ij/RrRTkmggu65Z/bU0x
         TTRvfH7LjowyEzvQm5gdfOYCuqujZAfnZBKJrg5LcJLPYkImdNL/Q+j5D1GM/aJIX5Dr
         FhWVZZ/1VbHxKZYiGYROTI10QdIVpgJvEluLrSTlkba8Wb4Y1OIJGiax0S7UjsSpqKxf
         ohpYtFiM5PMMGANyRW4LaFlczcXz1IGzDvpHlO/w3oN9/AuijiGshSxwGZVHUVyAUpCd
         HfMRTWMjsbEKOO4tvFV0a63o03Ox81j5GLO3XCBUsPiQcDCJ8SJburHiB+NJyBfct5lM
         TGTg==
ARC-Message-Signature: i=2; a=rsa-sha256; c=relaxed/relaxed; d=google.com; s=arc-20160816;
        h=to:subject:message-id:date:from:mime-version:dkim-signature
         :delivered-to;
        bh=GtXaP0Hjv13x3JUrXwmdBej0jo8Nte5eM7uIImHmRRA=;
        b=RwM0e3l1op8fHNTxX5w3w7Mo5FN7/+RXIGD5kZrPOeDxE5uJitf8PPE5BGDSog5wI9
         GtKIY2rAvymewDaPphpNmVSOTy19tgKKb2toVO6Eaj/hXsHkg8gAaIpAZqZZVkyAiQr4
         eoHWzMy27v8Zjk2CpctkoXX4InR4LGmWzK+F4cY8lTj47mvc7hb6GNCrguLZJ+YoL0Uf
         sUIAHVoh6QIT4/dyifVNd64kLVPgRhpXcFr+lb4ci2PxNd6TYFatvi2IJmEStpevstla
         3hJ8A3amYPc0dF6KsMvZQHu7zIelgipzS5r8hIyJPiQ0D44WQ7t6DW+4mocTdlsL+BnW
         UHaQ==
ARC-Authentication-Results: i=2; mx.google.com;
       dkim=pass header.i=@gmail.com header.s=20161025 header.b="in5LM/Bh";
       arc=pass (i=1 spf=pass spfdomain=gmail.com dkim=pass dkdomain=gmail.com dmarc=pass fromdomain=gmail.com);
       spf=pass (google.com: domain of palmer+caf_=palmer=dabbelt.com@sifive.com designates 209.85.220.41 as permitted sender) smtp.mailfrom="palmer+caf_=palmer=dabbelt.com@sifive.com";
       dmarc=pass (p=NONE sp=QUARANTINE dis=NONE) header.from=gmail.com
Return-Path: <palmer+caf_=palmer=dabbelt.com@sifive.com>
Received: from mail-sor-f41.google.com (mail-sor-f41.google.com. [209.85.220.41])
        by mx.google.com with SMTPS id u4sor4361765ljj.34.2019.10.04.08.09.12
        for <palmer@dabbelt.com>
        (Google Transport Security);
        Fri, 04 Oct 2019 08:09:12 -0700 (PDT)
Received-SPF: pass (google.com: domain of palmer+caf_=palmer=dabbelt.com@sifive.com designates 209.85.220.41 as permitted sender) client-ip=209.85.220.41;
Authentication-Results: mx.google.com;
       dkim=pass header.i=@gmail.com header.s=20161025 header.b="in5LM/Bh";
       arc=pass (i=1 spf=pass spfdomain=gmail.com dkim=pass dkdomain=gmail.com dmarc=pass fromdomain=gmail.com);
       spf=pass (google.com: domain of palmer+caf_=palmer=dabbelt.com@sifive.com designates 209.85.220.41 as permitted sender) smtp.mailfrom="palmer+caf_=palmer=dabbelt.com@sifive.com";
       dmarc=pass (p=NONE sp=QUARANTINE dis=NONE) header.from=gmail.com
X-Google-DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed;
        d=1e100.net; s=20161025;
        h=x-gm-message-state:delivered-to:dkim-signature:mime-version:from
         :date:message-id:subject:to;
        bh=GtXaP0Hjv13x3JUrXwmdBej0jo8Nte5eM7uIImHmRRA=;
        b=Rkj2v8JYL0NiI4Dj3C2Z0gTkfaqxnIiekX4zwmQEc6l6T5e7Qt/hB6np/V1Y7fZrU+
         EWILBOCiGMvOIYHelaMpnRSbKR5D2oE9ESFhL0D5HPxhbxk1dV5xd6qegJwGVDXLiAJB
         BD4TcWtCndXdkZ1bZKvnH2KWnr1hI04QZtTrWMjCOFSPfXHtjTZJyCtQ+YRdO5amNxKA
         nLq3TOUtMfYTVYQ3/x4epwiTELsEBeIgbzJUlpLUHFS1MMhS5x/8WnZbQG8AT2BP1E/Z
         OjlHqy1BkzWxUFLXkv+F9jYrKW4kiwWQ0n36gQutVE9gVJelxSJlGNQ+B0M1XJfTAiuk
         LDaA==
X-Gm-Message-State: APjAAAWl953T0S/BAF5zQkHUU2vxT8N+45tVEB0mMZ5afWcGo+PMqUBt TT9moVQpgJFMjjkPqNQjRQNEJas/dVRtnv2iJ5WgoVVPXu+LgB3sbw==
X-Received: by 2002:a2e:141c:: with SMTP id u28mr9082374ljd.44.1570201752152;
        Fri, 04 Oct 2019 08:09:12 -0700 (PDT)
X-Forwarded-To: palmer@dabbelt.com
X-Forwarded-For: palmer@sifive.com palmer@dabbelt.com
Delivered-To: palmer@sifive.com
Received: by 2002:a05:6504:749:0:0:0:0 with SMTP id g9csp819464ltq;
        Fri, 4 Oct 2019 08:09:10 -0700 (PDT)
X-Received: by 2002:a05:6000:160e:: with SMTP id u14mr12049651wrb.29.1570201750210;
        Fri, 04 Oct 2019 08:09:10 -0700 (PDT)
ARC-Seal: i=1; a=rsa-sha256; t=1570201750; cv=none;
        d=google.com; s=arc-20160816;
        b=PSBwrYflBxk2iCFo9jlvzVf4vYbBLVbhjZtrQBU6F5jx2nqNvFGclQXZz/r0BNYAbB
         VP9AQ7Ddnl16RI2SpHou1/8bt2HSeQCfhnF6KOVL18uHMhABcN9YLuUxrUXfms8n7xzO
         jRtD8JuAlytTt26YDAJyta5OM4nve4dv7rSmWIK/0f3DXXUnyYx1cFl5EgKZhCJxqtJy
         ZtNmziiv9UmmTzBLOngoNci94Vi9foCggQQsXEc5/MMv8ER3hL/JY1TZ2cNzwBAOZ0gm
         Q/2bJdmV7pfCHq+k6H74FPECLGT1hT61hO/eeWL+B1q8M9ydjCcWoxCTREh4+eCzG3br
         LtcQ==
ARC-Message-Signature: i=1; a=rsa-sha256; c=relaxed/relaxed; d=google.com; s=arc-20160816;
        h=to:subject:message-id:date:from:mime-version:dkim-signature;
        bh=GtXaP0Hjv13x3JUrXwmdBej0jo8Nte5eM7uIImHmRRA=;
        b=xDBfpjfxehxpZnuTwAUTIUCFy9pbLf5CguK3r+3Ie3HBFDAfAV3u10n6rgUin9hXhj
         n93ygnEm/2EhdfG2+GEnr1J0eHR3snfwFRCN6mmFVUQVR9dPZgEIAIzbRs8B8w43qJlX
         RM2hCs75NhjBG+QytfSz/fwSlumRX88NsFCEunQIka2gVbdPKUZ1de1btKxr18ENRkqI
         xuZiYZjG1juwc+617UVFtNgbozWhIvHodXwDu7QF56ei1EhXEmYKXcwb2C/1fCDNMjTr
         g7zu8m6I3JJUNL8RGh9kq68KkW11e7OTXCZGoP03MIsV7dFQ8ya84UyrGiDUimMjZ8Ni
         d4VA==
ARC-Authentication-Results: i=1; mx.google.com;
       dkim=pass header.i=@gmail.com header.s=20161025 header.b="in5LM/Bh";
       spf=pass (google.com: domain of david.abdurachmanov@gmail.com designates 209.85.220.65 as permitted sender) smtp.mailfrom=david.abdurachmanov@gmail.com;
       dmarc=pass (p=NONE sp=QUARANTINE dis=NONE) header.from=gmail.com
Return-Path: <david.abdurachmanov@gmail.com>
Received: from mail-sor-f65.google.com (mail-sor-f65.google.com. [209.85.220.65])
        by mx.google.com with SMTPS id z203sor3807312wmc.11.2019.10.04.08.09.09
        (Google Transport Security);
        Fri, 04 Oct 2019 08:09:10 -0700 (PDT)
Received-SPF: pass (google.com: domain of david.abdurachmanov@gmail.com designates 209.85.220.65 as permitted sender) client-ip=209.85.220.65;
DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed;
        d=gmail.com; s=20161025;
        h=mime-version:from:date:message-id:subject:to;
        bh=GtXaP0Hjv13x3JUrXwmdBej0jo8Nte5eM7uIImHmRRA=;
        b=in5LM/BhUTYoFuSG0fi+ztozdO5/KK1Sl4Fpxrt80vM4OBYHRGhUmVrB/ojv2m8B5U
         7SqfNyyrl5M9HSFLhbSw1tP5LYSH9Oqmxsno+DDBF6zPfEpk9j2vqRMLTYqYVzoMMj3Z
         tvhre7TCKu8nttpXk3cWj9Ave7/9Ll+X1sQ/4oxNgbiPPwleOPV+gXT/7R2RjHWAd27c
         EYzRul6XWf6znVGqUTSYGGNrz/Ebe52/9YP4m9ggJw2ZRo+zP5qcQHvRFW34+X3XdVhM
         rryc1WPJjID/ybwvGsIE8llaLJExs6HxdMSRIt+/X15i9CYvKj8yDj2Crt9s4q+3k8Xp
         u02Q==
X-Google-Smtp-Source: APXvYqzjTuHjVAwhFK1EOPMK00tb+ffadsoAEO4RcpEeSunS6tg/TiFydenGuhtBBQhA6l97eYWXEVE06qgo/kQgFzo=
X-Received: by 2002:a7b:cd1a:: with SMTP id f26mr5813091wmj.71.1570201749080; Fri, 04 Oct 2019 08:09:09 -0700 (PDT)
MIME-Version: 1.0
From: David Abdurachmanov <david.abdurachmanov@gmail.com>
Date: Fri, 4 Oct 2019 18:08:32 +0300
Message-ID: <CAEn-LTpx8z24D3qo39wW+BX9y6XMjs6VfoAkGiZYUUSB9Z_Png@mail.gmail.com>
Subject: Fedora single disk image for QEMU and FU540: console=?
To: Palmer Dabbelt <palmer@sifive.com>
Content-Type: text/plain; charset="UTF-8"

Hi folks,

TL;DR I can finally boot Fedora 31 on QEMU virt and SiFive Unleashed
with a single build. The last issue seems to be console= kernel
parameter.

IIUC Anup is using "console=ttyS0 earlycon=sbi" for QEMU virt machine.
That works fine for me. Without these options QEMU virt also works
just fine (I don't see any difference in booting messages).

For the SiFive Unleashed I must set console=ttySIF0 in order to get
boot messages from kernel and systemd.

The default EXTLINUX/PXE config generated my appliance-creator for
Fedora 31 incl. something like this: "ro
root=UUID=35f5b3e1-fd07-45e9-ae3c-d060db4cb74b rhgb quiet
LANG=en_US.UTF-8" on append line.

I noticed that multiple boards have console= and earlycon= specified
in board defconfigs or DTS files within U-Boot as part of bootargs.

That is a problem because EXTLINUX/PXE login seems to override bootargs. See:
https://github.com/u-boot/u-boot/blob/master/cmd/pxe.c#L585
https://github.com/u-boot/u-boot/blob/master/cmd/pxe.c#L703

My idea was to have something like:

diff --git a/configs/qemu-riscv64_smode_defconfig
b/configs/qemu-riscv64_smode_defconfig
index 1c7a2d15..abd157e8 100644
--- a/configs/qemu-riscv64_smode_defconfig
+++ b/configs/qemu-riscv64_smode_defconfig
@@ -10,3 +10,7 @@ CONFIG_DISPLAY_BOARDINFO=y
 CONFIG_CMD_BOOTEFI_SELFTEST=y
 # CONFIG_CMD_MII is not set
 CONFIG_OF_PRIOR_STAGE=y
+CONFIG_USE_BOOTARGS=y
+CONFIG_BOOTARGS="console=ttyS0 earlycon=sbi"
+CONFIG_USE_PREBOOT=y
+CONFIG_PREBOOT="cp.l ${fdtcontroladdr} ${fdt_addr_r} 0x10000;"
diff --git a/configs/sifive_fu540_defconfig b/configs/sifive_fu540_defconfig
index 48865e5f..940347ff 100644
--- a/configs/sifive_fu540_defconfig
+++ b/configs/sifive_fu540_defconfig
@@ -9,3 +9,5 @@ CONFIG_MISC_INIT_R=y
 CONFIG_DISPLAY_CPUINFO=y
 CONFIG_DISPLAY_BOARDINFO=y
 CONFIG_OF_PRIOR_STAGE=y
+CONFIG_USE_BOOTARGS=y
+CONFIG_BOOTARGS="console=ttySIF0 earlycon=sbi"

Which probably wouldn't work for EXTLINUX/PXE.

IIUC OpenSBI will add stdout-path in chosen node, and probably that's
how U-Boot figures out what serial console to use. Now, I think,
PowerPC Linux side also use it setup console.

If there a way this knowledge about console would be handled in
firmware instead of touching user side?

I want to avoid having "arm-image-installer" tool there you need to
have SoC and board specific knowledge to setup disk image, e.g.:
https://pagure.io/arm-image-installer/blob/master/f/socs.d/exynos5

Currently I could make a disk image which works for QEMU virt and
FU540, but then user needs to mount the 1st partition and modify
append line in extlinux.conf based on target, or I write something
like "arm-image-installer".

The logic is that DTB stays within firmware (FSBL, OpenSBI, or
U-Boot). Anything boards specific again should be handled within
firmware so it's more like PC. User should be able to provide more or
override defaults, but otherwise things should work out-of-the-box.

Any ideas/suggestions?

Cheers,
david
EOF

cat >out.gold <<"EOF"
From:       david.abdurachmanov@gmail.com
To:         palmer@sifive.com
Subject:    Fedora single disk image for QEMU and FU540: console=?
Date:       Fri, 04 Oct 2019 08:08:32 PDT (-0700)
Message-ID: <CAEn-LTpx8z24D3qo39wW+BX9y6XMjs6VfoAkGiZYUUSB9Z_Png@mail.gmail.com>

Hi folks,

TL;DR I can finally boot Fedora 31 on QEMU virt and SiFive Unleashed
with a single build. The last issue seems to be console= kernel
parameter.

IIUC Anup is using "console=ttyS0 earlycon=sbi" for QEMU virt machine.
That works fine for me. Without these options QEMU virt also works
just fine (I don't see any difference in booting messages).

For the SiFive Unleashed I must set console=ttySIF0 in order to get
boot messages from kernel and systemd.

The default EXTLINUX/PXE config generated my appliance-creator for
Fedora 31 incl. something like this: "ro
root=UUID=35f5b3e1-fd07-45e9-ae3c-d060db4cb74b rhgb quiet
LANG=en_US.UTF-8" on append line.

I noticed that multiple boards have console= and earlycon= specified
in board defconfigs or DTS files within U-Boot as part of bootargs.

That is a problem because EXTLINUX/PXE login seems to override bootargs. See:
https://github.com/u-boot/u-boot/blob/master/cmd/pxe.c#L585
https://github.com/u-boot/u-boot/blob/master/cmd/pxe.c#L703

My idea was to have something like:

diff --git a/configs/qemu-riscv64_smode_defconfig
b/configs/qemu-riscv64_smode_defconfig
index 1c7a2d15..abd157e8 100644
--- a/configs/qemu-riscv64_smode_defconfig
+++ b/configs/qemu-riscv64_smode_defconfig
@@ -10,3 +10,7 @@ CONFIG_DISPLAY_BOARDINFO=y
 CONFIG_CMD_BOOTEFI_SELFTEST=y
 # CONFIG_CMD_MII is not set
 CONFIG_OF_PRIOR_STAGE=y
+CONFIG_USE_BOOTARGS=y
+CONFIG_BOOTARGS="console=ttyS0 earlycon=sbi"
+CONFIG_USE_PREBOOT=y
+CONFIG_PREBOOT="cp.l ${fdtcontroladdr} ${fdt_addr_r} 0x10000;"
diff --git a/configs/sifive_fu540_defconfig b/configs/sifive_fu540_defconfig
index 48865e5f..940347ff 100644
--- a/configs/sifive_fu540_defconfig
+++ b/configs/sifive_fu540_defconfig
@@ -9,3 +9,5 @@ CONFIG_MISC_INIT_R=y
 CONFIG_DISPLAY_CPUINFO=y
 CONFIG_DISPLAY_BOARDINFO=y
 CONFIG_OF_PRIOR_STAGE=y
+CONFIG_USE_BOOTARGS=y
+CONFIG_BOOTARGS="console=ttySIF0 earlycon=sbi"

Which probably wouldn't work for EXTLINUX/PXE.

IIUC OpenSBI will add stdout-path in chosen node, and probably that's
how U-Boot figures out what serial console to use. Now, I think,
PowerPC Linux side also use it setup console.

If there a way this knowledge about console would be handled in
firmware instead of touching user side?

I want to avoid having "arm-image-installer" tool there you need to
have SoC and board specific knowledge to setup disk image, e.g.:
https://pagure.io/arm-image-installer/blob/master/f/socs.d/exynos5

Currently I could make a disk image which works for QEMU virt and
FU540, but then user needs to mount the 1st partition and modify
append line in extlinux.conf based on target, or I write something
like "arm-image-installer".

The logic is that DTB stays within firmware (FSBL, OpenSBI, or
U-Boot). Anything boards specific again should be handled within
firmware so it's more like PC. User should be able to provide more or
override defaults, but otherwise things should work out-of-the-box.

Any ideas/suggestions?

Cheers,
david
EOF

#include "harness_end.bash"
