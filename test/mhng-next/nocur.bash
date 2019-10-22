#include "harness_start.bash"

ARGS="inbox"

export TZ="America/Los_Angeles"
export TERM="xterm"
export COLUMNS="120"

sqlite3 $MHNG_MAILDIR/metadata.sqlite3 <<EOF
INSERT INTO MH__messages (uid, seq, folder, unread, subject, date, fadr, tadr) VALUES (2, 20, "inbox", 0, "test", 1, "from@example.com", "to@example.com");
EOF

cat >$MHNG_MAILDIR/mail/inbox/2 <<"EOF"
Delivered-To: palmer@dabbelt.com
Received: by 10.100.166.3 with SMTP id l3csp2533596pje;
        Thu, 16 Nov 2017 07:33:58 -0800 (PST)
X-Google-Smtp-Source: AGs4zMbukDkLUUkW/90oDVI76EaleTY8Ru7+qtjyYgNGdmuZvrx98OmkgJUmSU6CT0N/e7jR3Dol
X-Received: by 10.99.124.85 with SMTP id l21mr2028315pgn.372.1510846438070;
        Thu, 16 Nov 2017 07:33:58 -0800 (PST)
ARC-Seal: i=1; a=rsa-sha256; t=1510846438; cv=none;
        d=google.com; s=arc-20160816;
        b=cVhsFNyBqEGOjtQil+COeHtHMtr4TK0cBonI9B9dazOegoaaPteqso5cGpJIfUdOi7
         gNrD4+whskhwBuQOu3aAzoXubWgdUO6YuDWDoDFgqWsr/BX9kxoEWUQl90V3Om9BR9y3
         fVc31hwwYfr7iAgTeoo7eCIVwL77VuSetl6pDazyR81uMECbxXpk5MUq7sUBnVdgkrSp
         Pb81gO3zQx2+wKs8SGiKjQG82+iY4TbRhf7nLpTywL37tqXPBc3Yai7PmijegEiTjIqj
         sOOZmJygykPLKKs+e++XSAxmJaKL862b7qvcy1kjygSICtXV0z3dfZFO8ShOzcHXK+YD
         uGiQ==
ARC-Message-Signature: i=1; a=rsa-sha256; c=relaxed/relaxed; d=google.com; s=arc-20160816;
        h=mime-version:content-transfer-encoding:dlp-reaction:dlp-version
         :dlp-product:message-id:date:subject:cc:to:from:delivered-to:sender
         :list-help:list-post:list-archive:list-unsubscribe:list-id
         :precedence:mailing-list:dkim-signature:domainkey-signature
         :arc-authentication-results;
        bh=6a3Epw15vr5/LqNv1XvNbpc/JzdSoaeLELIh2bpWLTA=;
        b=dAMEVpsFkDeadkLrk6IuqrVE42qPVHD+RevjaMa02BeDkT0oqrwVAzDPxr1imiEJsI
         9bkeDY0pdhNvOAkSBiktXw7qhSY5Leb5Rm6JGUdGUJ/snkNd1+sUzNOvVO8vTX8Bud6E
         jFXoU2gZMnrOzECwe76py+XhLGA7KktY3/Nbjj+SZ/FVao/9vL9m+x8iHuvT9kg5x/AO
         qIgcy2ycbTCiRXIL2nTdH0oTNWInl3zCulm6+qwz//r5ZmLiIoe4nthOCkFxhspoUO9S
         5xQkewkt0XLhOGLZ6hE+zJsOL5ZSD/G0QY0/06bSOD4I5DTLyjyK+XaFv+rlf4d0TnzQ
         EBdQ==
ARC-Authentication-Results: i=1; mx.google.com;
       dkim=pass header.i=@gcc.gnu.org header.s=default header.b=jYaMkcYk;
       spf=pass (google.com: domain of gcc-patches-return-467004-palmer=dabbelt.com@gcc.gnu.org designates 209.132.180.131 as permitted sender) smtp.mailfrom=gcc-patches-return-467004-palmer=dabbelt.com@gcc.gnu.org
Return-Path: <gcc-patches-return-467004-palmer=dabbelt.com@gcc.gnu.org>
Received: from sourceware.org (server1.sourceware.org. [209.132.180.131])
        by mx.google.com with ESMTPS id 68si1172515pfx.6.2017.11.16.07.33.57
        for <palmer@dabbelt.com>
        (version=TLS1_2 cipher=ECDHE-RSA-AES128-GCM-SHA256 bits=128/128);
        Thu, 16 Nov 2017 07:33:58 -0800 (PST)
Received-SPF: pass (google.com: domain of gcc-patches-return-467004-palmer=dabbelt.com@gcc.gnu.org designates 209.132.180.131 as permitted sender) client-ip=209.132.180.131;
Authentication-Results: mx.google.com;
       dkim=pass header.i=@gcc.gnu.org header.s=default header.b=jYaMkcYk;
       spf=pass (google.com: domain of gcc-patches-return-467004-palmer=dabbelt.com@gcc.gnu.org designates 209.132.180.131 as permitted sender) smtp.mailfrom=gcc-patches-return-467004-palmer=dabbelt.com@gcc.gnu.org
DomainKey-Signature: a=rsa-sha1; c=nofws; d=gcc.gnu.org; h=list-id
	:list-unsubscribe:list-archive:list-post:list-help:sender:from
	:to:cc:subject:date:message-id:content-type
	:content-transfer-encoding:mime-version; q=dns; s=default; b=Ebx
	MqYh460k7JShWUkLjZPBG3nKir1vTBK0jN5ejPsfUUOGmUjGDFv0nnmESSZdLyui
	YqhU8OUKdhHWc0PtMFFoaEhNXVI2UdA9r0z2ZCZWT+fMBFalzMTbaWnWu6FbyjiQ
	irwvEZMxKKhK6PwqK6829iVtqVflgPC8nenQpdvw=
DKIM-Signature: v=1; a=rsa-sha1; c=relaxed; d=gcc.gnu.org; h=list-id
	:list-unsubscribe:list-archive:list-post:list-help:sender:from
	:to:cc:subject:date:message-id:content-type
	:content-transfer-encoding:mime-version; s=default; bh=k6S5sXnb5
	s5Oa393mXDkPmuaDss=; b=jYaMkcYkFY7GxIJMbQHvlk8JuLaGQXmqyUYKqSN/w
	zJc+4YkUgtq0syimvJ+xM75dbdhI4O7AVkfZjqcf/pj9OYNssVhoI5lH5o2JVVR2
	H/VX8fvVpH6ID3y5tkox72ozM8eAEfn//dGq3hbKGcHe4MP7eDw/ateyQEYih191
	ys=
Received: (qmail 101969 invoked by alias); 16 Nov 2017 15:33:47 -0000
Mailing-List: contact gcc-patches-help@gcc.gnu.org; run by ezmlm
Precedence: bulk
List-Id: <gcc-patches.gcc.gnu.org>
List-Unsubscribe: <mailto:gcc-patches-unsubscribe-palmer=dabbelt.com@gcc.gnu.org>
List-Archive: <http://gcc.gnu.org/ml/gcc-patches/>
List-Post: <mailto:gcc-patches@gcc.gnu.org>
List-Help: <mailto:gcc-patches-help@gcc.gnu.org>
Sender: gcc-patches-owner@gcc.gnu.org
Delivered-To: mailing list gcc-patches@gcc.gnu.org
Received: (qmail 101377 invoked by uid 89); 16 Nov 2017 15:33:46 -0000
Authentication-Results: sourceware.org; auth=none
X-Virus-Found: No
X-Spam-SWARE-Status: No, score=0.6 required=5.0 tests=AWL,BAYES_50,KB_WAM_FROM_NAME_SINGLEWORD,RP_MATCHES_RCVD,SPF_PASS autolearn=no version=3.3.2 spammy=UD:cp-objcp-common.c, cilkplus, cilk-plus, sk:tree-pr
X-HELO: mga09.intel.com
Received: from mga09.intel.com (HELO mga09.intel.com) (134.134.136.24) by sourceware.org (qpsmtpd/0.93/v0.84-503-g423c35a) with ESMTP; Thu, 16 Nov 2017 15:33:44 +0000
Received: from fmsmga005.fm.intel.com ([10.253.24.32])  by orsmga102.jf.intel.com with ESMTP/TLS/DHE-RSA-AES256-GCM-SHA384; 16 Nov 2017 07:33:42 -0800
X-ExtLoop1: 1
Received: from irsmsx105.ger.corp.intel.com ([163.33.3.28])  by fmsmga005.fm.intel.com with ESMTP; 16 Nov 2017 07:33:41 -0800
Received: from irsmsx101.ger.corp.intel.com ([169.254.1.22]) by irsmsx105.ger.corp.intel.com ([163.33.3.28]) with mapi id 14.03.0319.002; Thu, 16 Nov 2017 15:33:40 +0000
From: "Koval, Julia" <julia.koval@intel.com>
To: GCC Patches <gcc-patches@gcc.gnu.org>
CC: "law@redhat.com" <law@redhat.com>
Subject: [patch] remove cilk-plus
Date: Thu, 16 Nov 2017 15:33:40 +0000
Message-ID: <4E89A029A0F8D443B436A5167BA3C53F42AD12C2@IRSMSX101.ger.corp.intel.com>
x-titus-metadata-40: eyJDYXRlZ29yeUxhYmVscyI6IiIsIk1ldGFkYXRhIjp7Im5zIjoiaHR0cDpcL1wvd3d3LnRpdHVzLmNvbVwvbnNcL0ludGVsMyIsImlkIjoiZWYyMjkzYTItNzljZC00ZDAwLTlmOTctOTZlNjcxZTBjNDkxIiwicHJvcHMiOlt7Im4iOiJDVFBDbGFzc2lmaWNhdGlvbiIsInZhbHMiOlt7InZhbHVlIjoiQ1RQX0lDIn1dfV19LCJTdWJqZWN0TGFiZWxzIjpbXSwiVE1DVmVyc2lvbiI6IjE3LjIuNS4xOCIsIlRydXN0ZWRMYWJlbEhhc2giOiJEeDJvQUZ0T1hGbWhcL2YyOFhtUjJOUXRaVXhLXC9NZzdONEk5R2k2ZGZRSm10blV6OFpUR3BjMXhxTTFxQUpYd2sifQ==
x-ctpclassification: CTP_IC
dlp-product: dlpe-windows
dlp-version: 11.0.0.116
dlp-reaction: no-action
Content-Type: text/plain; charset="us-ascii"
Content-Transfer-Encoding: quoted-printable
MIME-Version: 1.0
X-IsSubscribed: yes

// I failed to send patch itself, it is too big even in gzipped form.  What=
 is the right way to send such big patches?

Hi, this patch removes cilkplus. Ok for trunk?

2017-11-16  Julia Koval  <julia.koval@intel.com>
	        Sebastian Peryt  <sebastian.peryt@intel.com>
gcc/
	* Makefile.def (target_modules): Remove libcilkrts.
	* Makefile.in: Ditto.
	* configure: Ditto.
	* configure.ac: Ditto.
	* contrib/gcc_update: Ditto.
	* Makefile.in (cilkplus.def, cilk-builtins.def, c-family/cilk.o,=20
	c-family/c-cilkplus.o, c-family/array-notation-common.o,
	cilk-common.o, cilk.h, cilk-common.c): Remove.
	* builtin-types.def (BT_FN_INT_PTR_PTR_PTR_FTYPE_BT_INT_BT_PTR_BT_PTR
	_BT_PTR): Remove.
	* builtins.c (is_builtin_name): Remove cilkplus condition.
	(BUILT_IN_CILK_DETACH, BUILT_IN_CILK_POP_FRAME): Remove.
	* builtins.def (DEF_CILK_BUILTIN_STUB, DEF_CILKPLUS_BUILTIN,
	cilk-builtins.def, cilkplus.def): Remove.
	* c-family/array-notation-common.c: Delete.
	* c-family/c-cilkplus.c: Ditto.
	* c-family/c-common.c (_Cilk_spawn, _Cilk_sync, _Cilk_for): Remove.
	* c-family/c-common.def (ARRAY_NOTATION_REF): Remove.
	* c-family/c-common.h (RID_CILK_SPAWN, build_array_notation_expr,
	build_array_notation_ref, C_ORT_CILK, c_check_cilk_loop,
	c_validate_cilk_plus_loop, cilkplus_an_parts, cilk_ignorable_spawn_rhs_op,
	cilk_recognize_spawn): Remove.
	* c-family/c-gimplify.c (CILK_SPAWN_STMT): Remove.
	* c-family/c-omp.c: Remove CILK_SIMD check.
	* c-family/c-pragma.c: Ditto.
	* c-family/c-pragma.h: Remove CILK related pragmas.
	* c-family/c-pretty-print.c (c_pretty_printer::postfix_expression): Remove
	ARRAY_NOTATION_REF condition.
	(c_pretty_printer::expression): Ditto.
	* c-family/c.opt (fcilkplus): Remove.
	* c-family/cilk.c: Delete.
	* c/Make-lang.in (c/c-array-notation.o): Remove.
	* c/c-array-notation.c: Delete.
	* c/c-decl.c: Remove cilkplus condition.
	* c/c-parser.c (c_parser_cilk_simd, c_parser_cilk_for,
	c_parser_cilk_verify_simd, c_parser_array_notation,
	c_parser_cilk_clause_vectorlength, c_parser_cilk_grainsize,
	c_parser_cilk_simd_fn_vector_attrs, c_finish_cilk_simd_fn_tokens): Delete.
	(c_parser_declaration_or_fndef): Remove cilkplus condition.
	(c_parser_direct_declarator_inner): Ditto.
	(CILK_SIMD_FN_CLAUSE_MASK): Delete.
	(c_parser_attributes): Remove cilk-plus condition.
	(c_parser_compound_statement): Ditto.
	(c_parser_statement_after_labels): Ditto.
	(c_parser_if_statement): Ditto.
	(c_parser_switch_statement): Ditto.
	(c_parser_while_statement): Ditto.
	(c_parser_do_statement): Ditto.
	(c_parser_for_statement): Ditto.
	(c_parser_unary_expression): Ditto.
	(c_parser_postfix_expression): Ditto.
	(c_parser_postfix_expression_after_primary): Ditto.
	(c_parser_pragma): Ditto.
	(c_parser_omp_clause_name): Ditto.
	(c_parser_omp_all_clauses): Ditto.
	(c_parser_omp_for_loop): Ditto.
	(c_finish_omp_declare_simd): Ditto.
	* c/c-typeck.c (build_array_ref, build_function_call_vec, convert_argument=
s,
	lvalue_p, build_compound_expr, c_finish_return, c_finish_if_stmt,
	c_finish_loop, build_binary_op): Remove cilkplus condition.
	* cif-code.def (CILK_SPAWN): Remove.
	* cilk-builtins.def: Delete.
	* cilk-common.c: Ditto.
	* cilk.h: Ditto.
	* cilkplus.def: Ditto.
	* config/darwin.h (fcilkplus): Delete.
	* cp/Make-lang.in (cp/cp-array-notation.o, cp/cp-cilkplus.o): Delete.
	* cp/call.c (convert_for_arg_passing, build_cxx_call): Remove cilkplus.
	* cp/constexpr.c (potential_constant_expression_1): Ditto.
	* cp/cp-array-notation.c: Delete.
	* cp/cp-cilkplus.c: Ditto.
	* cp/cp-cilkplus.h: Ditto.
	* cp/cp-gimplify.c (cp_gimplify_expr, cp_fold_r, cp_genericize): Remove
	cilkplus condition.
	* cp/cp-objcp-common.c (ARRAY_NOTATION_REF): Delete.
	* cp/cp-tree.h (cilkplus_an_triplet_types_ok_p): Delete.
	* cp/decl.c (grokfndecl, finish_function): Remove cilkplus condition.
	* cp/error.c (dump_decl, dump_expr): Remove ARRAY_NOTATION_REF condition.
	* cp/lambda.c (cp-cilkplus.h): Remove.
	* cp/parser.c (cp_parser_cilk_simd, cp_parser_cilk_for,
	cp_parser_cilk_simd_vectorlength): Delete.
	(cp_debug_parser, cp_parser_ctor_initializer_opt_and_function_body,
	cp_parser_postfix_expression, cp_parser_postfix_open_square_expression,
	cp_parser_statement, cp_parser_jump_statement, cp_parser_direct_declarator,
	cp_parser_late_return_type_opt, cp_parser_gnu_attribute_list,
	cp_parser_omp_clause_name, cp_parser_omp_clause_aligned,
	cp_parser_omp_clause_linear, cp_parser_omp_all_clauses, cp_parser_omp_flus=
h,
	cp_parser_omp_for_cond, cp_parser_omp_for_incr, cp_parser_omp_for_loop_ini=
t,
	cp_parser_omp_for_loop, cp_parser_omp_declare_simd): Remove cilkplus condt=
ions.
	(CILK_SIMD_FN_CLAUSE_MASK, cp_parser_late_parsing_cilk_simd_fn_info,
	cp_parser_cilk_grainsize): Remove.
	(cp_parser_pragma, c_parse_file): Remove cilkplus condition.
	(cp_parser_cilk_simd_vectorlength, cp_parser_cilk_simd_linear,
	cp_parser_cilk_simd_clause_name, cp_parser_cilk_simd_all_clauses,
	cp_parser_cilk_simd, cp_parser_cilk_for): Remove.
	* cp/parser.h (IN_CILK_SIMD_FOR, IN_CILK_SPAWN): Remove.
	* cp/pt.c (tsubst_attribute, tsubst_expr, tsubst_copy_and_build): Remove
	cilkplus condition.
	* cp/semantics.c (finish_goto_stmt, begin_while_stmt, finish_do_body,
	finish_init_stmt, finish_switch_cond, simplify_aggr_init_expr,
	finish_omp_clauses, finish_omp_clauses, finish_omp_for): Remove cilkplus
	condition.
	* cp/tree.c (lvalue_kind): Remove ARRAY_NOTATION_REF conditon.
	* cp/typeck.c (cp_build_array_ref, cp_build_compound_expr,
	check_return_expr): Remove cilkplus conditions.
	* cppbuiltin.c: Ditto.
	* doc/extend.texi: Remove cilkplus doc.
	* doc/generic.texi: Ditto.
	* doc/invoke.texi: Ditto.
	* doc/passes.texi: Ditto.
	* gcc.c (fcilkplus): Remove.
	* gengtype.c (cilk.h): Remove.
	* gimple-pretty-print.c (dump_gimple_omp_for): Remove cilkplus conditions.
	* gimple.h (GF_OMP_FOR_KIND_CILKFOR, GF_OMP_FOR_KIND_CILKSIMD): Remove.
	* gimplify.c (gimplify_return_expr, maybe_fold_stmt, gimplify_call_expr,
	is_gimple_stmt, gimplify_modify_expr, gimplify_scan_omp_clauses,
	gimplify_adjust_omp_clauses, gimplify_omp_for, gimplify_expr): Remove
	cilkplus conditions.
	* ipa-fnsummary.c (ipa_dump_fn_summary, compute_fn_summary,
	inline_read_section): Ditto.
	* ipa-inline-analysis.c (cilk.h): Remove.
	* ira.c (ira_setup_eliminable_regset): Remove cilkplus condition.
	* lto-wrapper.c (merge_and_complain, append_compiler_options,
	append_linker_options): Remove condition for fcilkplus.
	* lto/lto-lang.c (cilk.h): Remove.
	(lto_init): Remove condition for fcilkplus.
	* omp-expand.c (expand_cilk_for_call): Delete.
	(expand_omp_taskreg, expand_omp_for_static_chunk,
	expand_omp_for): Remove cilkplus
	conditions.
	(expand_cilk_for): Delete.
	* omp-general.c (omp_extract_for_data): Remove cilkplus condition.
	* omp-low.c (scan_sharing_clauses, create_omp_child_function,
	execute_lower_omp, diagnose_sb_0): Ditto.
	* omp-simd-clone.c (simd_clone_clauses_extract): Ditto.
	* tree-core.h
	* tree-nested.c
	* tree-pretty-print.c
	* tree.c
	* tree.def
	* tree.h

gcc/testsuite/
	* c-c++-common/attr-simd-3.c: Delete.
	* c-c++-common/cilk-plus/AN/an-if.c: Delete.
	* c-c++-common/cilk-plus/AN/array_test1.c: Delete.
	* c-c++-common/cilk-plus/AN/array_test2.c: Delete.
	* c-c++-common/cilk-plus/AN/array_test_ND.c: Delete.
	* c-c++-common/cilk-plus/AN/builtin_fn_custom.c: Delete.
	* c-c++-common/cilk-plus/AN/builtin_fn_mutating.c: Delete.
	* c-c++-common/cilk-plus/AN/builtin_func_double.c: Delete.
	* c-c++-common/cilk-plus/AN/builtin_func_double2.c: Delete.
	* c-c++-common/cilk-plus/AN/comma_exp.c: Delete.
	* c-c++-common/cilk-plus/AN/conditional.c: Delete.
	* c-c++-common/cilk-plus/AN/decl-ptr-colon.c: Delete.
	* c-c++-common/cilk-plus/AN/dimensionless-arrays.c: Delete.
	* c-c++-common/cilk-plus/AN/exec-once.c: Delete.
	* c-c++-common/cilk-plus/AN/exec-once2.c: Delete.
	* c-c++-common/cilk-plus/AN/fn_ptr-2.c: Delete.
	* c-c++-common/cilk-plus/AN/fn_ptr.c: Delete.
	* c-c++-common/cilk-plus/AN/fp_triplet_values.c: Delete.
	* c-c++-common/cilk-plus/AN/gather-scatter-errors.c: Delete.
	* c-c++-common/cilk-plus/AN/gather_scatter.c: Delete.
	* c-c++-common/cilk-plus/AN/if_test.c: Delete.
	* c-c++-common/cilk-plus/AN/if_test_errors.c: Delete.
	* c-c++-common/cilk-plus/AN/misc.c: Delete.
	* c-c++-common/cilk-plus/AN/n-ptr-test.c: Delete.
	* c-c++-common/cilk-plus/AN/parser_errors.c: Delete.
	* c-c++-common/cilk-plus/AN/parser_errors2.c: Delete.
	* c-c++-common/cilk-plus/AN/parser_errors3.c: Delete.
	* c-c++-common/cilk-plus/AN/parser_errors4.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57457-2.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57457.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57490.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57541-2.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57541.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57577.c: Delete.
	* c-c++-common/cilk-plus/AN/pr58942.c: Delete.
	* c-c++-common/cilk-plus/AN/pr61191.c: Delete.
	* c-c++-common/cilk-plus/AN/pr61455-2.c: Delete.
	* c-c++-common/cilk-plus/AN/pr61455.c: Delete.
	* c-c++-common/cilk-plus/AN/pr61962.c: Delete.
	* c-c++-common/cilk-plus/AN/pr61963.c: Delete.
	* c-c++-common/cilk-plus/AN/pr62008.c: Delete.
	* c-c++-common/cilk-plus/AN/pr63884.c: Delete.
	* c-c++-common/cilk-plus/AN/rank_mismatch.c: Delete.
	* c-c++-common/cilk-plus/AN/rank_mismatch2.c: Delete.
	* c-c++-common/cilk-plus/AN/rank_mismatch3.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_implicit.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_implicit2.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_implicit_ex.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_reduce_ind_same_value.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_reduce_max_min_ind.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_reduce_return.c: Delete.
	* c-c++-common/cilk-plus/AN/side-effects-1.c: Delete.
	* c-c++-common/cilk-plus/AN/test_builtin_return.c: Delete.
	* c-c++-common/cilk-plus/AN/test_sec_limits.c: Delete.
	* c-c++-common/cilk-plus/AN/tst_lngth.c: Delete.
	* c-c++-common/cilk-plus/AN/vla.c: Delete.
	* c-c++-common/cilk-plus/CK/Wparentheses-1.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk-for-2.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk-for-3.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk-fors.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk_for_errors.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk_for_grain.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk_for_grain_errors.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk_for_ptr_iter.c: Delete.
	* c-c++-common/cilk-plus/CK/compound_cilk_spawn.c: Delete.
	* c-c++-common/cilk-plus/CK/concec_cilk_spawn.c: Delete.
	* c-c++-common/cilk-plus/CK/errors.c: Delete.
	* c-c++-common/cilk-plus/CK/fib.c: Delete.
	* c-c++-common/cilk-plus/CK/fib_init_expr_xy.c: Delete.
	* c-c++-common/cilk-plus/CK/fib_no_return.c: Delete.
	* c-c++-common/cilk-plus/CK/fib_no_sync.c: Delete.
	* c-c++-common/cilk-plus/CK/invalid_spawns.c: Delete.
	* c-c++-common/cilk-plus/CK/invalid_sync.c: Delete.c
	* c-c++-common/cilk-plus/CK/nested_cilk_for.c: Delete.
	* c-c++-common/cilk-plus/CK/no_args_error.c: Delete.
	* c-c++-common/cilk-plus/CK/pr59631.c: Delete.
	* c-c++-common/cilk-plus/CK/pr60197-2.c: Delete.
	* c-c++-common/cilk-plus/CK/pr60197.c: Delete.
	* c-c++-common/cilk-plus/CK/pr60469.c: Delete.
	* c-c++-common/cilk-plus/CK/pr60586.c: Delete.
	* c-c++-common/cilk-plus/CK/pr63307.c: Delete.
	* c-c++-common/cilk-plus/CK/pr69826-1.c: Delete.
	* c-c++-common/cilk-plus/CK/pr69826-2.c: Delete.
	* c-c++-common/cilk-plus/CK/pr79428-4.c: Delete.
	* c-c++-common/cilk-plus/CK/pr79428-7.c: Delete.
	* c-c++-common/cilk-plus/CK/spawn_in_return.c: Delete.
	* c-c++-common/cilk-plus/CK/spawnee_inline.c: Delete.
	* c-c++-common/cilk-plus/CK/spawner_inline.c: Delete.
	* c-c++-common/cilk-plus/CK/spawning_arg.c: Delete.
	* c-c++-common/cilk-plus/CK/steal_check.c: Delete.
	* c-c++-common/cilk-plus/CK/sync_wo_spawn.c: Delete.
	* c-c++-common/cilk-plus/CK/test__cilk.c: Delete.
	* c-c++-common/cilk-plus/CK/varargs_test.c: Delete.
	* c-c++-common/cilk-plus/PS/Wparentheses-1.c: Delete.
	* c-c++-common/cilk-plus/PS/body.c: Delete.
	* c-c++-common/cilk-plus/PS/clauses1.c: Delete.
	* c-c++-common/cilk-plus/PS/clauses2.c: Delete.
	* c-c++-common/cilk-plus/PS/clauses3.c: Delete.
	* c-c++-common/cilk-plus/PS/clauses4.c: Delete.
	* c-c++-common/cilk-plus/PS/for1.c: Delete.
	* c-c++-common/cilk-plus/PS/for2.c: Delete.
	* c-c++-common/cilk-plus/PS/for3.c: Delete.
	* c-c++-common/cilk-plus/PS/pr69363.c: Delete.
	* c-c++-common/cilk-plus/PS/reduction-1.c: Delete.
	* c-c++-common/cilk-plus/PS/reduction-2.c: Delete.
	* c-c++-common/cilk-plus/PS/reduction-3.c: Delete.
	* c-c++-common/cilk-plus/PS/run-1.c: Delete.
	* c-c++-common/cilk-plus/PS/safelen.c: Delete.
	* c-c++-common/cilk-plus/PS/vectorlength-2.c: Delete.
	* c-c++-common/cilk-plus/PS/vectorlength-3.c: Delete.
	* c-c++-common/cilk-plus/PS/vectorlength.c: Delete.
	* c-c++-common/cilk-plus/SE/ef_error.c: Delete.
	* c-c++-common/cilk-plus/SE/ef_error2.c: Delete.
	* c-c++-common/cilk-plus/SE/ef_error3.c: Delete.
	* c-c++-common/cilk-plus/SE/ef_test.c: Delete.
	* c-c++-common/cilk-plus/SE/ef_test2.c: Delete.
	* c-c++-common/cilk-plus/SE/vlength_errors.c: Delete.
	* g++.dg/cilk-plus/AN/array_function.c: Delete.c
	* g++.dg/cilk-plus/AN/array_test1_tplt.c: Delete.c
	* g++.dg/cilk-plus/AN/array_test2_tplt.c: Delete.c
	* g++.dg/cilk-plus/AN/array_test_ND_tplt.c: Delete.c
	* g++.dg/cilk-plus/AN/braced_list.c: Delete.c
	* g++.dg/cilk-plus/AN/builtin_fn_custom_tplt.c: Delete.c
	* g++.dg/cilk-plus/AN/builtin_fn_mutating_tplt.c: Delete.c
	* g++.dg/cilk-plus/AN/fp_triplet_values_tplt.c: Delete.
	* g++.dg/cilk-plus/AN/postincr_test.c: Delete.c
	* g++.dg/cilk-plus/AN/preincr_test.c: Delete.c
	* g++.dg/cilk-plus/CK/catch_exc.c: Delete.c
	* g++.dg/cilk-plus/CK/cf3.c: Delete.c
	* g++.dg/cilk-plus/CK/cilk-for-tplt.c: Delete.c
	* g++.dg/cilk-plus/CK/const_spawn.c: Delete.c
	* g++.dg/cilk-plus/CK/fib-opr-overload.c: Delete.c
	* g++.dg/cilk-plus/CK/fib-tplt.c: Delete.c
	* g++.dg/cilk-plus/CK/for1.c: Delete.c
	* g++.dg/cilk-plus/CK/lambda_spawns.c: Delete.c
	* g++.dg/cilk-plus/CK/lambda_spawns_tplt.c: Delete.c
	* g++.dg/cilk-plus/CK/pr60586.c: Delete.c
	* g++.dg/cilk-plus/CK/pr66326.c: Delete.c
	* g++.dg/cilk-plus/CK/pr68001.c: Delete.c
	* g++.dg/cilk-plus/CK/pr68997.c: Delete.c
	* g++.dg/cilk-plus/CK/pr69024.c: Delete.c
	* g++.dg/cilk-plus/CK/pr69048.c: Delete.c
	* g++.dg/cilk-plus/CK/pr69267.c: Delete.c
	* g++.dg/cilk-plus/CK/pr80038.c: Delete.c
	* g++.dg/cilk-plus/CK/stl_iter.c: Delete.c
	* g++.dg/cilk-plus/CK/stl_rev_iter.c: Delete.c
	* g++.dg/cilk-plus/CK/stl_test.c: Delete.c
	* g++.dg/cilk-plus/cilk-plus.exp
	* g++.dg/cilk-plus/ef_test.C: Delete.
	* g++.dg/cilk-plus/for.C: Delete.
	* g++.dg/cilk-plus/for2.C: Delete.
	* g++.dg/cilk-plus/for3.C: Delete.
	* g++.dg/cilk-plus/for4.C: Delete.
	* g++.dg/cilk-plus/pr60967.C: Delete.
	* g++.dg/cilk-plus/pr69028.C: Delete.
	* g++.dg/cilk-plus/pr70565.C: Delete.
	* g++.dg/pr57662.C: Delete.
	* gcc.dg/cilk-plus/cilk-plus.exp
	* gcc.dg/cilk-plus/for1.c: Delete.
	* gcc.dg/cilk-plus/for2.c: Delete.
	* gcc.dg/cilk-plus/jump-openmp.c: Delete.
	* gcc.dg/cilk-plus/jump.c: Delete.
	* gcc.dg/cilk-plus/pr69798-1.c: Delete.
	* gcc.dg/cilk-plus/pr69798-2.c: Delete.
	* gcc.dg/cilk-plus/pr78306.c: Delete.
	* gcc.dg/cilk-plus/pr79116.c: Delete.
	* gcc.dg/graphite/id-28.c: Delete.
	* lib/cilk-plus-dg.exp: Delete.
	* lib/target-supports.exp (cilkplus_runtime): Delete.

libcilkrts: Delete

Thanks,
Julia
EOF

cat >out.gold <<"EOF"
From:       julia.koval@intel.com
To:         gcc-patches@gcc.gnu.org
CC:         law@redhat.com
Subject:    [patch] remove cilk-plus
Date:       Thu, 16 Nov 2017 07:33:40 PST (-0800)
Message-ID: <4E89A029A0F8D443B436A5167BA3C53F42AD12C2@IRSMSX101.ger.corp.intel.com>

// I failed to send patch itself, it is too big even in gzipped form.  What is
the right way to send such big patches?

Hi, this patch removes cilkplus. Ok for trunk?

2017-11-16  Julia Koval  <julia.koval@intel.com>
	        Sebastian Peryt  <sebastian.peryt@intel.com>
gcc/
	* Makefile.def (target_modules): Remove libcilkrts.
	* Makefile.in: Ditto.
	* configure: Ditto.
	* configure.ac: Ditto.
	* contrib/gcc_update: Ditto.
	* Makefile.in (cilkplus.def, cilk-builtins.def, c-family/cilk.o, 
	c-family/c-cilkplus.o, c-family/array-notation-common.o,
	cilk-common.o, cilk.h, cilk-common.c): Remove.
	* builtin-types.def (BT_FN_INT_PTR_PTR_PTR_FTYPE_BT_INT_BT_PTR_BT_PTR
	_BT_PTR): Remove.
	* builtins.c (is_builtin_name): Remove cilkplus condition.
	(BUILT_IN_CILK_DETACH, BUILT_IN_CILK_POP_FRAME): Remove.
	* builtins.def (DEF_CILK_BUILTIN_STUB, DEF_CILKPLUS_BUILTIN,
	cilk-builtins.def, cilkplus.def): Remove.
	* c-family/array-notation-common.c: Delete.
	* c-family/c-cilkplus.c: Ditto.
	* c-family/c-common.c (_Cilk_spawn, _Cilk_sync, _Cilk_for): Remove.
	* c-family/c-common.def (ARRAY_NOTATION_REF): Remove.
	* c-family/c-common.h (RID_CILK_SPAWN, build_array_notation_expr,
	build_array_notation_ref, C_ORT_CILK, c_check_cilk_loop,
	c_validate_cilk_plus_loop, cilkplus_an_parts, cilk_ignorable_spawn_rhs_op,
	cilk_recognize_spawn): Remove.
	* c-family/c-gimplify.c (CILK_SPAWN_STMT): Remove.
	* c-family/c-omp.c: Remove CILK_SIMD check.
	* c-family/c-pragma.c: Ditto.
	* c-family/c-pragma.h: Remove CILK related pragmas.
	* c-family/c-pretty-print.c (c_pretty_printer::postfix_expression): Remove
	ARRAY_NOTATION_REF condition.
	(c_pretty_printer::expression): Ditto.
	* c-family/c.opt (fcilkplus): Remove.
	* c-family/cilk.c: Delete.
	* c/Make-lang.in (c/c-array-notation.o): Remove.
	* c/c-array-notation.c: Delete.
	* c/c-decl.c: Remove cilkplus condition.
	* c/c-parser.c (c_parser_cilk_simd, c_parser_cilk_for,
	c_parser_cilk_verify_simd, c_parser_array_notation,
	c_parser_cilk_clause_vectorlength, c_parser_cilk_grainsize,
	c_parser_cilk_simd_fn_vector_attrs, c_finish_cilk_simd_fn_tokens): Delete.
	(c_parser_declaration_or_fndef): Remove cilkplus condition.
	(c_parser_direct_declarator_inner): Ditto.
	(CILK_SIMD_FN_CLAUSE_MASK): Delete.
	(c_parser_attributes): Remove cilk-plus condition.
	(c_parser_compound_statement): Ditto.
	(c_parser_statement_after_labels): Ditto.
	(c_parser_if_statement): Ditto.
	(c_parser_switch_statement): Ditto.
	(c_parser_while_statement): Ditto.
	(c_parser_do_statement): Ditto.
	(c_parser_for_statement): Ditto.
	(c_parser_unary_expression): Ditto.
	(c_parser_postfix_expression): Ditto.
	(c_parser_postfix_expression_after_primary): Ditto.
	(c_parser_pragma): Ditto.
	(c_parser_omp_clause_name): Ditto.
	(c_parser_omp_all_clauses): Ditto.
	(c_parser_omp_for_loop): Ditto.
	(c_finish_omp_declare_simd): Ditto.
	* c/c-typeck.c (build_array_ref, build_function_call_vec, convert_arguments,
	lvalue_p, build_compound_expr, c_finish_return, c_finish_if_stmt,
	c_finish_loop, build_binary_op): Remove cilkplus condition.
	* cif-code.def (CILK_SPAWN): Remove.
	* cilk-builtins.def: Delete.
	* cilk-common.c: Ditto.
	* cilk.h: Ditto.
	* cilkplus.def: Ditto.
	* config/darwin.h (fcilkplus): Delete.
	* cp/Make-lang.in (cp/cp-array-notation.o, cp/cp-cilkplus.o): Delete.
	* cp/call.c (convert_for_arg_passing, build_cxx_call): Remove cilkplus.
	* cp/constexpr.c (potential_constant_expression_1): Ditto.
	* cp/cp-array-notation.c: Delete.
	* cp/cp-cilkplus.c: Ditto.
	* cp/cp-cilkplus.h: Ditto.
	* cp/cp-gimplify.c (cp_gimplify_expr, cp_fold_r, cp_genericize): Remove
	cilkplus condition.
	* cp/cp-objcp-common.c (ARRAY_NOTATION_REF): Delete.
	* cp/cp-tree.h (cilkplus_an_triplet_types_ok_p): Delete.
	* cp/decl.c (grokfndecl, finish_function): Remove cilkplus condition.
	* cp/error.c (dump_decl, dump_expr): Remove ARRAY_NOTATION_REF condition.
	* cp/lambda.c (cp-cilkplus.h): Remove.
	* cp/parser.c (cp_parser_cilk_simd, cp_parser_cilk_for,
	cp_parser_cilk_simd_vectorlength): Delete.
	(cp_debug_parser, cp_parser_ctor_initializer_opt_and_function_body,
	cp_parser_postfix_expression, cp_parser_postfix_open_square_expression,
	cp_parser_statement, cp_parser_jump_statement, cp_parser_direct_declarator,
	cp_parser_late_return_type_opt, cp_parser_gnu_attribute_list,
	cp_parser_omp_clause_name, cp_parser_omp_clause_aligned,
	cp_parser_omp_clause_linear, cp_parser_omp_all_clauses, cp_parser_omp_flush,
	cp_parser_omp_for_cond, cp_parser_omp_for_incr, cp_parser_omp_for_loop_init,
	cp_parser_omp_for_loop, cp_parser_omp_declare_simd): Remove cilkplus condtions.
	(CILK_SIMD_FN_CLAUSE_MASK, cp_parser_late_parsing_cilk_simd_fn_info,
	cp_parser_cilk_grainsize): Remove.
	(cp_parser_pragma, c_parse_file): Remove cilkplus condition.
	(cp_parser_cilk_simd_vectorlength, cp_parser_cilk_simd_linear,
	cp_parser_cilk_simd_clause_name, cp_parser_cilk_simd_all_clauses,
	cp_parser_cilk_simd, cp_parser_cilk_for): Remove.
	* cp/parser.h (IN_CILK_SIMD_FOR, IN_CILK_SPAWN): Remove.
	* cp/pt.c (tsubst_attribute, tsubst_expr, tsubst_copy_and_build): Remove
	cilkplus condition.
	* cp/semantics.c (finish_goto_stmt, begin_while_stmt, finish_do_body,
	finish_init_stmt, finish_switch_cond, simplify_aggr_init_expr,
	finish_omp_clauses, finish_omp_clauses, finish_omp_for): Remove cilkplus
	condition.
	* cp/tree.c (lvalue_kind): Remove ARRAY_NOTATION_REF conditon.
	* cp/typeck.c (cp_build_array_ref, cp_build_compound_expr,
	check_return_expr): Remove cilkplus conditions.
	* cppbuiltin.c: Ditto.
	* doc/extend.texi: Remove cilkplus doc.
	* doc/generic.texi: Ditto.
	* doc/invoke.texi: Ditto.
	* doc/passes.texi: Ditto.
	* gcc.c (fcilkplus): Remove.
	* gengtype.c (cilk.h): Remove.
	* gimple-pretty-print.c (dump_gimple_omp_for): Remove cilkplus conditions.
	* gimple.h (GF_OMP_FOR_KIND_CILKFOR, GF_OMP_FOR_KIND_CILKSIMD): Remove.
	* gimplify.c (gimplify_return_expr, maybe_fold_stmt, gimplify_call_expr,
	is_gimple_stmt, gimplify_modify_expr, gimplify_scan_omp_clauses,
	gimplify_adjust_omp_clauses, gimplify_omp_for, gimplify_expr): Remove
	cilkplus conditions.
	* ipa-fnsummary.c (ipa_dump_fn_summary, compute_fn_summary,
	inline_read_section): Ditto.
	* ipa-inline-analysis.c (cilk.h): Remove.
	* ira.c (ira_setup_eliminable_regset): Remove cilkplus condition.
	* lto-wrapper.c (merge_and_complain, append_compiler_options,
	append_linker_options): Remove condition for fcilkplus.
	* lto/lto-lang.c (cilk.h): Remove.
	(lto_init): Remove condition for fcilkplus.
	* omp-expand.c (expand_cilk_for_call): Delete.
	(expand_omp_taskreg, expand_omp_for_static_chunk,
	expand_omp_for): Remove cilkplus
	conditions.
	(expand_cilk_for): Delete.
	* omp-general.c (omp_extract_for_data): Remove cilkplus condition.
	* omp-low.c (scan_sharing_clauses, create_omp_child_function,
	execute_lower_omp, diagnose_sb_0): Ditto.
	* omp-simd-clone.c (simd_clone_clauses_extract): Ditto.
	* tree-core.h
	* tree-nested.c
	* tree-pretty-print.c
	* tree.c
	* tree.def
	* tree.h

gcc/testsuite/
	* c-c++-common/attr-simd-3.c: Delete.
	* c-c++-common/cilk-plus/AN/an-if.c: Delete.
	* c-c++-common/cilk-plus/AN/array_test1.c: Delete.
	* c-c++-common/cilk-plus/AN/array_test2.c: Delete.
	* c-c++-common/cilk-plus/AN/array_test_ND.c: Delete.
	* c-c++-common/cilk-plus/AN/builtin_fn_custom.c: Delete.
	* c-c++-common/cilk-plus/AN/builtin_fn_mutating.c: Delete.
	* c-c++-common/cilk-plus/AN/builtin_func_double.c: Delete.
	* c-c++-common/cilk-plus/AN/builtin_func_double2.c: Delete.
	* c-c++-common/cilk-plus/AN/comma_exp.c: Delete.
	* c-c++-common/cilk-plus/AN/conditional.c: Delete.
	* c-c++-common/cilk-plus/AN/decl-ptr-colon.c: Delete.
	* c-c++-common/cilk-plus/AN/dimensionless-arrays.c: Delete.
	* c-c++-common/cilk-plus/AN/exec-once.c: Delete.
	* c-c++-common/cilk-plus/AN/exec-once2.c: Delete.
	* c-c++-common/cilk-plus/AN/fn_ptr-2.c: Delete.
	* c-c++-common/cilk-plus/AN/fn_ptr.c: Delete.
	* c-c++-common/cilk-plus/AN/fp_triplet_values.c: Delete.
	* c-c++-common/cilk-plus/AN/gather-scatter-errors.c: Delete.
	* c-c++-common/cilk-plus/AN/gather_scatter.c: Delete.
	* c-c++-common/cilk-plus/AN/if_test.c: Delete.
	* c-c++-common/cilk-plus/AN/if_test_errors.c: Delete.
	* c-c++-common/cilk-plus/AN/misc.c: Delete.
	* c-c++-common/cilk-plus/AN/n-ptr-test.c: Delete.
	* c-c++-common/cilk-plus/AN/parser_errors.c: Delete.
	* c-c++-common/cilk-plus/AN/parser_errors2.c: Delete.
	* c-c++-common/cilk-plus/AN/parser_errors3.c: Delete.
	* c-c++-common/cilk-plus/AN/parser_errors4.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57457-2.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57457.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57490.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57541-2.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57541.c: Delete.
	* c-c++-common/cilk-plus/AN/pr57577.c: Delete.
	* c-c++-common/cilk-plus/AN/pr58942.c: Delete.
	* c-c++-common/cilk-plus/AN/pr61191.c: Delete.
	* c-c++-common/cilk-plus/AN/pr61455-2.c: Delete.
	* c-c++-common/cilk-plus/AN/pr61455.c: Delete.
	* c-c++-common/cilk-plus/AN/pr61962.c: Delete.
	* c-c++-common/cilk-plus/AN/pr61963.c: Delete.
	* c-c++-common/cilk-plus/AN/pr62008.c: Delete.
	* c-c++-common/cilk-plus/AN/pr63884.c: Delete.
	* c-c++-common/cilk-plus/AN/rank_mismatch.c: Delete.
	* c-c++-common/cilk-plus/AN/rank_mismatch2.c: Delete.
	* c-c++-common/cilk-plus/AN/rank_mismatch3.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_implicit.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_implicit2.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_implicit_ex.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_reduce_ind_same_value.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_reduce_max_min_ind.c: Delete.
	* c-c++-common/cilk-plus/AN/sec_reduce_return.c: Delete.
	* c-c++-common/cilk-plus/AN/side-effects-1.c: Delete.
	* c-c++-common/cilk-plus/AN/test_builtin_return.c: Delete.
	* c-c++-common/cilk-plus/AN/test_sec_limits.c: Delete.
	* c-c++-common/cilk-plus/AN/tst_lngth.c: Delete.
	* c-c++-common/cilk-plus/AN/vla.c: Delete.
	* c-c++-common/cilk-plus/CK/Wparentheses-1.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk-for-2.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk-for-3.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk-fors.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk_for_errors.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk_for_grain.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk_for_grain_errors.c: Delete.
	* c-c++-common/cilk-plus/CK/cilk_for_ptr_iter.c: Delete.
	* c-c++-common/cilk-plus/CK/compound_cilk_spawn.c: Delete.
	* c-c++-common/cilk-plus/CK/concec_cilk_spawn.c: Delete.
	* c-c++-common/cilk-plus/CK/errors.c: Delete.
	* c-c++-common/cilk-plus/CK/fib.c: Delete.
	* c-c++-common/cilk-plus/CK/fib_init_expr_xy.c: Delete.
	* c-c++-common/cilk-plus/CK/fib_no_return.c: Delete.
	* c-c++-common/cilk-plus/CK/fib_no_sync.c: Delete.
	* c-c++-common/cilk-plus/CK/invalid_spawns.c: Delete.
	* c-c++-common/cilk-plus/CK/invalid_sync.c: Delete.c
	* c-c++-common/cilk-plus/CK/nested_cilk_for.c: Delete.
	* c-c++-common/cilk-plus/CK/no_args_error.c: Delete.
	* c-c++-common/cilk-plus/CK/pr59631.c: Delete.
	* c-c++-common/cilk-plus/CK/pr60197-2.c: Delete.
	* c-c++-common/cilk-plus/CK/pr60197.c: Delete.
	* c-c++-common/cilk-plus/CK/pr60469.c: Delete.
	* c-c++-common/cilk-plus/CK/pr60586.c: Delete.
	* c-c++-common/cilk-plus/CK/pr63307.c: Delete.
	* c-c++-common/cilk-plus/CK/pr69826-1.c: Delete.
	* c-c++-common/cilk-plus/CK/pr69826-2.c: Delete.
	* c-c++-common/cilk-plus/CK/pr79428-4.c: Delete.
	* c-c++-common/cilk-plus/CK/pr79428-7.c: Delete.
	* c-c++-common/cilk-plus/CK/spawn_in_return.c: Delete.
	* c-c++-common/cilk-plus/CK/spawnee_inline.c: Delete.
	* c-c++-common/cilk-plus/CK/spawner_inline.c: Delete.
	* c-c++-common/cilk-plus/CK/spawning_arg.c: Delete.
	* c-c++-common/cilk-plus/CK/steal_check.c: Delete.
	* c-c++-common/cilk-plus/CK/sync_wo_spawn.c: Delete.
	* c-c++-common/cilk-plus/CK/test__cilk.c: Delete.
	* c-c++-common/cilk-plus/CK/varargs_test.c: Delete.
	* c-c++-common/cilk-plus/PS/Wparentheses-1.c: Delete.
	* c-c++-common/cilk-plus/PS/body.c: Delete.
	* c-c++-common/cilk-plus/PS/clauses1.c: Delete.
	* c-c++-common/cilk-plus/PS/clauses2.c: Delete.
	* c-c++-common/cilk-plus/PS/clauses3.c: Delete.
	* c-c++-common/cilk-plus/PS/clauses4.c: Delete.
	* c-c++-common/cilk-plus/PS/for1.c: Delete.
	* c-c++-common/cilk-plus/PS/for2.c: Delete.
	* c-c++-common/cilk-plus/PS/for3.c: Delete.
	* c-c++-common/cilk-plus/PS/pr69363.c: Delete.
	* c-c++-common/cilk-plus/PS/reduction-1.c: Delete.
	* c-c++-common/cilk-plus/PS/reduction-2.c: Delete.
	* c-c++-common/cilk-plus/PS/reduction-3.c: Delete.
	* c-c++-common/cilk-plus/PS/run-1.c: Delete.
	* c-c++-common/cilk-plus/PS/safelen.c: Delete.
	* c-c++-common/cilk-plus/PS/vectorlength-2.c: Delete.
	* c-c++-common/cilk-plus/PS/vectorlength-3.c: Delete.
	* c-c++-common/cilk-plus/PS/vectorlength.c: Delete.
	* c-c++-common/cilk-plus/SE/ef_error.c: Delete.
	* c-c++-common/cilk-plus/SE/ef_error2.c: Delete.
	* c-c++-common/cilk-plus/SE/ef_error3.c: Delete.
	* c-c++-common/cilk-plus/SE/ef_test.c: Delete.
	* c-c++-common/cilk-plus/SE/ef_test2.c: Delete.
	* c-c++-common/cilk-plus/SE/vlength_errors.c: Delete.
	* g++.dg/cilk-plus/AN/array_function.c: Delete.c
	* g++.dg/cilk-plus/AN/array_test1_tplt.c: Delete.c
	* g++.dg/cilk-plus/AN/array_test2_tplt.c: Delete.c
	* g++.dg/cilk-plus/AN/array_test_ND_tplt.c: Delete.c
	* g++.dg/cilk-plus/AN/braced_list.c: Delete.c
	* g++.dg/cilk-plus/AN/builtin_fn_custom_tplt.c: Delete.c
	* g++.dg/cilk-plus/AN/builtin_fn_mutating_tplt.c: Delete.c
	* g++.dg/cilk-plus/AN/fp_triplet_values_tplt.c: Delete.
	* g++.dg/cilk-plus/AN/postincr_test.c: Delete.c
	* g++.dg/cilk-plus/AN/preincr_test.c: Delete.c
	* g++.dg/cilk-plus/CK/catch_exc.c: Delete.c
	* g++.dg/cilk-plus/CK/cf3.c: Delete.c
	* g++.dg/cilk-plus/CK/cilk-for-tplt.c: Delete.c
	* g++.dg/cilk-plus/CK/const_spawn.c: Delete.c
	* g++.dg/cilk-plus/CK/fib-opr-overload.c: Delete.c
	* g++.dg/cilk-plus/CK/fib-tplt.c: Delete.c
	* g++.dg/cilk-plus/CK/for1.c: Delete.c
	* g++.dg/cilk-plus/CK/lambda_spawns.c: Delete.c
	* g++.dg/cilk-plus/CK/lambda_spawns_tplt.c: Delete.c
	* g++.dg/cilk-plus/CK/pr60586.c: Delete.c
	* g++.dg/cilk-plus/CK/pr66326.c: Delete.c
	* g++.dg/cilk-plus/CK/pr68001.c: Delete.c
	* g++.dg/cilk-plus/CK/pr68997.c: Delete.c
	* g++.dg/cilk-plus/CK/pr69024.c: Delete.c
	* g++.dg/cilk-plus/CK/pr69048.c: Delete.c
	* g++.dg/cilk-plus/CK/pr69267.c: Delete.c
	* g++.dg/cilk-plus/CK/pr80038.c: Delete.c
	* g++.dg/cilk-plus/CK/stl_iter.c: Delete.c
	* g++.dg/cilk-plus/CK/stl_rev_iter.c: Delete.c
	* g++.dg/cilk-plus/CK/stl_test.c: Delete.c
	* g++.dg/cilk-plus/cilk-plus.exp
	* g++.dg/cilk-plus/ef_test.C: Delete.
	* g++.dg/cilk-plus/for.C: Delete.
	* g++.dg/cilk-plus/for2.C: Delete.
	* g++.dg/cilk-plus/for3.C: Delete.
	* g++.dg/cilk-plus/for4.C: Delete.
	* g++.dg/cilk-plus/pr60967.C: Delete.
	* g++.dg/cilk-plus/pr69028.C: Delete.
	* g++.dg/cilk-plus/pr70565.C: Delete.
	* g++.dg/pr57662.C: Delete.
	* gcc.dg/cilk-plus/cilk-plus.exp
	* gcc.dg/cilk-plus/for1.c: Delete.
	* gcc.dg/cilk-plus/for2.c: Delete.
	* gcc.dg/cilk-plus/jump-openmp.c: Delete.
	* gcc.dg/cilk-plus/jump.c: Delete.
	* gcc.dg/cilk-plus/pr69798-1.c: Delete.
	* gcc.dg/cilk-plus/pr69798-2.c: Delete.
	* gcc.dg/cilk-plus/pr78306.c: Delete.
	* gcc.dg/cilk-plus/pr79116.c: Delete.
	* gcc.dg/graphite/id-28.c: Delete.
	* lib/cilk-plus-dg.exp: Delete.
	* lib/target-supports.exp (cilkplus_runtime): Delete.

libcilkrts: Delete

Thanks,
Julia

EOF

#include "harness_end.bash"
