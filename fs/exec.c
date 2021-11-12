#include "../include/fs.h"
#include "../include/elf.h"
#include "../include/sched.h"

#define NULL ((void *)0)
#define MAX_ARG_PAGES 32

static int count(char ** argv){
	int i = 0;
	char ** tmp;
	if(tmp =  argv){
		while(get_fs_long((unsigned long *)(tmp++))){
			i++;
		}
	}
	return i;
}

int do_execve(unsigned long * eip,long tmp,char * filename,char ** argv,char ** envp){
	struct m_inode * inode = NULL;
	struct buffer_head * bh;
	struct elf32_head ex;
	unsigned long page[MAX_ARG_PAGES];
	unsigned long p = 4096 * MAX_ARG_PAGES - 4;
	int i,argc,envc;

	if((0xffff & eip[1]) != 0x000f){
		panic("execuve from supervisor mode");
	}
	for(i=0;i<MAX_ARG_PAGES;i++){
		page[i] = 0;
	}
	inode = fat16_namei(filename);
	if(!inode){
		return -1;
	}
	argc = count(argv);
	envc = count(envp);
	bh = bread(inode->i_dev,inode->i_num);
	ex = *((struct elf32_head *)bh->b_data);
	//此时任务2的基址0x8000000，段限640k
	//1.释放页表及物理内存
	free_page_tables(get_base(current->ldt[1]),get_limit(0x0f));
	free_page_tables(get_base(current->ldt[2]),get_limit(0x17));
	//2.根据elf头部信息设置ldt的基址和段限
	printk("e_entry=%x\n",ex.e_entry);
	current->executable = inode;
	//eip+cs=实际执行的指令地址，task2
	//故实际指令地址0x080480a4=cs(0x08000000)-eip,eip=0x480a4
//	eip[0] = ex.e_entry;
	eip[0] = 0x480a4;
//	eip[3] = 0xc000000-4;
	eip[3] = 0xa0000;
	return 0;
}
