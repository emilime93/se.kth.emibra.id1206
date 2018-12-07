	.file	"switch.c"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Hello "
	.section	.text.startup,"ax",@progbits
	.p2align 4,,15
	.globl	main
	.type	main, @function
main:
.LFB41:
	.cfi_startproc
	subq	$1896, %rsp
	.cfi_def_cfa_offset 1904
	movq	%rsp, %rdi
	movq	%fs:40, %rax
	movq	%rax, 1880(%rsp)
	xorl	%eax, %eax
	call	getcontext@PLT
	leaq	.LC0(%rip), %rdi
	call	puts@PLT
	leaq	944(%rsp), %rdi
	movq	%rsp, %rsi
	call	swapcontext@PLT
	movq	1880(%rsp), %rdx
	xorq	%fs:40, %rdx
	jne	.L8
	xorl	%eax, %eax
	addq	$1896, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L8:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE41:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
