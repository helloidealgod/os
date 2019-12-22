.code16

.global bootstart

.equ BOOTSEG,0x07c0
.equ INITSEG,0x9000
.equ SETUPSEG,0x9020
.equ SYSSEG,0x1000
.equ SECTORS,72

.text

ljmp $BOOTSEG,$bootstart

bootstart:
	mov $0x03,%ah
	int $0x10	#get cursor position
	
	mov $BOOTSEG,%ax
	mov %ax,%es
	mov $string,%bp
	mov $0x1301,%ax
	mov $0x000a,%bx
	mov $19,%cx
	int $0x10	#printf string

	# copy bootsect from 0x7c00:0x0000 to 0x9000:0x0000 total 512 bytes 
	cld
	mov $BOOTSEG,%ax
	mov %ax,%ds
	mov $INITSEG,%ax
	mov %ax,%es
	mov $0,%ax
	mov %ax,%si
	mov %ax,%di
	mov $512,%cx
	rep movsb

	ljmp $INITSEG,$set_reg
set_reg:
	mov %cs,%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%ss
	mov $0xFF00,%sp
load_setup:
	mov $0x0000,%dx
	mov $0x0002,%cx
	mov $SETUPSEG,%ax
	mov %ax,%es
	mov $0x0000,%bx  # load data to %es:%bx
	mov $0x02,%ah
	mov $4,%al
	int $0x13	

	jnc load_system #load setup ok then load system
	jmp load_setup

load_system:
	mov $0x0000,%dx  #DL driver number(0=a),
			 #DH head number(0 - 15)
	mov $0x0006,%cx  #CH track/cylinder number (0 - 1023)
			 #CL sector number(1 - 17)
	mov $SYSSEG,%ax
	mov %ax,%es
	mov $0x0000,%bx  # load data to %es:%bx
	mov $0x02,%ah     
	mov $SECTORS,%al       # number of sectors to read
	int $0x13

	jnc system_load_ok
	jmp load_system

system_load_ok:
	mov $SETUPSEG,%ax
	mov %ax,%ds
	ljmp $0x9020,$0	
string:
	.ascii "hello bootloader!"
	.byte 13,10
.=510

signature:
	.word 0xaa55
