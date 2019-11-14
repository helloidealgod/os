#用于实现大部分硬件异常所引起的中断的汇编语言处理过程
.code32

.globl divide_error,debug,nmi,int3,overflow,bounds,invalid_op
.globl double_fault,coprocessor_segment_overrun
.globl invalid_TSS,segment_not_present,stack_segment
.globl general_potection,coprocessor_error,irq13,reserved
.globl device_not_available,general_protection,page_exception,parallel_interrupt

.globl timer_interrupt

timer_interrupt:
	pushl $do_timer
	jmp no_error_code

divide_error:
	pushl $do_divide_error #调用的函数地址
no_error_code:			#这里是无出错号处理入口
	xchgl %eax,(%esp)
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi
	pushl %ebp
	push %ds
	push %es
	push %fs
	pushl $0
	lea 44(%esp),%edx
	pushl %edx
	movl $0x10,%edx
	mov %dx,%ds
	mov %dx,%es
	mov %dx,%fs
	call *%eax
	addl $8,%esp
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

debug:
	pushl $do_int3
	jmp no_error_code

nmi:
	pushl $do_nmi
	jmp no_error_code

int3:
	pushl $do_int3
	jmp no_error_code

overflow:
	pushl $do_overflow
	jmp no_error_code

bounds:
	pushl $do_bounds
	jmp no_error_code

invalid_op:
	pushl $do_invalid_op
	jmp no_error_code

coprocessor_segment_overrun:
	pushl $do_coprocessor_segment_overrun
	jmp no_error_code

reserved:
	pushl $do_reserved
	jmp no_error_code

irq13:
	pushl %eax
	xorb %al,%al
	outb %al,$0xf0
	movb $0x20,%al
	outb %al,$0x20
	jmp 1f
1:	jmp 1f
1:	outb %al,$0xa0
	popl %eax
	jmp coprocessor_error

double_fault:
	pushl $do_double_fault

error_code:
	xchgl %eax,4(%esp)
	xchgl %ebx,(%esp)
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi
	pushl %ebp
	pushl %ds
	pushl %es
	pushl %fs
	pushl %eax
	lea 44(%esp),%eax
	pushl %eax
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	call *%ebx
	addl $8,%esp
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

invalid_TSS:
	pushl $do_invalid_TSS
	jmp error_code

segment_not_present:
	pushl $do_segment_not_present
	jmp error_code

stack_segment:
	pushl $do_stack_segment
	jmp error_code

general_protection:
	pushl $do_general_protection
	jmp error_code

coprocessor_error:
	pushl $do_coprocessor_error
	jmp error_code

device_not_available:
	pushl $do_device_not_available
	jmp error_code

parallel_interrupt:
	pushl $do_parallel_interrupt
	jmp error_code
page_exception:
	pushl $do_page_exception
	jmp error_code

