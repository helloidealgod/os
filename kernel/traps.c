#include "../include/system.h"
#include "../include/io.h"
#include "../include/head.h"
/*
typedef struct desc_struct{
	unsigned long a,b;
}desc_table[256];

extern desc_table idt,gdt;
*/
//int do_exit(long code);
void divide_error(void);
void debug(void);
void nmi(void);
void int3(void);
void overflow(void);
void bounds(void);
void invalid_op(void);
void coprocessor_segment_overrun(void);
void reserved(void);
void irq13(void);
void double_fault(void);
void invalid_TSS(void);
void segment_not_present(void);
void stack_segment(void);
void general_protection(void);
void coprocessor_error(void);
void device_not_available(void);
void parallel_interrupt(void);
void page_exception(void);

static void die(char * str, long esp_ptr, long nr){
	long * esp = (long *)esp_ptr;
	int i;
}

void do_double_fault(long esp,long error_code){
	printk("do double fault\r\n");
}

void do_general_protection(long esp,long error_code){
	printk("do general protection\r\n");
}

void do_divide_error(long esp,long error_code){
	printk("do devide error\r\n");
}

void do_int3(long *esp,long error_code,
		long fs,long es,long ds,
		long ebp,long esi,long edi,
		long edx,long ecx,long ebx,long eax){
	printk("interrupt int3\r\n");
}

void do_nmi(long esp,long error_code){
	printk("do nmi\r\n");
}

void do_debug(long esp,long error_code){
	printk("do debug\r\n");
}

void do_overflow(long esp,long error_code){
	printk("do overflow\r\n");
}

void do_bounds(long esp,long error_code){
	printk("do bounds\r\n");
}

void do_invalid_op(long esp,long error_code){
	printk("do invalid op\r\n");
}

void do_device_not_available(long esp,long error_code){
	printk("do device not available\r\n");
}

void do_coprocessor_segment_overrun(long esp,long error_code){
	printk("do coprocessor segment overrun\r\n");
}

void do_invalid_TSS(long esp,long error_code){
	printk("do invalid TSS\r\n");
}

void do_segment_not_present(long esp,long error_code){
	printk("do segment not present\r\n");
}

void do_stack_segment(long esp,long error_code){
	printk("do stack segment\r\n");
}

void do_coprocessor_error(long esp,long error_code){
	printk("do coprocessor\r\n");
}

void do_reserved(long esp,long error_code){
	printk("do reserved\r\n");
}

void do_parallel_interrupt(long esp,long error_code){
	printk("do parallel interrupt\r\n");
}

void do_page_exception(long esp,long error_code){
	printk("do page exception\r\n");
}

void trap_init(void){
	int i;
	set_trap_gate(0,&divide_error);		
	set_trap_gate(1,&debug);
	set_trap_gate(2,&nmi);
	set_trap_gate(3,&int3);
	set_trap_gate(4,&overflow);
	set_trap_gate(5,&bounds);
	set_trap_gate(6,&invalid_op);
	set_trap_gate(7,&device_not_available);
	set_trap_gate(8,&double_fault);
	set_trap_gate(9,&coprocessor_segment_overrun);
	set_trap_gate(10,&invalid_TSS);
	set_trap_gate(11,&segment_not_present);
	set_trap_gate(12,&stack_segment);
	set_trap_gate(13,&general_protection);
	set_trap_gate(14,&page_exception);
	set_trap_gate(15,&reserved);
	set_trap_gate(16,&coprocessor_error);
	for(i=17;i<48;i++){
		set_trap_gate(i,&reserved);
	}
	set_trap_gate(45,&irq13);
	outb_p(inb_p(0x21)&0xfb,0x21);
	outb(inb_p(0xa1)&0xdf,0xa1);
	set_trap_gate(39,&parallel_interrupt);
	
}
