.code32

.text

.equ SYSSEG,0x1000
.equ LEN,54
.global idt,gdt, _pg_dir

_pg_dir:
starup_32:
	mov $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
#	lss _stack_start,%esp
	mov %ax,%ss
	mov $0x1e25c,%ebp
	mov %ebp,%esp

	call setup_idt
	call setup_gdt

	mov $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs
#	lss _stack_start,%esp
	mov %ax,%ss
	mov $0x1e25c,%ebp
	mov %ebp,%esp

	#check is A20 open
1:	mov $24,%eax
	mov %eax,0x000000
	cmp %eax,0x100000
	je 1b
	
	movl %cr0,%eax
	andl $0x80000011,%eax
	orl $2,%eax
	movl %eax,%cr0
	call check_x87
	jmp after_page_tables 

check_x87:
	fninit
	fstsw %ax
	cmpb $0,%al
	je 1f
	movl %cr0,%eax
	xorl $6,%eax
	ret
.align 2
1:	.byte 0xdb,0xe4
	ret
setup_idt:
//	lea ignore_int,%edx
	lea interrupt0,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	lea idt,%edi
//	mov $256,%ecx
//rp_sidt:
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi

	lea interrupt1,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi

	lea interrupt2,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi

	lea interrupt3,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi

	lea interrupt4,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi

	lea interrupt5,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi

	lea interrupt6,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi

	lea interrupt7,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	
	lea interrupt8,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi

	lea interrupt9,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	
	lea interrupt10,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	
	lea interrupt11,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	
	lea interrupt12,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	
	lea interrupt13,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	
	lea interrupt14,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	
	lea interrupt15,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	
	lea interrupt16,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi

	lea ignore_int,%edx
	movl $0x00080000,%eax
	movw %dx,%ax
	movw $0x8e00,%dx
	mov $240,%ecx
rp_sidt:
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	dec %ecx
	jne rp_sidt
	lidt idt_descr
	ret

setup_gdt:
	lgdt gdt_descr
	ret

.org 0x1000
pg0:
.org 0x2000
pg1:
.org 0x3000
pg2:
.org 0x4000
pg3:

.org 0x5000
_tmp_floppy_area:
	.fill 1024,1,0
after_page_tables:
	pushl $0
	pushl $0
	pushl $0
	pushl $L6
	pushl $main
	ret
L6:
	jmp L6
int_msg:
	.asciz "Unknown interrupt\r\n"
.align 2
ignore_int:
	pushl %eax
	pushl %ecx
	pushl %edx
	push %ds

	push %es
	push %fs
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%fs
	push $int_msg
	call printk

	popl %eax
	pop %fs
	pop %es
	pop %ds
	popl %edx
	popl %ecx
	popl %eax
	iret
.align 2
setup_paging:
	movl $1024*5,%ecx
	xorl %eax,%eax
	xorl %edi,%edi
	cld;rep;stosl
	movl $pg0+7,_pg_dir
	movl $pg1+7,_pg_dir+4
	movl $pg2+7,_pg_dir+8
	movl $pg3+7,_pg_dir+12
	
	movl $pg3+4096,%edi
	movl $0xfff007,%eax

	std
1:	stosl
	subl $0x1000,%eax
	jge 1b

	xorl %eax,%eax
	movl %eax,%cr3
	movl %cr0,%eax
	orl $0x80000000,%eax
	movl %eax,%cr0
	ret
.align 2
.word 0
idt_descr:
	.word 256*8-1
	.long idt
.align 2
.word 0
gdt_descr:
	.word 256*8-1
	.long gdt

#	.align 3
idt: 	.fill 256,8,0

gdt:	.quad 0x0000000000000000	#NULL
	.quad 0x00c09a0000000fff	#16Mb 0x08 kernel code
	.quad 0x00c0920000000fff	#16Mb 0x10 kernel data
	.quad 0x0000000000000000	#temporary don't use
	.fill 252,8,0			#space for ldt and tss
