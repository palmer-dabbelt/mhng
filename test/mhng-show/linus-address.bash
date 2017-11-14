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
Received: by 10.100.166.3 with SMTP id l3csp347070pje;
        Tue, 14 Nov 2017 12:19:00 -0800 (PST)
X-Google-Smtp-Source: AGs4zMY5cM+KWN8vOyrXAtWRMc91SJdE8DDGgs50aWPLVNoQd1n2q8O2gwx05J34qo2bDO5ZoPyw
X-Received: by 10.84.179.3 with SMTP id a3mr13462592plc.25.1510690740238;
        Tue, 14 Nov 2017 12:19:00 -0800 (PST)
ARC-Seal: i=1; a=rsa-sha256; t=1510690740; cv=none;
        d=google.com; s=arc-20160816;
        b=0aM+SPH44Vp9rJF4rKDDekWSmMjMoSkUtuZRJYHx8yC4kf0X+G0QB7z4KBFmNCE8mx
         nXbM+IqpnY0FMTu/8G3lU9iNfdkKLUEQ77mX0/xLFJtbZHP1/f/ZQVaAPXC5/7wZbIFt
         6SgfnxM330XAuC544qo7y+jJhWZXjg57uKBrr8xp6UCYgSHOlWyG9YRSmckaDEhwxoHk
         riAXJf8/E8tuhZ1hOp/e64oD5QLPDshodgM3f1pY3U9Yc7TKp9KA3lN7W0xph3yzeJYd
         BdMZrlcX+2exhBMfx2SqEvn2quiePvExn0y4FN0nUDxrxwgEHqCQI45N85UQbVOWDuz/
         0eeQ==
ARC-Message-Signature: i=1; a=rsa-sha256; c=relaxed/relaxed; d=google.com; s=arc-20160816;
        h=list-id:precedence:sender:cc:to:subject:message-id:date:from
         :references:in-reply-to:mime-version:dkim-signature
         :arc-authentication-results;
        bh=zPTeeGSyZ1tDDwZf9BsShysDKNMYZy16NCxv+Dy5Xb4=;
        b=UWPFZzCgwtwIe+JYhzn5sjMc5cVTnRrip+cq6UD0iVInApHzSnyiylgxw0Y4Kjp+1O
         pVlCtP8ftL6TJcaHxIfG6sv5mgMBm23jtYULGTUKKyQt1bNhadr+4/oNTfRpjGk//O8M
         mpGAfoA+ZwercZcT3/Jxs2GFjZdy/5japWgsQ+SowJj+4aFfNsSNxOHX27/FMMBsDTrX
         QBOiH5wgjT8yix1h5w4B40KPGqGiaFFQFXFHSyXTeEbee1mO93KLz2/DNXkJE7EnWBh5
         FkMhF3RroeiGDgXL3DUag1BBb2guJkeTqdAgHpiq941SWAOgUPjDD49swD5oTvKZla6q
         oCDA==
ARC-Authentication-Results: i=1; mx.google.com;
       dkim=fail header.i=@gmail.com header.s=20161025 header.b=To8sRiwQ;
       spf=pass (google.com: best guess record for domain of linux-kernel-owner@vger.kernel.org designates 209.132.180.67 as permitted sender) smtp.mailfrom=linux-kernel-owner@vger.kernel.org
Return-Path: <linux-kernel-owner@vger.kernel.org>
Received: from vger.kernel.org (vger.kernel.org. [209.132.180.67])
        by mx.google.com with ESMTP id 3si17251514plu.523.2017.11.14.12.19.00
        for <palmer@dabbelt.com>;
        Tue, 14 Nov 2017 12:19:00 -0800 (PST)
Received-SPF: pass (google.com: best guess record for domain of linux-kernel-owner@vger.kernel.org designates 209.132.180.67 as permitted sender) client-ip=209.132.180.67;
Authentication-Results: mx.google.com;
       dkim=fail header.i=@gmail.com header.s=20161025 header.b=To8sRiwQ;
       spf=pass (google.com: best guess record for domain of linux-kernel-owner@vger.kernel.org designates 209.132.180.67 as permitted sender) smtp.mailfrom=linux-kernel-owner@vger.kernel.org
Received: (majordomo@vger.kernel.org) by vger.kernel.org via listexpand
        id S1755553AbdKNUS5 (ORCPT <rfc822;palmer@dabbelt.com>);
        Tue, 14 Nov 2017 15:18:57 -0500
Received: from mail-it0-f68.google.com ([209.85.214.68]:55921 "EHLO
        mail-it0-f68.google.com" rhost-flags-OK-OK-OK-OK) by vger.kernel.org
        with ESMTP id S1753595AbdKNUS4 (ORCPT
        <rfc822;linux-kernel@vger.kernel.org>);
        Tue, 14 Nov 2017 15:18:56 -0500
Received: by mail-it0-f68.google.com with SMTP id l196so15111347itl.4;
        Tue, 14 Nov 2017 12:18:56 -0800 (PST)
DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed;
        d=gmail.com; s=20161025;
        h=mime-version:sender:in-reply-to:references:from:date:message-id
         :subject:to:cc;
        bh=zPTeeGSyZ1tDDwZf9BsShysDKNMYZy16NCxv+Dy5Xb4=;
        b=To8sRiwQZIrXv2juv7l8e7q3gdIyH4rpfzl5CWeRG2vZfzzJxpW82c21YuSGKaYeIR
         xhcOZvok07LYDwziPCdtq3KwQhaFLTdBchB7OzME0Xd2hluPXxXB3JwR/zU1VZ9uKPgj
         ORvPQKDVYvB5EF/685xHO3mvsKjF9wZxqzDiMjcZt63QtUW+BcsSxqjNOECtQKbXFi9S
         QslZIxbmJU+BkeCdKnyhRfiynpTIbL5HUJpvgZxQQ0ta3Zjz8B7LIsd3WMDMliEfeiN7
         jqgZSGSxz7DohvjF84wReaB1V5KMmVry3cnD9Vm0FCQhFxAopmqAb3bm5n/B3wG+/nT7
         4EjA==
X-Google-DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed;
        d=1e100.net; s=20161025;
        h=x-gm-message-state:mime-version:sender:in-reply-to:references:from
         :date:message-id:subject:to:cc;
        bh=zPTeeGSyZ1tDDwZf9BsShysDKNMYZy16NCxv+Dy5Xb4=;
        b=tAAcKC+AKN0205JzSNCnqn2foQXMafbY3MTsIzF4e17v40NcnzZNeT4gWoDcnuIsis
         u015aGrPNemOpNVFPXo1bq44WeRxH671/mjB+oPzzYu4KxjlDp5i+GXRTpAqQAeVKNWL
         qC7lzgCuOQ0YM9SLDIrGpnELPAzKlRVShycNT8XUMn/18MKcwL4jM9qYg5TJVeayFfv0
         u79Dx0ZPAlII9TyqsYyob82lkTeNTydNpIBgsVuHncYY7nLA3KrDNtKqzl9hNA6b0m6V
         4FMPucdHFcl0TjULG4I6Y27Y4XUICSYcrAzaE+jCavD6tn2+Q+GkSudJWjKkMlgUnvWM
         RC0A==
X-Gm-Message-State: AJaThX6SPkiaGARLhuoCyqlFO4xWlzWmBMs/m0P7z4NO+ISro0XXOK1i
        dQd0TSs4lDTmaecF5ybUd5zFwLY6tFyLQCiAg+s=
X-Received: by 10.36.189.205 with SMTP id x196mr10222471ite.152.1510690735621;
 Tue, 14 Nov 2017 12:18:55 -0800 (PST)
MIME-Version: 1.0
Received: by 10.107.141.144 with HTTP; Tue, 14 Nov 2017 12:18:54 -0800 (PST)
In-Reply-To: <CACdnJuvP=0AHGtfGJ5+cT+kHRy3fU4BLjwkvzP0rLO6q5ejAQQ@mail.gmail.com>
References: <150842463163.7923.11081723749106843698.stgit@warthog.procyon.org.uk>
 <14219.1509660259@warthog.procyon.org.uk> <1509660641.3416.24.camel@linux.vnet.ibm.com>
 <20171107230700.GJ22894@wotan.suse.de> <20171108061551.GD7859@linaro.org>
 <20171108194626.GQ22894@wotan.suse.de> <20171109014841.GF7859@linaro.org>
 <1510193857.4484.95.camel@linux.vnet.ibm.com> <20171109044619.GG7859@linaro.org>
 <20171111023240.2398ca55@alans-desktop> <20171113174250.GA22894@wotan.suse.de>
 <20171113210848.4dc344bd@alans-desktop> <454.1510609487@warthog.procyon.org.uk>
 <CA+55aFzrK29hbxoKs3GsvVUY4_uR7aBgZv2eeVyVq7MzfATY3w@mail.gmail.com>
 <1510662098.3711.139.camel@linux.vnet.ibm.com> <CA+55aFzvuvqOfsJ9arzcc1QbTGs+U-TsNmsyem9UAVVQC8YkZQ@mail.gmail.com>
 <CACdnJuvP=0AHGtfGJ5+cT+kHRy3fU4BLjwkvzP0rLO6q5ejAQQ@mail.gmail.com>
From:   Linus Torvalds <torvalds@linux-foundation.org>
Date:   Tue, 14 Nov 2017 12:18:54 -0800
X-Google-Sender-Auth: JRWjD0Kq9mN-MUCg9aZM3G4nAUc
Message-ID: <CA+55aFxeLwgwxh2iJTf6Dz0T_a_TZfTdhBw5TkcSsCmjt2N5pw@mail.gmail.com>
Subject: Re: Firmware signing -- Re: [PATCH 00/27] security, efi: Add kernel lockdown
To:     Matthew Garrett <mjg59@google.com>
Cc:     Mimi Zohar <zohar@linux.vnet.ibm.com>,
        David Howells <dhowells@redhat.com>,
        Alan Cox <gnomes@lxorguk.ukuu.org.uk>,
        "Luis R. Rodriguez" <mcgrof@kernel.org>,
        "AKASHI, Takahiro" <takahiro.akashi@linaro.org>,
        Greg Kroah-Hartman <gregkh@linuxfoundation.org>,
        Jan Blunck <jblunck@infradead.org>,
        Julia Lawall <julia.lawall@lip6.fr>,
        Marcus Meissner <meissner@suse.de>, Gary Lin <GLin@suse.com>,
        LSM List <linux-security-module@vger.kernel.org>,
        linux-efi <linux-efi@vger.kernel.org>,
        Linux Kernel Mailing List <linux-kernel@vger.kernel.org>
Content-Type: text/plain; charset="UTF-8"
Sender: linux-kernel-owner@vger.kernel.org
Precedence: bulk
List-ID: <linux-kernel.vger.kernel.org>
X-Mailing-List: linux-kernel@vger.kernel.org

On Tue, Nov 14, 2017 at 11:58 AM, Matthew Garrett <mjg59@google.com> wrote:
>
> Our ability to determine that userland hasn't been tampered with
> depends on the kernel being trustworthy. If userland can upload
> arbitrary firmware to DMA-capable devices then we can no longer trust
> the kernel. So yes, firmware is special.

You're ignoring the whole "firmware is already signed by the hardware
manufacturer and we don't even have access to it" part.

You're also ignoring the fact that we can't trust firmware _anyway_,
as Alan pointed out.

Seriously. Some of the worst security issues have been with exactly
the fact that we can't trust the hardware to begin with, where
firmware/hardware combinations are not trusted to begin with.

This is all theoretical security masturbation. The _real_ attacks have
been elsewhere.

               Linus
EOF

cat >out.gold <<"EOF"
From:       torvalds@linux-foundation.org
To:         mjg59@google.com
CC:         zohar@linux.vnet.ibm.com
CC:         dhowells@redhat.com
CC:         gnomes@lxorguk.ukuu.org.uk
CC:         mcgrof@kernel.org
CC:         takahiro.akashi@linaro.org
CC:         gregkh@linuxfoundation.org
CC:         jblunck@infradead.org
CC:         julia.lawall@lip6.fr
CC:         meissner@suse.de
CC:         GLin@suse.com
CC:         linux-security-module@vger.kernel.org
CC:         linux-efi@vger.kernel.org
CC:         linux-kernel@vger.kernel.org
Subject:    Re: Firmware signing -- Re: [PATCH 00/27] security, efi: Add kernel lockdown
Date:       Tue, 14 Nov 2017 12:18:54 PST (-0800)
Message-ID: <CA+55aFxeLwgwxh2iJTf6Dz0T_a_TZfTdhBw5TkcSsCmjt2N5pw@mail.gmail.com>

On Tue, Nov 14, 2017 at 11:58 AM, Matthew Garrett <mjg59@google.com> wrote:
>
> Our ability to determine that userland hasn't been tampered with
> depends on the kernel being trustworthy. If userland can upload
> arbitrary firmware to DMA-capable devices then we can no longer trust
> the kernel. So yes, firmware is special.

You're ignoring the whole "firmware is already signed by the hardware
manufacturer and we don't even have access to it" part.

You're also ignoring the fact that we can't trust firmware _anyway_,
as Alan pointed out.

Seriously. Some of the worst security issues have been with exactly
the fact that we can't trust the hardware to begin with, where
firmware/hardware combinations are not trusted to begin with.

This is all theoretical security masturbation. The _real_ attacks have
been elsewhere.

               Linus
EOF

#include "harness_end.bash"
