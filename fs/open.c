#include "../include/sched.h"

int sys_open(const char * filename,int flag,int mode){
	struct m_inode * inode;
	struct file * f;
	int i,fd;
	for(fd=0;fd<20;fd++){
		if(!current->filp[fd]){
			break;
		}
	}
	if(fd>20){
		return -1;
	}
	printk("sys_open\n");
	i = open_namei(filename,flag,mode,&inode);
	return 0;
}

int sys_close(unsigned int fd){
	return 0;
}
