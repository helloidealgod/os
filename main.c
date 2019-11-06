#include "traps.c"
//#include "printk.c"
#include "console.c"
//void test(const char* format,...);
int main(void){
	/*||内核||高速缓存||虚拟盘||主内存区*/
	//高速缓存结束4M 主内存区开始4.5M  物理内存 16M
//	int buffer_memory_end = 4 * 1024 * 1024;
//	int main_memory_start = 4.5 * 1024 * 1024;
	//mem_init(main_memory_start,memory_end);
	trap_init();
//	con_init();	
/*	char *a = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *d = "0123456789";
	int h = 0;
	int l = 0;
	for(h=0;h<25;h++){
		for(l=0;l<8;l++){
			printk(d);
		}
	}
*///	_printk("hello world!",0x04);
//	_printk(a,0x02);
	printk("hello world! \r\n");
	printk("say hi,I'm Jiang! \r\n");
	printk("test\r\n");
//	printk(a);
/*	char c = 'a';
	int i = 123;
	long l = 456;
	double d = 9876;
	long long ll = 1098765;
	float pi = 3.1415926;
	test("test",c,i,l,d);
*/	//blk_dev_init();
	//chr_dev_init();
	//tty_init();
	//time_init();
	//sched_init();
	//buffer_init(buffer_memory_end);
	//hd_init();
	//floppy_init();
	//sti();
	//move_to_user_mode();
	//if(!fork()){
	//	init();
	//}
	//for(;;) pause();

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
	}
//	float c5 = *((float *)(&format + 5));
//	if(3.1415926 == c5){
//		printk(" success");
	}else{
		printk(" failed");
	}

}*/
