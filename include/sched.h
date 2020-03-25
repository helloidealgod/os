#ifndef _SCHED_H
#define _SCHED_H
#include "head.h"
#define FIRST_TSS_ENTRY 4
#define _TSS(n) ((((unsigned long)n)<<4)+(FIRST_TSS_ENTRY<<3))
struct i387_struct {
	long cwd;
	long swd;
	long twd;
	long fip;
	long fcs;
	long foo;
	long fos;
	long st_space[20];
};

struct tss_struct {
// 前一任务链接（TSS Back Link）：前一个任务的TSS描述符的选择子。
// 当Call指令、中断或者异常造成任务切换，处理器会把旧任务的TSS选择子复制到新任务的TSS的Back Link字段中，并且设置新任务的NT（EFLAGS的bit14）为1，以表明新任务嵌套于旧任务中。
// SS0，SS1，SS2和ESP0，ESP1，ESP2分别是0,1,2特权级堆栈的选择子和栈顶指针
	long back_link;
	long esp0;
	long ss0;
	long esp1;
	long ss1;
	long esp2;
	long ss2;
	long cr3;
	long eip;
	long eflags;
	long eax,ecx,edx,ebx;
	long esp;
	long ebp;
	long esi;
	long edi;
	long es;
	long cs;
	long ss;
	long ds;
	long fs;
	long gs;
	long ldt;
	long trace_bitmap;
};

struct task_struct{
	long state;
	long counter;
	long priority;
	long pid,father;
	struct desc_struct ldt[3];
	struct tss_struct tss;
};

union task_union{
	struct task_struct task;
	char stack[4096];
};

#define switch_to(n) {\
struct {long a,b;} _tmp;\
__asm__ ("cmpl %%ecx,current\n\t"\
         "je 1f\n\t"\
         "movw %%dx,%1\n\t"\
         "xchgl %%ecx,current\n\t"\
         "ljmp *%0\n\t"\
         "1:"\
         ::"m"(*&_tmp.a),"m"(*&_tmp.b),\
         "d"(_TSS(n)),"c" ((long)task[n]));\
}
//extern struct task_struct * task[64];

#define _set_base(addr,base) \
__asm__("movw %%dx,%0\n\t" \
		"rorl $16,%%edx\n\t" \
		"movb %%dl,%1\n\t" \
		"movb %%dh,%2" \
		::"m" (*((addr) + 2)), \
		"m" (*((addr) + 4)), \
		"m" (*((addr) + 7)), \
		"d" (base) \
		:"dx")
		
#define set_base(ldt,base) _set_base(((char *)&(ldt)),base)

#define _get_base(addr) ({\
unsigned long _base; \
__asm__("movb %3,%%dh\n\t" \
		"movb %2,%%dl\n\t" \
		"shll $16,%%edx\n\t" \
		"movw %1,%%dx" \
		:"=d" (_base) \
		:"m" (*((addr) + 2)), \
		"m" (*((addr) + 4)), \
		"m" (*((addr) + 7))); \
_base;})

#define get_base(ldt) _get_base( ((char *)&(ldt)) )

#define get_limit(segment) ({ \
unsigned long _limit; \
__asm__("lsll %1,%0\n\tincl %0":"=r" (_limit):"r" (segment)); \
_limit;})

#endif
