#include "../include/io.h"
#include "../include/system.h"
#include "../include/head.h"
#include "../include/tty.h"

#define ORIG_VIDEO_MODE ((*(unsigned short *)0x90006) & 0xff)
#define ORIG_VIDEO_EGA_BX (*(unsigned short *)0x9000a)

static unsigned long video_mem_start;
static unsigned long video_mem_end;
static unsigned short video_port_reg;
static unsigned short video_port_val;
static unsigned long x = 0;
static unsigned long y = 0;

extern void keyboard_interrupt(void);

void con_init(){
	register unsigned char a;
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
	set_trap_gate(0x21,&keyboard_interrupt);
	outb_p(0x21,inb_p(0x21)&0xfd);
	a=inb_p(0x61);
	outb_p(0x61,a|0x80);
	outb_p(0x61,a);
}

void con_write(struct tty_struct * tty){
	int nr;
	char c;

	char * ptr;
	ptr=(unsigned char *)video_mem_start + 2*x + 160*y;

	nr = CHARS(tty->write_q);
	while (nr--){
		GETCH(tty->write_q,c);
		if (c>31 && c <127){
		/*	if(x >= video_num_columns){
				x -= video_num_columns;
				pos -= video_size_row;
				lf();
			}
			__asm__("movb _attr,%%ah\n\t"
				“movw %%ax,%1\n\t”
				::"a" (c),"m" (*(short *)pos)
				:"ax");
			pos += 2;
			x++;*/
			if('\r' == c){
				x = 0;
			}else if('\n' == c){
				x = 0;
				y ++;
			}else{
				*ptr++ = c;
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
			set_cursor(x,y);
		}
	}
}

/**
void printk(char c[]){
	char * ptr;
	int length = strlen(c);
	ptr=(unsigned char *)video_mem_start + 2*x + 160*y;
	clear_line();
	int i;
	for(i=0;i<length;i++){
		if('\r' == c[i]){
			x = 0;
		}else if('\n' == c[i]){
			x = 0;
			y ++;
			clear_line();
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
*/

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



static void cr(int currcons)
{
	pos -= x<<1;
	x=0;
}

static void lf(int currcons)
{
	if (y+1<bottom) {
		y++;
		pos += video_size_row;
		return;
	}
	scrup(currcons);
}

void console_print(const char * b)
{
	int currcons = fg_console;
	char c;

	while (c = *(b++)) {
		if (c == 10) {
			cr(currcons);
			lf(currcons);
			continue;
		}
		if (c == 13) {
			cr(currcons);
			continue;
		}
		if (x>=video_num_columns) {
			x -= video_num_columns;
			pos -= video_size_row;
			lf(currcons);
		}
		__asm__("movb %2,%%ah\n\t"
			"movw %%ax,%1\n\t"
			::"a" (c),
			"m" (*(short *)pos),
			"m" (attr)
			:"ax");
		pos += 2;
		x++;
	}
	set_cursor(currcons);
}
