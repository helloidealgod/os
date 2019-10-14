#用于实现大部分硬件异常所引起的中断的汇编语言处理过程
.code32
.globl _divide_error,_debug,_nmi,_int3,_overflow,_bounds,_invalid_op

_divide_error:
	pushl $_do_divide_error #调用的函数地址
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

_debug:
	pushl $_do_int3
	jmp no_error_code

_nmi:
	pushl $_do_nmi
	jmp no_error_code

_int3:
	pushl $_do_int3
	jmp no_error_code

_overflow:
	pushl $_do_overflow
	jmp no_error_code

_bounds:
	pushl $_do_bounds
	jmp no_error_code

_invalid_op:
	pushl $_do_invalid_op
	jmp no_error_code

_reserved:
	pushl $_do_reserved
	jmp no_error_code

_irq13:
	pushl %eax
	xorb %al,%al
	outb %al,$0xf0
	movb $0x20,%al
	outb %al,$0x20
	jmp 1f
	jmp 1f
	outb %al,$0xa0
	popl %eax
	jmp _coprocessor_error

_double_fault:
	pushl $_do_double_fault

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

_invalid_TSS:
	pushl $_do_invalid_TSS
	jmp error_code

_segment_not_present:
	pushl $_do_segment_not_present
	jmp error_code

_stack_segment:
	pushl $_do_stack_segment
	jmp error_code

_general_protection:
	pushl $_do_general_protection
	jmp error_code

