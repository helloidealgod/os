.code32

.globl timer_interrupt,system_call,sys_fork
nr_system_calls = 72

bad_system_call:
	movl $-1,%eax
	iret

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
	popl %edi
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

system_call:
	cmpl $nr_system_calls-1,%eax
	ja bad_system_call
	push %ds
	push %es
	push %fs
	pushl %edx
	pushl %ecx
	pushl %ebx
	movl $0x10,%edx
	mov %dx,%ds
	mov %dx,%es
	mov $0x17,%edx
	mov %dx,%fs
	push $msg
	call printk
	addl $4,%esp
//	call sys_fork
	movl $0,%eax 		#??
	call sys_call_table(,%eax,4) #call sys_call_table + 2*4
	popl %ebx
	popl %ecx
	popl %edx
	pop %fs
	pop %es 	
	pop %ds
	movl $0,%eax
	iret

msg:
	.asciz "hello int 0x80\n"
.align 2

sys_fork:
	call find_empty_process
	testl %eax,%eax
	js 1f
	push %gs
	pushl %esi
	pushl %edi
	pushl %ebp
	pushl %eax
	call copy_process
	addl $20,%esp
1:	ret
	
