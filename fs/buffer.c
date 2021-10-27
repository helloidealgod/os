#include "../include/fs.h"
#include "../include/system.h"
#define NULL ((void *)0)
#define NR_HASH 307

//内核末端，需修改，暂时用这个
int end = 1024*1024;
//struct buffer_head * start_buffer = (struct buffer_head *)&end;
struct buffer_head * start_buffer = (struct buffer_head *)(1024*1024);
struct buffer_head * hash_table[NR_HASH];
static struct buffer_head * free_list;
static struct stask_struct * buffer_wait = NULL;

int NR_BUFFERS = 0;

static inline void wait_on_buffer(struct buffer_head * bh){
	cli();
	int i = 10000000;
	while(i--&bh->b_lock);
	while(bh->b_lock)
		sleep_on(&bh->b_wait);
	sti();
}

void brelse(struct buffer_head * buf){
	if(!buf){
		return;
	}
	wait_on_buffer(buf);
	if(!(buf->b_count--)){
		panic("Trying to free free buffer");
	}
	wake_up(&buffer_wait);
}

struct buffer_head * getblk(int dev,int block){
	struct buffer_head * tmp, *bh;
	free_list->b_dev = dev;
	free_list->b_blocknr = block;

	tmp = free_list;
	free_list = tmp->b_next_free;
	return tmp;
}

struct buffer_head * bread(int dev, int block){
	struct buffer_head * bh;
	if (!(bh = getblk(dev,block)))
		panic("getblk failed\n");
	if (bh->b_uptodata)
		return bh;

	ll_rw_block(READ,bh);
	wait_on_buffer(bh);
/*	printk("after wait_on_buffer\n");
	int i;
	for(i=0;i<512;i++){
		printk("%c",bh->b_data[i]);
	}
	
	printk("\n");
//	bh->b_uptodata = 1;
*/
	if(bh->b_uptodata)
		return bh;
	printk("in bread():before brelse\n");
	brelse(bh);
	return NULL;
}

void buffer_init(long buffer_end){
	struct buffer_head * h = start_buffer;
	void * b;
	int i;

	if(buffer_end == 1<<20)
		b = (void *)(640*1024);
	else
		b = (void *)buffer_end;
	while((b-=BLOCK_SIZE) >= ((void *)(h+1))){
		h->b_dev = 0;
		h->b_dirt = 0;
		h->b_count = 0;
		h->b_lock = 0;
		h->b_uptodata = 0;
		h->b_wait = NULL;
		h->b_next = NULL;
		h->b_prev = NULL;
		h->b_data = (char *)b;
		h->b_prev_free = h-1;
		h->b_next_free = h+1;
		h++;
		NR_BUFFERS++;
		if(b == (void *)0x100000)
			b = (void *)0xA0000;
	}
	h--;
	free_list = start_buffer;
	free_list->b_prev_free = h;
	h->b_next_free = free_list;
	for(i=0;i<NR_HASH;i++)
		hash_table[i] = NULL;
}
