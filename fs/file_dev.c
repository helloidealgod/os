#include "../include/fs.h"

int file_read(struct file * file,struct m_inode * inode,char * buf,int count){
	struct buffer_head * bh;
	unsigned long file_size = inode->i_size;
	int i;
	int dev,block;
	
	dev = inode->i_dev;
	block = inode->i_num;

//	printk("in file_read:dev=%x,block=%d,file_size=%d,buf=%d\n",dev,block,file_size,buf);
	bh = bread(dev,block);
	if(bh){
		for(i=0;i<count;i++){
			if(i <= file_size){
				put_fs_byte(bh->b_data[i],buf+i);
			}else{
				put_fs_byte(0,buf+i);
			}
		}
	}
	return count;
}
