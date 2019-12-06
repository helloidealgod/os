#include "../include/unistd.h"
#define EXT_MEM_K (*(unsigned short *)0x90002)

#define move_to_user_mode() \
	__asm__ ("movl %%esp,%%eax\n\t" \
		"pushl $0x17\n\t" \
		"pushl %%eax\n\t" \
		"pushfl\n\t" \
		"pushl $0x0f\n\t" \
		"pushl $1f\n\t" \
		"iret\n" \
		"1:\t movl $0x17,%%eax\n\t" \
		"movw %%ax,%%ds\n\t" \
		"movw %%ax,%%es\n\t" \
		"movw %%ax,%%fs\n\t" \
		"movw %%ax,%%gs"\
		:::"ax");

static long memory_end = 0;
static long buffer_memory_end = 0;
static long main_memory_start = 0;

static inline _syscall0(int,fork)
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
	printk("hello world! \r\n");
	int i = (int)(EXT_MEM_K & 0x0000ffff);
	char s[10];
	itoa(i,s);
	printk(s);
	printk("\n");
	i = (int)(memory_end >> 20);
	itoa(i,s);
	printk("total memory: ");
	printk(s);
	printk("M \n");
	sched_init();
//	sti();
	move_to_user_mode();
	int c = 1 + 2 + 3+ 4 + 5 + 6 + 7 + 8 + 9;
	itoa(12,s);
	if(!fork()){
	}
	while(1);
	return 0;
}
