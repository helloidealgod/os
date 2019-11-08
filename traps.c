#include "system.h"
#include "io.h"
/*
typedef struct desc_struct{
	unsigned long a,b;
}desc_table[256];

desc_table idt,gdt;
*/
/*
int do_exit(long code);
void page_exception(void);
void divide_error(void);
void debug(void);
void nmi(void);
void int3(void);
void overflow(void);
void bounds(void);
void invalid_op(void);
void device_not_available(void);
void double_fault(void);
void coprocessor_segment_overrun(void);
void invalid_TSS(void);
void segment_not_present(void);
void stack_segment(void);
void general_protection(void);
void page_fault(void);
void coprocessor_error(void);
void reserved(void);
void parallel_interrupt(void);
void irq13(void);

int do_exit(long code){

}
void page_exception(void){

}
void divide_error(void){
	printk("divide error");
}
void debug(void){
	printk("debug ");
}
void nmi(void){
	printk("nmi ");
}
void int3(void){
	printk("int3 ");

}
void overflow(void){
	printk("overflow ");

}
void bounds(void){
	printk("bounds ");

}
void invalid_op(void){
	printk("invalid op ");

}
void device_not_available(void){
	printk("device not available ");

}
void double_fault(void){
	printk("double fault ");

}
void coprocessor_segment_overrun(void){
	printk("coprocessor segment overrun ");

}
void invalid_TSS(void){
	printk("invalid TSS ");

}
void segment_not_present(void){
	printk("segment not present ");

}
void stack_segment(void){
	printk("stack segment ");

}
void general_protection(void){
	printk("general protection ");

}
void page_fault(void){
	printk("page fault ");

}
void coprocessor_error(void){
	printk("coprocessor error ");

}
void reserved(void){
	printk("reserved ");

}
void parallel_interrupt(void){
	printk("parallel interrupt ");

}
void irq13(void){
	printk("irp13 ");
}


static void die(char * str, long esp_ptr, long nr){
	long * esp = (long *)esp_ptr;
	int i;
}*/
void trap_init(void){
	int i;
	printk("init traps\r\n");
/*	set_trap_gate(0,&divide_error);		
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
	set_trap_gate(14,&page_fault);
	set_trap_gate(15,&reserved);
	set_trap_gate(16,&coprocessor_error);
	for(i=17;i<48;i++){
		set_trap_gate(i,&reserved);
	}
	set_trap_gate(45,&irq13);
	outb_p(inb_p(0x21)&0xfb,0x21);
	outb(inb_p(0xa1)&0xdf,0xa1);
	set_trap_gate(39,&parallel_interrupt);
*/	
}
