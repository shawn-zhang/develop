	.file	"bios.c"
	.section	.rodata
	.align 4
.LC0:
	.string	"%s[0x%x, %u]: mmap(, %u,,,, 0x%x) failed: %s\n"
	.align 4
.LC1:
	.string	"%s[0x%x, %u]: mmap(, %u,,,, 0x%x) ok\n"
	.text
	.globl	read_mmap
	.type	read_mmap, @function
read_mmap:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$148, %esp
	.cfi_offset 3, -12
	call	getpagesize
	movl	%eax, -32(%ebp)
	cmpl	$0, 20(%ebp)
	je	.L2
	cmpl	$0, 8(%ebp)
	jne	.L3
.L2:
	movl	$0, %eax
	jmp	.L4
.L3:
	movl	12(%ebp), %eax
	movl	20(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	call	memset
	movl	-32(%ebp), %eax
	negl	%eax
	andl	16(%ebp), %eax
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %eax
	movl	16(%ebp), %edx
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	movl	%eax, -24(%ebp)
	movl	-24(%ebp), %eax
	movl	%eax, %edx
	addl	20(%ebp), %edx
	movl	-32(%ebp), %eax
	addl	%edx, %eax
	leal	-1(%eax), %edx
	movl	-32(%ebp), %eax
	negl	%eax
	andl	%edx, %eax
	movl	%eax, -20(%ebp)
	movl	$0, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	open
	movl	%eax, -16(%ebp)
	cmpl	$-1, -16(%ebp)
	jne	.L5
	movl	$0, %eax
	jmp	.L4
.L5:
	leal	-120(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	fstat
	testl	%eax, %eax
	jne	.L6
	movl	-104(%ebp), %eax
	andl	$61440, %eax
	cmpl	$32768, %eax
	jne	.L6
	movl	-76(%ebp), %eax
	movl	%eax, %edx
	movl	16(%ebp), %eax
	addl	20(%ebp), %eax
	cmpl	%eax, %edx
	jae	.L6
	movl	-76(%ebp), %eax
	cmpl	16(%ebp), %eax
	jle	.L7
	movl	-76(%ebp), %eax
	subl	16(%ebp), %eax
	movl	%eax, 20(%ebp)
	jmp	.L6
.L7:
	movl	$0, 20(%ebp)
.L6:
	cmpl	$0, 20(%ebp)
	jne	.L8
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	close
	movl	$0, %eax
	jmp	.L4
.L8:
	movl	-28(%ebp), %eax
	movl	%eax, 20(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	$2, 12(%esp)
	movl	$1, 8(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	$0, (%esp)
	call	mmap
	movl	%eax, -12(%ebp)
	cmpl	$-1, -12(%ebp)
	jne	.L9
	call	__errno_location
	movl	(%eax), %eax
	movl	%eax, (%esp)
	call	strerror
	movl	-28(%ebp), %ebx
	movl	16(%ebp), %ecx
	movl	$.LC0, %edx
	movl	%eax, 24(%esp)
	movl	%ebx, 20(%esp)
	movl	-20(%ebp), %eax
	movl	%eax, 16(%esp)
	movl	20(%ebp), %eax
	movl	%eax, 12(%esp)
	movl	%ecx, 8(%esp)
	movl	8(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	%edx, (%esp)
	call	printf
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	close
	movl	$0, %eax
	jmp	.L4
.L9:
	movl	-28(%ebp), %ecx
	movl	16(%ebp), %edx
	movl	$.LC1, %eax
	movl	%ecx, 20(%esp)
	movl	-20(%ebp), %ecx
	movl	%ecx, 16(%esp)
	movl	20(%ebp), %ecx
	movl	%ecx, 12(%esp)
	movl	%edx, 8(%esp)
	movl	8(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	movl	-12(%ebp), %edx
	movl	-24(%ebp), %eax
	addl	%eax, %edx
	movl	12(%ebp), %eax
	movl	20(%ebp), %ecx
	movl	%ecx, 8(%esp)
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	memcpy
	movl	-20(%ebp), %eax
	movl	%eax, 4(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, (%esp)
	call	munmap
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	close
	movl	$1, %eax
.L4:
	addl	$148, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_def_cfa 4, 4
	.cfi_restore 5
	ret
	.cfi_endproc
.LFE0:
	.size	read_mmap, .-read_mmap
	.section	.rodata
.LC2:
	.string	"/dev/mem"
.LC3:
	.string	"BIOS ROM DATA:"
.LC4:
	.string	" %02x"
.LC5:
	.string	"BIOS RAM DATA:"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%edi
	pushl	%ebx
	andl	$-16, %esp
	subl	$262448, %esp
	movl	12(%ebp), %eax
	movl	%eax, 28(%esp)
	movl	%gs:20, %eax
	movl	%eax, 262444(%esp)
	xorl	%eax, %eax
	movl	$0, 40(%esp)
	leal	44(%esp), %eax
	movl	$262144, %edx
	movl	%edx, 8(%esp)
	movl	$0, 4(%esp)
	movl	%eax, (%esp)
	.cfi_offset 3, -16
	.cfi_offset 7, -12
	call	memset
	leal	262188(%esp), %ebx
	movl	$0, %eax
	movl	$64, %edx
	movl	%ebx, %edi
	movl	%edx, %ecx
	rep stosl
	movl	$262144, 12(%esp)
	movl	$786432, 8(%esp)
	leal	44(%esp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC2, (%esp)
	call	read_mmap
	movl	$256, 12(%esp)
	movl	$1024, 8(%esp)
	leal	262188(%esp), %eax
	movl	%eax, 4(%esp)
	movl	$.LC2, (%esp)
	call	read_mmap
	movl	$.LC3, %eax
	movl	%eax, (%esp)
	call	printf
	movl	$0, 40(%esp)
	jmp	.L11
.L13:
	movl	40(%esp), %eax
	andl	$31, %eax
	testl	%eax, %eax
	jne	.L12
	movl	$10, (%esp)
	call	putchar
.L12:
	leal	44(%esp), %eax
	addl	40(%esp), %eax
	movzbl	(%eax), %eax
	movzbl	%al, %edx
	movl	$.LC4, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	addl	$1, 40(%esp)
.L11:
	cmpl	$262143, 40(%esp)
	jle	.L13
	movl	$10, (%esp)
	call	putchar
	movl	$.LC5, %eax
	movl	%eax, (%esp)
	call	printf
	movl	$0, 40(%esp)
	jmp	.L14
.L16:
	movl	40(%esp), %eax
	andl	$31, %eax
	testl	%eax, %eax
	jne	.L15
	movl	$10, (%esp)
	call	putchar
.L15:
	leal	262188(%esp), %eax
	addl	40(%esp), %eax
	movzbl	(%eax), %eax
	movzbl	%al, %edx
	movl	$.LC4, %eax
	movl	%edx, 4(%esp)
	movl	%eax, (%esp)
	call	printf
	addl	$1, 40(%esp)
.L14:
	cmpl	$255, 40(%esp)
	jle	.L16
	movl	$10, (%esp)
	call	putchar
	movl	$0, %eax
	movl	262444(%esp), %edx
	xorl	%gs:20, %edx
	je	.L17
	call	__stack_chk_fail
.L17:
	leal	-8(%ebp), %esp
	popl	%ebx
	.cfi_restore 3
	popl	%edi
	.cfi_restore 7
	popl	%ebp
	.cfi_def_cfa 4, 4
	.cfi_restore 5
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
	.section	.note.GNU-stack,"",@progbits
