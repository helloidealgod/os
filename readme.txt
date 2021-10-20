#新增内核代码时，注意修改bootsect.s和setup.s中读取磁盘扇区数量的修改
1 bootsect.s
2 makefile
3 as --32 bootsect.s
4 objdump -m i8086 -S bootsect.o
5 objcopy -O binary bootsect.o
6 .ld 
7 .code16 & .code32

实时模式 -- 》保护模式
分段机制 -- 》分页机制

内存管理
进程调度
进程通信
文件系统
网络

boot/
fs/
include/
init/
kernel/
lib/
mm/
tools/


挂载img
losetup /dev/loop0 16.img
分区 fdisk /dev/loop0

mount /dev/loop0 /mnt
umount /mnt

losetup -d /dev/loop0
