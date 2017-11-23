#include "harness_start.bash"

ARGS="inbox 1"

export TZ="America/Los_Angeles"
export TERM="xterm"
export COLUMNS="120"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (2, 1, "inbox", 0, "test", 1, "from@example.com", "to@example.com");
EOF

cat >$MHNG_MAILDIR/mail/inbox/2 <<"EOF"

Delivered-To: palmer@dabbelt.com
Received: by 10.100.243.70 with SMTP id j6csp149853pjs;
        Wed, 22 Nov 2017 17:58:35 -0800 (PST)
X-Google-Smtp-Source: AGs4zMZEBPINQU0QzuBXK97YOF0S02yhETHD0+joREjAJfzoy1dvcFq7CYEUWzz2Q7/1yrxkUKnA
X-Received: by 10.84.168.37 with SMTP id e34mr18881700plb.145.1511402315607;
        Wed, 22 Nov 2017 17:58:35 -0800 (PST)
ARC-Seal: i=1; a=rsa-sha256; t=1511402315; cv=none;
        d=google.com; s=arc-20160816;
        b=pXbBnK4ImZOvbNKHzEiaYNb+m14gmF2tIrZPYGKfU60w0O33WTEBO3KbdyHY7QYlvZ
         0F9vFq3OGEf0Of0AaYOUrJhwZQ+qbguHS7S6nYMl4p7qSKgZCsc70qWhxW1BCzube/yp
         IkDU/5OcB3RluS2oFHtIzY8WkUV86ZXPH0ZF1gfTXLG/4DppEcGQlH8HmAY+F/x2vgaD
         Y9y3H2TWJb3UkQS1ZfpovOn7dGGVBGjz2/0ri9cg9LYhfMYG451ATREJG0dDzljN1Cdw
         eArbvL5EFs30O3weRbZI6XSuHZYEmE0boyCXsWeXPvsxVH3ValgyDJGTzkYyMpTPDnfK
         6SBA==
ARC-Message-Signature: i=1; a=rsa-sha256; c=relaxed/relaxed; d=google.com; s=arc-20160816;
        h=list-id:precedence:sender:cc:to:subject:message-id:date:from
         :references:in-reply-to:mime-version:dkim-signature
         :arc-authentication-results;
        bh=C4xG3GSRDICg9jHum5r0aCN3MsfeRDqe0yJYSxNdrpQ=;
        b=Y8nBOGI6zNF4U8uuFN4SnGTD8+32ItWbLV8p91QMJPhrEZzDFIlShxoLQCEQ4JB1id
         s2V115xEEqCQG6Hls8E+IqJsRvsObHWsoPZVY42kVhoWx3pJxdF0XDVxQM21ViuF6Up4
         zv5V1ZeayubwBzrCq6aCfcZwfU2XzBgINFBQDam1hUAFzFaMlybLq2XBdfYXrPrXJLyD
         AINt6lCOggW/Sf8Bsh3JUbLfOSLZX+Sd43Idv0Un8rmg6wVidcPj3ZQWmMUx5Hb22JG3
         ElXD4Zt43dJTvIaDeOkWWN7/PTkQMiCKGkfwZx8OO36kjxk8Hhp40da3ztFEj3piPve8
         jU0Q==
ARC-Authentication-Results: i=1; mx.google.com;
       dkim=pass header.i=@gmail.com header.s=20161025 header.b=M1T7C/Lj;
       spf=pass (google.com: best guess record for domain of linux-kernel-owner@vger.kernel.org designates 209.132.180.67 as permitted sender) smtp.mailfrom=linux-kernel-owner@vger.kernel.org;
       dmarc=pass (p=NONE sp=NONE dis=NONE) header.from=gmail.com
Return-Path: <linux-kernel-owner@vger.kernel.org>
Received: from vger.kernel.org (vger.kernel.org. [209.132.180.67])
        by mx.google.com with ESMTP id v75si7776988pfd.392.2017.11.22.17.58.35
        for <palmer@dabbelt.com>;
        Wed, 22 Nov 2017 17:58:35 -0800 (PST)
Received-SPF: pass (google.com: best guess record for domain of linux-kernel-owner@vger.kernel.org designates 209.132.180.67 as permitted sender) client-ip=209.132.180.67;
Authentication-Results: mx.google.com;
       dkim=pass header.i=@gmail.com header.s=20161025 header.b=M1T7C/Lj;
       spf=pass (google.com: best guess record for domain of linux-kernel-owner@vger.kernel.org designates 209.132.180.67 as permitted sender) smtp.mailfrom=linux-kernel-owner@vger.kernel.org;
       dmarc=pass (p=NONE sp=NONE dis=NONE) header.from=gmail.com
Received: (majordomo@vger.kernel.org) by vger.kernel.org via listexpand
        id S1751760AbdKWB6d (ORCPT <rfc822;palmer@dabbelt.com>);
        Wed, 22 Nov 2017 20:58:33 -0500
Received: from mail-lf0-f67.google.com ([209.85.215.67]:33342 "EHLO
        mail-lf0-f67.google.com" rhost-flags-OK-OK-OK-OK) by vger.kernel.org
        with ESMTP id S1751525AbdKWB6c (ORCPT
        <rfc822;linux-kernel@vger.kernel.org>);
        Wed, 22 Nov 2017 20:58:32 -0500
Received: by mail-lf0-f67.google.com with SMTP id x68so20350286lff.0
        for <linux-kernel@vger.kernel.org>; Wed, 22 Nov 2017 17:58:31 -0800 (PST)
DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed;
        d=gmail.com; s=20161025;
        h=mime-version:in-reply-to:references:from:date:message-id:subject:to
         :cc;
        bh=C4xG3GSRDICg9jHum5r0aCN3MsfeRDqe0yJYSxNdrpQ=;
        b=M1T7C/LjMniyOOS1h9/RJ2E1Z8Q3++uzFHqNrIlXPLM2M7l0/bOaM2gWRT0aXm1rbr
         /qwQ/X/4Uqd9tDmqG81sXox3aQJn6bnXfeo3t4+t+yu0h433uzE450Aukw5z5qRDYKHY
         49z2RL/+Ei38TR3N4LhvKVSZZCeWUx4LAcvqjLH60tocwdBncir0IcNfXf7CwwKjiJae
         Tdc75IvzCSOnfyEh0Y/v0SNB4d338YcmIIpAa8f0W3kBaYQXHo/XZdzL0BlBeOYJ8Y4d
         jexkPCDyMGnU8trxTxIHTOpmBN+mzllwDwn0ZjZHAyEKWn/hjd7znuVsKrpbWwVt+31b
         /0Kw==
X-Google-DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed;
        d=1e100.net; s=20161025;
        h=x-gm-message-state:mime-version:in-reply-to:references:from:date
         :message-id:subject:to:cc;
        bh=C4xG3GSRDICg9jHum5r0aCN3MsfeRDqe0yJYSxNdrpQ=;
        b=RmLVM7r++iUmip9fklgCvUtCx5F2ToFHbZR9o/NXWlE4u+TxKnsQx9aCJizlBLsYgv
         mXzkiwQdBc65D2VvFRCktX0RR2+map8VBzI8uGL/a9lRkNDco7wp1PUOFLFbqZ3TxxI3
         2JVdrdSDEYAsRjUMuXGvLwxdsrIcYvY6vYLrkbmmczJ0Dwvlcc1lijGO+WQCGJsG+Pc1
         u7hNNe8eqIKya1rJLe6OjyS4KlvVMHm0yM3L2oONx+8MO6zRte0O9vszFeSEyrF1gfcL
         sL5ZHRmf5ipzjkpI1TRZ2E376G23a3Pm9CrcTgoiT0MZ8CljH/G6EJ+MdSdHIcVlEr7a
         KeVw==
X-Gm-Message-State: AJaThX7LalBEVFcduJ52XeiGc9vUFZE9mWTpmUV2oM3t/QKmLkiRZVnj
        BGhioDv5qiPkS7yUS8KGGfrpuHmv0oHuYywEIgw=
X-Received: by 10.25.89.206 with SMTP id n197mr6342213lfb.96.1511402311045;
 Wed, 22 Nov 2017 17:58:31 -0800 (PST)
MIME-Version: 1.0
Received: by 10.25.206.68 with HTTP; Wed, 22 Nov 2017 17:58:30 -0800 (PST)
In-Reply-To: <97148c12-58bb-0f16-1606-03e9dcaba1bf@redhat.com>
References: <97148c12-58bb-0f16-1606-03e9dcaba1bf@redhat.com>
From:   Dave Airlie <airlied@gmail.com>
Date:   Thu, 23 Nov 2017 11:58:30 +1000
Message-ID: <CAPM=9ty2tS3KYTpH3Ci=VjoZoto4mkEtBnGhJ2QyR===0xHfZg@mail.gmail.com>
Subject: Re: Regression in TTM driver w/Linus' master
To:     Laura Abbott <labbott@redhat.com>,
        =?UTF-8?Q?Christian_K=C3=B6nig?= <deathsimple@vodafone.de>
Cc:     =?UTF-8?Q?Christian_K=C3=B6nig?= <christian.koenig@amd.com>,
        Alex Deucher <alexander.deucher@amd.com>,
        Adam Williamson <awilliam@redhat.com>,
        dri-devel <dri-devel@lists.freedesktop.org>,
        LKML <linux-kernel@vger.kernel.org>
Content-Type: text/plain; charset="UTF-8"
Sender: linux-kernel-owner@vger.kernel.org
Precedence: bulk
List-ID: <linux-kernel.vger.kernel.org>
X-Mailing-List: linux-kernel@vger.kernel.org

On 23 November 2017 at 11:17, Laura Abbott <labbott@redhat.com> wrote:
> Hi,
>
> Fedora QA testing reported a panic when booting up VMs
> using qmeu vga drivers
> (https://paste.fedoraproject.org/paste/498yRWTCJv2LKIrmj4EliQ)
>
> [   30.108507] ------------[ cut here ]------------
> [   30.108920] kernel BUG at ./include/linux/gfp.h:408!
> [   30.109356] invalid opcode: 0000 [#1] SMP
> [   30.109700] Modules linked in: fuse nf_conntrack_netbios_ns
> nf_conntrack_broadcast xt_CT ip6t_rpfilter ip6t_REJECT nf_reject_ipv6
> xt_conntrack devlink ip_set nfnetlink ebtable_nat ebtable_broute bridge
> ip6table_nat nf_conntrack_ipv6 nf_defrag_ipv6 nf_nat_ipv6 ip6table_mangle
> ip6table_raw ip6table_security iptable_nat nf_conntrack_ipv4 nf_defrag_ipv4
> nf_nat_ipv4 nf_nat nf_conntrack libcrc32c iptable_mangle iptable_raw
> iptable_security ebtable_filter ebtables ip6table_filter ip6_tables
> snd_hda_codec_generic kvm_intel kvm snd_hda_intel snd_hda_codec irqbypass
> ppdev snd_hda_core snd_hwdep snd_seq snd_seq_device snd_pcm bochs_drm ttm
> joydev drm_kms_helper virtio_balloon snd_timer snd parport_pc drm soundcore
> parport i2c_piix4 nls_utf8 isofs squashfs zstd_decompress xxhash 8021q garp
> mrp stp llc virtio_net
> [   30.115605]  virtio_console virtio_scsi crct10dif_pclmul crc32_pclmul
> crc32c_intel ghash_clmulni_intel serio_raw virtio_pci virtio_ring virtio
> ata_generic pata_acpi qemu_fw_cfg sunrpc scsi_transport_iscsi loop
> [   30.117425] CPU: 0 PID: 1347 Comm: gnome-shell Not tainted
> 4.15.0-0.rc0.git6.1.fc28.x86_64 #1
> [   30.118141] Hardware name: QEMU Standard PC (i440FX + PIIX, 1996), BIOS
> 1.10.2-2.fc27 04/01/2014
> [   30.118866] task: ffff923a77e03380 task.stack: ffffa78182228000
> [   30.119366] RIP: 0010:__alloc_pages_nodemask+0x35e/0x430
> [   30.119810] RSP: 0000:ffffa7818222bba8 EFLAGS: 00010202
> [   30.120250] RAX: 0000000000000001 RBX: 00000000014382c6 RCX:
> 0000000000000006
> [   30.120840] RDX: 0000000000000000 RSI: 0000000000000009 RDI:
> 0000000000000000
> [   30.121443] RBP: ffff923a760d6000 R08: 0000000000000000 R09:
> 0000000000000006
> [   30.122039] R10: 0000000000000040 R11: 0000000000000300 R12:
> ffff923a729273c0
> [   30.122629] R13: 0000000000000000 R14: 0000000000000000 R15:
> ffff923a7483d400
> [   30.123223] FS:  00007fe48da7dac0(0000) GS:ffff923a7cc00000(0000)
> knlGS:0000000000000000
> [   30.123896] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
> [   30.124373] CR2: 00007fe457b73000 CR3: 0000000078313000 CR4:
> 00000000000006f0
> [   30.124968] Call Trace:
> [   30.125186]  ttm_pool_populate+0x19b/0x400 [ttm]
> [   30.125578]  ttm_bo_vm_fault+0x325/0x570 [ttm]
> [   30.125964]  __do_fault+0x19/0x11e
> [   30.126255]  __handle_mm_fault+0xcd3/0x1260
> [   30.126609]  handle_mm_fault+0x14c/0x310
> [   30.126947]  __do_page_fault+0x28c/0x530
> [   30.127282]  do_page_fault+0x32/0x270
> [   30.127593]  async_page_fault+0x22/0x30
> [   30.127922] RIP: 0033:0x7fe48aae39a8
> [   30.128225] RSP: 002b:00007ffc21c4d928 EFLAGS: 00010206
> [   30.128664] RAX: 00007fe457b73000 RBX: 000055cd4c1041a0 RCX:
> 00007fe457b73040
> [   30.129259] RDX: 0000000000300000 RSI: 0000000000000000 RDI:
> 00007fe457b73000
> [   30.129855] RBP: 0000000000000300 R08: 000000000000000c R09:
> 0000000100000000
> [   30.130457] R10: 0000000000000001 R11: 0000000000000246 R12:
> 000055cd4c1041a0
> [   30.131054] R13: 000055cd4bdfe990 R14: 000055cd4c104110 R15:
> 0000000000000400
> [   30.131648] Code: 11 01 00 0f 84 a9 00 00 00 65 ff 0d 6d cc dd 44 e9 0f
> ff ff ff 40 80 cd 80 e9 99 fe ff ff 48 89 c7 e8 e7 f6 01 00 e9 b7 fe ff ff
> <0f> 0b 0f ff e9 40 fd ff ff 65 48 8b 04 25 80 d5 00 00 8b 40 4c
> [   30.133245] RIP: __alloc_pages_nodemask+0x35e/0x430 RSP: ffffa7818222bba8
> [   30.133836] ---[ end trace d4f1deb60784f40a ]---
>
> This is based off of Linus' master branch at
> c8a0739b185d11d6e2ca7ad9f5835841d1cfc765
> Configs are at
> https://git.kernel.org/pub/scm/linux/kernel/git/jwboyer/fedora.git/commit/?h=rawhide&id=0be14662c54f49b4e640868b9d67df18d39edff0
>

Looks like a TTM regression due to:

0284f1ead87463bc17cf5e81a24fc65c052486f3
drm/ttm: add transparent huge page support for cached allocations v2

If the driver requests dma32 pages, we can end up trying to alloc huge
dma32 pages which triggers the oops. The bochs driver always requests
dma32 here.

I'll send a rough patch once I boot it.

Dave.
EOF

cat >out.gold <<"EOF"

From:       airlied@gmail.com
To:         labbott@redhat.com
To:         deathsimple@vodafone.de
CC:         christian.koenig@amd.com
CC:         alexander.deucher@amd.com
CC:         awilliam@redhat.com
CC:         dri-devel@lists.freedesktop.org
CC:         linux-kernel@vger.kernel.org
Subject:    Re: Regression in TTM driver w/Linus' master
Date:       Wed, 22 Nov 2017 17:58:30 PST (-0800)
Message-ID: <CAPM=9ty2tS3KYTpH3Ci=VjoZoto4mkEtBnGhJ2QyR===0xHfZg@mail.gmail.com>

On 23 November 2017 at 11:17, Laura Abbott <labbott@redhat.com> wrote:
> Hi,
>
> Fedora QA testing reported a panic when booting up VMs
> using qmeu vga drivers
> (https://paste.fedoraproject.org/paste/498yRWTCJv2LKIrmj4EliQ)
>
> [   30.108507] ------------[ cut here ]------------
> [   30.108920] kernel BUG at ./include/linux/gfp.h:408!
> [   30.109356] invalid opcode: 0000 [#1] SMP
> [   30.109700] Modules linked in: fuse nf_conntrack_netbios_ns
> nf_conntrack_broadcast xt_CT ip6t_rpfilter ip6t_REJECT nf_reject_ipv6
> xt_conntrack devlink ip_set nfnetlink ebtable_nat ebtable_broute bridge
> ip6table_nat nf_conntrack_ipv6 nf_defrag_ipv6 nf_nat_ipv6 ip6table_mangle
> ip6table_raw ip6table_security iptable_nat nf_conntrack_ipv4 nf_defrag_ipv4
> nf_nat_ipv4 nf_nat nf_conntrack libcrc32c iptable_mangle iptable_raw
> iptable_security ebtable_filter ebtables ip6table_filter ip6_tables
> snd_hda_codec_generic kvm_intel kvm snd_hda_intel snd_hda_codec irqbypass
> ppdev snd_hda_core snd_hwdep snd_seq snd_seq_device snd_pcm bochs_drm ttm
> joydev drm_kms_helper virtio_balloon snd_timer snd parport_pc drm soundcore
> parport i2c_piix4 nls_utf8 isofs squashfs zstd_decompress xxhash 8021q garp
> mrp stp llc virtio_net
> [   30.115605]  virtio_console virtio_scsi crct10dif_pclmul crc32_pclmul
> crc32c_intel ghash_clmulni_intel serio_raw virtio_pci virtio_ring virtio
> ata_generic pata_acpi qemu_fw_cfg sunrpc scsi_transport_iscsi loop
> [   30.117425] CPU: 0 PID: 1347 Comm: gnome-shell Not tainted
> 4.15.0-0.rc0.git6.1.fc28.x86_64 #1
> [   30.118141] Hardware name: QEMU Standard PC (i440FX + PIIX, 1996), BIOS
> 1.10.2-2.fc27 04/01/2014
> [   30.118866] task: ffff923a77e03380 task.stack: ffffa78182228000
> [   30.119366] RIP: 0010:__alloc_pages_nodemask+0x35e/0x430
> [   30.119810] RSP: 0000:ffffa7818222bba8 EFLAGS: 00010202
> [   30.120250] RAX: 0000000000000001 RBX: 00000000014382c6 RCX:
> 0000000000000006
> [   30.120840] RDX: 0000000000000000 RSI: 0000000000000009 RDI:
> 0000000000000000
> [   30.121443] RBP: ffff923a760d6000 R08: 0000000000000000 R09:
> 0000000000000006
> [   30.122039] R10: 0000000000000040 R11: 0000000000000300 R12:
> ffff923a729273c0
> [   30.122629] R13: 0000000000000000 R14: 0000000000000000 R15:
> ffff923a7483d400
> [   30.123223] FS:  00007fe48da7dac0(0000) GS:ffff923a7cc00000(0000)
> knlGS:0000000000000000
> [   30.123896] CS:  0010 DS: 0000 ES: 0000 CR0: 0000000080050033
> [   30.124373] CR2: 00007fe457b73000 CR3: 0000000078313000 CR4:
> 00000000000006f0
> [   30.124968] Call Trace:
> [   30.125186]  ttm_pool_populate+0x19b/0x400 [ttm]
> [   30.125578]  ttm_bo_vm_fault+0x325/0x570 [ttm]
> [   30.125964]  __do_fault+0x19/0x11e
> [   30.126255]  __handle_mm_fault+0xcd3/0x1260
> [   30.126609]  handle_mm_fault+0x14c/0x310
> [   30.126947]  __do_page_fault+0x28c/0x530
> [   30.127282]  do_page_fault+0x32/0x270
> [   30.127593]  async_page_fault+0x22/0x30
> [   30.127922] RIP: 0033:0x7fe48aae39a8
> [   30.128225] RSP: 002b:00007ffc21c4d928 EFLAGS: 00010206
> [   30.128664] RAX: 00007fe457b73000 RBX: 000055cd4c1041a0 RCX:
> 00007fe457b73040
> [   30.129259] RDX: 0000000000300000 RSI: 0000000000000000 RDI:
> 00007fe457b73000
> [   30.129855] RBP: 0000000000000300 R08: 000000000000000c R09:
> 0000000100000000
> [   30.130457] R10: 0000000000000001 R11: 0000000000000246 R12:
> 000055cd4c1041a0
> [   30.131054] R13: 000055cd4bdfe990 R14: 000055cd4c104110 R15:
> 0000000000000400
> [   30.131648] Code: 11 01 00 0f 84 a9 00 00 00 65 ff 0d 6d cc dd 44 e9 0f
> ff ff ff 40 80 cd 80 e9 99 fe ff ff 48 89 c7 e8 e7 f6 01 00 e9 b7 fe ff ff
> <0f> 0b 0f ff e9 40 fd ff ff 65 48 8b 04 25 80 d5 00 00 8b 40 4c
> [   30.133245] RIP: __alloc_pages_nodemask+0x35e/0x430 RSP: ffffa7818222bba8
> [   30.133836] ---[ end trace d4f1deb60784f40a ]---
>
> This is based off of Linus' master branch at
> c8a0739b185d11d6e2ca7ad9f5835841d1cfc765
> Configs are at
> https://git.kernel.org/pub/scm/linux/kernel/git/jwboyer/fedora.git/commit/?h=rawhide&id=0be14662c54f49b4e640868b9d67df18d39edff0
>

Looks like a TTM regression due to:

0284f1ead87463bc17cf5e81a24fc65c052486f3
drm/ttm: add transparent huge page support for cached allocations v2

If the driver requests dma32 pages, we can end up trying to alloc huge
dma32 pages which triggers the oops. The bochs driver always requests
dma32 here.

I'll send a rough patch once I boot it.

Dave.
EOF

#include "harness_end.bash"
