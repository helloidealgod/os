#include "../include/io.h"
#include "../include/system.h"
#include "../include/head.h"
#define HZ 100
#define LATCH (1193180/HZ)

extern void timer_interrupt(void);

void sched_init(void){
	outb_p(0x43,0x36);
	outb_p(0x40,LATCH & 0xff);
	outb_p(0x40,LATCH >> 8);
	set_intr_gate(0x20,&timer_interrupt);
	outb_p(0x21,inb_p(0x21) & ~0x01);
}

void do_timer(){
	printk("timer");
}
