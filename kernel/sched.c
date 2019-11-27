#include "../include/io.h"
#include "../include/system.h"
#include "../include/head.h"
#define HZ 100
#define LATCH (1193180/HZ)
#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY + 1)
#define _TSS(n) ((((unsigned long)n)<<4)+(FIRST_TSS_ENTRY<<3))
#define _LDT(n) ((((unsigned long)n)<<4)+(FIRST_LDT_ENTRY<<3))
#define ltr(n) __asm__("ltr %%ax"::"a"(_TSS(n)))
#define lldt(n) __asm__("lldt %%ax"::"a"(_LDT(n)))
#define NR_TASKS 64

struct tss_struct {
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
#define INIT_TASK 

union task_union{
	struct task_struct task;
	char stack[4096];
};

extern void timer_interrupt(void);
static union task_union init_task = {INIT_TASK,};
static struct task_struct * task[NR_TASKS]={&(init_task.task),};
void sched_init(void){
	int i;
	struct desc_struct *p;
	
	set_tss_desc(gdt+FIRST_TSS_ENTRY,&(init_task.task.tss));
	set_ldt_desc(gdt+FIRST_LDT_ENTRY,&(init_task.task.ldt));

	p = gdt + 2 + FIRST_TSS_ENTRY;
	for(i =1;i<NR_TASKS;i++){
		task[i] = NULL;
		P->a = p->b = 0;
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
