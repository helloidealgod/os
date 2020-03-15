#ifndef _TTY_H
#define _TTY_H

#include <termios.h>

#define TTY_BUF_SIZE 1024

#define INC(a) ((a) = ((a)+1) & (TTY_BUF_SIZE-1))
#define DEC(a) ((a) = ((a)-1) & (TTY_BUF_SIZE-1))
#define EMPTY(a) ((a).head == (a).tail)
#define LEFT(a) (((a).tail-(a).head-1)&(TTY_BUF_SIZE-1))
#define LAST(a) ((a).buf[(TTY_BUF_SIZE-1)&((a).head-1)])
#define FULL(a) (!LEFT(a))
#define CHARS(a) (((a).head-(a).tail)&(TTY_BUF_SIZE-1))
#define GETCH(queue,c) (void)({c=(queue).buf[(queue).tail];INC((queue).tail);})
#define PUTCH(c,queue) (void)({(queue).buf[(queue).head]=(c);INC((queue).head);})

struct tty_queue {
    unsigned long data;
    unsigned long head;
    unsigned long tail;
    struct task_struct * proc_list;//等待本队列的进程列表
    char buf[TTY_BUF_SIZE];//缓存区长度为1024
};

struct tty_struct {
    struct termios termios;//驱动设备属性
    int pgrp;
    int stopped;
    void (*write)(struct tty_struct * tty);//写函数指针，具体设备的写处理，会在tty_write中被调用！
    struct tty_queue read_q;
    struct tty_queue write_q;
    struct tty_queue secondary;//tty辅助队列，存放将read_q中数据经过行规则处理后的cooked数据。
};
extern struct tty_struct tty_table[];

#endif
