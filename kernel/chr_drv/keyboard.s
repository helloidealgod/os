.code32
.text
.globl keyboard_interrupt
size = 1024
head = 4
tail = 8
proc_list = 12
buf = 16

mode:	.byte 0
leds:	.byte 2
e0:	.byte 0

keyboard_interrupt:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	push %ds
	push %es
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	xor %al,%al
	inb $0x60,%al

#	push %ax
#	call printks
#	pop %ax

	cmpb $0xe0,%al
	je set_e0
	cmpb $0xe1,%al
	je set_e1
	call key_table(,%eax,4)
	movb $0,e0
e0_e1:	inb $0x61,%al
	jmp 1f
1:	jmp 1f
1:	orb $0x80,%al
	jmp 1f
1:	jmp 1f
1:	outb %al,$0x61
	jmp 1f
1:	jmp 1f
1:	andb $0x7f,%al
	outb %al,$0x61
	movb $0x20,%al
	outb %al,$0x20
	pushl $0
	call do_tty_interrupt
	addl $4,%esp
	pop %es
	pop %ds
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret
set_e0:	movb $1,e0
	jmp e0_e1
set_e1:	movb $2,e0
	jmp e0_e1
	
put_queue: 
	pushl %ecx
	pushl %edx
	movl table_list,%edx
	movl head(%edx),%ecx
1:	movb %al,buf(%edx,%ecx)
	incl %ecx
	andl $size-1,%ecx
	cmpl tail(%edx),%ecx
	je 3f
	shrdl $8,%ebx,%eax
	je 2f
	shrl $8,%ebx
	jmp 1b
2:	movl %ecx,head(%edx)
	movl proc_list(%edx),%ecx
	test %ecx,%ecx
	je 3f
	movl $0,(%ecx)
3:	popl %edx
	popl %ecx
	ret

ctrl:	movb $0x04,%al
	jmp 1f
alt:	movb $0x10,%al
1:	cmpb $0,e0
	je 2f
	addb %al,%al
2:	orb %al,mode
	ret

unctrl:	movb $0x04,%al
	jmp 1f
unalt:	movb $0x10,%al
1:	cmpb $0,e0
	je 2f
	addb %al,%al
2:	notb %al
	andb %al,mode
	ret

lshift:
	orb $0x01,mode
	ret
unlshift:
	andb $0xfe,mode
	ret
rshift:
	orb $0x02,mode
	ret
unrshift:
	andb $0xfd,mode
	ret

do_self:
	lea alt_map,%ebx
	testb $0x20,mode
	jne 1f
	lea shift_map,%ebx
	testb $0x03,mode
	jne 1f
	lea key_map,%ebx
1:	movb (%ebx,%eax),%al
	orb %al,%al
	je none
	testb $0x4c,mode
	je 2f
	cmpb $'a,%al
	jb 2f
	cmpb $'},%al
	ja 2f
	subb $32,%al
2:	testb $0x0c,mode
	je 3f
	cmpb $64,%al
	jb 3f
	cmpb $64+32,%al
	jae 3f
	subb $64,%al
3:	testb $0x10,mode
	je 4f
	orb $0x80,%al
4:	andl $0xff,%eax
	xorl %ebx,%ebx
	call put_queue
none:	ret

key_map:
	.byte	0,27
	.ascii	"1234567890-="
	.byte	127,9
	.ascii	"qwertyuiop[]"
	.byte 	13,0
	.ascii 	"asdfghjkl;'"
	.byte 	'`,0
	.ascii 	"\\zxcvbnm,./"
	.byte 	0,'*,0,32
	.fill 	16,1,0
	.byte 	'-,0,0,0,'+
	.byte 	0,0,0,0,0,0,0
	.byte 	'<
	.fill 	10,1,0

shift_map:
	.byte 	0,27
	.ascii 	"!@#$%^&*()_+"
	.byte 	127,9
	.ascii 	"QWERTYUIOP{}"
	.byte 	13,0
	.ascii 	"ASDFGHJKL:\""
	.byte 	'~,0
	.ascii 	"|ZXCVBNM<>?"
	.byte 	0,'*,0,32
	.fill 	16,1,0
	.byte 	'-,0,0,0,'+
	.byte 	0,0,0,0,0,0,0
	.byte 	'>
	.fill 	10,1,0

alt_map:
	.byte 	0,0
	.ascii 	"\0@\0$\0\0{[]}\\\0"
	.byte 	0,0
	.byte 	0,0,0,0,0,0,0,0,0,0,0
	.byte 	'~,13,0
	.byte 	0,0,0,0,0,0,0,0,0,0,0
	.byte 	0,0
	.byte 	0,0,0,0,0,0,0,0,0,0,0
	.byte 	0,0,0,0
	.fill 	16,1,0
	.byte 	0,0,0,0,0
	.byte 	0,0,0,0,0,0,0
	.byte 	'|
	.fill 	10,1,0

key_table:
	.long none,none,do_self,do_self		/* 12*/
	.long do_self,do_self,do_self,do_self	/*3456*/
	.long do_self,do_self,do_self,do_self	/*7890*/
	.long do_self,do_self,none,none		/*+'*/
	.long do_self,do_self,do_self,do_self	/*qwer*/
	.long do_self,do_self,do_self,do_self	/*tyui*/
	.long do_self,do_self,do_self,do_self	/*op}*/
	.long none,none,do_self,do_self		/*as*/
	.long do_self,do_self,do_self,do_self	/*dfgh*/
	.long do_self,do_self,do_self,do_self	/*jkl|*/
	.long do_self,do_self,lshift,do_self	/*{*/
	.long do_self,do_self,do_self,do_self	/*zxcv*/
	.long do_self,do_self,do_self,do_self	/*bnm,*/
	.long do_self,do_self,rshift,do_self	/*.-*/
	.long none,do_self,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,unlshift,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,unrshift,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none
	.long none,none,none,none