#include "../include/fs.h"
#include "../include/elf.h"
#include "../include/sched.h"

#define NULL ((void *)0)
int do_execve(unsigned long * eip,long tmp,char * filename,char ** argv,char ** envp){
	struct m_inode * inode = NULL;
	struct buffer_head * bh;
	struct elf32_head ex;

	if((0xffff & eip[1]) != 0x000f){
		panic("execuve from supervisor mode");
	}
	inode = fat16_namei(filename);
	if(!inode){
		return -1;
	}
	bh = bread(inode->i_dev,inode->i_num);
	ex = *((struct elf32_head *)bh->b_data);
	//此时任务2的基址0x8000000，段限640k
	//1.释放页表及物理内存
	//2.根据elf头部信息设置ldt的基址和段限
	printk("e_entry=%x\n",ex.e_entry);
	//eip+cs=实际执行的指令地址，task2
	//故实际指令地址0x080480a4=cs(0x08000000)-eip,eip=0x480a4
//	eip[0] = ex.e_entry;
//	eip[0] = 0x480a4;
//	eip[3] = 0xc000000-4;
/*	unsigned long code_base,code_limit,data_base,data_limit;
	code_limit = get_limit(0x0f);
	data_limit = get_limit(0x17);
	code_base = get_base(current->ldt[1]);
	data_base = get_base(current->ldt[2]);
	printk("code_limit=%x,data_limit=%x\n",code_limit,data_limit);
	printk("code_base=%x,data_base=%x\n",code_base,data_base);
*/	return 0;
}
