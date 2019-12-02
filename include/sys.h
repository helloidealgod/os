#ifndef _SYS_H
#define _SYS_H
typedef int (*fn_ptr)();
extern int sys_fork();
extern void printk(char * s);

fn_ptr sys_call_table[]={printk,sys_fork};
#endif
