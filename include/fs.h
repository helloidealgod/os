#ifndef _FS_H
#define _FS_H

#include "types.h"

#define BLOCK_SIZE 1024
#define READ 0
#define WRITE 1
#define READA 2
#define WRITEA 3

#define MAJOR(a) (((unsigned)(a))>>8)
#define MINOR(a) ((a)&0xff)

struct buffer_head {
	char * b_data;
	unsigned long b_blocknr;
	unsigned short b_dev;
	unsigned char b_uptodata;
	unsigned char b_dirt;
	unsigned char b_count;
	unsigned char b_lock;
	struct task_struct * b_wait;
	struct buffer_head * b_prev;
	struct buffer_head * b_next;
	struct buffer_head * b_prev_free;
	struct buffer_head * b_next_free;
};

struct m_inode{
	unsigned short i_mode;
	unsigned short i_uid;
	unsigned long i_size;
	struct task_struct * i_wait;
	struct task_struct * i_wait2;
	unsigned short i_dev;
	unsigned short i_num;
	unsigned short i_count;
	unsigned char i_lock;
};

struct file{
	unsigned short f_mode;
	unsigned short f_flags;
	unsigned short f_count;
	struct m_inode * f_inode;
	off_t f_pos;
};
#endif
