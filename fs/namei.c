#include "../include/types.h"
#include "../include/fs.h"

struct m_inode inode_table[64];

static struct m_inode * get_dir(const char * pathname,struct m_inode * inode){
	return NULL;
}

static struct m_inode * dir_namei(const char * pathname,int * namelen,const char ** name,
		struct m_inode * base){
	char c;
	const char * basename;
	struct m_inode * dir;

//	if(!(dir = get_dir(pathname,base)))
//		return NULL;
	basename = pathname;
	while(c=get_fs_byte(pathname++)){
		if(c=='/'){
			basename = pathname;
		}
	}
	*namelen = pathname - basename - 1;
	*name = basename;
	printk("dir_namei\n");
	return dir;
}

void namei(const char * pathname){

}

struct m_inode * fat16_namei(const char * filename){
	struct buffer_head * bh;
	struct m_inode * inode;
	unsigned short start_cluster;
	int dev,block;
       	char c,c1,c2;
	char * basename;
	long i_size;

	inode = NULL;
	dev = root_inode.i_dev;
	block = root_inode.i_num;
	basename = filename;

label1:
	c = get_fs_byte(basename);
	if('/' == c){
//		printk("dev=%x,block=%d\n",dev,block);
		bh = bread(dev,block);

//		printk("%c",c);
		basename++;
		int i,j,len,match,off;
		for(i=0;i<32;i++){
			match = 0;
			off = 0;
			if(0xE5 != (unsigned char)bh->b_data[0x00 + i*32]
			&& 0x0F != (unsigned char)bh->b_data[0x0B + i*32]
			&& 0x00 != (unsigned char)bh->b_data[0x0B + i*32]){
				for(len=0;len<11;len++){
					c1 = get_fs_byte(basename+len+off);
					c1 = (c1>= 97 && c1 <=122)? c1-32 : c1; 
					c2 = (unsigned char)bh->b_data[len + i*32];
					if(0x20 == c2){
						if(8 >= len && '\0' == c1){
							match = 2;
							break;
						}
						if(8 > len){
							if('/' == c1){
								c2 = (unsigned char)bh->b_data[8+i*32];
								if(0x20 == c2){
									match = 1;
									basename += len;	
									break;
								}
							}else if('.' == c1){
								len = 7;
								off = 7 - len - 1;
								c2 = '.';
							}
						}
					}
					if(c1 != c2){
						match = 0;
						break;
					}
			//		printk("%c",c1);
					match = 1;
					if(10 == len){
						c1 = get_fs_byte(basename + off + 11);
						if('\0' == c1){
							match = 2;
						}else{
							match = 0;
						}
					}
				}
				if(match){
					start_cluster = *((unsigned short *)&bh->b_data[0x1A + i*32]);
					block = root_inode.i_num * 2 + 32 + (start_cluster - 2)*4;
					block /= 2;
					if(2 == match){
			//			printk(" MATCH start_cluster=%d",start_cluster);
						i_size = *((unsigned long *)&bh->b_data[0x1C + i*32]);
						for(len=0;len<64;len++){
							if(!inode_table[len].i_count){
								inode = inode_table+len;
								break;
							}
						}
						inode->i_num = block;
						inode->i_dev = dev;
						inode->i_size = i_size;
						inode->i_count = 1;
			//			printk(" dev=%x,block=%d,file_size=%dbytes\n",dev,block,i_size);
					}
					goto label1;
				}
			}
		}
	}

	return inode;
}

int open_namei(const char * pathname,int flag,int mode,
		struct m_inode ** res_inode){
	const char * basename;
	int dev,inr,namelen;
	struct m_inode *dir,*inode;
	struct buffer_head * bh;

	dir = fat16_namei(pathname);
	if(NULL == dir){
		return 0;
	}
	*res_inode = dir;
	return 1;
}
