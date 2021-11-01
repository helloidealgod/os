#include "../include/unistd.h"
#include "../include/system.h"
#define EXT_MEM_K (*(unsigned short *)0x90002)

#include "../include/io.h"
#include "../include/hdreg.h"
#include "../include/head.h"
#define NR_BLK_DEV	7
#define NR_REQUEST	32
#define NULL ((void *)0)
#define READA 1
#define READ 1
#define WRITEA 2
#define WRITE 2
#define CURRENT (blk_dev[3].current_request)
#define CURRENT_DEV DEVICE_NR(CURRENT->dev)

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
static inline int execve(char * filename,char * argv,char * envp) __attribute__((always_inline));

static inline _syscall0(int,fork)
static inline _syscall0(int,exit)
static inline _syscall0(int,pause)
static inline _syscall1(int,setup,void *,BIOS)
static inline _syscall3(int,write,unsigned int,fd,char *,buff,int,count)
static inline _syscall3(int,execve,char *,filename,char *,argv,char *,envp)



extern void read_intr(void);
extern void hd_out(unsigned int drive,unsigned int nsect,unsigned int sect,unsigned int head,unsigned int cyl,unsigned int cmd,void (*intr_addr)(void));

static long memory_end = 0;
static long buffer_memory_end = 0;
static long main_memory_start = 0;

static char * argv = {"-/bin/sh",NULL};
static char * envp = {"HOME=/usr/root",NULL,NULL};

struct hd_i_struct{
	int head,sect,cyl,wpcom,lzone,ctl;
};
extern struct hd_i_struct hd_info[2];

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
	sched_init();
	printk("init complete!\n");
	hd_init();
	printk("hd init complete\n");	
	sti();
/*	hd_out(0,1,1,1,1,WIN_WRITE,&write_intr);	
	int i,r;
	for(i=0 ; i<10000 && !(r=inb_p(HD_STATUS)&DRQ_STAT) ; i++){}
	if(!r){
		printk("error\n");	
	}else{
		char buffer[512]={24};
		for(i=0;i<512;i++){
			buffer[i] = 24;
		}
		port_write(HD_DATA,buffer,256);
	}
*/
//	hd_out(0,1,1,0,0,WIN_READ,&read_intr);	
	//buffer 1M ~ 4M
	buffer_init(4*1024*1024);
	move_to_user_mode();
	if(!fork()){
		init();
		while(1);
	}
	for(;;)
		pause();
	return 0;
}

static int printf(const char * fmt,...){
	int i;
	va_list args;
	char printbuf[128];
	write(1,printbuf,i=vsprintf(printbuf,fmt,args));
	va_end(args);
	return i;
}
void init(void){
//	setup((void *)&drive_info);	
	setup(0x90080);
	printf("test printf:hello init\n");
	int pid;
	if(!(pid=fork())){
		execve("",argv,envp);
	}
}
