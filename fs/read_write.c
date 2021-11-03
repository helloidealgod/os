#include "../include/fs.h"
#include "../include/sched.h"

int sys_read(unsigned int fd,char * buff,int count){
	printk("in sys_read\n");
	struct file * file;
	struct m_inode * inode;
	
	if(fd >= 32 || 0 > count || !(file = current->filp[fd])){
		return -1;
	}
	if(!count){
		return 0;
	}
	inode = file->f_inode;

	return file_read(file,inode,buff,count);
}
int sys_write(unsigned int fd,char * buff,int count){
	int i;
	if(1==fd){
		for(i=0;i<count;i++){
			printk("%c",get_fs_byte(buff+i));
		}
	}
	return 0;
}
