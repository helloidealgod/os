all: System 

.PHONY=all clean run-qemu bochs

run-qemu: System 
	@qemu-system-i386 -m 16M -boot a -fda System 

bochs:
	bochs -q

clean:
	@- rm -f *.o bootsect setup System
	@- rm -f console.s
CFLAGS=-fomit-frame-pointer
OBJS =

VPATH = boot:kernel:init:mm:kernel/chr_drv:kernel/blk_drv:fs

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

#itoa.o: itoa.c
#	@gcc -m32 -c ./kernel/itoa.c -o itoa.o
#OBJS += itoa.o

system_call.o: system_call.s
	@as --32 ./kernel/system_call.s -o system_call.o
OBJS += system_call.o

sched.o: sched.c
	@gcc -m32 -fomit-frame-pointer -c ./kernel/sched.c -o sched.o
#-fomit-frame-pointer 不加这个参数 则set_tssldt_desc 这个宏编译不通过 asm operand has imposible constraints
OBJS += sched.o

memory.o: memory.c
	@gcc -m32 -c ./mm/memory.c -o memory.o
OBJS += memory.o

page.o: page.s
	@as --32 ./mm/page.s -o page.o
OBJS += page.o

fork.o: fork.c
	@gcc -m32 -fomit-frame-pointer -c ./kernel/fork.c -o fork.o
OBJS += fork.o

keyboard.o: keyboard.s
	@as --32 ./kernel/chr_drv/keyboard.s -o keyboard.o
OBJS += keyboard.o

tty_io.o: tty_io.c
	@gcc -m32 -c ./kernel/chr_drv/tty_io.c -o tty_io.o
OBJS += tty_io.o

signal.o: signal.c
	@gcc -m32 -c ./kernel/signal.c -o signal.o
OBJS += signal.o

printk.o: printk.c
	@gcc -m32 -c ./kernel/printk.c -o printk.o
OBJS += printk.o

vsprintf.o: vsprintf.c
	@gcc -m32 -c ./kernel/vsprintf.c -o vsprintf.o
OBJS += vsprintf.o

hd.o: hd.c
	@gcc -m32 -c ./kernel/blk_drv/hd.c -o hd.o
OBJS += hd.o

read_write.o: read_write.c
	@gcc -m32 -c ./fs/read_write.c -o read_write.o
OBJS += read_write.o

open.o: open.c
	@gcc -m32 -c ./fs/open.c -o open.o
OBJS += open.o

panic.o: panic.c
	@gcc -m32 -c ./kernel/panic.c -o panic.o
OBJS += panic.o

buffer.o: buffer.c
	@gcc -m32 -c ./fs/buffer.c -o buffer.o
OBJS += buffer.o

ll_rw_blk.o: ll_rw_blk.c
	@gcc -m32 -c ./kernel/blk_drv/ll_rw_blk.c -o ll_rw_blk.o
OBJS += ll_rw_blk.o

namei.o: namei.c
	@gcc -m32 -c ./fs/namei.c -o namei.o
OBJS += namei.o

exec.o: exec.c
	@gcc -m32 -c ./fs/exec.c -o exec.o
OBJS += exec.o

kernel.o: $(OBJS) ld-bootsect.ld
	@ld -T ld-bootsect.ld $(OBJS) -o kernel.o
	@objcopy -O binary -R .note -R .comment kernel.o
