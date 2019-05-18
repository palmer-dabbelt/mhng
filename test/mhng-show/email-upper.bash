#include "harness_start.bash"

ARGS="inbox 1 --nowrap"

export TZ="America/Los_Angeles"
export TERM="xterm"
export COLUMNS="90"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (2, 1, "inbox", 0, "test", 1, "from@example.com", "to@example.com");
EOF

cat >$MHNG_MAILRC <<"EOF"
alias alistair  Alistair Francis <Alistair.Francis@wdc.com>
EOF

cat >$MHNG_MAILDIR/mail/inbox/2 <<"EOF"
Delivered-To: palmer@dabbelt.com
Received: by 2002:ac8:8c3:0:0:0:0:0 with SMTP id y3csp2514008qth;
        Fri, 17 May 2019 15:12:48 -0700 (PDT)
X-Received: by 2002:a19:81c7:: with SMTP id c190mr16091003lfd.55.1558131168179;
        Fri, 17 May 2019 15:12:48 -0700 (PDT)
ARC-Seal: i=2; a=rsa-sha256; t=1558131168; cv=pass;
        d=google.com; s=arc-20160816;
        b=n0OumbQ6b6dF93pJ+zcgJ2tXMEbsckTi3su+CzNCiYJUTBbspkJpAsN+ijjDlAnkoC
         oDSQYK2xmJeK7bU4mlSMbFKdzjctgRXnxavCM12NqBAO7XJHTcwb/BVD6RDc5W8HYy8X
         yvORI7myrIc0g4XlU+5rXBnV6wvbcLghrNS9tyfDNfm+BVbkf/fu9qvPlkjgRAZ5bmlQ
         Z/86eFyRRkr7AGzquX2jUS/hzbgYH4KbTPBPdLV3zcZcr1aSv2D7VabtihXO1IcxVPRA
         reZeY/BwBvY2h+fZc/Pab7pCRi5Huaq7TkaRDe+DLuE5UWAN/hqZBb6FqwAYKaJxXi7M
         8hDA==
ARC-Message-Signature: i=2; a=rsa-sha256; c=relaxed/relaxed; d=google.com; s=arc-20160816;
        h=content-transfer-encoding:mime-version:references:in-reply-to
         :message-id:date:subject:cc:to:from:ironport-sdr:ironport-sdr
         :dkim-signature:delivered-to;
        bh=KvuhrySpeb2Eavr68cqF8hfldtdGZn4QPq1qI2AcMG4=;
        b=oAT1EmpNKPUytVHr1bY0zc8scaud6EWsiOhXhR4L7BrKuFp220IVgL/DFAeIsH9/pk
         8OUkkKhMyBFz9qXiAUY+eaLvFOdEO8gpUOmANjYIiAUHKX9FsCK3r1CEzVq/UE3irTfP
         NhVoLI1HXanslubVZ+WwuQ+3DoIVeUk5T0mtmOjvSO0eUYa0V0QY/n20ccpcvVgW0dIM
         /I5Razyo0oj7rvOTNq0VPulUY1NpEi21dgG/Om5pcsFwTGjpeXdbJeUimWPwh6Nz5Lmw
         zzh6U/KO0qjRSX3RnAdsuNoL2qb/4azbgoCaqWmUehu/hZVUJXvc0GgwVHlYMZRQacGU
         pqsw==
ARC-Authentication-Results: i=2; mx.google.com;
       dkim=pass header.i=@wdc.com header.s=dkim.wdc.com header.b=BslBoJmI;
       arc=pass (i=1 spf=pass spfdomain=wdc.com dkim=pass dkdomain=wdc.com dmarc=pass fromdomain=wdc.com);
       spf=pass (google.com: domain of palmer+caf_=palmer=dabbelt.com@sifive.com designates 209.85.220.41 as permitted sender) smtp.mailfrom="palmer+caf_=palmer=dabbelt.com@sifive.com";
       dmarc=pass (p=NONE sp=NONE dis=NONE) header.from=wdc.com
Return-Path: <palmer+caf_=palmer=dabbelt.com@sifive.com>
Received: from mail-sor-f41.google.com (mail-sor-f41.google.com. [209.85.220.41])
        by mx.google.com with SMTPS id j11sor6618747ljh.42.2019.05.17.15.12.48
        for <palmer@dabbelt.com>
        (Google Transport Security);
        Fri, 17 May 2019 15:12:48 -0700 (PDT)
Received-SPF: pass (google.com: domain of palmer+caf_=palmer=dabbelt.com@sifive.com designates 209.85.220.41 as permitted sender) client-ip=209.85.220.41;
Authentication-Results: mx.google.com;
       dkim=pass header.i=@wdc.com header.s=dkim.wdc.com header.b=BslBoJmI;
       arc=pass (i=1 spf=pass spfdomain=wdc.com dkim=pass dkdomain=wdc.com dmarc=pass fromdomain=wdc.com);
       spf=pass (google.com: domain of palmer+caf_=palmer=dabbelt.com@sifive.com designates 209.85.220.41 as permitted sender) smtp.mailfrom="palmer+caf_=palmer=dabbelt.com@sifive.com";
       dmarc=pass (p=NONE sp=NONE dis=NONE) header.from=wdc.com
X-Google-DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed;
        d=1e100.net; s=20161025;
        h=x-gm-message-state:delivered-to:dkim-signature:ironport-sdr
         :ironport-sdr:from:to:cc:subject:date:message-id:in-reply-to
         :references:mime-version:content-transfer-encoding;
        bh=KvuhrySpeb2Eavr68cqF8hfldtdGZn4QPq1qI2AcMG4=;
        b=dRQHfPQUf6sO3ngRtp4qzbJxNL+we2vKy0uC3FVFfq0xHbL+soV0EenM3EP6tOcehJ
         e6dQReWzg3vrxBCSmeRzMWYTiexpHSL9PmDGansBjZPN0jXXXitQ1zyfyAsMNanVcti7
         ZruAYrL4j5hX2mZdHFhArVHI7M7BlXNLl2AWPzK7JqNKqSESxCNi/Irmc6zOVNjHKCl7
         0g7I2gYVoGCGsGvWvV3H/LbviQZUOdNMt019Yqp5d1hTIS6bpFPiOvv/3YJSuwu8zg6+
         ObYSNhySe/PyfGwAWrg60fZjh0Ee+GucS3QNKCUrvmLbj3xCrHrmb9A16DoOiABJL7vi
         Qadg==
X-Gm-Message-State: APjAAAWnT99rYDgOg/qbng9Yec5Dsfke/USmEIQ/SQdlffpoi7dfCCYM
	z7vALfF5u555q6/R5NGLG1RT1h+JJsoVWZYM9wpZ/ucyqRN+rhxynA==
X-Received: by 2002:a2e:968c:: with SMTP id q12mr21171678lji.195.1558131167812;
        Fri, 17 May 2019 15:12:47 -0700 (PDT)
X-Forwarded-To: palmer@dabbelt.com
X-Forwarded-For: palmer@sifive.com palmer@dabbelt.com
Delivered-To: palmer@sifive.com
Received: by 2002:ab3:5a81:0:0:0:0:0 with SMTP id l1csp2067079lte;
        Fri, 17 May 2019 15:12:45 -0700 (PDT)
X-Google-Smtp-Source: APXvYqxcgkfTWqLfNOzD4Sd3Kt/T0f7tGvI0MYr0cEc7IAY1afqZdOGvIImI+jFFAgqw5DGb5lNG
X-Received: by 2002:a65:6402:: with SMTP id a2mr14741993pgv.438.1558131165717;
        Fri, 17 May 2019 15:12:45 -0700 (PDT)
ARC-Seal: i=1; a=rsa-sha256; t=1558131165; cv=none;
        d=google.com; s=arc-20160816;
        b=RoyjkYuUSxntP0Qy3gsLenBfxKydfBn1+KqCjh+avplxsUXcTo2vrbxRhuibrLLSMb
         A558PzpnhnY2RuhqkDfFHBrU0gVDgmM53+YG+gFzwjk48jxoHYsX3xDhVNucZ6SuTSGD
         pzWAsZiwUqL4uMcOQKXaIfC6Fy1w+d8LBNhfkTjFNM9ONpU2OVvY1169q/M26YtkNRrO
         AOydnGJxg9H+zVuWXBonHifrPUYTYQToez+w5348AqWU0Y7eQAlDbeXHhARNTKpCL+3+
         VMqMTVIlH2ejIWDVz2v8LyewjJZ/YZ+j2CmHW+Pqt2qPZsLrNhFhMydKNt69CUFb3cc2
         rMUA==
ARC-Message-Signature: i=1; a=rsa-sha256; c=relaxed/relaxed; d=google.com; s=arc-20160816;
        h=content-transfer-encoding:mime-version:references:in-reply-to
         :message-id:date:subject:cc:to:from:ironport-sdr:ironport-sdr
         :dkim-signature;
        bh=KvuhrySpeb2Eavr68cqF8hfldtdGZn4QPq1qI2AcMG4=;
        b=wPE0IEshKRS1McuEOn4kUDhcR23yvGh3bu/HAw9wvLqBuMWTaptTCmvb6aI9Av4vZC
         NCf0buhCscYj+J8GRm0AP2Hwh8JB5eDL0WvZV5sTaHTddNlb65SpoNKfkwppKWNlvPxo
         7H77xq4/sa8JIpOelMjdAJVP9OQ/7KsLIhjnEU19rWTGFsGf8JbzNWxXoKzII0IEgR6X
         6yqD+FpnBtgo/GVZMeygnexxa1X4vwza8nExHz75lmOzxPnb4OmdtPURQpmET1w7Hn8f
         +imSC1ezyg950rU1xfBQosKktAwkG/Elv3lRi9+SHqcCl0lQI/sih1YzQzYCEiP3tXUI
         k6pg==
ARC-Authentication-Results: i=1; mx.google.com;
       dkim=pass header.i=@wdc.com header.s=dkim.wdc.com header.b=BslBoJmI;
       spf=pass (google.com: domain of prvs=033a92d29=alistair.francis@wdc.com designates 216.71.153.141 as permitted sender) smtp.mailfrom="prvs=033a92d29=alistair.francis@wdc.com";
       dmarc=pass (p=NONE sp=NONE dis=NONE) header.from=wdc.com
Return-Path: <prvs=033a92d29=alistair.francis@wdc.com>
Received: from esa3.hgst.iphmx.com (esa3.hgst.iphmx.com. [216.71.153.141])
        by mx.google.com with ESMTPS id u68si9615370pgc.72.2019.05.17.15.12.45
        for <palmer@sifive.com>
        (version=TLS1_2 cipher=ECDHE-RSA-AES128-GCM-SHA256 bits=128/128);
        Fri, 17 May 2019 15:12:45 -0700 (PDT)
Received-SPF: pass (google.com: domain of prvs=033a92d29=alistair.francis@wdc.com designates 216.71.153.141 as permitted sender) client-ip=216.71.153.141;
DKIM-Signature: v=1; a=rsa-sha256; c=simple/simple;
  d=wdc.com; i=@wdc.com; q=dns/txt; s=dkim.wdc.com;
  t=1558131166; x=1589667166;
  h=from:to:cc:subject:date:message-id:in-reply-to:
   references:mime-version:content-transfer-encoding;
  bh=1P5qQVKzrpTrWYHCy5Y+UMpW1pxkR7dCHPH3WsXz+P8=;
  b=BslBoJmIX2lGnkIBxI20jaBjSsvUbIGvGiGPngGD3Payn1r0wWecuvtJ
   /GH6o32EwD+ByaXhF0FsbRbQg+OydWPODBeqp/eF+8XrDlcNUJEpUl3w9
   5lJoeHCD/wSlfBAq39EfUF+TafdIbi3V4r/GSQo3y3UTre/lfEJ6tvY8F
   PLkM4z+PBgAuWWIqhMVoUQY98u6Nle55ZOJvibNw8Zo0dWd+N/SrhMCSn
   +6zJb2fy0WOMqPAEWU7HiiALaMwnm8lS/nMapiwyfM542J3tUO4v07cw6
   TmNlQ2tD+Dbh8wGSc8wkur4cYYFvPqWGFZF8tNpWNrVsq7oofdDdejOwu
   Q==;
X-IronPort-AV: E=Sophos;i="5.60,481,1549900800"; 
   d="scan'208";a="113491233"
Received: from h199-255-45-15.hgst.com (HELO uls-op-cesaep02.wdc.com) ([199.255.45.15])
  by ob1.hgst.iphmx.com with ESMTP; 18 May 2019 06:12:46 +0800
IronPort-SDR: 5cUXKLzDgNl9Cgq+SlmyEJlZQSJRRrMm1gdDhn/VNuOsxcJ73XWmb5MquRFTZv9uCzqYkqhSth
 EoEm/5m6GdS+gb5GpTPMigVlmavF6kMkjNa9Pi7Y3whG19Cq0Djnu4TjBngZ6pDhNR8ZRkmkw3
 GfFvK03HxApQzaufDKksOs6SLod4+N3dQ/q30LglPTzQW+VyZYbgSwfqeuh6gTQvTuZwp+/AeZ
 45cLp9OQ+Fn7K0WDs0q29ZjxmipEB7GOgI/bkhPFgET1rw6MWzApZvZz6hZvq8sMdqmK4K9ZzW
 zIwXuFwI4hw9CMIBvz+LjfP2
Received: from uls-op-cesaip01.wdc.com ([10.248.3.36])
  by uls-op-cesaep02.wdc.com with ESMTP; 17 May 2019 14:50:36 -0700
IronPort-SDR: iO/Mcn4Z0aOYIqHXKCpsljROJ4JsxDQNKRw4JTLJAkeNoSdzWk7g7n/PITcgsHDFI24DBlX2Gz
 vMbJy4n+Efy3FRKEN71Xw0cQmz9Hh49SjrRJktPIDYuDqJN72iRsz36Axt66tuXHns6fXUpCvt
 ZVyAGoiyZaG1kPvAYZmFOKQu3jRaVAU1tlclhc5XTMRXw/4QVv62wiRvAEznIUG20K+FxudiZ0
 JD7HGjD/Q4XpfC4f+Fgz5ClpvzZ9JGUUpVJH/qFO4RLItuwZzmhHzAcKltDlH1JUFpY9yzApuK
 7WY=
Received: from risc6-mainframe.sdcorp.global.sandisk.com (HELO risc6-mainframe.int.fusionio.com) ([10.196.157.140])
  by uls-op-cesaip01.wdc.com with ESMTP; 17 May 2019 15:12:45 -0700
From: Alistair Francis <alistair.francis@wdc.com>
To: qemu-devel@nongnu.org,
	qemu-riscv@nongnu.org
Cc: palmer@sifive.com,
	alistair.francis@wdc.com,
	alistair23@gmail.com
Subject: [PATCH v1 1/4] target/riscv: Fix PMP range boundary address bug
Date: Fri, 17 May 2019 15:10:58 -0700
Message-Id: <bf4b3ebaaf429e6df494b0b6c06834910ae727f7.1558131003.git.alistair.francis@wdc.com>
X-Mailer: git-send-email 2.21.0
In-Reply-To: <cover.1558131003.git.alistair.francis@wdc.com>
References: <cover.1558131003.git.alistair.francis@wdc.com>
MIME-Version: 1.0
Content-Transfer-Encoding: 8bit

From: Dayeol Lee <dayeol@berkeley.edu>

A wrong address is passed to `pmp_is_in_range` while checking if a
memory access is within a PMP range.
Since the ending address of the pmp range (i.e., pmp_state.addr[i].ea)
is set to the last address in the range (i.e., pmp base + pmp size - 1),
memory accesses containg the last address in the range will always fail.

For example, assume that a PMP range is 4KB from 0x87654000 such that
the last address within the range is 0x87654fff.
1-byte access to 0x87654fff should be considered to be fully inside the
PMP range.
However the access now fails and complains partial inclusion because
pmp_is_in_range(env, i, addr + size) returns 0 whereas
pmp_is_in_range(env, i, addr) returns 1.

Signed-off-by: Dayeol Lee <dayeol@berkeley.edu>
Reviewed-by: Alistair Francis <alistair.francis@wdc.com>
Reviewed-by: Michael Clark <mjc@sifive.com>
Signed-off-by: Alistair Francis <alistair.francis@wdc.com>
---
 target/riscv/pmp.c | 2 +-
 1 file changed, 1 insertion(+), 1 deletion(-)

diff --git a/target/riscv/pmp.c b/target/riscv/pmp.c
index b11c4ae22f..a2fcc90d73 100644
--- a/target/riscv/pmp.c
+++ b/target/riscv/pmp.c
@@ -246,7 +246,7 @@ bool pmp_hart_has_privs(CPURISCVState *env, target_ulong addr,
          from low to high */
     for (i = 0; i < MAX_RISCV_PMPS; i++) {
         s = pmp_is_in_range(env, i, addr);
-        e = pmp_is_in_range(env, i, addr + size);
+        e = pmp_is_in_range(env, i, addr + size - 1);
 
         /* partially inside */
         if ((s + e) == 1) {
-- 
2.21.0
EOF

cat >out.gold <<"EOF"
From:       Alistair Francis <Alistair.Francis@wdc.com>
To:         qemu-devel@nongnu.org
To:         qemu-riscv@nongnu.org
CC:         palmer@sifive.com
CC:         Alistair Francis <Alistair.Francis@wdc.com>
CC:         alistair23@gmail.com
Subject:    [PATCH v1 1/4] target/riscv: Fix PMP range boundary address bug
Date:       Fri, 17 May 2019 15:10:58 PDT (-0700)
Message-ID: <bf4b3ebaaf429e6df494b0b6c06834910ae727f7.1558131003.git.alistair.francis@wdc.com>

From: Dayeol Lee <dayeol@berkeley.edu>

A wrong address is passed to `pmp_is_in_range` while checking if a
memory access is within a PMP range.
Since the ending address of the pmp range (i.e., pmp_state.addr[i].ea)
is set to the last address in the range (i.e., pmp base + pmp size - 1),
memory accesses containg the last address in the range will always fail.

For example, assume that a PMP range is 4KB from 0x87654000 such that
the last address within the range is 0x87654fff.
1-byte access to 0x87654fff should be considered to be fully inside the
PMP range.
However the access now fails and complains partial inclusion because
pmp_is_in_range(env, i, addr + size) returns 0 whereas
pmp_is_in_range(env, i, addr) returns 1.

Signed-off-by: Dayeol Lee <dayeol@berkeley.edu>
Reviewed-by: Alistair Francis <alistair.francis@wdc.com>
Reviewed-by: Michael Clark <mjc@sifive.com>
Signed-off-by: Alistair Francis <alistair.francis@wdc.com>
---
 target/riscv/pmp.c | 2 +-
 1 file changed, 1 insertion(+), 1 deletion(-)

diff --git a/target/riscv/pmp.c b/target/riscv/pmp.c
index b11c4ae22f..a2fcc90d73 100644
--- a/target/riscv/pmp.c
+++ b/target/riscv/pmp.c
@@ -246,7 +246,7 @@ bool pmp_hart_has_privs(CPURISCVState *env, target_ulong addr,
          from low to high */
     for (i = 0; i < MAX_RISCV_PMPS; i++) {
         s = pmp_is_in_range(env, i, addr);
-        e = pmp_is_in_range(env, i, addr + size);
+        e = pmp_is_in_range(env, i, addr + size - 1);

         /* partially inside */
         if ((s + e) == 1) {
--
2.21.0
EOF

#include "harness_end.bash"
