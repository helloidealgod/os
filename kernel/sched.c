#include "../include/io.h"
#include "../include/system.h"
#include "../include/head.h"
#define HZ 100
#define LATCH (1193180/HZ)

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
extern void timer_interrupt(void);

void sched_init(void){

/*	outb_p(0x43,0x36);
	outb_p(0x40,LATCH & 0xff);
	outb_p(0x40,LATCH >> 8);
	set_intr_gate(0x20,&timer_interrupt);
	outb_p(0x21,inb_p(0x21) & ~0x01);
	*/
}

void do_timer(){
	printk("timer");
}
