1 bootsect.s
2 makefile
3 as --32 bootsect.s
4 objdump -m i8086 -S bootsect.o
5 objcopy -O binary bootsect.o
6 .ld 
