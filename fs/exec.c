#include "../include/fs.h"
#define NULL ((void *)0)
int do_execve(unsigned long * eip,long tmp,char * filename,char ** argv,char ** envp){
	struct m_inode * inode = NULL;
	inode = fat16_namei(filename);
	if(!inode){
		printk("inode = NULL\n");
		printk("%s",filename);
	}else{
		printk("inode != NULL\n");
		printk("dev=%x,block=%d,i_size=%d\n",inode->i_dev,inode->i_num,inode->i_size);
	}
	return 0;
}
