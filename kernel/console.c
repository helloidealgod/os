#include "../include/io.h"
#include "../include/system.h"

#define ORIG_VIDEO_MODE ((*(unsigned short *)0x90006) & 0xff)
#define ORIG_VIDEO_EGA_BX (*(unsigned short *)0x9000a)

static unsigned long video_mem_start;
static unsigned long video_mem_end;
static unsigned short video_port_reg;
static unsigned short video_port_val;
static unsigned long x = 0;
static unsigned long y = 0;

void con_init(){
	char *display_desc = "????";
	if(ORIG_VIDEO_MODE == 7){
		video_mem_start = 0xb0000;
		video_port_reg = 0x3b4;
		video_port_val = 0x3b5;
		if((ORIG_VIDEO_EGA_BX & 0xff) != 0x10){
			video_mem_end = 0xb8000;
			display_desc = "EGAm";
		}else{
			video_mem_end = 0xb2000;
			display_desc = "*MDA";
		}
	}else{
		video_mem_start = 0xb8000;
		video_port_reg = 0x3d4;
		video_port_val = 0x3d5;
		if((ORIG_VIDEO_EGA_BX & 0xff) != 0x10){
			video_mem_end = 0xbc000;
			display_desc = "EGAC";
		}else{
			video_mem_end = 0xba000;
			display_desc = "*CGA";
		}
	}
	
	x = 76;y = 0;
	printk(display_desc);
}
void printk(char c[]){
	char * ptr;
	int length = strlen(c);
	ptr=(unsigned char *)video_mem_start + 2*x + 160*y;
//	clear_line();
	int i;
	for(i=0;i<length;i++){
		if('\r' == c[i]){
			x = 0;
		}else if('\n' == c[i]){
			y ++;
//			clear_line();
		}else{
			*ptr++ = c[i];
			*ptr++ = 0x07;
			x++;
			if(80 <= x){
				y++;
				x-=80;
			}
		}
		if(25 <= y){
			y -= 25;
		}
	}	
	set_cursor(x,y);
}
void set_cursor(unsigned char x, unsigned char y){
	unsigned int p;
	p = x + y * 80;
	if(p >= 25*80){
		p = 0;
	}
	y = p & 0x00ff;
	x = (p & 0xff00) >> 8;
	cli();
	outb_p(video_port_reg,0x0e);
	outb_p(video_port_val,x);
	outb_p(video_port_reg,0x0f);
	outb_p(video_port_val,y);
	sti(); // 设置好IDT后取消注释，现在未设置中断，不能打开中断屏蔽
}
int strlen(char *s){
	int length = 0;
	while(*s++ != '\0'){
		length++;
	}
	return length;
}
void clear_line(){
	char * ptr;
	ptr=(unsigned char *)video_mem_start + 2*x + 160*y;
	int i;
	for(i=0;i<80-x;i++){
		*ptr++ = 32;
		*ptr++ = 0x07;
	}	
}
