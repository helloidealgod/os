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
	
	#use bios to read system data,and save in 0x90000 ~ 0x901fd

	#start change real model to protected model 

	#disable interrupt and copy system to 0x00000
	cli #disable interrupt
	    #copy system
	sti #enable interrupt	

	#set GDT & IDT

	#open A20,and check it

	#reprogram interruptor

	#set cr0,set pe = 1
	ljmp $0x1000,$0x0000
msg:
	.byte 13,10
	.ascii "You've successfully load the setup into RAM"
	.byte 13,10
