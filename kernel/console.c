#include "../include/io.h"
#include "../include/system.h"
#include "../include/head.h"
#include "../include/tty.h"
#include "../include/sched.h"

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

#define MAX_CONSOLES 1
int currcons=0;
#define NPAR 1
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

#define DEF_TERMIOS \
(struct termios) { \
	ICRNL, \
	OPOST | ONLCR, \
	0, \
	IXON | ISIG | ICANON | ECHO | ECHOCTL | ECHOKE, \
	0, \
	INIT_C_CC \
}

#define ORIG_X			(*(unsigned char *)0x90000)
#define ORIG_Y			(*(unsigned char *)0x90001)
#define ORIG_VIDEO_PAGE		(*(unsigned short *)0x90004)
#define ORIG_VIDEO_MODE		((*(unsigned short *)0x90006) & 0xff)
#define ORIG_VIDEO_COLS 	(((*(unsigned short *)0x90006) & 0xff00) >> 8)
#define ORIG_VIDEO_LINES	((*(unsigned short *)0x9000e) & 0xff)
#define ORIG_VIDEO_EGA_AX	(*(unsigned short *)0x90008)
#define ORIG_VIDEO_EGA_BX	(*(unsigned short *)0x9000a)
#define ORIG_VIDEO_EGA_CX	(*(unsigned short *)0x9000c)

#define VIDEO_TYPE_MDA		0x10	/* Monochrome Text Display	*/
#define VIDEO_TYPE_CGA		0x11	/* CGA Display 			*/
#define VIDEO_TYPE_EGAM		0x20	/* EGA/VGA in Monochrome Mode	*/
#define VIDEO_TYPE_EGAC		0x21	/* EGA/VGA in Color Mode	*/

static char * translations[] = {
/* normal 7-bit ascii */
	" !\"#$%&'()*+,-./0123456789:;<=>?"
	"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
	"`abcdefghijklmnopqrstuvwxyz{|}~ ",
/* vt100 graphics */
	" !\"#$%&'()*+,-./0123456789:;<=>?"
	"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^ "
	"\004\261\007\007\007\007\370\361\007\007\275\267\326\323\327\304"
	"\304\304\304\304\307\266\320\322\272\363\362\343\\007\234\007 "
};

#define NORM_TRANS (translations[0])
#define GRAF_TRANS (translations[1])

int blankinterval = 0;
int blankcount = 0;

static inline void gotoxy(int currcons, int new_x,unsigned int new_y)
{
	if (new_x > video_num_columns || new_y >= video_num_lines)
		return;
	x = new_x;
	y = new_y;
	pos = origin + y*video_size_row + (x<<1);
}

static inline void set_origin(int currcons)
{
	if (video_type != VIDEO_TYPE_EGAC && video_type != VIDEO_TYPE_EGAM)
		return;
	if (currcons != fg_console)
		return;
	cli();
	outb_p(video_port_reg,12);
	outb_p(video_port_val,0xff&((origin-video_mem_base)>>9));
	outb_p(video_port_reg,13);
	outb_p(video_port_val,0xff&((origin-video_mem_base)>>1));
	sti();
}

static void scrup(int currcons)
{
	if (bottom<=top)
		return;
	if (video_type == VIDEO_TYPE_EGAC || video_type == VIDEO_TYPE_EGAM)
	{
		if (!top && bottom == video_num_lines) {
			origin += video_size_row;
			pos += video_size_row;
			scr_end += video_size_row;
			if (scr_end > video_mem_end) {
				__asm__("cld\n\t"
					"rep\n\t"
					"movsl\n\t"
					"movl video_num_columns,%1\n\t"
					"rep\n\t"
					"stosw"
					::"a" (video_erase_char),
					"c" ((video_num_lines-1)*video_num_columns>>1),
					"D" (video_mem_start),
					"S" (origin));
				//	:"cx","di","si");
				scr_end -= origin-video_mem_start;
				pos -= origin-video_mem_start;
				origin = video_mem_start;
			} else {
				__asm__("cld\n\t"
					"rep\n\t"
					"stosw"
					::"a" (video_erase_char),
					"c" (video_num_columns),
					"D" (scr_end-video_size_row));
				//	:"cx","di");
			}
			set_origin(currcons);
		} else {
			__asm__("cld\n\t"
				"rep\n\t"
				"movsl\n\t"
				"movl video_num_columns,%%ecx\n\t"
				"rep\n\t"
				"stosw"
				::"a" (video_erase_char),
				"c" ((bottom-top-1)*video_num_columns>>1),
				"D" (origin+video_size_row*top),
				"S" (origin+video_size_row*(top+1)));
			//	:"cx","di","si");
		}
	}
	else		/* Not EGA/VGA */
	{
		__asm__("cld\n\t"
			"rep\n\t"
			"movsl\n\t"
			"movl video_num_columns,%%ecx\n\t"
			"rep\n\t"
			"stosw"
			::"a" (video_erase_char),
			"c" ((bottom-top-1)*video_num_columns>>1),
			"D" (origin+video_size_row*top),
			"S" (origin+video_size_row*(top+1)));
		//	:"cx","di","si");
	}
}

static void scrdown(int currcons)
{
	if (bottom <= top)
		return;
	if (video_type == VIDEO_TYPE_EGAC || video_type == VIDEO_TYPE_EGAM)
	{
		__asm__("std\n\t"
			"rep\n\t"
			"movsl\n\t"
			"addl $2,%%edi\n\t"	/* %edi has been decremented by 4 */
			"movl video_num_columns,%%ecx\n\t"
			"rep\n\t"
			"stosw"
			::"a" (video_erase_char),
			"c" ((bottom-top-1)*video_num_columns>>1),
			"D" (origin+video_size_row*bottom-4),
			"S" (origin+video_size_row*(bottom-1)-4));
		//	:"ax","cx","di","si");
	}
	else		/* Not EGA/VGA */
	{
		__asm__("std\n\t"
			"rep\n\t"
			"movsl\n\t"
			"addl $2,%%edi\n\t"	/* %edi has been decremented by 4 */
			"movl video_num_columns,%%ecx\n\t"
			"rep\n\t"
			"stosw"
			::"a" (video_erase_char),
			"c" ((bottom-top-1)*video_num_columns>>1),
			"D" (origin+video_size_row*bottom-4),
			"S" (origin+video_size_row*(bottom-1)-4));
		//	:"ax","cx","di","si");
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

static void del(int currcons){
	if(x){
		pos -= 2;
		x--;
		*(unsigned short *)pos = video_erase_char;
	}
}

static inline void set_cursor(int currcons){
	blankcount = blankinterval;
	if (currcons != fg_console)
		return;
	cli();
	outb_p(video_port_reg,14);
	outb_p(video_port_val,0xff&((pos-video_mem_base)>>9));
	outb_p(video_port_reg,15);
	outb_p(video_port_val,0xff&((pos-video_mem_base)>>1));
	sti();
}

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

//	x = 76;y = 0;
	gotoxy(currcons,ORIG_X,ORIG_Y);
	update_screen();
	printk(display_desc);
	set_trap_gate(0x21,&keyboard_interrupt);
	outb_p(0x21,inb_p(0x21)&0xfd);
	a=inb_p(0x61);
	outb_p(0x61,a|0x80);
	outb_p(0x61,a);
}

void update_screen(void){
	set_origin(fg_console);
	set_cursor(fg_console);
}
/*
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
/*			if('\r' == c){
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
*/

enum{ESnormal,ESesc,ESsquare,ESgetpars,ESgotpars,ESfunckey,ESsetterm,ESsetgraph};

void con_write(struct tty_struct * tty)
{
	int nr;
	char c;
	int currcons;
     
	currcons = tty - tty_table;
	if ((currcons>=MAX_CONSOLES) || (currcons<0))
		panic("con_write: illegal tty");
 	   
	nr = CHARS(tty->write_q);
	while (nr--) {
		if (tty->stopped)
			break;
		GETCH(tty->write_q,c);
		if (c == 24 || c == 26)
			state = ESnormal;
		switch(state) {
			case ESnormal:
				if (c>31 && c<127) {
					if (x>=video_num_columns) {
						x -= video_num_columns;
						pos -= video_size_row;
						lf(currcons);
					}
					__asm__("movb %2,%%ah\n\t"
						"movw %%ax,%1\n\t"
						::"a" (translate[c-32]),
						"m" (*(short *)pos),
						"m" (attr));
					//	:"ax");
					pos += 2;
					x++;
				} else if (c==27)
					state=ESesc;
				else if (c==10 || c==11 || c==12)
					lf(currcons);
				else if (c==13)
					cr(currcons);
				else if (c==ERASE_CHAR(tty))
					del(currcons);
				else if (c==8) {
					if (x) {
						x--;
						pos -= 2;
					}
				} else if (c==9) {
					c=8-(x&7);
					x += c;
					pos += c<<1;
					if (x>video_num_columns) {
						x -= video_num_columns;
						pos -= video_size_row;
						lf(currcons);
					}
					c=9;
				} else if (c==7){
				//	sysbeep();
				}else if (c == 14)
			  		translate = GRAF_TRANS;
			  	else if (c == 15)
					translate = NORM_TRANS;
				break;
//			case ESesc:
//				state = ESnormal;
//				switch (c)
//				{
//				  case '[':
//					state=ESsquare;
//					break;
//				  case 'E':
//					gotoxy(currcons,0,y+1);
//					break;
//				  case 'M':
//					ri(currcons);
//					break;
//				  case 'D':
//					lf(currcons);
//					break;
//				  case 'Z':
//					respond(currcons,tty);
//					break;
//				  case '7':
//					save_cur(currcons);
//					break;
//				  case '8':
//					restore_cur(currcons);
//					break;
//				  case '(':  case ')':
//				    	state = ESsetgraph;		
//					break;
//				  case 'P':
//				    	state = ESsetterm;  
//				    	break;
//				  case '#':
//				  	state = -1;
//				  	break;  	
//				  case 'c':
//					tty->termios = DEF_TERMIOS;
//				  	state = restate = ESnormal;
//					checkin = 0;
//					top = 0;
//					bottom = video_num_lines;
//					break;
//				 /* case '>':   Numeric keypad */
//				 /* case '=':   Appl. keypad */
//				}	
//				break;
//			case ESsquare:
//				for(npar=0;npar<NPAR;npar++)
//					par[npar]=0;
//				npar=0;
//				state=ESgetpars;
//				if (c =='[')  /* Function key */
//				{ state=ESfunckey;
//				  break;
//				}  
//				if (ques=(c=='?'))
//					break;
//			case ESgetpars:
//				if (c==';' && npar<NPAR-1) {
//					npar++;
//					break;
//				} else if (c>='0' && c<='9') {
//					par[npar]=10*par[npar]+c-'0';
//					break;
//				} else state=ESgotpars;
//			case ESgotpars:
//				state = ESnormal;
//				if (ques)
//				{ ques =0;
//				  break;
//				}  
//				switch(c) {
//					case 'G': case '`':
//						if (par[0]) par[0]--;
//						gotoxy(currcons,par[0],y);
//						break;
//					case 'A':
//						if (!par[0]) par[0]++;
//						gotoxy(currcons,x,y-par[0]);
//						break;
//					case 'B': case 'e':
//						if (!par[0]) par[0]++;
//						gotoxy(currcons,x,y+par[0]);
//						break;
//					case 'C': case 'a':
//						if (!par[0]) par[0]++;
//						gotoxy(currcons,x+par[0],y);
//						break;
//					case 'D':
//						if (!par[0]) par[0]++;
//						gotoxy(currcons,x-par[0],y);
//						break;
//					case 'E':
//						if (!par[0]) par[0]++;
//						gotoxy(currcons,0,y+par[0]);
//						break;
//					case 'F':
//						if (!par[0]) par[0]++;
//						gotoxy(currcons,0,y-par[0]);
//						break;
//					case 'd':
//						if (par[0]) par[0]--;
//						gotoxy(currcons,x,par[0]);
//						break;
//					case 'H': case 'f':
//						if (par[0]) par[0]--;
//						if (par[1]) par[1]--;
//						gotoxy(currcons,par[1],par[0]);
//						break;
//					case 'J':
//						csi_J(currcons,par[0]);
//						break;
//					case 'K':
//						csi_K(currcons,par[0]);
//						break;
//					case 'L':
//						csi_L(currcons,par[0]);
//						break;
//					case 'M':
//						csi_M(currcons,par[0]);
//						break;
//					case 'P':
//						csi_P(currcons,par[0]);
//						break;
//					case '@':
//						csi_at(currcons,par[0]);
//						break;
//					case 'm':
//						csi_m(currcons);
//						break;
//					case 'r':
//						if (par[0]) par[0]--;
//						if (!par[1]) par[1] = video_num_lines;
//						if (par[0] < par[1] &&
//						    par[1] <= video_num_lines) {
//							top=par[0];
//							bottom=par[1];
//						}
//						break;
//					case 's':
//						save_cur(currcons);
//						break;
//					case 'u':
//						restore_cur(currcons);
//						break;
//					case 'l': /* blank interval */
//					case 'b': /* bold attribute */
//						  if (!((npar >= 2) &&
//						  ((par[1]-13) == par[0]) && 
//						  ((par[2]-17) == par[0]))) 
//						    break;
//						if ((c=='l')&&(par[0]>=0)&&(par[0]<=60))
//						{  
//						  blankinterval = HZ*60*par[0];
//						  blankcount = blankinterval;
//						}
//						if (c=='b')
//						  vc_cons[currcons].vc_bold_attr
//						    = par[0];
//				}
//				break;
//			case ESfunckey:
//				state = ESnormal;
//				break;
//			case ESsetterm:  /* Setterm functions. */
//				state = ESnormal;
//				if (c == 'S') {
//					def_attr = attr;
//					video_erase_char = (video_erase_char&0x0ff) | (def_attr<<8);
//				} else if (c == 'L')
//					; /*linewrap on*/
//				else if (c == 'l')
//					; /*linewrap off*/
//				break;
//			case ESsetgraph:
//				state = ESnormal;
//				if (c == '0')
//					translate = GRAF_TRANS;
//				else if (c == 'B')
//					translate = NORM_TRANS;
//				break;
			default:
				state = ESnormal;
		}
	}
	set_cursor(currcons);
}

/*
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
*/

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
			"m" (attr));
		//	:"ax");
		pos += 2;
		x++;
	}
	set_cursor(currcons);
}

