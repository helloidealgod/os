.code16

.text

.equ SETUPSEG,0x9020
.equ SYSSEG,0x1000
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
	mov $0,%bx
	mov $2048,%cx
_mov_system:
	mov $SYSSEG,%ax
	mov %ax,%ds
	mov %ds:(%bx),%dl

	mov $0,%ax
	mov %ax,%ds
	mov %dl,%ds:(%bx)

	inc %bx
	loop _mov_system

	sti #enable interrupt	

	#set GDT & IDT
	mov $SETUPSEG,%ax
	mov %ax,%ds
#	lidt idt_48  #set IDTR,from ds:idt_48 total 48 bytes
	lgdt gdt_48  #set GDTR,from ds:gdt_48 total 48 bytes

//	call empty_8042
//	mov $0xd1,%al
//	out %al,$0x64
	
//	call empty_8042
//	mov $0xdf,%al
//	out %al,$0x60
	
//	call empty_8042

//empty_8042:
//	.word 0x00eb,0x00eb
//	in $0x64,%al
//	test $2,%al
//	jnz empty_8042
//	ret
	#open A20,and check it

	#reprogram interruptor

	#set cr0,set pe = 1
	mov %cr0,%eax
	or  $1,%eax   #cr0 seg no.0 is pe bit
	mov %eax,%cr0

	ljmp $8,$0    # jump to 0x0000:0x0000
gdt:
	.word 0,0,0,0 #GDT,48 bytes,first GDT dummy
	#second GDT
	.word 0x07ff  #8Mb, limit 2047
	.word 0x0000  #base address 0
	.word 0x9a00  #code segment,readonly,excutable 
	.word 0x00c0  #granularity=4096 i386,粒度4096 / 32,
		      #D/B bit 0=16,1=32,
		      #no.7 byte,no.7bit 0xc0 = 11000000
	
	.word 0x07ff  #8Mb, limit 2047
	.word 0x0000  #base address 0
	.word 0x9200  #data segment,read/write
	.word 0x00c0  #granularity=4096 /32 bit

idt_48:
	.word 0
	.word 0,0
gdt_48:
	.word 0x800
	.word 512+gdt,0x9
msg:
	.byte 13,10
	.ascii "You've successfully load the setup into RAM"
	.byte 13,10
