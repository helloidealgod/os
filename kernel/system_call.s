.code32

//.globl timer_interrupt

timer_interrupt:
	push %ds
	push %es
	push %fs
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	movl $0x17,%eax
	mov %ax,%fs
//	incl _jiffies
	movb $0x20,%al
	outb %al,$0x20
//	movl CS(%esp),%eax
//	andl $3,%eax
	call do_timer
//	addl $4,%esp
//	jmp ret_from_sys_call
	iret
