all: System 

.PHONY=clean run-qemu

run-qemu: System 
	@qemu-system-i386 -boot a -fda System 

bochs:
	bochs -q

clean:
	@- rm -f *.o bootsect setup System
	@- rm -f console.s kernel

OBJS =

System: bootsect setup kernel 
	@dd if=bootsect of=System bs=512 count=1
	@dd if=setup of=System bs=512 count=4 seek=1
	@dd if=kernel of=System bs=512 seek=5
	@echo "System Image built done"
	@rm -f *.0 

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
OBJS += head.o

main.o: main.c
	@gcc -m32 -c main.c -o main.o
OBJS += main.o

console.o: console.c
	@gcc -m32 -c console.c -o console.o
OBJS += console.o

asm.o: asm.s
	@as --32 -c asm.s -o asm.o
OBJS += asm.o

traps.o: traps.c
	@gcc -m32 -c traps.c -o traps.o
#OBJS += traps.o


kernel: head.o $(OBJS) ld-bootsect.ld
	@ld -T ld-bootsect.ld $(OBJS) -o kernel
	@objcopy -O binary -R .note -R .comment kernel
