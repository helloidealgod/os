#include "../../include/hdreg.h"
#include "../../include/io.h"
#include "../../include/head.h"
#include "../../include/system.h"
#include "../../include/fs.h"
#include "blk.h"

#define IN_ORDER(s1,s2) \
((s1)->cmd<(s2)->cmd || (s1)->cmd==(s2)->cmd && \
((s1)->dev < (s2)->dev || ((s1)->dev == (s2)->dev && \
(s1)->sector < (s2)->sector)))

#define NR_BLK_DEV	7
#define NR_REQUEST	32
#define NULL ((void *)0)
#define CURRENT (blk_dev[3].current_request)
#define CURRENT_DEV DEVICE_NR(CURRENT->dev)

#define DEVICE_INTR do_hd
#ifdef DEVICE_INTR
void (*DEVICE_INTR)(void) = NULL;
#endif
#define SET_INTR(x) (DEVICE_INTR = (x))
#define MAX_HD 2
#define MAJOR_NR 3

#define port_read(port,buf,nr)\
__asm__("cld;rep;insw"::"d"(port),"D"(buf),"c"(nr):)

#define port_write(port,buf,nr)\
__asm__("cld;rep;outsw"::"d"(port),"S"(buf),"c"(nr):)

extern hd_interrupt(void);

struct request request[NR_REQUEST];

struct hd_i_struct {
	int head,sect,cyl,wpcom,lzone,ctl;
};
struct hd_i_struct hd_info[] = { {0,0,0,0,0,0},{0,0,0,0,0,0,} };

static int NR_HD = 0;

static struct hd_struct{
	long start_sect;
	long nr_sects;
}hd[5*MAX_HD]={{0,0},};

int sys_setup(void * BIOS){
	static int callable = 1;
	int drive,i;
	struct buffer_head * bh;

	if(!callable){
		return -1;
	}
	callable = 0;
	for(drive=0;drive<2;drive++){
		hd_info[drive].cyl = *(unsigned short *)BIOS;
		hd_info[drive].head = *(unsigned char *)(2+BIOS);
		hd_info[drive].wpcom = *(unsigned short *)(5+BIOS);
		hd_info[drive].ctl = *(unsigned char *)(8+BIOS);
		hd_info[drive].lzone = *(unsigned short *)(12+BIOS);
		hd_info[drive].sect = *(unsigned char *)(14+BIOS);
		BIOS +=16;

		printk("cyl=%u,head=%u,sect=%u\n",hd_info[drive].cyl,hd_info[drive].head,hd_info[drive].sect);
	}
	if(hd_info[1].cyl){
		NR_HD = 2;
	}else{
		NR_HD = 1;
	}
	for(i=0;i<NR_HD;i++){
		hd[i*5].start_sect = 0;
		hd[i*5].nr_sects = hd_info[i].head * hd_info[i].sect * hd_info[i].cyl;
		printk("start_sect=%u,nr_sects=%u\n",hd[i*5].start_sect,hd[i*5].nr_sects);
	}

	for(drive=0;drive<NR_HD;drive++){
		if(!(bh = bread(0x300 + drive*5,0))){
			printk("Unable to read partition table of drive %d\n",drive);
			panic("");
		}
		printk("%02x %02x\n",(unsigned char)bh->b_data[510],(unsigned char)bh->b_data[511]);
		if(bh->b_data[510] != 0x55 || (unsigned char)bh->b_data[511] != 0xAA){
			printk("Bad partition table on drive %d\n",drive);
			panic("");
		}
		unsigned short DBR_sects = *((unsigned short *)&bh->b_data[0x0E]);
		unsigned short FAT_sects = *((unsigned short *)&bh->b_data[0x16]);
		unsigned char fats = (unsigned char)bh->b_data[0x10];
		unsigned short root_max_dirs =*((unsigned short *)&bh->b_data[0x11]);
		unsigned char cluster_sects = (unsigned char)bh->b_data[0x0D];
		int data_start = DBR_sects + FAT_sects*fats + 32;
		printk("fat start sect no=%d\n",DBR_sects);
		printk("fdt start sect no=%d\n",DBR_sects+FAT_sects*fats);
		printk("root max dirs=%d,cluster_sects=%d\n",root_max_dirs,cluster_sects);
		printk("DATA start sect = %d\n",data_start);

		if(!(bh = bread(0x300 + drive*5,34))){
			printk("read FAT failed\n");
		}
/*		for(i=0;i<32*10;i++){
			if(0!=i && 0==i%16){
				printk("\n");
			}
			printk("%02X ",(unsigned char)bh->b_data[i]);
		}
		printk("\n");
*/
		int length;
		for(i=0;i<32;i++){
			if(0xE5 == (unsigned char)bh->b_data[0x00 + i*32]){
				//item has been deleted
			}else if(0x0F == (unsigned char)bh->b_data[0x0B + i*32]){
				printk("long file dir,no=%d;",0x0f & bh->b_data[0 + i*32]);
			}else if(0x00 != (unsigned char)bh->b_data[0x0B + i*32]){
				unsigned short no = (unsigned short)bh->b_data[0x1A + i*32];
				printk("short file dir,start_cluster_no=%d,filename=",no);
				for(length=0;length<8;length++){
					if(0x20 == (unsigned char)bh->b_data[length+i*32]){
						printk("\n");
						break;
					}
					printk("%c",(unsigned char)bh->b_data[length+i*32]);
					if(7==length){
						printk("\n");
					}
				}
			}
		}
		int cluster = 12;
		int home_sects = data_start + (cluster - 2) * cluster_sects;
	        int home_block = home_sects / 2;	
		printk("h_sects=%d,h_block=%d\n",home_sects,home_block);
		if(!(bh = bread(0x300 + drive*5,home_block))){
			printk("read FAT failed\n");
		}
		for(i=0;i<12;i++){
			if(0xE5 == (unsigned char)bh->b_data[0x00 + i*32]){
				//item has been deleted
			}else if(0x0F == (unsigned char)bh->b_data[0x0B + i*32]){
				printk("long file dir,no=%d;",0x0f & bh->b_data[0 + i*32]);
			}else if(0x00 != (unsigned char)bh->b_data[0x0B + i*32]){
				unsigned short no = (unsigned short)bh->b_data[0x1A + i*32];
				printk("short file dir,start_cluster_no=%d,filename=",no);
				for(length=0;length<8;length++){
					if(0x20 == (unsigned char)bh->b_data[length+i*32]){
						printk("\n");
						break;
					}
					printk("%c",(unsigned char)bh->b_data[length+i*32]);
					if(7==length){
						printk("\n");
					}
				}
			}
		}
		cluster = 13;
		home_sects = data_start + (cluster - 2) * cluster_sects;
	        home_block = home_sects / 2;	
		if(!(bh = bread(0x300 + drive*5,home_block))){
			printk("read FAT failed\n");
		}
		for(i=0;i<128;i++){
			if(0==bh->b_data[i]){
				break;
			}else{
				printk("%c",(unsigned char)bh->b_data[i]);
			}
		}
	}
	printk("setup complete\n");
	return 0;
}
static int controller_ready(void){
	int retries = 100000;
	//while(--retries && (inb_p(HD_STATUS)&0xc0) != 0x40);
	while(--retries && (inb_p(HD_STATUS)&0x80));
	return retries;
}

static int win_result(void){
	int i = inb_p(HD_STATUS);	
	if((i & (BUSY_STAT | READY_STAT | WRERR_STAT | SEEK_STAT |ERR_STAT)) == (READY_STAT | SEEK_STAT)){
		return (0);
	}
	if(i&1){
		i = inb(HD_ERROR);
	}
	i = inb(HD_ERROR);
	return (1);
}

static void hd_out(unsigned int drive,unsigned int nsect,unsigned int sect,
		unsigned int head,unsigned int cyl,unsigned int cmd,
		void (*intr_addr)(void))
/*
void hd_out(unsigned int drive,unsigned int nsect,unsigned int sect,
		unsigned int head,unsigned int cyl,unsigned int cmd,
		void (*intr_addr)(void))
*/
{
	register int port asm("dx");

	if (drive>1 || head>15)
		panic("Trying to write bad sector");
	if (!controller_ready()){
		panic("HD controller not ready\n");
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

static void lock_buffer(struct buffer_head * bh){
	cli();
	while(bh->b_lock){
		sleep_on(&bh->b_wait);
	}
	bh->b_lock = 1;
	sti();
}

static void unlock_buffer(struct buffer_head * bh){
	bh->b_lock = 0;
	wake_up(&bh->b_wait);
}

static void end_request(int uptodata){
	if(CURRENT->bh){
		CURRENT->bh->b_uptodata = uptodata;
		unlock_buffer(CURRENT->bh);
	}
	if(!uptodata){
		printk("I/O error\n");
	}
	wake_up(&CURRENT->waiting);
//	wake_up(&wait_for_request);
	CURRENT->dev = -1;
	CURRENT = CURRENT->next;
}

static void read_intr(void)
{
	if (win_result()) {
//		bad_rw_intr();
		do_hd_request();
		return;
	}
	int i=0;

	port_read(HD_DATA,CURRENT->buffer,256);

/*	printk("read_intr\n");
	for(i=0;i<512;i++){
		printk("%d ",CURRENT->buffer[i]);
	}
*/
	CURRENT->errors = 0;
	CURRENT->buffer += 512;
	CURRENT->sector++;
	if (--CURRENT->nr_sectors) {
		SET_INTR(&read_intr);
		return;
	}
	printk("read_intr end\n");
	end_request(1);
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
	end_request(1);
	do_hd_request();
}

void do_hd_request(void)
{
	int i,r;
	unsigned int block,dev;
	unsigned int sec,head,cyl;
	unsigned int nsect;

//	INIT_REQUEST;
	dev = MINOR(CURRENT->dev);
	block = CURRENT->sector;
/*	if (dev >= 5*NR_HD || block+2 > hd[dev].nr_sects) {
		end_request(0);
		goto repeat;
	}
*/	block += hd[dev].start_sect;
//	printk("block=%d\n");
	dev /= 5;
	__asm__("divl %4":"=a" (block),"=d" (sec):"0" (block),"1" (0),
		"r" (hd_info[dev].sect));
	__asm__("divl %4":"=a" (cyl),"=d" (head):"0" (block),"1" (0),
		"r" (hd_info[dev].head));
	sec++;
	nsect = CURRENT->nr_sectors;
/*	if (reset) {
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
*/	
	if (CURRENT->cmd == WRITE) {
		hd_out(dev,nsect,sec,head,cyl,WIN_WRITE,&write_intr);
		for(i=0 ; i<10000 && !(r=inb_p(HD_STATUS)&DRQ_STAT) ; i++)
			// nothing ;
		if (!r) {
//			bad_rw_intr();
//			goto repeat;
		}
		port_write(HD_DATA,CURRENT->buffer,256);
	} else if (CURRENT->cmd == READ) {
//		printk("dev=%d,nsect=%d,sec=%d,head=%d,cyl=%d\n",dev,nsect,sec,head,cyl);
		hd_out(dev,nsect,sec,head,cyl,WIN_READ,&read_intr);
	} else
		panic("unknown hd-command");
}

void unexpected_hd_interrupt(void){
	printk("unexpected hd interrupt\n");
}

void hd_init(void)
{
	blk_dev[MAJOR_NR].request_fn = DEVICE_REQUEST;
	set_intr_gate(46,&hd_interrupt);
	outb_p(0x21,inb_p(0x21)&0xfb);
	outb(0xA1,inb_p(0xA1)&0xbf);
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
	if (rw != READ && rw != WRITE)
		panic("");
	lock_buffer(bh);
	if ((rw == WRITE && !bh->b_dirt) || (rw == READ && bh->b_uptodata)) {
		unlock_buffer(bh);
		return;
	}
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
			unlock_buffer(bh);
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

