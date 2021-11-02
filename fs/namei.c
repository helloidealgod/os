#include "../include/types.h"

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

int open_namei(const char * pathname,int flag,int mode,
		struct m_inode ** res_inode){
	const char * basename;
	int dev,inr,namelen;
	struct m_inode *dir,*inode;
	struct buffer_head * bh;
	struct dir_entry * de;

	printk("open_namei\n");
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
	return 0;
}
