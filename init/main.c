#include "../include/unistd.h"
#include "../include/system.h"
#define EXT_MEM_K (*(unsigned short *)0x90002)

#define IN_ORDER(s1,s2) \
((s1)->cmd<(s2)->cmd || (s1)->cmd==(s2)->cmd && \
((s1)->dev < (s2)->dev || ((s1)->dev == (s2)->dev && \
(s1)->sector < (s2)->sector)))

#define NR_BLK_DEV	7
#define NR_REQUEST	32
#define NULL ((void *)0)
#define READA 1
#define READ 1
#define WRITEA 2
#define WRITE 2


typedef int (*fn_ptr)();
extern int sys_fork();
extern int sys_pause();
fn_ptr sys_call_table[]={sys_fork,sys_pause};

static long memory_end = 0;
static long buffer_memory_end = 0;
static long main_memory_start = 0;

static inline int fork(void) __attribute__((always_inline));
static inline int pause(void) __attribute__((always_inline));
static inline _syscall0(int,fork)
static inline _syscall0(int,exit)
static inline _syscall0(int,pause)

struct buffer_head {
	char * b_data;
	unsigned long b_blocknr;
	unsigned short b_dev;
	unsigned char b_uptodate;
	unsigned char b_dirt;
	unsigned char b_count;
	unsigned char b_lock;
	struct task_struct * b_wait;
	struct buffer_head * b_prev;
	struct buffer_head * b_next;
	struct buffer_head * b_prev_free;
	struct buffer_head * b_next_free;
};
struct request{
	int dev;
	int cmd;
	int errors;
	unsigned long sector;
	unsigned long nr_sectors;
	char * buffer;
	struct task_struct * waiting;
	struct buffer_head * bh;
	struct request * next;
};
struct blk_dev_struct {
	void (*request_fn)(void);
	struct request * current_request;
};

struct request request[NR_REQUEST];
struct blk_dev_struct blk_dev[NR_BLK_DEV]={
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL}
};

void ll_rw_block(int rw, struct buffer_heard *bh){

}

void blk_dev_init(){

}

void hd_init(){

}

static void add_request(struct blk_dev_struct * dev, struct request * req){
    struct request * tmp;
	req->next = NULL;
	cli();
	if (req->bh)
		req->bh->b_dirt = 0;
	if (!(tmp = dev->current_request)){
		dev->current_request = req;
		sti();
		(dev->request_fn)();
		return;
	}
	for (; tmp->next; tmp=tmp->next){
		if (!req->bh)
			if (tmp->next->bh)
				break;
			else
				continue;
		if ((IN_ORDER(tmp,req) ||
		 !IN_ORDER(tmp,tmp->next))&&
		 IN_ORDER(req,tmp->next))
		break;
	}
	req->next = tmp->next;
	tmp->next = req;
	sti();
}

static void make_request(int major, int rw, struct buffer_head * bh){
    struct request * req;
	int rw_ahead;

	if (rw_ahead = (rw == READA || rw == WRITEA)){
		if (bh->b_lock)
			return;
		if (rw == READA)
			rw = READ;
		else
			rw = WRITE;
	}
//	if (rw != READ && rw != WRITE)
//		panic("");
//	lock_buffer(bh);
//	if ((rw == WRITE && !bh->b_dirt) || (rw == READ && bh->b_uptodate)) {
//		unlock_buffer(bh);
//		return;
//	}
repeat:
	if (rw == READ)
		req = request + NR_REQUEST;
	else
		req = request + ((NR_REQUEST*2)/3);
	while (--req >= request)
		if (req->dev < 0 )
			break;
	if (req < request){
		if (rw_ahead){
//			unlock_buffer(bh);
			return;
		}
//		sleep_on(&wait_for_request);
		goto repeat;
	}

	req->dev = bh->b_dev;
	req->cmd = rw;
	req->errors = 0;
	req->sector = bh->b_blocknr<<1;
	req->nr_sectors = 2;
	req->buffer = bh->b_data;
	req->waiting = NULL;
	req->bh = bh;
	req->next = NULL;
	add_request(major + blk_dev, req);
}

int main(void){
	memory_end = (1<<20) + (EXT_MEM_K << 10);
	memory_end &= 0xfffff000;
	if (memory_end > 16*1024*1024)
		memory_end = 16*1024*1024;
	if (memory_end > 12*1024*1024)
		buffer_memory_end = 4*1024*1024;
	else if (memory_end > 6*1024*1024)
		buffer_memory_end = 2*1024*1024;
	else
		buffer_memory_end = 1*1024*1024;
	main_memory_start = buffer_memory_end;

	mem_init(main_memory_start,memory_end);
	trap_init();
	con_init();
	sched_init();
	printk("init complete!\n");
	int test = 10;
	printk("test=%d\n",test);
	sti();
	move_to_user_mode();
	if(!fork()){
		int a,b,c,d;
		if(!fork()){
			while(1){
				a++;
			}
		}
		if(!fork()){
			while(1){
			  	b++;
			}
		}
		if(!fork()){
			while(1){
				c++;
			}
		}
		if(!fork()){
			while(1){
				d++;
			}
		}
		for(;;)
			pause();
	}
	for(;;)
		pause();
	return 0;
}
