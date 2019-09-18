.code16

.text

.equ SETUPSEG,0x9020
.equ LEN,54

show_text:
	mov $SETUPSEG,%ax
	mov %ax,%es
	mov $0x03,%ah
	xor %bh,%bh
	int $0x10

	mov $0x000a,%bx
	mov $0x1301,%ax
	mov $LEN,%cx
	mov $msg,%bp
	int $0x10
load_system:
	ljmp $0x1000,$0x0000
msg:
	.byte 13,10
	.ascii "You've successfully load the setup into RAM"
	.byte 13,10
