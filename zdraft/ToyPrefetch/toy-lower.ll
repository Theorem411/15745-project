	.text
	.file	"cilkfor-sidebyside.cpp"
	.globl	_Z4loopPiS_i                    # -- Begin function _Z4loopPiS_i
	.p2align	4, 0x90
	.type	_Z4loopPiS_i,@function
_Z4loopPiS_i:                           # @_Z4loopPiS_i
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$168, %rsp
	.cfi_restore %rbx
	.cfi_restore %r12
	.cfi_restore %r13
	.cfi_restore %r14
	.cfi_restore %r15
	movq	%rdi, -168(%rbp)                # 8-byte Spill
	movq	%rsi, -160(%rbp)                # 8-byte Spill
	movl	%edx, -148(%rbp)                # 4-byte Spill
	callq	__cilkrts_get_tls_worker@PLT
	movq	%rax, -144(%rbp)                # 8-byte Spill
	cmpq	$0, %rax
	jne	.LBB0_2
# %bb.1:                                # %slowpath.i
	callq	__cilkrts_bind_thread_1@PLT
	movl	$16777344, -136(%rbp)           # imm = 0x1000080
	movq	%rax, -176(%rbp)                # 8-byte Spill
	jmp	.LBB0_3
.LBB0_2:                                # %fastpath.i
	movq	-144(%rbp), %rax                # 8-byte Reload
	movl	$16777216, -136(%rbp)           # imm = 0x1000000
	movq	%rax, -176(%rbp)                # 8-byte Spill
.LBB0_3:                                # %__cilkrts_enter_frame_1.exit
	movl	-148(%rbp), %eax                # 4-byte Reload
	movq	-176(%rbp), %rcx                # 8-byte Reload
	movq	72(%rcx), %rdx
	movq	%rdx, -128(%rbp)
	movq	%rcx, -120(%rbp)
	leaq	-136(%rbp), %rdx
	movq	%rdx, 72(%rcx)
	cmpl	$100, %eax
	jle	.LBB0_8
# %bb.4:                                # %if.then
	xorl	%eax, %eax
	movl	%eax, -180(%rbp)                # 4-byte Spill
	jmp	.LBB0_5
.LBB0_5:                                # %for.body
                                        # =>This Inner Loop Header: Depth=1
	movq	-160(%rbp), %rax                # 8-byte Reload
	movl	-180(%rbp), %ecx                # 4-byte Reload
	movl	%ecx, -184(%rbp)                # 4-byte Spill
	movl	%ecx, %edx
	addl	$1, %edx
	movl	%edx, %edx
                                        # kill: def $rdx killed $edx
	movl	(%rax,%rdx,4), %edx
	movl	%ecx, %ecx
                                        # kill: def $rcx killed $ecx
	movl	%edx, (%rax,%rcx,4)
# %bb.6:                                # %for.inc
                                        #   in Loop: Header=BB0_5 Depth=1
	movl	-184(%rbp), %ecx                # 4-byte Reload
	movl	%ecx, %eax
	addl	$1, %eax
	cmpl	$9, %ecx
	movl	%eax, -180(%rbp)                # 4-byte Spill
	jb	.LBB0_5
# %bb.7:                                # %for.end
	jmp	.LBB0_22
.LBB0_8:                                # %if.else
	movb	$1, %al
	testb	$1, %al
	jne	.LBB0_9
	jmp	.LBB0_21
.LBB0_9:                                # %pfor.ph
	xorl	%eax, %eax
	movl	%eax, -188(%rbp)                # 4-byte Spill
	jmp	.LBB0_10
.LBB0_10:                               # %pfor.cond
                                        # =>This Inner Loop Header: Depth=1
	movl	-188(%rbp), %eax                # 4-byte Reload
	movl	%eax, -192(%rbp)                # 4-byte Spill
# %bb.11:                               # %pfor.detach
                                        #   in Loop: Header=BB0_10 Depth=1
	leaq	-136(%rbp), %rax
	addq	$72, %rax
	leaq	-136(%rbp), %rcx
	addq	$76, %rcx
	#APP
	stmxcsr	(%rax)
	fnstcw	(%rcx)
	#NO_APP
	leaq	-136(%rbp), %rax
	addq	$32, %rax
	movq	%rbp, %rcx
	movq	%rcx, -104(%rbp)
	movq	%rsp, %rcx
	movq	%rcx, -88(%rbp)
	movq	$.LBB0_27, 8(%rax)
	#EH_SjLj_Setup	.LBB0_27
# %bb.25:                               # %pfor.detach
                                        #   in Loop: Header=BB0_10 Depth=1
	xorl	%eax, %eax
	movl	%eax, -196(%rbp)                # 4-byte Spill
.LBB0_26:                               # %pfor.detach
                                        #   in Loop: Header=BB0_10 Depth=1
	movl	-196(%rbp), %eax                # 4-byte Reload
	cmpl	$0, %eax
	jne	.LBB0_13
# %bb.12:                               # %pfor.detach.split
                                        #   in Loop: Header=BB0_10 Depth=1
	movl	-192(%rbp), %esi                # 4-byte Reload
	movq	-168(%rbp), %rdi                # 8-byte Reload
	callq	.L_Z4loopPiS_i.outline_pfor.body.entry.otd1
.LBB0_13:                               # %pfor.inc
                                        #   in Loop: Header=BB0_10 Depth=1
	movl	-192(%rbp), %ecx                # 4-byte Reload
	movl	%ecx, %eax
	addl	$1, %eax
	cmpl	$9, %ecx
	movl	%eax, -188(%rbp)                # 4-byte Spill
	jb	.LBB0_10
# %bb.14:                               # %pfor.cond.cleanup
	movl	-136(%rbp), %eax
	andl	$2, %eax
	cmpl	$0, %eax
	je	.LBB0_19
# %bb.15:                               # %cilk.sync.savestate.i
	movq	-120(%rbp), %rcx
	movq	96(%rcx), %rax
	movq	104(%rcx), %rcx
	movq	%rcx, -48(%rbp)
	movq	%rax, -56(%rbp)
	leaq	-136(%rbp), %rax
	addq	$72, %rax
	leaq	-136(%rbp), %rcx
	addq	$76, %rcx
	#APP
	stmxcsr	(%rax)
	fnstcw	(%rcx)
	#NO_APP
	leaq	-136(%rbp), %rax
	addq	$32, %rax
	movq	%rbp, %rcx
	movq	%rcx, -104(%rbp)
	movq	%rsp, %rcx
	movq	%rcx, -88(%rbp)
	movq	$.LBB0_30, 8(%rax)
	#EH_SjLj_Setup	.LBB0_30
# %bb.28:                               # %cilk.sync.savestate.i
	xorl	%eax, %eax
	movl	%eax, -200(%rbp)                # 4-byte Spill
.LBB0_29:                               # %cilk.sync.savestate.i
	movl	-200(%rbp), %eax                # 4-byte Reload
	cmpl	$0, %eax
	jne	.LBB0_17
# %bb.16:                               # %cilk.sync.runtimecall.i
	leaq	-136(%rbp), %rdi
	callq	__cilkrts_sync@PLT
	jmp	.LBB0_19
.LBB0_17:                               # %cilk.sync.excepting.i
	movl	-136(%rbp), %eax
	andl	$16, %eax
	cmpl	$0, %eax
	je	.LBB0_19
# %bb.18:                               # %cilk.sync.rethrow.i
	leaq	-136(%rbp), %rdi
	callq	__cilkrts_rethrow@PLT
.LBB0_19:                               # %__cilk_sync.exit
	movq	-120(%rbp), %rax
	movq	96(%rax), %rcx
	addq	$1, %rcx
	movq	%rcx, 96(%rax)
# %bb.20:                               # %sync.continue
.LBB0_21:                               # %pfor.end
	jmp	.LBB0_22
.LBB0_22:                               # %if.end
	movq	-120(%rbp), %rax
	movq	-128(%rbp), %rcx
	movq	%rcx, 72(%rax)
	movq	$0, -128(%rbp)
	cmpl	$16777216, -136(%rbp)           # imm = 0x1000000
	je	.LBB0_24
# %bb.23:                               # %body.i
	leaq	-136(%rbp), %rdi
	callq	__cilkrts_leave_frame@PLT
.LBB0_24:                               # %__cilk_parent_epilogue.exit
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.LBB0_27:                               # Block address taken
                                        # %pfor.detach
                                        #   in Loop: Header=BB0_10 Depth=1
	.cfi_def_cfa %rbp, 16
	movl	$1, %eax
	movl	%eax, -196(%rbp)                # 4-byte Spill
	jmp	.LBB0_26
.LBB0_30:                               # Block address taken
                                        # %cilk.sync.savestate.i
	movl	$1, %eax
	movl	%eax, -200(%rbp)                # 4-byte Spill
	jmp	.LBB0_29
.Lfunc_end0:
	.size	_Z4loopPiS_i, .Lfunc_end0-_Z4loopPiS_i
	.cfi_endproc
                                        # -- End function
	.p2align	4, 0x90                         # -- Begin function _Z4loopPiS_i.outline_pfor.body.entry.otd1
	.type	.L_Z4loopPiS_i.outline_pfor.body.entry.otd1,@function
.L_Z4loopPiS_i.outline_pfor.body.entry.otd1: # @_Z4loopPiS_i.outline_pfor.body.entry.otd1
	.cfi_startproc
# %bb.0:                                # %pfor.detach.otd1
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$112, %rsp
	movl	%esi, -108(%rbp)                # 4-byte Spill
	movq	%rdi, -104(%rbp)                # 8-byte Spill
	callq	__cilkrts_get_tls_worker_fast@PLT
	movl	$16777216, -96(%rbp)            # imm = 0x1000000
	movq	72(%rax), %rcx
	movq	%rcx, -88(%rbp)
	movq	%rax, -80(%rbp)
	leaq	-96(%rbp), %rcx
	movq	%rcx, 72(%rax)
	movq	-88(%rbp), %rdx
	movq	-80(%rbp), %rax
	movq	(%rax), %rcx
	movq	96(%rax), %rdi
	movq	104(%rax), %r8
	leaq	-16(%rbp), %rsi
	movq	%r8, -8(%rbp)
	movq	%rdi, -16(%rbp)
	movq	%r8, 88(%rdx)
	movq	%rdi, 80(%rdx)
	movq	$0, 96(%rax)
	movq	%rsi, 104(%rax)
	#MEMBARRIER
	movq	%rdx, (%rcx)
	addq	$8, %rcx
	movq	%rcx, (%rax)
	movl	-96(%rbp), %eax
	orl	$4, %eax
	movl	%eax, -96(%rbp)
# %bb.1:                                # %pfor.body.entry.otd1
	jmp	.LBB1_2
.LBB1_2:                                # %pfor.body.otd1
	movq	-104(%rbp), %rax                # 8-byte Reload
	movl	-108(%rbp), %ecx                # 4-byte Reload
	movl	%ecx, %ecx
                                        # kill: def $rcx killed $ecx
	movl	(%rax,%rcx,4), %edx
	addl	$1, %edx
	movl	%edx, (%rax,%rcx,4)
# %bb.3:                                # %pfor.preattach.otd1
	jmp	.LBB1_4
.LBB1_4:                                # %pfor.inc.otd1
	movq	-80(%rbp), %rax
	movq	-88(%rbp), %rcx
	movq	%rcx, 72(%rax)
	movq	$0, -88(%rbp)
	cmpl	$16777216, -96(%rbp)            # imm = 0x1000000
	je	.LBB1_6
# %bb.5:                                # %body.i
	leaq	-96(%rbp), %rdi
	callq	__cilkrts_leave_frame@PLT
.LBB1_6:                                # %__cilk_parent_epilogue.exit
	addq	$112, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end1:
	.size	.L_Z4loopPiS_i.outline_pfor.body.entry.otd1, .Lfunc_end1-.L_Z4loopPiS_i.outline_pfor.body.entry.otd1
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 14.0.6 (/afs/ece/project/seth_group/ziqiliu/uli-opencilk-project/clang a3c5ebae5d3682f08ee5fffc20678b8b69c3ae06)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym __cilkrts_get_tls_worker_fast
	.addrsig_sym __cilkrts_leave_frame
	.addrsig_sym __cilkrts_get_tls_worker
	.addrsig_sym __cilkrts_bind_thread_1
	.addrsig_sym __cilkrts_sync
	.addrsig_sym __cilkrts_rethrow
