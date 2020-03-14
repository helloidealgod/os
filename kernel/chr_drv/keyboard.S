.text
.globl keyboard_interrupt
size = 1024
head = 4
tail = 8
proc_list = 12
buf = 16

mode:	.byte 0
leds:	.byte 2
e0:	.byte 0

keyboard_interrupt:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	push %ds
	push %es
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	xor %al,%al
	inb $0x60,%al
	cmpb $0xe0,%al
	je set_e0
	cmpb $0xe1,%al
	je set_e1
#	call key_table(,%eax,4)
#	movb $0,e0

	push $keyboard_msg
	call printk

e0_e1:	inb $0x61,%al
	jmp 1f
1:	jmp 1f
1:	orb $0x80,%al
	jmp 1f
1:	jmp 1f
1:	outb %al,$0x61
	jmp 1f
1:	jmp 1f
1:	andb $0x7f,%al
	outb %al,$0x61
	movb $0x20,%al
	outb %al,$0x20

#	pushl $0
#	call do_tty_interrupt
#	addl $4,%esp
	
	popl %eax
	
	pop %es
	pop %ds
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret
set_e0:	movb $1,e0
	jmp e0_e1
set_e1:	movb $2,e0
	jmp e0_e1
	
keyboard_msg:
	.asciz "hello keyboard\n"

key_map:
	.byte 0,27
	.ascii "1234567890-="
	.byte 127,9
	.ascii "qwertyuiop[]"
	.byte 13,0
	.ascii "asdfghjkl;'"
	.byte '`,0
	.ascii "//zxcvbnm,./"
	.byte 0,'*,0,32 /* 36-39 */
	.fill 16,1,0 /* 3A-49 */
	.byte '-,0,0,0,'+ /* 4A-4E */
	.byte 0,0,0,0,0,0,0 /* 4F-55 */
	.byte '<
	.fill 10,1,0

shift_map:
	.byte 0,27
	.ascii "!@#$%^&*()_+"
	.byte 127,9
	.ascii "QWERTYUIOP{}"
	.byte 13,0
	.ascii "ASDFGHJKL:/""
	.byte '~,0
	.ascii "|ZXCVBNM<>?"
	.byte 0,'*,0,32 /* 36-39 */
	.fill 16,1,0 /* 3A-49 */
	.byte '-,0,0,0,'+ /* 4A-4E */
	.byte 0,0,0,0,0,0,0 /* 4F-55 */
	.byte '>
	.fill 10,1,0

alt_map:
	.byte 0,0
	.ascii "/0@/0$/0/0{[]}///0"
	.byte 0,0
	.byte 0,0,0,0,0,0,0,0,0,0,0
	.byte '~,13,0
	.byte 0,0,0,0,0,0,0,0,0,0,0
	.byte 0,0
	.byte 0,0,0,0,0,0,0,0,0,0,0
	.byte 0,0,0,0 /* 36-39 */
	.fill 16,1,0 /* 3A-49 */
	.byte 0,0,0,0,0 /* 4A-4E */
	.byte 0,0,0,0,0,0,0 /* 4F-55 */
	.byte '|
	.fill 10,1,0