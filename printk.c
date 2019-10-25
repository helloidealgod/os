#include "io.h"
#include "system.h"
static unsigned char cursor_x = 0;
static unsigned char cursor_y = 0;
void _printk(char c[],int length,char color){
	char *ptr;
	ptr = (unsigned char *)0xb8000 + 2 * cursor_x + 160 * cursor_y;
	int i;
	for(i=0;i<length;i++){
		*ptr++ = c[i];
		*ptr++ = color;
		cursor_x++;
		if(80 <= cursor_x){
			cursor_y+=1;
			cursor_x-=80;
		}
	}
	set_cursor(cursor_x,cursor_y);
}
void set_cursor(unsigned char x,unsigned char y){
	unsigned int p;
	p = x + y * 80;
	if(p >= 25*80){
		p = 0;
	}
	y = p & 0x00ff;
	x = (p & 0xff00) >> 8;
	cli();
	outb(0x3d4,0x0e);
	outb(0x3d5,x);
	outb(0x3d4,0x0f);
	outb(0x3d5,y);
	sti();
}
