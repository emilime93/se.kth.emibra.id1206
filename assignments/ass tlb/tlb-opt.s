	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 14
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4               ## -- Begin function main
LCPI0_0:
	.long	1127219200              ## 0x43300000
	.long	1160773632              ## 0x45300000
	.long	0                       ## 0x0
	.long	0                       ## 0x0
LCPI0_1:
	.quad	4841369599423283200     ## double 4503599627370496
	.quad	4985484787499139072     ## double 1.9342813113834067E+25
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3
LCPI0_2:
	.quad	4696837146684686336     ## double 1.0E+6
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
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
	pushq	%rax
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	leaq	L_str(%rip), %rdi
	callq	_puts
	xorl	%r14d, %r14d
	movl	$1, %r13d
	.p2align	4, 0x90
LBB0_1:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB0_6 Depth 2
                                        ##     Child Loop BB0_8 Depth 2
                                        ##     Child Loop BB0_3 Depth 2
	incl	%r14d
	movl	$1048576, %eax          ## imm = 0x100000
	xorl	%edx, %edx
	divl	%r13d
	movl	%eax, %r12d
	callq	_clock
	movq	%rax, %r15
	movl	$1048576, %eax          ## imm = 0x100000
	xorl	%edx, %edx
	divl	%r14d
	cmpl	$1, %eax
	movl	$1, %ecx
	cmovbel	%ecx, %eax
	cmpl	$3, %eax
	ja	LBB0_4
## %bb.2:                               ##   in Loop: Header=BB0_1 Depth=1
	xorl	%ecx, %ecx
	xorl	%ebx, %ebx
	jmp	LBB0_3
	.p2align	4, 0x90
LBB0_4:                                 ##   in Loop: Header=BB0_1 Depth=1
	movl	%eax, %ecx
	andl	$2097148, %ecx          ## imm = 0x1FFFFC
	movq	%r13, %xmm0
	pshufd	$68, %xmm0, %xmm0       ## xmm0 = xmm0[0,1,0,1]
	leal	-4(%rcx), %edi
	movl	%edi, %esi
	shrl	$2, %esi
	incl	%esi
	movl	%esi, %edx
	andl	$7, %edx
	pxor	%xmm1, %xmm1
	pxor	%xmm2, %xmm2
	cmpl	$28, %edi
	jb	LBB0_7
## %bb.5:                               ##   in Loop: Header=BB0_1 Depth=1
	subl	%edx, %esi
	pxor	%xmm1, %xmm1
	pxor	%xmm2, %xmm2
	.p2align	4, 0x90
LBB0_6:                                 ##   Parent Loop BB0_1 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	paddq	%xmm0, %xmm1
	paddq	%xmm0, %xmm2
	paddq	%xmm0, %xmm1
	paddq	%xmm0, %xmm2
	paddq	%xmm0, %xmm1
	paddq	%xmm0, %xmm2
	paddq	%xmm0, %xmm1
	paddq	%xmm0, %xmm2
	paddq	%xmm0, %xmm1
	paddq	%xmm0, %xmm2
	paddq	%xmm0, %xmm1
	paddq	%xmm0, %xmm2
	paddq	%xmm0, %xmm1
	paddq	%xmm0, %xmm2
	paddq	%xmm0, %xmm1
	paddq	%xmm0, %xmm2
	addl	$-8, %esi
	jne	LBB0_6
LBB0_7:                                 ##   in Loop: Header=BB0_1 Depth=1
	testl	%edx, %edx
	je	LBB0_9
	.p2align	4, 0x90
LBB0_8:                                 ##   Parent Loop BB0_1 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	paddq	%xmm0, %xmm1
	paddq	%xmm0, %xmm2
	decl	%edx
	jne	LBB0_8
LBB0_9:                                 ##   in Loop: Header=BB0_1 Depth=1
	paddq	%xmm2, %xmm1
	pshufd	$78, %xmm1, %xmm0       ## xmm0 = xmm1[2,3,0,1]
	paddq	%xmm1, %xmm0
	movq	%xmm0, %rbx
	cmpl	%ecx, %eax
	je	LBB0_10
	.p2align	4, 0x90
LBB0_3:                                 ##   Parent Loop BB0_1 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	incl	%ecx
	addq	%r13, %rbx
	cmpl	%r12d, %ecx
	jb	LBB0_3
LBB0_10:                                ##   in Loop: Header=BB0_1 Depth=1
	callq	_clock
	subq	%r15, %rax
	movq	%rax, %xmm0
	punpckldq	LCPI0_0(%rip), %xmm0 ## xmm0 = xmm0[0],mem[0],xmm0[1],mem[1]
	subpd	LCPI0_1(%rip), %xmm0
	haddpd	%xmm0, %xmm0
	divsd	LCPI0_2(%rip), %xmm0
	movb	$1, %al
	leaq	L_.str.1(%rip), %rdi
	movl	%r13d, %esi
	movq	%rbx, %rdx
	callq	_printf
	incq	%r13
	cmpq	$17, %r13
	jne	LBB0_1
## %bb.11:
	xorl	%eax, %eax
	addq	$8, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str.1:                               ## @.str.1
	.asciz	"%d\t %.4f\t %ld\n"

	.p2align	4               ## @str
L_str:
	.asciz	"#pages\t proc\t\t sum"


.subsections_via_symbols
