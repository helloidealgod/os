#include "../include/fs.h"
#include "../include/elf.h"

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
//	eip[0] = ex.e_entry;
//	eip[3] = 0xc000000-4;
	return 0;
}
