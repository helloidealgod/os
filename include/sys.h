#ifndef _SYS_H
#define _SYS_H
extern int sys_fork();

fn_ptr sys_call_table[]={sys_fork};
#endif
