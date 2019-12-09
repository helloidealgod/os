#ifndef _SCHED_H
#define _SCHED_H
#include "head.h"
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
#endif
