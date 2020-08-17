#include "../include/unistd.h"
#include "../include/system.h"
#define EXT_MEM_K (*(unsigned short *)0x90002)

typedef int (*fn_ptr)();
extern int sys_fork();
extern int sys_printk(char *s);
extern int sys_pause();
fn_ptr sys_call_table[]={sys_fork,sys_printk,sys_pause};

static long memory_end = 0;
static long buffer_memory_end = 0;
static long main_memory_start = 0;

static inline int fork(void) __attribute__((always_inline));
static inline int pause(void) __attribute__((always_inline));
static inline _syscall0(int,fork)
static inline _syscall0(int,exit)
static inline _syscall0(int,pause)
static inline _syscall1(int,sprintk,char *,p)

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
	sti();
	move_to_user_mode();
	if(!fork()){
		if(!fork()){
			while(1) sprintk("A");
		}
		if(!fork()){
			while(1) sprintk("B");
		}
		if(!fork()){
			while(1) sprintk("C");
		}
		if(!fork()){
			while(1) sprintk("D");
		}
		while(1);
	}
	for(;;) 
		pause();
	return 0;
}
