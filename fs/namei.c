#include "../include/types.h"
#include "../include/fs.h"

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
	unsigned short start_cluster;
	int block = root_inode.i_num;
	long i_size;
	char * basename = filename;
	char c,c1,c2;
label1:
	c = get_fs_byte(basename);
	printk("label %c",c);
	if('/' == c){
		printk("block=%d",block);
		bh = bread(root_inode.i_dev,block);

		basename++;
		int i,len,match;
		for(i=0;i<32;i++){
			match = 0;
			if(0xE5 != (unsigned char)bh->b_data[0x00 + i*32]
			&& 0x0F != (unsigned char)bh->b_data[0x0B + i*32]
			&& 0x00 != (unsigned char)bh->b_data[0x0B + i*32]){
				for(len=0;len<8;len++){
					c1 = get_fs_byte(basename+len);
					c2 = (unsigned char)bh->b_data[len + i*32];
					if(0x20 == c2){
						printk("\n");
						if('/'== c1 || '.' == c1 || '\0' == c1){
							match = 1;
							basename += len;
						}
						break;
					}
					c1 = (c1>= 97 && c1 <=122)? c1-32 : c1; 
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
					if('.' != c1 || '\0' != c1){
						i_size = (unsigned long)bh->b_data[0x1C + i*32];
						//m_inode.i_num = block;
						//m_inode.i_dev = dev;
						//m_inode.i_size = i_size;
						printk("file_size=%dbytes\n",i_size);
					}
					goto label1;
				}
			}
		}
	}

	return NULL;
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
