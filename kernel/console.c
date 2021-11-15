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

int fg_console = 0;
static unsigned char	video_type;		/* Type of display being used	*/
static unsigned long	video_num_columns;	/* Number of text columns	*/
static unsigned long	video_mem_base;		/* Base of video memory		*/
static unsigned long	video_mem_term;		/* End of video memory		*/
static unsigned long	video_size_row;		/* Bytes per row		*/
static unsigned long	video_num_lines;	/* Number of test lines		*/
static unsigned char	video_page;		/* Initial video page		*/
static unsigned short	video_port_reg;		/* Video register select port	*/
static unsigned short	video_port_val;		/* Video register value port	*/

static struct {
	unsigned short	vc_video_erase_char;	
	unsigned char	vc_attr;
	unsigned char	vc_def_attr;
	int		vc_bold_attr;
	unsigned long	vc_ques;
	unsigned long	vc_state;
	unsigned long	vc_restate;
	unsigned long	vc_checkin;
	unsigned long	vc_origin;		/* Used for EGA/VGA fast scroll	*/
	unsigned long	vc_scr_end;		/* Used for EGA/VGA fast scroll	*/
	unsigned long	vc_pos;
	unsigned long	vc_x,vc_y;
	unsigned long	vc_top,vc_bottom;
	unsigned long	vc_npar,vc_par[NPAR];
	unsigned long	vc_video_mem_start;	/* Start of video RAM		*/
	unsigned long	vc_video_mem_end;	/* End of video RAM (sort of)	*/
	unsigned int	vc_saved_x;
	unsigned int	vc_saved_y;
	unsigned int	vc_iscolor;
	char *		vc_translate;
} vc_cons [MAX_CONSOLES];

#define origin		(vc_cons[currcons].vc_origin)
#define scr_end		(vc_cons[currcons].vc_scr_end)
#define pos		(vc_cons[currcons].vc_pos)
#define top		(vc_cons[currcons].vc_top)
#define bottom		(vc_cons[currcons].vc_bottom)
#define x		(vc_cons[currcons].vc_x)
#define y		(vc_cons[currcons].vc_y)
#define state		(vc_cons[currcons].vc_state)
#define restate		(vc_cons[currcons].vc_restate)
#define checkin		(vc_cons[currcons].vc_checkin)
#define npar		(vc_cons[currcons].vc_npar)
#define par		(vc_cons[currcons].vc_par)
#define ques		(vc_cons[currcons].vc_ques)
#define attr		(vc_cons[currcons].vc_attr)
#define saved_x		(vc_cons[currcons].vc_saved_x)
#define saved_y		(vc_cons[currcons].vc_saved_y)
#define translate	(vc_cons[currcons].vc_translate)
#define video_mem_start	(vc_cons[currcons].vc_video_mem_start)
#define video_mem_end	(vc_cons[currcons].vc_video_mem_end)
#define def_attr	(vc_cons[currcons].vc_def_attr)
#define video_erase_char  (vc_cons[currcons].vc_video_erase_char)	
#define iscolor		(vc_cons[currcons].vc_iscolor)

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



static void cr(int currcons){
	pos -= x<<1;
	x=0;
}

static void lf(int currcons){
	if (y+1<bottom) {
		y++;
		pos += video_size_row;
		return;
	}
	scrup(currcons);
}

void console_print(const char * b){
	char * ptr;
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

	set_cursor(currcons);
}

