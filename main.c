//#include "traps.c"
//#include "console.c"
int main(void){
	/*||内核||高速缓存||虚拟盘||主内存区*/
	//高速缓存结束4M 主内存区开始4.5M  物理内存 16M
//	int buffer_memory_end = 4 * 1024 * 1024;
//	int main_memory_start = 4.5 * 1024 * 1024;
	//mem_init(main_memory_start,memory_end);
//	trap_init();
//	con_init();	
	char a[26]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n',
		'o','p','q','r','s','t','u','v','w','x','y','z'};
	char d[10]={'0','1','2','3','4','5','6','7','8','9'};
	printk(a,26);
/*	_printk(a,26,0x04);
	_printk(a,26,0x02);
	_printk(a,26,0x01);
	_printk(d,10,0x06);*/
//	int i;
/*	for(i = 0;i < 8*25;i++){
		_printk(d,10,0x06);
	}*/
	//blk_dev_init();
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
