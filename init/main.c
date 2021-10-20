#include "../include/unistd.h"
#include "../include/system.h"
#define EXT_MEM_K (*(unsigned short *)0x90002)

#include "../include/io.h"
#include "../include/hdreg.h"
#include "../include/head.h"
#define IN_ORDER(s1,s2) \
((s1)->cmd<(s2)->cmd || (s1)->cmd==(s2)->cmd && \
((s1)->dev < (s2)->dev || ((s1)->dev == (s2)->dev && \
(s1)->sector < (s2)->sector)))

#define NR_BLK_DEV	7
#define NR_REQUEST	32
#define NULL ((void *)0)
#define READA 1
#define READ 1
#define WRITEA 2
#define WRITE 2
#define CURRENT (blk_dev[3].current_request)
#define CURRENT_DEV DEVICE_NR(CURRENT->dev)

#define DEVICE_INTR do_hd
#ifdef DEVICE_INTR
void (*DEVICE_INTR)(void) = NULL;
#endif
#define SET_INTR(x) (DEVICE_INTR = (x))

#define port_read(port,buf,nr)\
__asm__("cld;rep;insw"::"d"(port),"D"(buf),"c"(nr):)

#define port_write(port,buf,nr)\
__asm__("cld;rep;outsw"::"d"(port),"S"(buf),"c"(nr):)




typedef int (*fn_ptr)();
extern int sys_fork();
extern int sys_pause();
fn_ptr sys_call_table[]={sys_fork,sys_pause};
extern hd_interrupt(void);

static long memory_end = 0;
static long buffer_memory_end = 0;
static long main_memory_start = 0;

static inline int fork(void) __attribute__((always_inline));
static inline int pause(void) __attribute__((always_inline));
static inline _syscall0(int,fork)
static inline _syscall0(int,exit)
static inline _syscall0(int,pause)

struct buffer_head {
	char * b_data;
	unsigned long b_blocknr;
	unsigned short b_dev;
	unsigned char b_uptodate;
	unsigned char b_dirt;
	unsigned char b_count;
	unsigned char b_lock;
	struct task_struct * b_wait;
	struct buffer_head * b_prev;
	struct buffer_head * b_next;
	struct buffer_head * b_prev_free;
	struct buffer_head * b_next_free;
};
struct request{
	int dev;
	int cmd;
	int errors;
	unsigned long sector;
	unsigned long nr_sectors;
	char * buffer;
	struct task_struct * waiting;
	struct buffer_head * bh;
	struct request * next;
};
struct blk_dev_struct {
	void (*request_fn)(void);
	struct request * current_request;
};

struct request request[NR_REQUEST];
struct blk_dev_struct blk_dev[NR_BLK_DEV]={
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL},
	{NULL, NULL}
};

struct hd_i_struct {
	int head,sect,cyl,wpcom,lzone,ctl;
};
struct hd_i_struct hd_info[] = { {0,0,0,0,0,0},{0,0,0,0,0,0,} };

static int controller_ready(void){
	int retries = 100000;
	//while(--retries && (inb_p(HD_STATUS)&0xc0) != 0x40);
	while(--retries && (inb_p(HD_STATUS)&0x80));
	return retries;
}
static int win_result(void){
	int i = inb_p(HD_STATUS);	
//	printk("HD_STATUS=%X\n",i);
	if((i & (BUSY_STAT | READY_STAT | WRERR_STAT | SEEK_STAT |ERR_STAT)) == (READY_STAT | SEEK_STAT)){
		return (0);
	}
	if(i&1){
		i = inb(HD_ERROR);
		printk("HD_ERROR=%X\n",i);
	}
	i = inb(HD_ERROR);
	return (1);
}
static void hd_out(unsigned int drive,unsigned int nsect,unsigned int sect,
		unsigned int head,unsigned int cyl,unsigned int cmd,
		void (*intr_addr)(void))
{
	register int port asm("dx");

//	if (drive>1 || head>15)
//		panic("Trying to write bad sector");
	if (!controller_ready()){
//		panic("HD controller not ready");
		printk("HD contrller not ready\n");
		return;
	}
	SET_INTR(intr_addr);
	outb_p(HD_CMD,hd_info[drive].ctl);
	port=HD_DATA;
	outb_p(++port,hd_info[drive].wpcom>>2);
	outb_p(++port,nsect);
	outb_p(++port,sect);
	outb_p(++port,cyl);
	outb_p(++port,cyl>>8);
	outb_p(++port,0xA0|(drive<<4)|head);
	outb(++port,cmd);
}
static void read_intr(void)
{
//	if (win_result()) {
//		bad_rw_intr();
//		do_hd_request();
//		return;
//	}
	printk("read_intr\n");
//	cli();
	char buffer[512]={0};
	int i=0;
	for(i=0;i<512;i++){
		buffer[i] = 1;
	}
	port_read(HD_DATA,buffer,256);

	for(i=0;i<512;i++){
		printk("%d ",buffer[i]);
	}
	printk("\n");
	SET_INTR(&read_intr);
//	sti();
	return;
	port_read(HD_DATA,CURRENT->buffer,256);
	CURRENT->errors = 0;
	CURRENT->buffer += 512;
	CURRENT->sector++;
	if (--CURRENT->nr_sectors) {
		SET_INTR(&read_intr);
		return;
	}
//	end_request(1);
//	do_hd_request();
}
static void write_intr(void)
{
	if (win_result()) {
//		bad_rw_intr();
//		do_hd_request();
		printk("write intr error\n");
		return;
	}
	printk("write intr\n");
	char buffer[512] = {2};
	int i=0;
	for(i=0;i<512;i++){
		buffer[i] = 1;
	}
	SET_INTR(&write_intr);
//	port_write(HD_DATA,buffer,256);
	return;
	if (--CURRENT->nr_sectors) {
		CURRENT->sector++;
		CURRENT->buffer += 512;
		SET_INTR(&write_intr);
		port_write(HD_DATA,CURRENT->buffer,256);
		return;
	}
//	end_request(1);
//	do_hd_request();
}
/*
void do_hd_request(void)
{
	int i,r;
	unsigned int block,dev;
	unsigned int sec,head,cyl;
	unsigned int nsect;

	INIT_REQUEST;
	dev = MINOR(CURRENT->dev);
	block = CURRENT->sector;
	if (dev >= 5*NR_HD || block+2 > hd[dev].nr_sects) {
		end_request(0);
		goto repeat;
	}
	block += hd[dev].start_sect;
	dev /= 5;
	__asm__("divl %4":"=a" (block),"=d" (sec):"0" (block),"1" (0),
		"r" (hd_info[dev].sect));
	__asm__("divl %4":"=a" (cyl),"=d" (head):"0" (block),"1" (0),
		"r" (hd_info[dev].head));
	sec++;
	nsect = CURRENT->nr_sectors;
	if (reset) {
		recalibrate = 1;
		reset_hd();
		return;
	}
	if (recalibrate) {
		recalibrate = 0;
		hd_out(dev,hd_info[CURRENT_DEV].sect,0,0,0,
			WIN_RESTORE,&recal_intr);
		return;
	}
	if (CURRENT->cmd == WRITE) {
		hd_out(dev,nsect,sec,head,cyl,WIN_WRITE,&write_intr);
		for(i=0 ; i<10000 && !(r=inb_p(HD_STATUS)&DRQ_STAT) ; i++)
			// nothing ;
		if (!r) {
			bad_rw_intr();
			goto repeat;
		}
		port_write(HD_DATA,CURRENT->buffer,256);
	} else if (CURRENT->cmd == READ) {
		hd_out(dev,nsect,sec,head,cyl,WIN_READ,&read_intr);
	} else
		panic("unknown hd-command");
}
*/
void unexpected_hd_interrupt(void){
	printk("unexpected hd interrupt\n");
}
void hd_init(void)
{
//	blk_dev[MAJOR_NR].request_fn = DEVICE_REQUEST;
	set_intr_gate(46,&hd_interrupt);
	outb_p(0x21,inb_p(0x21)&0xfb);
	outb(0xA1,inb_p(0xA1)&0xbf);
}

void ll_rw_block(int rw, struct buffer_heard *bh){

}

void blk_dev_init(){

}

static void add_request(struct blk_dev_struct * dev, struct request * req){
    struct request * tmp;
	req->next = NULL;
	cli();
	if (req->bh)
		req->bh->b_dirt = 0;
	if (!(tmp = dev->current_request)){
		dev->current_request = req;
		sti();
		(dev->request_fn)();
		return;
	}
	for (; tmp->next; tmp=tmp->next){
		if (!req->bh)
			if (tmp->next->bh)
				break;
			else
				continue;
		if ((IN_ORDER(tmp,req) ||
		 !IN_ORDER(tmp,tmp->next))&&
		 IN_ORDER(req,tmp->next))
		break;
	}
	req->next = tmp->next;
	tmp->next = req;
	sti();
}

static void make_request(int major, int rw, struct buffer_head * bh){
    struct request * req;
	int rw_ahead;

	if (rw_ahead = (rw == READA || rw == WRITEA)){
		if (bh->b_lock)
			return;
		if (rw == READA)
			rw = READ;
		else
			rw = WRITE;
	}
//	if (rw != READ && rw != WRITE)
//		panic("");
//	lock_buffer(bh);
//	if ((rw == WRITE && !bh->b_dirt) || (rw == READ && bh->b_uptodate)) {
//		unlock_buffer(bh);
//		return;
//	}
repeat:
	if (rw == READ)
		req = request + NR_REQUEST;
	else
		req = request + ((NR_REQUEST*2)/3);
	while (--req >= request)
		if (req->dev < 0 )
			break;
	if (req < request){
		if (rw_ahead){
//			unlock_buffer(bh);
			return;
		}
//		sleep_on(&wait_for_request);
		goto repeat;
	}

	req->dev = bh->b_dev;
	req->cmd = rw;
	req->errors = 0;
	req->sector = bh->b_blocknr<<1;
	req->nr_sectors = 2;
	req->buffer = bh->b_data;
	req->waiting = NULL;
	req->bh = bh;
	req->next = NULL;
	add_request(major + blk_dev, req);
}


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
	void * BIOS;
	BIOS = 0x90080;
	hd_info[0].cyl = *(unsigned short *)BIOS;
	hd_info[0].head = *(unsigned char *)(2+BIOS);
	hd_info[0].wpcom = *(unsigned short *)(5+BIOS);

	hd_info[0].ctl = *(unsigned char *)(8+BIOS);
	hd_info[0].lzone = *(unsigned short *)(12+BIOS);
	
	hd_info[0].sect = *(unsigned char *)(14+BIOS);
	
	printk("cyl=%u\n",hd_info[0].cyl);
	printk("head=%u\n",hd_info[0].head);
	printk("wpcom=%u\n",hd_info[0].wpcom);
	printk("ctl=%u\n",hd_info[0].ctl);
	printk("sect=%u\n",hd_info[0].sect);
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
	hd_out(0,1,1,1,1,WIN_READ,&read_intr);	
//	move_to_user_mode();
/*	if(!fork()){
		int a,b,c,d;
		if(!fork()){
			while(1){
				a++;
			}
		}
		if(!fork()){
			while(1){
			  	b++;
			}
		}
		if(!fork()){
			while(1){
				c++;
			}
		}
		if(!fork()){
			while(1){
				d++;
			}
		}
		for(;;)
			pause();
	}
	for(;;)
		pause();
*/	return 0;
}
