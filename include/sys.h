#ifndef _SYS_H
#define _SYS_H
typedef int (*fn_ptr)();
extern int sys_fork();
extern void sys_printk(char s[]);

fn_ptr sys_call_table[]={sys_fork,sys_printk};
#endif
