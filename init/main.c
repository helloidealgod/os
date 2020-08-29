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
