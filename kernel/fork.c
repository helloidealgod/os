#include "../include/sched.h"
#define NR_TASKS 64
#define EAGAIN 11
#define NULL ((void *)0)
long last_pid = 0;
//extern struct task_struct * task[NR_TASKS];
int find_empty_process(){
	printk("find empty process\n");
	int i;
//	repeat:
//		if((++last_pid)<0) last_pid=1;
//		for(i=0;i<NR_TASKS;i++)
//			if(task[i] && task[i]->pid == last_pid)
//				goto repeat;
	last_pid = 1;
/*	for(i=1;i<NR_TASKS;i++)
		if(!task[i]){
			char s[10];
			itoa(i,s);
			printk(s);
			return i;
		}
	return -EAGAIN;*/
	return 1;
}
void copy_process(){
	printk("copy process\n");
}
