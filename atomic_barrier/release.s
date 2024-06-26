	.file	"release.c"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"release.c"
.LC1:
	.string	"a == 11"
	.text
	.p2align 4
	.globl	thread_main
	.type	thread_main, @function
thread_main:
.LFB1:
	.cfi_startproc
	cmpl	$12, b(%rip)
	je	.L2
.L3:
	jmp	.L3
	.p2align 4,,10
	.p2align 3
.L2:
	cmpl	$11, a(%rip)
	jne	.L9
	xorl	%eax, %eax
	ret
.L9:
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$__PRETTY_FUNCTION__.0, %ecx
	movl	$15, %edx
	movl	$.LC0, %esi
	movl	$.LC1, %edi
	call	__assert_fail
	.cfi_endproc
.LFE1:
	.size	thread_main, .-thread_main
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	xorl	%ecx, %ecx
	movl	$thread_main, %edx
	xorl	%esi, %esi
	leaq	8(%rsp), %rdi
	call	pthread_create
	movl	$1, %edi
	call	sleep
	xorl	%esi, %esi
	movl	$12, b(%rip)
	movl	$11, a(%rip)
	movl	$21, aint(%rip)
	movq	8(%rsp), %rdi
	call	pthread_join
	xorl	%eax, %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.section	.rodata
	.align 8
	.type	__PRETTY_FUNCTION__.0, @object
	.size	__PRETTY_FUNCTION__.0, 12
__PRETTY_FUNCTION__.0:
	.string	"thread_main"
	.globl	aint
	.bss
	.align 4
	.type	aint, @object
	.size	aint, 4
aint:
	.zero	4
	.globl	b
	.align 4
	.type	b, @object
	.size	b, 4
b:
	.zero	4
	.globl	a
	.align 4
	.type	a, @object
	.size	a, 4
a:
	.zero	4
	.ident	"GCC: (GNU) 13.2.1 20240316 (Red Hat 13.2.1-7)"
	.section	.note.GNU-stack,"",@progbits
