#include "../include/sched.h"
#include "../include/head.h"
#include "../include/system.h"
#define NR_TASKS 64
#define EAGAIN 11
#define NULL ((void *)0)
#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY + 1)
#define _LDT(n) ((((unsigned long)n)<<4)+(FIRST_LDT_ENTRY<<3))

long last_pid = 0;
extern struct task_struct * current;
extern struct task_struct * task[NR_TASKS];
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
	if(NULL == p){
	       	return -EAGAIN;
	}
	task[nr] = p;
/*	char s[10];
	itoa(nr,s);
	printk(s);
	printk("\n");
*/	*p = *current;

	p->state = 0;
	p->pid = last_pid;
	p->father = current->pid;
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
	p->tss.ldt = _LDT(nr);
	p->tss.trace_bitmap = 0x80000000;
	set_tss_desc(gdt + (nr<<1) + FIRST_TSS_ENTRY,&(p->tss));
	set_ldt_desc(gdt + (nr<<1) + FIRST_LDT_ENTRY,&(p->ldt));
	printk("copy process\n");
	if (copy_mem(nr,p)){
		task[nr] = NULL;
		return -11;
	}
}

int copy_mem(int nr,struct task_struct * p){
	unsigned long old_data_base,new_data_base,data_limit;
	unsigned long old_code_base,new_code_base,code_limit;
	
	code_limit = get_limit(0x0f);
	data_limit = get_limit(0x17);
	
	old_code_base = get_base(current->ldt[1]);
	old_data_base = get_base(current->ldt[2]);
	
	if (old_code_base != old_data_base){
		printk("Wo don't support separate I&D\n");
	}
	if (data_limit < code_limit){
		printk("Bad data_limit\n");
	}
	
	new_data_base = new_code_base = nr * 0x4000000;
	
	set_base(p->ldt[1],new_code_base);
	set_base(p->ldt[2],new_data_base);
	if(copy_page_tables(old_data_base,new_data_base,data_limit)){
		free_page_tables(new_data_base,data_limit);
		return -EAGAIN;
	}
	return 0;
}