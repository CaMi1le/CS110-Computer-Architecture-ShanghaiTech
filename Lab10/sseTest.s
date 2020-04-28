	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 14	sdk_version 10, 14
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$528, %rsp              ## imm = 0x210
	leaq	-112(%rbp), %rax
	xorl	%ecx, %ecx
	movl	$32, %edx
                                        ## kill: def $rdx killed $edx
	movq	___stack_chk_guard@GOTPCREL(%rip), %r8
	movq	(%r8), %r8
	movq	%r8, -8(%rbp)
	movl	$0, -396(%rbp)
	movl	%edi, -400(%rbp)
	movq	%rsi, -408(%rbp)
	movq	l___const.main.A(%rip), %rsi
	movq	%rsi, -48(%rbp)
	movq	l___const.main.A+8(%rip), %rsi
	movq	%rsi, -40(%rbp)
	movq	l___const.main.A+16(%rip), %rsi
	movq	%rsi, -32(%rbp)
	movq	l___const.main.A+24(%rip), %rsi
	movq	%rsi, -24(%rbp)
	movq	l___const.main.B(%rip), %rsi
	movq	%rsi, -80(%rbp)
	movq	l___const.main.B+8(%rip), %rsi
	movq	%rsi, -72(%rbp)
	movq	l___const.main.B+16(%rip), %rsi
	movq	%rsi, -64(%rbp)
	movq	l___const.main.B+24(%rip), %rsi
	movq	%rsi, -56(%rbp)
	movq	%rax, %rsi
	movq	%rsi, %rdi
	movl	%ecx, %esi
	movq	%rax, -520(%rbp)        ## 8-byte Spill
	callq	_memset
	movq	-520(%rbp), %rax        ## 8-byte Reload
	movq	%rax, -392(%rbp)
	movq	-392(%rbp), %rdx
	movupd	(%rdx), %xmm0			### __m128d c1 = _mm_loadu_pd( C+0 ); ###
	movapd	%xmm0, -432(%rbp)
	addq	$16, %rax
	movq	%rax, -120(%rbp)
	movq	-120(%rbp), %rax
	movupd	(%rax), %xmm0			### __m128d c2 = _mm_loadu_pd( C+2 ); ###
	movapd	%xmm0, -448(%rbp)
	movl	$0, -452(%rbp)
####################### LOOP START HERE #######################
LBB0_1:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$2, -452(%rbp)
	jge	LBB0_4
## %bb.2:                               ##   in Loop: Header=BB0_1 Depth=1
	movl	-452(%rbp), %eax
	addl	%eax, %eax
	movslq	%eax, %rcx
	leaq	-48(%rbp,%rcx,8), %rcx
	movq	%rcx, -128(%rbp)
	movq	-128(%rbp), %rcx
	movupd	(%rcx), %xmm0			### __m128d a  = _mm_loadu_pd( A+i*2 ); ###
	movapd	%xmm0, -480(%rbp)		### __m128d b1 = _mm_load1_pd( B+0+i ); ###
	movslq	-452(%rbp), %rcx
	leaq	-80(%rbp,%rcx,8), %rcx
	movq	%rcx, -136(%rbp)
	movq	-136(%rbp), %rcx
	movsd	(%rcx), %xmm0           ## xmm0 = mem[0],zero
	movsd	%xmm0, -144(%rbp)
	movsd	-144(%rbp), %xmm0       ## xmm0 = mem[0],zero
	movddup	%xmm0, %xmm0            ## xmm0 = xmm0[0,0]
	movapd	%xmm0, -160(%rbp)
	movapd	-160(%rbp), %xmm0
	movapd	%xmm0, -496(%rbp)		### __m128d b2 = _mm_load1_pd( B+2+i ); ###
	movslq	-452(%rbp), %rcx
	leaq	-64(%rbp,%rcx,8), %rcx
	movq	%rcx, -168(%rbp)
	movq	-168(%rbp), %rcx
	movsd	(%rcx), %xmm0           ## xmm0 = mem[0],zero
	movsd	%xmm0, -176(%rbp)
	movsd	-176(%rbp), %xmm0       ## xmm0 = mem[0],zero
	movddup	%xmm0, %xmm0            ## xmm0 = xmm0[0,0]
	movapd	%xmm0, -192(%rbp)
	movapd	-192(%rbp), %xmm0
	movapd	%xmm0, -512(%rbp)
	movapd	-432(%rbp), %xmm0
	movapd	-480(%rbp), %xmm1
	movapd	-496(%rbp), %xmm2
	movapd	%xmm1, -208(%rbp)
	movapd	%xmm2, -224(%rbp)
	movapd	-208(%rbp), %xmm1
	mulpd	-224(%rbp), %xmm1		### _mm_mul_pd( a, b1 ) ###
	movapd	%xmm0, -240(%rbp)
	movapd	%xmm1, -256(%rbp)
	movapd	-240(%rbp), %xmm0
	addpd	-256(%rbp), %xmm0		### c1 = _mm_add_pd( c1, _mm_mul_pd( a, b1 ) );
	movapd	%xmm0, -432(%rbp)
	movapd	-448(%rbp), %xmm0
	movapd	-480(%rbp), %xmm1
	movapd	-512(%rbp), %xmm2
	movapd	%xmm1, -272(%rbp)
	movapd	%xmm2, -288(%rbp)
	movapd	-272(%rbp), %xmm1
	mulpd	-288(%rbp), %xmm1		### _mm_mul_pd( a, b2 ) ###
	movapd	%xmm0, -304(%rbp)
	movapd	%xmm1, -320(%rbp)
	movapd	-304(%rbp), %xmm0		### c2 = _mm_add_pd( c2, _mm_mul_pd( a, b2 ) ); ###
	addpd	-320(%rbp), %xmm0
	movapd	%xmm0, -448(%rbp)
## %bb.3:                               ##   in Loop: Header=BB0_1 Depth=1
	movl	-452(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -452(%rbp)
	jmp	LBB0_1
####################### LOOP END HERE #######################
LBB0_4:
	leaq	-112(%rbp), %rax
	movapd	-432(%rbp), %xmm0
	movq	%rax, -328(%rbp)
	movapd	%xmm0, -352(%rbp)
	movapd	-352(%rbp), %xmm0
	movq	-328(%rbp), %rcx
	movupd	%xmm0, (%rcx)			### _mm_storeu_pd( C+0, c1 ); ###
	addq	$16, %rax
	movapd	-448(%rbp), %xmm0
	movq	%rax, -360(%rbp)
	movapd	%xmm0, -384(%rbp)
	movapd	-384(%rbp), %xmm0
	movq	-360(%rbp), %rax
	movupd	%xmm0, (%rax)			### _mm_storeu_pd( C+2, c2 ); ###
	movsd	-48(%rbp), %xmm0        ## xmm0 = mem[0],zero
	movsd	-32(%rbp), %xmm1        ## xmm1 = mem[0],zero
	movsd	-80(%rbp), %xmm2        ## xmm2 = mem[0],zero
	movsd	-64(%rbp), %xmm3        ## xmm3 = mem[0],zero
	movsd	-112(%rbp), %xmm4       ## xmm4 = mem[0],zero
	movsd	-96(%rbp), %xmm5        ## xmm5 = mem[0],zero
	leaq	L_.str(%rip), %rdi
	movb	$6, %al
	callq	_printf
	movsd	-40(%rbp), %xmm0        ## xmm0 = mem[0],zero
	movsd	-24(%rbp), %xmm1        ## xmm1 = mem[0],zero
	movsd	-72(%rbp), %xmm2        ## xmm2 = mem[0],zero
	movsd	-56(%rbp), %xmm3        ## xmm3 = mem[0],zero
	movsd	-104(%rbp), %xmm4       ## xmm4 = mem[0],zero
	movsd	-88(%rbp), %xmm5        ## xmm5 = mem[0],zero
	leaq	L_.str.1(%rip), %rdi
	movl	%eax, -524(%rbp)        ## 4-byte Spill
	movb	$6, %al
	callq	_printf
	movq	___stack_chk_guard@GOTPCREL(%rip), %rcx
	movq	(%rcx), %rcx
	movq	-8(%rbp), %rdi
	cmpq	%rdi, %rcx
	movl	%eax, -528(%rbp)        ## 4-byte Spill
	jne	LBB0_6
## %bb.5:
	xorl	%eax, %eax
	addq	$528, %rsp              ## imm = 0x210
	popq	%rbp
	retq
LBB0_6:
	callq	___stack_chk_fail
	ud2
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__const
	.p2align	4               ## @__const.main.A
l___const.main.A:
	.quad	4607182418800017408     ## double 1
	.quad	4611686018427387904     ## double 2
	.quad	4613937818241073152     ## double 3
	.quad	4616189618054758400     ## double 4

	.p2align	4               ## @__const.main.B
l___const.main.B:
	.quad	4613937818241073152     ## double 3
	.quad	0                       ## double 0
	.quad	0                       ## double 0
	.quad	4611686018427387904     ## double 2

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"|%g %g| * |%g %g| = |%g %g|\n"

L_.str.1:                               ## @.str.1
	.asciz	"|%g %g|   |%g %g|   |%g %g|\n"


.subsections_via_symbols
