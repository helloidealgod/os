#include "../include/sched.h"
#define NR_TASKS 64
#define EAGAIN 11
#define NULL ((void *)0)

long last_pid = 0;
unsigned long get_free_page(void);
int find_empty_process(){
	printk("find empty process\n");
	int i;
	repeat:
		if((++last_pid)<0) last_pid=1;
		for(i=0;i<NR_TASKS;i++)
			if(task[i] && task[i]->pid == last_pid)
				goto repeat;
	last_pid = 1;
	for(i=1;i<NR_TASKS;i++)
		if(!task[i]){
			char s[10];
			itoa(i,s);
			printk("pid=");
			printk(s);
			printk("\n");
			return i;
		}
	return -EAGAIN;
}
int copy_process(int nr,long ebp,long edi,long esi,long gs,long none,
		long ebx,long ecx,long edx,
		long fs,long es,long ds,
		long eip,long cs,long eflags,long esp,long ss){
	struct task_struct *p;
	int i;
	p = (struct task_struct *)get_free_page();
	p = (void *)0;
	if(NULL != p){
	       	return -11;
	}
	task[nr] = p;
//	*p = *current;

	p->state = 0;
	p->pid = last_pid;
//	p->father = current->pid;
	p->counter = p->priority;

	p->tss.back_link = 0;
	p->tss.esp0 = 4096 + (long)p;
	p->tss.ss0 = 0x10;
	p->tss.eip = eip;
	p->tss.eflags = eflags;
	p->tss.eax = 0;
	p->tss.ecx = ecx;
	p->tss.edx = edx;
	p->tss.ebx = ebx;
	p->tss.esp = esp;
	p->tss.ebp = ebp;
	p->tss.esi = esi;
	p->tss.edi = edi;
	p->tss.es = es & 0xffff;
	p->tss.cs = cs & 0xffff;
	p->tss.ss = ss & 0xffff;
	p->tss.ds = ds & 0xffff;
	p->tss.fs = fs & 0xffff;
	p->tss.gs = gs & 0xffff;
//	p->tss.ldt = _LDT(nr);
	p->tss.trace_bitmap = 0x80000000;
	printk("copy process\n");
}
