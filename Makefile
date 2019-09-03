all: Image

bootsect.o: bootsect.s
	@as --32 bootsect.s -o bootsect.o

.PHONY=clean run-qemu

run-qemu: bootsect
	@qemu-system-i386 -boot a -fda bootsect

bootsect: bootsect.o ld-bootsect.ld
	@- ld -T ld-bootsect.ld bootsect.o -o bootsect
	@- objcopy -O binary -j .text bootsect

clean:
	@- rm -f *.o
