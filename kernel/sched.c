#include "../include/io.h"
#include "../include/system.h"
#include "../include/head.h"
#define HZ 100
#define PAGE_SIZE 4096
#define LATCH (1193180/HZ)
#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY + 1)
#define _TSS(n) ((((unsigned long)n)<<4)+(FIRST_TSS_ENTRY<<3))
#define _LDT(n) ((((unsigned long)n)<<4)+(FIRST_LDT_ENTRY<<3))
#define ltr(n) __asm__("ltr %%ax"::"a"(_TSS(n)))
#define lldt(n) __asm__("lldt %%ax"::"a"(_LDT(n)))
#define NR_TASKS 64
#define NULL ((void *)0)

#define _set_tssldt_desc(n,addr,type) \
__asm__ ("movw $104,%1\n\t"\
	"movw %%ax,%2\n\t"\
	"rorl $16,%%eax\n\t"\
	"movb %%al,%3\n\t"\
	"movb $"type ",%4\n\t"\
	"movb $0,%5\n\t"\
	"movb %%ah,%6\n\t"\
	"rorl $16,%%eax\n\t"\
	::"a" (addr),\
	"m" (*(n)),\
	"m" (*(n+2)),\
	"m" (*(n+4)),\
	"m" (*(n+5)),\
	"m" (*(n+6)),\
	"m" (*(n+7))\
	)

#define set_tss_desc(n,addr) _set_tssldt_desc(((char *)(n)),addr,"0x89")

#define set_ldt_desc(n,addr) _set_tssldt_desc(((char *)(n)),addr,"0x82")
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
#define INIT_TASK \
	{0,15,15,0,-1, \
	{ \
		{0,0},\
		{0x9f,0xc0fa00},\
		{0x9f,0xc0f200}\
	},\
	{0} \
	}

union task_union{
	struct task_struct task;
	char stack[4096];
};

extern void timer_interrupt(void);
//static union task_union init_task = {INIT_TASK,};
//static struct task_struct * task[NR_TASKS]={&(init_task.task),};
static union task_union init_task;
static struct task_struct * task[NR_TASKS];
extern unsigned long pg_dir[1024];
void sched_init(void){
	int i;
	struct desc_struct *p;
	init_task.task.state = 0;
	init_task.task.counter = 15;
	init_task.task.priority = 15;
	init_task.task.pid = 0;
	init_task.task.father = -1;

	init_task.task.ldt[0].a = 0;
	init_task.task.ldt[0].b = 0;
	//task0 代码段640k大小 3级权限 0～640K
	init_task.task.ldt[1].a = 0x0000009f;
	init_task.task.ldt[1].b = 0x00c0fa00;
	//task0 数据段0～640K 3级权限
	init_task.task.ldt[2].a = 0x0000009f;
	init_task.task.ldt[2].b = 0x00c0f200;
	//TSS
	init_task.task.tss.back_link = 0;							
	init_task.task.tss.esp0 = PAGE_SIZE + (long)&init_task;
	init_task.task.tss.ss0 = 0x10;
	init_task.task.tss.esp1 = 0;
	init_task.task.tss.ss1 = 0;
	init_task.task.tss.esp2 = 0;
	init_task.task.tss.ss2 = 0;
	init_task.task.tss.cr3 = (long)&pg_dir;
	init_task.task.tss.eip = 0;
	init_task.task.tss.eflags = 0;
	init_task.task.tss.eax = 0;
	init_task.task.tss.ecx = 0;
	init_task.task.tss.edx = 0;
	init_task.task.tss.ebx = 0;
	init_task.task.tss.esp = 0;
	init_task.task.tss.ebp = 0;
	init_task.task.tss.esi = 0;
	init_task.task.tss.edi = 0;
	init_task.task.tss.es = 0x17;//段选择符0x10 1(LDT) 11(3级权限)
	init_task.task.tss.cs = 0x17;
	init_task.task.tss.ss = 0x17;
	init_task.task.tss.ds = 0x17;
	init_task.task.tss.fs = 0x17;
	init_task.task.tss.gs = 0x17;
	init_task.task.tss.ldt = _LDT(0);
	init_task.task.tss.trace_bitmap = 0x80000000;

	set_tss_desc(gdt+FIRST_TSS_ENTRY,&(init_task.task.tss));
	set_ldt_desc(gdt+FIRST_LDT_ENTRY,&(init_task.task.ldt));
	p = gdt + 2 + FIRST_TSS_ENTRY;
	task[0] = &(init_task.task);
	for(i =1;i<NR_TASKS;i++){
		task[i] = NULL;
		p->a = p->b = 0;
		p++;
		p->a = p->b = 0;
		p++;
	}

	ltr(0);
	lldt(0);

/*	outb_p(0x43,0x36);
	outb_p(0x40,LATCH & 0xff);
	outb_p(0x40,LATCH >> 8);
	set_intr_gate(0x20,&timer_interrupt);
	outb_p(0x21,inb_p(0x21) & ~0x01);
	*/
//	set_system_gate(0x80,&system_call);
}

void do_timer(){
	printk("timer");
}
