.code16

.global _bootstart

.equ BOOTSEG,0x07c0
.equ INITSEG,0x9000
.equ SETUPSEG,0x9020
.equ SYSSEG,0x1000

.text

ljmp $BOOTSEG,$_bootstart

_bootstart:
	mov $0x03,%ah
	int $0x10	#get cursor position
	
	mov $BOOTSEG,%ax
	mov %ax,%es
	mov $_string,%bp
	mov $0x1301,%ax
	mov $0x000a,%bx
	mov $20,%cx
	int $0x10	#printf string

	# copy bootsect from 0x7c00:0x0000 to 0x9000:0x0000 total 512 bytes 
	mov $0,%bx
	mov $512,%cx
_copy_bootsect:
	mov $BOOTSEG,%ax
	mov %ax,%ds
	mov %ds:(%bx),%dl
	
	mov $INITSEG,%ax
	mov %ax,%ds
	mov %dl,%ds:(%bx)
	
	inc %bx
	loop _copy_bootsect

	ljmp $INITSEG,$_set_reg
_set_reg:
	mov %cs,%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%ss
	mov $0xFF00,%sp
_load_setup:
	mov $0x0000,%dx
	mov $0x0002,%cx
	mov $SETUPSEG,%ax
	mov %ax,%es
	mov $0x0000,%bx  # load data to %es:%bx
	mov $0x02,%ah
	mov $4,%al
	int $0x13
	jnc _setup_load_ok
	jmp _load_setup

_setup_load_ok:
	mov $SETUPSEG,%ax
	mov %ax,%ds
	ljmp $0x9020,$0			
	
_string:
	.ascii "hello bootloader!"
	.byte 13,10,13,10
.=510

signature:
	.word 0xaa55
