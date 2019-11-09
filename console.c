#include "io.h"
#include "system.h"

static unsigned long x = 0;
static unsigned long y = 0;

void printk(char c[]){
	char * ptr;
	int length = strlen(c);
	unsigned long video_mem_start = 0xb8000;
	ptr=(unsigned char *)0xb8000 + 2*x + 160*y;
	clear_line();
	int i;
	for(i=0;i<length;i++){
		if('\r' == c[i]){
			x = 0;
		}else if('\n' == c[i]){
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
void set_cursor(unsigned char x, unsigned char y){
	unsigned int p;
	p = x + y * 80;
	if(p >= 25*80){
		p = 0;
	}
	y = p & 0x00ff;
	x = (p & 0xff00) >> 8;
	cli();
	outb_p(0x3d4,0x0e);
	outb_p(0x3d5,x);
	outb_p(0x3d4,0x0f);
	outb_p(0x3d5,y);
//	sti();
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
	unsigned long video_mem_start = 0xb8000;
	ptr=(unsigned char *)0xb8000 + 2*x + 160*y;
	int i;
	for(i=0;i<80-x;i++){
		*ptr++ = 32;
		*ptr++ = 0x07;
	}	
}
