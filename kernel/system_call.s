.code32

.globl timer_interrupt

timer_interrupt:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi
	pushl %ebp
	push %ds
	push %es
	push %fs
	mov $0x10,%edx
	mov %dx,%ds
	mov %dx,%es
	mov %dx,%fs
	movb $0x20,%al
	outb %al,$0x20
	call do_timer
	pop %fs
	pop %es
	pop %ds
	popl %ebp
	popl %esi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret

#timer_interrupt:
#	push %ds
#	push %es
#	push %fs
#	pushl %edx
#	pushl %ecx
#	pushl %ebx
#	pushl %eax
#	movl $0x10,%eax
#	mov %ax,%ds
#	mov %ax,%es
##	movl $0x17,%eax
##	mov %ax,%fs
#//	incl _jiffies
#	movb $0x20,%al
#	outb %al,$0x20
#//	movl CS(%esp),%eax
#//	andl $3,%eax
#	call do_timer
#//	addl $4,%esp
#//	jmp ret_from_sys_call
#	iret
