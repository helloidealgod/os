#include "../include/io.h"
#define HZ 100
#define LATCH (1193180/HZ)

extern void timer_interrupt(void);

void sched_init(void){
	outb_p(0x36,0x43);
	outb_p(LATCH & 0xff,0x40);
	outb_p(LATCH >> 8,0x40);
	set_intr_gate(0x20,&timer_interrupt);
	outb_p(inb_p(0x21) & ~0x01,0x21);
}

void do_timer(){
	printk("timer\r\n");
}
