#include "../include/unistd.h"
#include "../include/system.h"
#define EXT_MEM_K (*(unsigned short *)0x90002)

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
