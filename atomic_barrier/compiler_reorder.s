	.file	"compiler_reorder.c"
	.text
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	movl	j(%rip), %edx
	movl	i(%rip), %eax
	testl	%edx, %edx
	movl	%eax, a(%rip)
	leal	15(%rdx), %eax
	cmovns	%edx, %eax
	sarl	$4, %eax
	movl	%eax, b(%rip)
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.globl	j
	.bss
	.align 4
	.type	j, @object
	.size	j, 4
j:
	.zero	4
	.globl	i
	.align 4
	.type	i, @object
	.size	i, 4
i:
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
