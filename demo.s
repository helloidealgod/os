.code16

.text

.equ DEMOSEG,0x1020
.equ LEN,47
.global myprint
.global show_text

show_text:
	mov $DEMOSEG,%ax
	mov %ax,%es
	mov $0x03,%ah
	xor %bh,%bh
	int $0x10

	mov $0x000a,%bx
	mov $0x1301,%ax
	mov $LEN,%cx
	mov $msg,%bp
	int $0x10

#	call mainppushl $main
	#pushl $main
	pushl $0
	pushl $0
	pushl $0
	pushl $main
	#pushl $show_text + 0x01ff
	ret

loop_forever:
	jmp loop_forever
myprint:
	movl 4(%esp),%ecx
	movl 8(%esp),%edx
	movl $1,%ebx
	movl $4,%eax
	int $0x80
	ret
msg:
	.byte 13,10
	.ascii "You've successfully load the floppy int RAM"
	.byte 13,10

.align 4
