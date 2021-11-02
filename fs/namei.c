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
	printk("fat16_namei:i_dev=%x,i_num=%d",root_inode.i_dev,root_inode.i_num);
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
		bh = bread(dev,block);

		basename++;
		int i,j,len,match;
		for(i=0;i<32;i++){
			match = 0;
			if(0xE5 != (unsigned char)bh->b_data[0x00 + i*32]
			&& 0x0F != (unsigned char)bh->b_data[0x0B + i*32]
			&& 0x00 != (unsigned char)bh->b_data[0x0B + i*32]){
				for(len=0;len<8;len++){
					c1 = get_fs_byte(basename+len);
					c1 = (c1>= 97 && c1 <=122)? c1-32 : c1; 
					c2 = (unsigned char)bh->b_data[len + i*32];
					if(0x20 == c2){
						if('.' == c1){
							printk("%c",c1);
						//	c2 = (unsigned char)bh->b_data[0x08 + j + i*32];
							for(j=0;j<1;j++){
							//	c1 = get_fs_byte(basename+len+j);
								c1 = (c1>= 97 && c1 <=122)? c1-32 : c1; 
								printk("c1=%c,c2=%c\n",c1,c2);
							/*	if(0x20 == c2){
									if('\0' == c1){
										printk(" ");
										match = 1;
										basename += len;
									}
								}
								if(c1 != c2){
									match = 0;
									break;
								}
								printk("%c",c2);
								if(2 == j){
									basename += len;
								}
						*/	}
						}
						if('/'== c1 || '\0' == c1){
							printk(" ");
							match = 1;
							basename += len;
						}
						break;
					}
					if(c1 != c2){
						match = 0;
						break;
					}
					match = 1;
					printk("%c",c2);
					if(7==len){
						basename += len;
					}
				}
				if(match){
					c1 = get_fs_byte(basename);
					start_cluster = (unsigned short)bh->b_data[0x1A + i*32];
					printk("MATCH start_cluster=%d,c=%c\n",start_cluster,c1);
					block = root_inode.i_num * 2 + 32 + (start_cluster - 2)*4;
					block /= 2;
					if('.' == c1 || '\0' == c1){
						i_size = (unsigned long)bh->b_data[0x1C + i*32];
						inode = inode_table+0;
						inode->i_num = block;
						inode->i_dev = dev;
						inode->i_size = i_size;
						printk("file_size=%dbytes\n",i_size);
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
	struct dir_entry * de;

/*	printk("open_namei\n");
	if(!(dir = dir_namei(pathname,&namelen,&basename,NULL))){
		return -1;
	}
	if(!namelen){

	}

	*res_inode = dir;

	int i=0;
	for(i=0;i<namelen;i++){
		printk("%c",get_fs_byte(basename+i));
	}
	printk("\n");
*/	
	dir = fat16_namei(pathname);
	*res_inode = dir;
	return 0;
}
