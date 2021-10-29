
int sys_read(unsigned int fd,char * buff,int count){
	return 0;
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
