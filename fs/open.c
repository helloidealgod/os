#include "../include/sched.h"
#include "../include/fs.h"

int sys_open(const char * filename,int flag,int mode){
	struct m_inode * inode;
	struct file * f;
	int i,fd;

	for(fd=0;fd<20;fd++){
		if(!current->filp[fd]){
			break;
		}
	}
	if(fd>=20){
		//current task has not place to open another file
		return -1;
	}

	//find an empty file_table item
	f = 0 + file_table;
	for(i=0;i<64;i++,f++){
		if(!f->f_count){
			break;
		}
	}

//	printk("sys_open:fd=%d,file_table=%d\n",fd,i);
	i = open_namei(filename,flag,mode,&inode);
	if(!i){
		return -1;
	}
	f->f_count++;
	current->filp[fd] = f;
	f->f_flags = flag;
	f->f_count = 1;
	f->f_pos = 0;
	f->f_inode = inode;
	return fd;
}

int sys_close(unsigned int fd){
	return 0;
}
