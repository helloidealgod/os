.code32
.text
#entry start
start:
	pushl $0
	pushl $0
	pushl $0
	pushl $L6
	pushl $main
	ret
L6:
	jmp L6
