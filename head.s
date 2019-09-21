.code32

.text

.equ SYSSEG,0x1000
.equ LEN,54

show_text:
	#mov $SYSSEG,%ax
	#mov %ax,%es
	#mov $0x03,%ah
	#xor %bh,%bh
	#int $0x10

	#mov $0x000a,%bx
	#mov $0x1301,%ax
	#mov $LEN,%cx
	#mov $msg,%bp
	#int $0x10
	mov $0x10,%ax
	mov %ax,%ds
#	mov %ax,%es
#	mov %ax,%ss
	cli
