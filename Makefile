all: System 

.PHONY=all clean run-qemu bochs

run-qemu: System 
	@qemu-system-i386 -boot a -fda System 

bochs:
	bochs -q

clean:
	@- rm -f *.o bootsect setup System
	@- rm -f console.s

OBJS =

VPATH = ./boot:./kernel:init

System: bootsect setup kernel.o 
	@dd if=bootsect of=System bs=512 count=1
	@dd if=setup of=System bs=512 count=4 seek=1
	@dd if=kernel.o of=System bs=512 seek=5
	@echo "System Image built done"
#	@rm -f *.o 
#	@rm -f bootsect setup 


bootsect.o: bootsect.s
	@as --32 ./boot/bootsect.s -o bootsect.o

bootsect: bootsect.o ld-bootsect.ld
	@- ld -T ld-bootsect.ld bootsect.o -o bootsect
	@- objcopy -O binary -j .text bootsect

setup.o: ./boot/setup.s
	@as --32 ./boot/setup.s -o setup.o

setup: setup.o ld-bootsect.ld
	@ld -T ld-bootsect.ld setup.o -o setup
	@objcopy -O binary -j .text setup

head.o: head.s
	@as --32 ./boot/head.s -o head.o
OBJS += head.o

main.o: main.c
	@gcc -m32 -c ./init/main.c -o main.o
OBJS += main.o

console.o: console.c
	@gcc -m32 -c ./kernel/console.c -o console.o
OBJS += console.o

asm.o: asm.s
	@as --32 -c ./kernel/asm.s -o asm.o
OBJS += asm.o

traps.o: traps.c
	@gcc -m32 -c ./kernel/traps.c -o traps.o
OBJS += traps.o

itoa.o: itoa.c
	@gcc -m32 -c ./kernel/itoa.c -o itoa.o
OBJS += itoa.o

#system_call.o: system_call.s
#	@as --32 ./kernel/system_call.s -o system_call.o
#OBJS += system_call.o

#sched.o: sched.c
#	@gcc -m32 ./kernel/sched.c -o sched.o
#OBJS += sched.o

kernel.o: $(OBJS) ld-bootsect.ld
	@ld -T ld-bootsect.ld $(OBJS) -o kernel.o
	@objcopy -O binary -R .note -R .comment kernel.o
