#ifndef _FS_H
#define _FS_H

#define BLOCK_SIZE 1024

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
#endif
