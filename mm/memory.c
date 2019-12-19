#define LOW_MEM 0x100000
#define PAGING_MEMORY (15*1024*1024)
#define PAGING_PAGES (PAGING_MEMORY>>12)
#define MAP_NR(addr) (((addr)-LOW_MEM)>>12)
#define USED 100

static long HIGH_MEMORY = 0;
static unsigned char mem_map[PAGING_PAGES] = {0,};

void mem_init(long start_mem,long end_mem){
	int i;
	HIGH_MEMORY = end_mem;
	for (i=0; i<PAGING_PAGES; i++)
		mem_map[i] = USED;
	i = MAP_NR(start_mem);
	end_mem -= start_mem;
	end_mem >>= 12;
	while(end_mem-- > 0)
		mem_map[i++]=0;
}

unsigned long get_free_page(void){
	register unsigned long _res asm("ax");
	__asm__ volatile ("std; repne; scasb\n\t"
		"jne 1f\n\t"
		"movb $1, 1(%%edi)\n\t"
		"sall $12, %%ecx\n\t"
		"addl %2, %%ecx\n\t"
		"movl %%ecx, %%edx\n\t"
		"movl $1024, %%ecx\n\t"
		"leal 4096(%%edx), %%edi\n\t"
		"rep; stosl;\n\t"
		"movl %%edx, %%eax\n"
		"1: cld"
		:"=a" (_res)
		:"0" (0),"i" (LOW_MEM),"c" (PAGING_PAGES),
		"D" (mem_map + PAGING_PAGES - 1));
}
