all: System 

.PHONY=clean run-qemu

run-qemu: System 
	@qemu-system-i386 -boot a -fda System 

bochs:
	bochs -q

clean:
	@- rm -f *.o bootsect setup System

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

System: bootsect setup 
	@dd if=bootsect of=System bs=512 count=1
	@dd if=setup of=System bs=512 count=4 seek=1
	@echo "System Image built done"


