#define sti() __asm__ volatile ("sti"::)
#define cli() __asm__ volatile ("cli"::)
#define nop() __asm__ volatile ("nop"::)
#define iret() __asm__ volatile ("iret"::)
/*
#define _set_gate(gate_addr, type, dp1, addr)\
	__asm__ volatile (\
		"movw %%dx, %%ax\n\t"\
		"movw %0, %%dx\n\t"\
		"movl %%eax, %1\n\t"\
		"movl %%edx, %2"\
		:\
		:"i" ((short)(0x8000+((dp1)<<13)+((type)<<8))),"o" (*((char *) (gate_addr))),\
		"o" (*(4+(char *) (gate_addr))),\
		"a" (0x00080000),\
		"d" ((char *)(addr)))
*/

#define _set_gate(gate_addr, type, dp1, addr) \
	__asm__ volatile("movw %%dx,%%ax\n\t" \
			"movw %0, %%dx\n\t" \
			"movl %%eax, %1\n\t"\
			"movl %%edx, %2"\
			:\
			:"i" ((short)(0x8000+((dp1)<<13)+((type)<<8))),"o" (*((char *) (gate_addr))),\
			"o" (*(4+(char *) (gate_addr))),\
			"d" ((char *)(addr)),"a"(0x00080000))

#define set_intr_gate(n,addr) \
	_set_gate(&idt[n],14,0,addr)

#define set_trap_gate(n,addr) \
	_set_gate(&idt[n],15,0,addr)

#define set_system_gate(n,addr) \
	_set_gate(&idt[n],15,3,addr)
