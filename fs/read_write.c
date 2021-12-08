#include "../include/fs.h"
#include "../include/sched.h"

int sys_read(unsigned int fd,char * buf,int count){
	struct file * file;
	struct m_inode * inode;
	
	if(fd >= 32 || 0 > count || !(file = current->filp[fd])){
		return -1;
	}
	if(!count){
		return 0;
	}
	inode = file->f_inode;

	return file_read(file,inode,buf,count);
}

int sys_write(unsigned int fd,char * buf,int count){
	int i;
	if(1==fd){
	//	printk("buf addr=%x\n",buf);
		if(buf > 0x8000000){
			buf -= 0x8000000;
		//	printk("buf addr=%x\n",buf);
		}
		for(i=0;i<count;i++){
			printk("%c",get_fs_byte(buf+i));
		}
	}
	return 0;
}
