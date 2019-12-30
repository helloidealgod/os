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
#	jnc start_read
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
	mov $13,%al  
#	mov $SECTORS,%al       # number of sectors to read
	int $0x13
	
	jnc load_one
	jmp load_system
load_one:
	mov $0x0000,%dx  #DL driver number(0=a),
			 #DH head number(0 - 15)
	mov $0x010C,%cx  #CH track/cylinder number (0 - 1023)
			 #CL sector number(1 - 17)
	mov $SYSSEG,%ax
	mov %ax,%es
	mov $0x5400,%bx  # load data to %es:%bx
	mov $0x02,%ah     
	mov $72,%al
	int $0x13
	
	jnc system_load_ok
	jmp load_one
start_read:
	mov $6,%ax
	mov %ax,%si
read_sectors:
	sub $1,%si
	mov %si,%ax
	mov $36,%bx	# ax / bl = al商,ah余
	div %bx		# (dxax) / bx = ax商，dx余数
	mov %ax,track	# ch=(相对扇区号-1)/36
	
	xor %dx,%dx
	mov %si,%ax
	mov $18,%bx
	div %bx
	inc %dx
	mov %dx,sector	# cl=(相对扇区号-1)%18+1

	xor %dx,%dx
	mov %si,%ax
	mov $18,%bx
	div %bx

	xor %dx,%dx
	mov $2,%bx
	div %bx
	mov %dx,head	#dh=((相对扇区号-1)/18)%2

	mov %si,%ax
	sub $5,%ax
	mov $512,%bx
	mul %bx
	mov %ax,%bx

	add $2,%si
	cmp $72,%si
	jne read_sectors
	jmp system_load_ok
#test_one:
#	mov $0x0100,%dx  #DL driver number(0=a),
#			 #DH head number(0 - 15)
#	mov $0x0001,%cx  #CH track/cylinder number (0 - 1023)
#			 #CL sector number(1 - 17)
#	mov $SYSSEG,%ax
#	mov %ax,%es
#	mov $0x1a00,%bx  # load data to %es:%bx
#	mov $0x02,%ah     
#	mov $18,%al
#	int $0x13
#
#	mov $0x0000,%dx  #DL driver number(0=a),
#			 #DH head number(0 - 15)
#	mov $0x0101,%cx  #CH track/cylinder number (0 - 1023)
#			 #CL sector number(1 - 17)
#	mov $SYSSEG,%ax
#	mov %ax,%es
#	mov $0x3e00,%bx  # load data to %es:%bx
#	mov $0x02,%ah     
#	mov $18,%al
#	int $0x13
#
#	mov $0x0100,%dx  #DL driver number(0=a),
#			 #DH head number(0 - 15)
#	mov $0x0101,%cx  #CH track/cylinder number (0 - 1023)
#			 #CL sector number(1 - 17)
#	mov $SYSSEG,%ax
#	mov %ax,%es
#	mov $0x6200,%bx  # load data to %es:%bx
#	mov $0x02,%ah     
#	mov $18,%al
#	int $0x13
#
#	mov $0x0000,%dx  #DL driver number(0=a),
#			 #DH head number(0 - 15)
#	mov $0x0201,%cx  #CH track/cylinder number (0 - 1023)
#			 #CL sector number(1 - 17)
#	mov $SYSSEG,%ax
#	mov %ax,%es
#	mov $0x8600,%bx  # load data to %es:%bx
#	mov $0x02,%ah     
#	mov $18,%al
#	int $0x13
#
#	jnc system_load_ok
#	jmp load_system
#
system_load_ok:
	mov $SETUPSEG,%ax
	mov %ax,%ds
	ljmp $0x9020,$0	
string:
	.ascii "hello bootloader!"
	.byte 13,10

track:	.word 0
sector:	.word 0
head:	.word 0

.=510

signature:
	.word 0xaa55
