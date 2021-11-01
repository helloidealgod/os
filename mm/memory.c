#define LOW_MEM 0x100000
#define PAGING_MEMORY (15*1024*1024)
#define PAGING_PAGES (PAGING_MEMORY>>12)
#define MAP_NR(addr) (((addr)-LOW_MEM)>>12)
#define USED 100
#define invalidate() __asm__("movl %%eax,%%cr3"::"a" (0))
#define copy_page(from,to) \
__asm__("cld;rep;movsl"\
		::"S" (from),"D" (to),"c" (1024))

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
		"leal 4092(%%edx), %%edi\n\t"
		"rep; stosl;\n\t"
		"movl %%edx, %%eax\n"
		"1: cld"
		:"=a" (_res)
		:"0" (0),"i" (LOW_MEM),"c" (PAGING_PAGES),
		"D" (mem_map + PAGING_PAGES - 1));
}
int copy_page_tables(unsigned long from,unsigned long to,long size){
	unsigned long * from_page_table;
	unsigned long * to_page_table;
	unsigned long this_page;
	unsigned long * from_dir, * to_dir;
	unsigned long nr;
	
	if ((from & 0x3fffff) || (to & 0x3fffff)){
		return -1;
	}
	
	from_dir = (unsigned long *) ((from >> 20) & 0xffc);
	to_dir = (unsigned long *) ((to >> 20) & 0xffc);
	size = ((unsigned) (size + 0x3fffff)) >> 22;
	for (;size-->0;from_dir++,to_dir++){
		if (1 & *to_dir){
			return -1;
		}
		if (!(1 & *from_dir)){
			continue;
		}
		
		from_page_table = (unsigned long *)(0xfffff00 & *from_dir);
		if (!(to_page_table = (unsigned long *)get_free_page())){
			return -1;
		}
		
		*to_dir = ((unsigned long) to_page_table) | 7;
		nr = (from == 0) ? 0xa0 : 1024;
		for (;nr-- > 0;from_page_table++,to_page_table++){
			this_page = *from_page_table;
			if (!(1 & this_page)){
				continue;
			}
			this_page &= ~2;
			*to_page_table = this_page;
			if (this_page > LOW_MEM){
				*from_page_table = this_page;
				this_page -= LOW_MEM;
				this_page >>= 12;
				mem_map[this_page]++;
			}
		}
	}
	invalidate();
	return 0;
}

void free_page(unsigned long addr){
	if (addr < LOW_MEM) return;
	if (addr >= HIGH_MEMORY)
		panic("trying to free noneexistern");
	addr -= LOW_MEM;
	addr >>= 12;
	if (mem_map[addr]--) return;
	mem_map[addr] = 0;
	panic("trying to free page");
}
int free_page_tables(unsigned long from,unsigned long size){
	unsigned long * pg_table;
	unsigned long * dir,nr;
	
	if (from & 0x3fffff){
		
	}
	if(!from){
		
	}
	size = (size + 0x3fffff) >> 22;
	dir = (unsigned long *)((from >> 20) & 0xffc);
	for (;size-- >0; dir++){
		if (!(1 & *dir)){
			continue;
		}
		pg_table = (unsigned long *)(0xfffff000 & *dir);
		for (nr = 0; nr < 1024; nr++){
			if (1 & *pg_table){
				free_page(0xfffff000 & *pg_table);
			}
			*pg_table = 0;
			pg_table++;
		}
		free_page(0xfffff000 & *dir);
		*dir = 0;
	}
	invalidate();
	return 0;
}

void do_no_page(unsigned long error_code,unsigned long address){
	printk("do_no_page");
}

void un_wp_page(unsigned long * table_entry){
	unsigned long old_page,new_page;
	old_page = 0xfffff000 & *table_entry;
	if (old_page >= LOW_MEM && mem_map[MAP_NR(old_page)] == 1){
		*table_entry |= 2;
		invalidate();
		return;
	}
	if (!(new_page=get_free_page()))
		return;
	if (old_page >= LOW_MEM)
		mem_map[MAP_NR(old_page)]--;
	*table_entry = new_page | 7;
	invalidate();
	copy_page(old_page,new_page);
}

void do_wp_page(unsigned long error_code,unsigned long address){
	un_wp_page((unsigned long *)
		(((address>>10) & 0xffc) + (0xfffff000 &
		*((unsigned long *)((address>>20) & 0xffc)))));
}
