all: System 

AS	=gas
LD	=gld
LDFLAGS	=-s -x
CC	=gcc
CFLAGS	=-Wall -fstrength-reduce -fomit-frame-pointer \
	 -finline-functions

.PHONY=clean run-qemu

run-qemu: System 
	@qemu-system-i386 -boot a -fda System 

bochs:
	bochs -q

clean:
	@- rm -f *.o bootsect setup head System
	@- rm -f console.s kernel.o

bootsect.o: bootsect.s
	@as --32 bootsect.s -o bootsect.o

bootsect: bootsect.o ld-bootsect.ld
	@- ld -T ld-bootsect.ld bootsect.o -o bootsect
	@- objcopy -O binary -j .text bootsect

setup.o: setup.s
	@as --32 setup.s -o setup.o

setup: setup.o ld-bootsect.ld
	@ld -T ld-bootsect.ld setup.o -o setup
	@objcopy -O binary -j .text setup

head.o: head.s
	@as --32 head.s -o head.o

head: head.o main.o console.o ld-bootsect.ld
	@ld -T ld-bootsect.ld head.o main.o console.o -o head
	@objcopy -O binary -j .text head
main.o: main.c
	@gcc -m32 -c main.c -o main.o

#console.s:console.c
#	@gcc -m32 -S console.c -o console.s
console.o: console.c
	@gcc -m32 -c console.c -o console.o
#	@as --32 -c console.c -o console.o
#kernel.o: console.o
#	@ld -T ld-bootsect.ld console.o -o kernel.o
#	@sync
#kernel:	kernel.o
#	@echo "hello kernel.o"
System: bootsect setup head 
	@dd if=bootsect of=System bs=512 count=1
	@dd if=setup of=System bs=512 count=4 seek=1
	@dd if=head of=System bs=512 seek=5
	@echo "System Image built done"
	@rm -f *.0 

