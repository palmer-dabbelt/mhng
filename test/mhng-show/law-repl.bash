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
Received: by 10.100.149.78 with SMTP id y14csp1152126pja;
        Wed, 6 Sep 2017 11:45:49 -0700 (PDT)
X-Received: by 10.36.16.130 with SMTP id 124mr919576ity.105.1504723549293;
        Wed, 06 Sep 2017 11:45:49 -0700 (PDT)
ARC-Seal: i=2; a=rsa-sha256; t=1504723549; cv=pass;
        d=google.com; s=arc-20160816;
        b=mNIDSJTJGVOAi8sNvnrp+mcjABUraCQaGuDvVb1yQ7PAt5LYjDYUKJHNO/r6ejxKD0
         jMldCVhqFjfJL8nc2NSCGVZhfqAJUEJ3JT2rHTgtlPEUVJRfLm1cU1HI+B48n2hT3eQW
         mjioSNscDLABKUHeYkwjxzRS34VDeTjiT2tQjpVjWxcgTN7jBzLZS3zH4TZ9c9lGhV5T
         j9VhZuCV0oek4o40Nkp79tRYHW7MNJsQ2jv3qYNHyen9gvZIWXCEoeO0QBGLLjE9hZtI
         +efNDg8st8Gnb6NwPlo4AjFuhe8iRglh6u+C/6yKMZ9l1O2yrBJID4QhJliTuLUPRQfU
         yiaQ==
ARC-Message-Signature: i=2; a=rsa-sha256; c=relaxed/relaxed; d=google.com; s=arc-20160816;
        h=content-transfer-encoding:content-language:in-reply-to:mime-version
         :user-agent:date:message-id:from:references:cc:to:subject
         :dmarc-filter:arc-authentication-results:arc-message-signature
         :delivered-to:arc-authentication-results;
        bh=4CsG0L3cw/v3O3wy1WocoolBNGiHX3WhPN4axMX+H84=;
        b=eGOpEOaGJ5RcRGUKoMt8zZgkI1uSEHLimLO+AcDQdIMKFpOD7sieNx2DpCS25S/jJp
         IYqK+sHgsGWvB0QWJCDEqH02BwUdZXfxVgTkhKHtetwrGxq7eUJfmIoWCuIACr/Cu5FC
         JBUb74rQTPLvXAk/ypTdN8JP3Ikmz10eojHBO173ye6EnlgoAUP4XiAxDixH1itYblof
         xhANHtk3nkmWwol8nBavIvqN+tsnaZyGyW54g87Eyj5YAlvw3yk96ux3XiAkRMk2BGKG
         WGboDnMrbFl67fTyp5nsblcdxIP4JkjYb6gLyWi9k4ehp9YKpQk+1gd4E/AGtgUKot2s
         s3vw==
ARC-Authentication-Results: i=2; mx.google.com;
       arc=pass (i=1 spf=pass spfdomain=redhat.com dmarc=pass fromdomain=redhat.com);
       spf=pass (google.com: domain of palmer+caf_=palmer=dabbelt.com@sifive.com designates 209.85.220.41 as permitted sender) smtp.mailfrom=palmer+caf_=palmer=dabbelt.com@sifive.com;
       dmarc=fail (p=NONE sp=NONE dis=NONE arc=pass) header.from=redhat.com
Return-Path: <palmer+caf_=palmer=dabbelt.com@sifive.com>
Received: from mail-sor-f41.google.com (mail-sor-f41.google.com. [209.85.220.41])
        by mx.google.com with SMTPS id 80sor879336ity.2.2017.09.06.11.45.49
        for <palmer@dabbelt.com>
        (Google Transport Security);
        Wed, 06 Sep 2017 11:45:49 -0700 (PDT)
Received-SPF: pass (google.com: domain of palmer+caf_=palmer=dabbelt.com@sifive.com designates 209.85.220.41 as permitted sender) client-ip=209.85.220.41;
Authentication-Results: mx.google.com;
       arc=pass (i=1 spf=pass spfdomain=redhat.com dmarc=pass fromdomain=redhat.com);
       spf=pass (google.com: domain of palmer+caf_=palmer=dabbelt.com@sifive.com designates 209.85.220.41 as permitted sender) smtp.mailfrom=palmer+caf_=palmer=dabbelt.com@sifive.com;
       dmarc=fail (p=NONE sp=NONE dis=NONE arc=pass) header.from=redhat.com
Authentication-Results: ext-mx09.extmail.prod.ext.phx2.redhat.com; spf=fail smtp.mailfrom=law@redhat.com
Authentication-Results: ext-mx09.extmail.prod.ext.phx2.redhat.com; dmarc=none (p=none dis=none) header.from=redhat.com
X-Google-DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed;
        d=1e100.net; s=20161025;
        h=x-original-authentication-results:x-gm-message-state:delivered-to
         :dmarc-filter:subject:to:cc:references:from:message-id:date
         :user-agent:mime-version:in-reply-to:content-language
         :content-transfer-encoding;
        bh=4CsG0L3cw/v3O3wy1WocoolBNGiHX3WhPN4axMX+H84=;
        b=cocfnQz3O2x7nztCdCetszeURpJu8TBOCtorj+g6byiqoD+UqvniWRLp0st/zdfBo1
         Qcv931/lzay1ei1DtlSMNWUoO/npttGANKJ6n+Pfsqmr23T3pAK1CIL1rSNCOgNqqHuV
         mxjNS4vkNB50ebH270phpxk4EmCyAikIdVhsnLjKxRfWlTHo1V488GQU/xc29jGDqq7O
         PS/OI2eqHz/xzgpEKDXVeoqtZiEJzPESGPM56PWAp7mbT3KpznerONrDqrzh8A2cCJlU
         ahe29R+BAYyy0aqWdjNRXPuuSMrJnIFFzI+qAvAzcH4TEbo/D3VEv8wLWNAFZ19Lu1AG
         b/kA==
X-Original-Authentication-Results: mx.google.com;       spf=pass (google.com: domain of law@redhat.com designates 209.132.183.28 as permitted sender) smtp.mailfrom=law@redhat.com;       dmarc=pass (p=NONE sp=NONE dis=NONE) header.from=redhat.com
X-Gm-Message-State: AHPjjUhvsDR5SXc6fjciJU5tJSEdfm7sM5i0/FevLRwktErXf76jhbJK
	bZStVazfWoTgrFXaCxN+GducewpNGKdik4vTtRz7eAJi
X-Received: by 10.36.165.69 with SMTP id w5mr719980iti.40.1504723548890;
        Wed, 06 Sep 2017 11:45:48 -0700 (PDT)
X-Forwarded-To: palmer@dabbelt.com
X-Forwarded-For: palmer@sifive.com palmer@dabbelt.com
Delivered-To: palmer@sifive.com
Received: by 10.36.39.137 with SMTP id g131csp1252614ita;
        Wed, 6 Sep 2017 11:45:47 -0700 (PDT)
X-Google-Smtp-Source: AOwi7QDSJEPiBSRLqC/r4j2rszcyBzbg2NAXS6vFf6r1DVfv0B5mKNKMV25eu6gX1+M3O9xYSJ4Z
X-Received: by 10.55.182.194 with SMTP id g185mr138996qkf.127.1504723547490;
        Wed, 06 Sep 2017 11:45:47 -0700 (PDT)
ARC-Seal: i=1; a=rsa-sha256; t=1504723547; cv=none;
        d=google.com; s=arc-20160816;
        b=q2kMnWeQrLR9FDsOtQMxZ1PCnhxy93n1uDNPyKgAgexNtfdSEOxhBx+Z5ZYsnIqNYp
         VD3IlUmOUsmN8xKfrEX4pfDIb0o4UW/tah9c2hzqogtBUgtSA4hnfF3yM2sWDAKRCgPU
         KRyuN+HaWoQPABg5DsZaFIHQYXLi2YdnLZwr/VLov7bPNmQHiObJ97SvfD1wo5ygsLR1
         y4lm/BaA8LOE/tFc9OYzoIhzECRx1k7timD0lnr+QsT3CpGW+U6NBbDcG/MHilhqv7Eq
         IAFALXaOfKE1pDoy+K+XwiKc98k2Kv04LK1JBXI7zQlU175FePlTRzuRPh7DQRAVygHS
         WIEg==
ARC-Message-Signature: i=1; a=rsa-sha256; c=relaxed/relaxed; d=google.com; s=arc-20160816;
        h=content-transfer-encoding:content-language:in-reply-to:mime-version
         :user-agent:date:message-id:from:references:cc:to:subject
         :dmarc-filter:arc-authentication-results;
        bh=4CsG0L3cw/v3O3wy1WocoolBNGiHX3WhPN4axMX+H84=;
        b=rMqZTwfeXud160mknt/ZgdrGrsTua0A4qQFPaU0o5WEd7jvTKupG/4yP/2qS6H+9Sd
         8YOw42hnoyuFl7OBIc0+Vj5st4djEiTxHPX0GrGz5O9F4D1hk8M/X3TIro4Yl01EhpFJ
         amBMiOYmjrWFaFJVrHlgxeJ/H7kO8Y/Tyd6h0l8ZXzpSqdx8FVfsY5drZRIGL7u4nyhM
         1ZwJ6n2XyfO6Mh4t/bG8DUcX9R1NhBno3xorTCBoW0ZOUnrSqedXKxRXVDAUohRJTwXG
         +GrvxK5DP6pyDJz5Xv9NWV+j45jYrj7cAnyOBrd4ceqMv4/QIyvUjVc/t31yXvaUPbsb
         zLEg==
ARC-Authentication-Results: i=1; mx.google.com;
       spf=pass (google.com: domain of law@redhat.com designates 209.132.183.28 as permitted sender) smtp.mailfrom=law@redhat.com;
       dmarc=pass (p=NONE sp=NONE dis=NONE) header.from=redhat.com
Return-Path: <law@redhat.com>
Received: from mx1.redhat.com (mx1.redhat.com. [209.132.183.28])
        by mx.google.com with ESMTPS id e71si572639qka.139.2017.09.06.11.45.47
        (version=TLS1_2 cipher=ECDHE-RSA-AES128-GCM-SHA256 bits=128/128);
        Wed, 06 Sep 2017 11:45:47 -0700 (PDT)
Received-SPF: pass (google.com: domain of law@redhat.com designates 209.132.183.28 as permitted sender) client-ip=209.132.183.28;
Received: from smtp.corp.redhat.com (int-mx01.intmail.prod.int.phx2.redhat.com [10.5.11.11])
	(using TLSv1.2 with cipher AECDH-AES256-SHA (256/256 bits))
	(No client certificate requested)
	by mx1.redhat.com (Postfix) with ESMTPS id 608F34E4C3;
	Wed,  6 Sep 2017 18:45:46 +0000 (UTC)
DMARC-Filter: OpenDMARC Filter v1.3.2 mx1.redhat.com 608F34E4C3
Received: from localhost.localdomain (ovpn-112-2.rdu2.redhat.com [10.10.112.2])
	by smtp.corp.redhat.com (Postfix) with ESMTP id A2C5960460;
	Wed,  6 Sep 2017 18:45:44 +0000 (UTC)
Subject: Re: Redundant sign-extension instructions on RISC-V
To: Richard Henderson <rth@twiddle.net>, Michael Clark
 <michaeljclark@mac.com>, GCC Development <gcc@gcc.gnu.org>
Cc: Andrew Waterman <andrew@sifive.com>, Palmer Dabbelt <palmer@sifive.com>
References: <25835613-5493-42EB-B4C8-E44BAC9F401E@mac.com>
 <d3d07db5-928a-09d0-d41c-dcdf95c0c5fa@twiddle.net>
 <c2a3bb80-c2f6-f8ca-9c3e-d88652c2deb6@redhat.com>
 <d8638038-46ab-6b87-d972-40198fbd0e33@twiddle.net>
From: Jeff Law <law@redhat.com>
Message-ID: <6e311220-460f-b529-ee76-350118d8050a@redhat.com>
Date: Wed, 6 Sep 2017 12:45:43 -0600
User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101
 Thunderbird/52.2.1
MIME-Version: 1.0
In-Reply-To: <d8638038-46ab-6b87-d972-40198fbd0e33@twiddle.net>
Content-Type: text/plain; charset=utf-8
Content-Language: en-US
Content-Transfer-Encoding: 8bit
X-Scanned-By: MIMEDefang 2.79 on 10.5.11.11
X-Greylist: Sender IP whitelisted, not delayed by milter-greylist-4.5.16 (mx1.redhat.com [10.5.110.38]); Wed, 06 Sep 2017 18:45:46 +0000 (UTC)

On 09/06/2017 11:17 AM, Richard Henderson wrote:
> On 09/06/2017 09:53 AM, Jeff Law wrote:
>>> I think the easiest solution to this is for combine to notice when IOR has
>>> operands with non-zero-bits that do not overlap, convert the operation to ADD.
>>> That allows the final two insns to fold to "addw" and the compiler need do no
>>> further analysis.
>> I thought we had combine support for that.  I don't think it's ever been
>> particularly good though.  With your alpha background you're probably
>> more familiar with it than anyone -- IIRC it fell out of removal of low
>> order bit masking to deal with alignment issues on the Alpha.
>
> Yes, but that would have been within AND to drop unnecessary/redundant masks.
> We probably just need a few lines over in IOR to handle this case with the same
> machinery.
Right.  My point was most of the infrastructure ought to be in place.

>
> Heh...  Just having a browse shows that we currently perform exactly the
> opposite transformation:
>
>       /* If we are adding two things that have no bits in common, convert
>          the addition into an IOR.  This will often be further simplified,
>          for example in cases like ((a & 1) + (a & 2)), which can
>          become a & 3.  */
>
> So managing these conflicting goals might be tricky...
And I wonder how often that happens in practice.

>
>> I wrote some match.pd patterns to do it in gimple as part of a larger
>> problem.  The work as a whole on that larger problem ultimately didn't
>> pan out (generated even worse code than what we have on the trunk).  But
>> it might be possible to resurrect those patterns and see if they are
>> useful independently.  My recollection was I was looking at low order
>> bits only, but the concepts were general enough.
>
> That would be interesting, yes.
So I just dug up the BZ.  59393.  Sadly it was the other way around --
turning a PLUS into BIT_IOR like combine.  So not helpful here.

The PLUS->IOR apparently was still profitable from a codesize
standpoint.  But in general, I'm not sure how to select between the two
forms.  I guess IOR is slightly better because we know precisely what
bits are potentially changed based on the constant.

>
>>>> $ cat rshift.c
>>>> unsigned rs24(unsigned rs1) { return rs1 >> 24; }
>>>> $ cat rshift.s
>>>> rs24:
>>>> 	sllw	a0,a0,24
>>>> 	sext.w	a0,a0	# redundant
>>>> 	ret
>>>
>>> That seems like a missing check somewhere (combine? simplify-rtx? both?) for
>>> SUBREG_PROMOTED_SIGNED_P.  Since Alpha didn't have a 32-bit shift you're in new
>>> territory for this one.
>> Yea.  I'd also expect zero/nonzero bits tracking in combine to catch
>> this.  Shouldn't the sign bit be known to be zero after the shift which
>> makes the extension redundant regardless of the SUBREG_PROMOTED flag?
>
> You're right, this should be irrelevant.  Any anyway combine should be
> constrained by modes, so it should require the SIGN_EXTEND to be present just
> to make the modes match up.  Perhaps this test case is being suppressed because
> of something else, e.g. failure to combine insns when a hard-reg is involved?
It could well be hard register involvement.

jeff
EOF

cat >out.gold <<"EOF"
From:       law@redhat.com
To:         rth@twiddle.net
To:         michaeljclark@mac.com
To:         gcc@gcc.gnu.org
CC:         andrew@sifive.com
CC:         palmer@sifive.com
Subject:    Re: Redundant sign-extension instructions on RISC-V
Date:       Wed, 06 Sep 2017 11:45:43 PDT (-0700)
Message-ID: <6e311220-460f-b529-ee76-350118d8050a@redhat.com>

On 09/06/2017 11:17 AM, Richard Henderson wrote:
> On 09/06/2017 09:53 AM, Jeff Law wrote:
>>> I think the easiest solution to this is for combine to notice when IOR has
>>> operands with non-zero-bits that do not overlap, convert the operation to ADD.
>>> That allows the final two insns to fold to "addw" and the compiler need do no
>>> further analysis.
>> I thought we had combine support for that.  I don't think it's ever been
>> particularly good though.  With your alpha background you're probably
>> more familiar with it than anyone -- IIRC it fell out of removal of low
>> order bit masking to deal with alignment issues on the Alpha.
>
> Yes, but that would have been within AND to drop unnecessary/redundant masks.
> We probably just need a few lines over in IOR to handle this case with the same
> machinery.
Right.  My point was most of the infrastructure ought to be in place.

>
> Heh...  Just having a browse shows that we currently perform exactly the
> opposite transformation:
>
>       /* If we are adding two things that have no bits in common, convert
>          the addition into an IOR.  This will often be further simplified,
>          for example in cases like ((a & 1) + (a & 2)), which can
>          become a & 3.  */
>
> So managing these conflicting goals might be tricky...
And I wonder how often that happens in practice.

>
>> I wrote some match.pd patterns to do it in gimple as part of a larger
>> problem.  The work as a whole on that larger problem ultimately didn't
>> pan out (generated even worse code than what we have on the trunk).  But
>> it might be possible to resurrect those patterns and see if they are
>> useful independently.  My recollection was I was looking at low order
>> bits only, but the concepts were general enough.
>
> That would be interesting, yes.
So I just dug up the BZ.  59393.  Sadly it was the other way around --
turning a PLUS into BIT_IOR like combine.  So not helpful here.

The PLUS->IOR apparently was still profitable from a codesize
standpoint.  But in general, I'm not sure how to select between the two
forms.  I guess IOR is slightly better because we know precisely what
bits are potentially changed based on the constant.

>
>>>> $ cat rshift.c
>>>> unsigned rs24(unsigned rs1) { return rs1 >> 24; }
>>>> $ cat rshift.s
>>>> rs24:
>>>> 	sllw	a0,a0,24
>>>> 	sext.w	a0,a0	# redundant
>>>> 	ret
>>>
>>> That seems like a missing check somewhere (combine? simplify-rtx? both?) for
>>> SUBREG_PROMOTED_SIGNED_P.  Since Alpha didn't have a 32-bit shift you're in new
>>> territory for this one.
>> Yea.  I'd also expect zero/nonzero bits tracking in combine to catch
>> this.  Shouldn't the sign bit be known to be zero after the shift which
>> makes the extension redundant regardless of the SUBREG_PROMOTED flag?
>
> You're right, this should be irrelevant.  Any anyway combine should be
> constrained by modes, so it should require the SIGN_EXTEND to be present just
> to make the modes match up.  Perhaps this test case is being suppressed because
> of something else, e.g. failure to combine insns when a hard-reg is involved?
It could well be hard register involvement.

jeff
EOF

#include "harness_end.bash"
