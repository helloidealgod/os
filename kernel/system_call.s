.code32

SIG_CHLD = 17
EAX = 0x00
EBX = 0x04
ECX = 0x08
EDX = 0x0C
FS  = 0x10
ES  = 0x14
DS  = 0x18
EIP = 0x1C
CS  = 0x20
EFLAGS = 0x24
OLDESP = 0x28
OLDSS  = 0x2C

state = 0
counter = 4
priority = 8
signal = 12
sigaction = 16
blocked = (33*16)

sa_handler = 0
sa_mask = 4
sa_flags = 8
sa_restorer = 12

nr_system_calls = 72

.globl timer_interrupt,system_call,sys_fork,sys_printk,hd_interrupt

.align 2
bad_system_call:
	movl $-1,%eax
	iret

.align 2
reschedule:
	pushl $ret_from_sys_call
	jmp schedule

.align 2	
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
	call sys_call_table(,%eax,4) #call sys_call_table + 2*4
	pushl %eax
	movl current,%eax
	cmpl $0,state(%eax)
	jne reschedule
	cmpl $0,counter(%eax)
	je reschedule		

ret_from_sys_call:
	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	pop %fs
	pop %es
	pop %ds
	iret

.align 2
timer_interrupt:
	push %es
	push %ds
	push %fs
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	mov $0x10,%edx
	mov %dx,%ds
	mov %dx,%es
	mov %dx,%fs
	incl jiffies
	movb $0x20,%al
	outb %al,$0x20
	movl CS(%esp),%eax
	andl $3,%eax		#eax is CPL (3:USER,0:supervisor)
	pushl %eax
	call do_timer
	addl $4,%esp
	jmp ret_from_sys_call

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
	addl $20,%esp	#相当于 popl eax ebp edi esi gs (all 32bit)
1:	ret

hd_interrupt:
	pushl %eax
	pushl %ecx
	pushl %edx
	push %ds
	push %es
	push %fs

	movb $0x20,%al
	outb %al,$0xA0
	movl $unexpected_hd_interrupt,%edx
	outb %al,$0x20
#	call *%edx
	
	pop %fs
	pop %es
	pop %ds
	popl %edx
	popl %ecx
	popl %eax
	iret

unexpected_msg:
	.asciz "upexpected hd interrupt"
