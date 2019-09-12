all: Image 

bootsect.o: bootsect.s
	@as --32 bootsect.s -o bootsect.o

.PHONY=clean run-qemu

run-qemu: Image 
	@qemu-system-i386 -boot a -fda Image

bochs:
	bochs -q

bootsect: bootsect.o ld-bootsect.ld
	@- ld -T ld-bootsect.ld bootsect.o -o bootsect
	@- objcopy -O binary -j .text bootsect

demo: demo.o ld-bootsect.ld hello.o
	#@ld -T ld-bootsect.ld demo.o hello.o -o demo
	@ld -T ld-bootsect.ld -Ttext 0 demo.o hello.o -o demo.sym
	@strip demo.sym -o demo.o
	@objcopy -O binary -j .text demo.o demo
	#@objcopy -O binary -j .text demo

demo.o: demo.s
	@as --32 demo.s -o demo.o
hello.o: hello.c
	@gcc -m32 -c hello.c -o hello.o
Image: bootsect demo
	@dd if=bootsect of=Image bs=512 count=1
	@dd if=demo of=Image bs=512 count=4 seek=1
	@echo "Image built done"

clean:
	@- rm -f *.o bootsect demo Image *.sym
