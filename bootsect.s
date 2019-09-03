.code16

.global _bootstart

.equ BOOTSEG,0x07c0

.text

ljmp $BOOTSEG,$_bootstart

_bootstart:
	mov $0x03,%ah
	int $0x10	#get cursor position
	
	mov $BOOTSEG,%ax
	mov %ax,%es
	mov $_string,%bp
	mov $0x13,%ah
	mov $0x01,%al
	mov $0x00,%bh
	mov $0x07,%bl
	mov $20,%cx
	int $0x10
loop: jmp loop

_string:
	.ascii "hello bootloader!"
	.byte 13,10,13,10
.=510

signature:
	.word 0xaa55
