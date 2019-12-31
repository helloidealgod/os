.code16

.global bootstart

.equ BOOTSEG,0x07c0
.equ INITSEG,0x9000
.equ SETUPSEG,0x9020
.equ SYSSEG,0x1000
.equ SECTORS,85		#最多128个扇区，否则bx溢出

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
	mov $SYSSEG,%ax
	mov %ax,%es
	mov $5,%ax	#相对扇区号-1
	mov %ax,%si
read_sectors:
	mov %si,%ax
	mov $36,%bl	# ax / bl = al商,ah余
	div %bl		# (dxax) / bx = ax商，dx余数
	mov %al,track	# ch=(相对扇区号-1)/36
	
	mov %si,%ax
	mov $18,%bl
	div %bl
	inc %ah
	mov %ah,sector	# cl=(相对扇区号-1)%18+1

	mov %si,%ax
	mov $18,%bl
	div %bl
	xor %ah,%ah
	mov $2,%bl
	div %bl
	mov %ah,head	#dh=((相对扇区号-1)/18)%2

	mov %si,%ax
	sub $5,%ax
	mov $512,%bx
	mul %bx		#8位*8位=ax,16位*16位=dxax
	mov %ax,%bx	#实时模式，bx=16位
	mov track,%ah
	mov sector,%al
	mov %ax,%cx
	mov head,%ah
	mov $0,%al
	mov %ax,%dx
	mov $0x0201,%ax
	int $0x13

	add $1,%si
	mov %si,%ax
	sub $5,%ax
	cmp $SECTORS,%ax
	jne read_sectors
	jmp system_load_ok

system_load_ok:
	mov $SETUPSEG,%ax
	mov %ax,%ds
	ljmp $0x9020,$0	
string:
	.ascii "hello bootloader!"
	.byte 13,10

track:	.byte 0
sector:	.byte 0
head:	.byte 0

.=510

signature:
	.word 0xaa55
