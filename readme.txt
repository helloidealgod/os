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


笔记：
	1.中断表
		typedef struct desc_struct {
			unsigned long a,b;
		}desc_table[256];
		extern desc_table idt中断表,gdt全局描述符表;
	
	2.页表管理16M
	3.物理内存表
		mem_map[] 4kb一项，100未占用，0未空闲，管理1M~末尾内存
	5.进程表
	struct task_struct{
		long state;
		long counter;
		long priority;
		long signal;
		struct sigaction sigaction[32];
		long blocked;
		int exit_code;
		unsigned long start_code,end_code,end_data,brk,start_stack;
		long pid,father,pgrp,session,leader;
		unsigned short uid,euid,suid;
		unsigned short gid,egid,sgid;
		long alarm;
		long utime,stime,cutime,cstime,start_time;
		unsigned short used_math;
		int tty;
		unsigned short umask;
		unsigned long close_on_exec;
		struct desc_struct ldt[3];
		struct tss_struct tss;
	};

	union task_union{
		struct task_struct task;
		char stack[4096];
	};
	struct task_struct * task[NR_TASKS] 64个
	6.当前进程current
	7.设备表：0虚拟盘、1软盘、2硬盘、3ttyx、4tty、5并行口、6非命名管道
		blk_dev[7]{
			*do_xx_request(),current * request
		}
		request[32]{
			dev、cmd、errors、起始扇区、读写扇区数量、char * buffer,task_struct * waiting、
			buffer_head * bh、下一个请求项 * next (电梯算法)
		}
		高速缓存表的主要内容
		*hash_table[307] 已使用的
		*free_list 空闲的
		buffer_head{
			char * b_data; 指向1024字节的缓存区
			unsigned long b_blocknr; 逻辑块号
			unsigned short b_dev;
			unsigned char b_uptodata; unsigned char b_dirt;
			unsigned char b_count;
			unsigned char b_lock;
			struct task_struct * b_wait;
			struct buffer_head * b_prev; //hash
			struct buffer_head * b_next; //hash
			struct buffer_head * b_prev_free;
			struct buffer_head * b_next_free;
		}
		读写流程：
			a.找到要读写的磁盘块号
			b.bread()找到空闲的缓存项->->ll_rw_block(bh)->make_request(bh)(组装request)
			c.add_request(bh)->执行do_hd_request()
			d.bh->b_wait休眠等待读写完成
			
	8.struct tty_struct tty_table[3]{
			struct termios termios;//驱动设备属性
			int pgrp;int stopped;
			void (*write)(struct tty_struct * tty);//写函数指针，具体设备的写处理，会在tty_write中被调用！
			struct tty_queue read_q;	//tty读队列 原始数据
			struct tty_queue write_q;	//tty写队列
			struct tty_queue secondary;	//tty辅助队列，存放将read_q中数据经过行规则处理后的cooked数据。
		};
		struct tty_queue {
			unsigned long data;	缓冲区中数据个数
			unsigned long head; 用于在buf中实现循环数组
			unsigned long tail;
			struct task_struct * proc_list;//等待本队列的进程列表
			char buf[TTY_BUF_SIZE];//缓存区长度为1024
		};
		键盘输入流程：
			a.按键引发键盘中断，获取字符码，将原始数据放到read_q中
			b.调用do_tty_interrupt(int tty)，处理read_q,将处理后的数据放到secondary中，若需回显，将数据也放如write_q中
			c.调用tty->write(tty);
			
	9.标准输入输出
	10.文件的打开、关闭、读写
