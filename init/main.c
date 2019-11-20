#define EXT_MEM_K (*(unsigned short *)0x90002)

static long memory_end = 0;
static long buffer_memory_end = 0;
static long main_memory_start = 0;

int main(void){
	memory_end = (1<<20) + (EXT_MEM_K << 10);
	memory_end &= 0xfffff000;
	if (memory_end > 16*1024*1024)
		memory_end = 16*1024*1024;
	if (memory_end > 12*1024*1024)
		buffer_memory_end = 4*1024*1024;
	else if (memory_end > 6*1024*1024)
		buffer_memory_end = 2*1024*1024;
	else
		buffer_memory_end = 1*1024*1024;
	main_memory_start = buffer_memory_end;
	mem_init(main_memory_start,memory_end);

	trap_init();
	con_init();	
	printk("hello world! \r\n");
	int i = (int)(EXT_MEM_K & 0x0000ffff);
	char s[10];
	itoa(i,s);
	printk(s);
	printk("\n");
	i = (int)(memory_end >> 20);
	itoa(i,s);
	printk("total memory: ");
	printk(s);
	printk("M \n");
//	sched_init();
	while(1);
	return 0;
}
/*
void test(const char* format,...){
	char *c = *(&format);
	printk(c);
	char c1 = *((char *)(&format + 1));
	if(c1 == 'a'){
		printk("success");
	}else{
		printk("failed");
	}
	int c2 = *((int *)(&format + 2));
	if(123 == c2){
		printk(" success");
	}else{
		printk(" failed");
	}
	long c3 = *((long *)(&format + 3));
	if(456 == c3){
		printk(" success");
	}else{
		printk(" failed");
	}
	double c4 = *((double *)(&format + 4));
	if(9876 == c4){
		printk(" success");
	}else{
		printk(" failed");
	}else{
		printk(" failed");
	}

}*/
