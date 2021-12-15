#include "../include/unistd.h"
#include "../include/system.h"
#define EXT_MEM_K (*(unsigned short *)0x90002)

#include "../include/io.h"
#include "../include/hdreg.h"
#include "../include/head.h"
#define NULL ((void *)0)
#include "../include/stdarg.h"
typedef int (*fn_ptr)();

extern int sys_setup();
extern int sys_fork();
extern int sys_read();
extern int sys_write();
extern int sys_open();
extern int sys_close();
extern int sys_pause();
extern int sys_execve();
fn_ptr sys_call_table[]={sys_setup,sys_fork,sys_read,sys_write,sys_open,sys_close,sys_pause,
		NULL,NULL,NULL,NULL,sys_execve};

static inline int fork(void) __attribute__((always_inline));
static inline int pause(void) __attribute__((always_inline));
static inline int setup(void * BIOS) __attribute__((always_inline));
static inline int write(unsigned int fd,char * buff,int count) __attribute__((always_inline));
static inline int read(unsigned int fd,char * buff,int count) __attribute__((always_inline));
static inline int execve(char * filename,char * argv,char * envp) __attribute__((always_inline));
static inline int open(const char * filename,int flag,int mode)__attribute__((always_inline));

static inline _syscall0(int,fork)
static inline _syscall0(int,exit)
static inline _syscall0(int,pause)
static inline _syscall1(int,setup,void *,BIOS)
static inline _syscall3(int,write,unsigned int,fd,char *,buff,int,count)
static inline _syscall3(int,read,unsigned int,fd,char *,buff,int,count)
static inline _syscall3(int,execve,char *,filename,char *,argv,char *,envp)
static inline _syscall3(int,open,const char *,filename,int,flag,int,mode)

static long memory_end = 0;
static long buffer_memory_end = 0;
static long main_memory_start = 0;

static char * argv = {"-/bin/sh",NULL};
static char * envp = {"HOME=/usr/root",NULL,NULL};

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
	blk_dev_init();
	sched_init();
	printk("init complete!\n");
	hd_init();
	printk("hd init complete\n");	
	//buffer 1M ~ 4M
	buffer_init(4*1024*1024);
	int i;
	for(i=0;i<15;i++){
//		printk("hello world!%d\n",i);
	}
	sti();
	move_to_user_mode();
	if(!fork()){
		init();
	}
	for(;;)
		pause();
	return 0;
}

static int printf(const char * fmt,...){
	int i;
	va_list args;
	char printbuf[128];

	va_start(args,fmt);
	i=vsprintf(printbuf,fmt,args);
	va_end(args);
	write(1,printbuf,i);
	
	return i;
}

void init(void){
//	setup((void *)&drive_info);	
	setup(0x90080);
//	printf("open file:/home/readme.txt\n");
//	int fd = open("/home/readme.txt",0,0);
	
	printf("open file:/init/hello\n");
	int fd1 = open("/init/hello",0,0);

	unsigned char s[512];
	int i;
	int count = read(fd1,s,512);
	for(i=0;i<count;i++){
		if(0 != i && 0 == i%26){
		//	printf("\n");
		}
		printf("i=%d %02x",i,s[i]);
	}
	printf("count=%d\n",count);
	printf("hi\n");
	printf("how are u\n");

	int pid;
	if(!(pid=fork())){
		execve("/init/hello",argv,envp);	
		while(1);
	}else{
		count = read(-999,s,512);
		printf("count1=%d\n",count);
	}
}
