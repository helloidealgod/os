#define ORIG_X		(*(unsigned char *)0x90000)
#define ORIG_Y		(*(unsigned char *)0x90001)
#define ORIG_VIDEO_PAGE	(*(unsigned short *)0x90004)
#define ORIG_VIDEO_MODE	((*(unsigned short *)0x90006) & 0xff)
#define ORIG_VIDEO_COLS	(((*(unsigned short *)0x90006) &0xff00) >> 8)
#define ORIG_VIDEO_LINES (25)
#define ORIG_VIDEO_EGA_AX (*(unsigned short *)0x90008)
#define ORIG_VIDEO_EGA_BX (*(unsigned short *)0x9000a)
#define ORIG_VIDEO_EGA_CX (*(unsigned short *)0x9000c)

#define VIDEO_TYPE_MDA 0x10
#define VIDEO_TYPE_CGA 0x11
#define VIDEO_TYPE_EGAM 0x20
#define VIDEO_TYPE_EGAC 0x21

#define NPAR 16

static unsigned char video_type;
static unsigned long video_num_columns;
static unsigned long video_size_row;
static unsigned long video_num_lines;
static unsigned char video_page;
static unsigned long video_mem_start;
static unsigned long video_mem_end;
static unsigned short video_port_reg;
static unsigned short video_port_val;
static unsigned short video_erase_char;

static unsigned long origin;
static unsigned long scr_end;
static unsigned long pos;
static unsigned long x,y;
static unsigned long top,bottom;
static unsigned long state=0;
static unsigned long npar,par[NPAR];
static unsigned char attr=0x07;

#define RESPONSE "\033[?1;2c"

void con_init(void)
{
	register unsigned char a;
	char *display_desc = "????";	
	char *display_desc_1 = "ABCD";

	char *display_ptr;

	video_num_columns = ORIG_VIDEO_COLS;
	video_size_row = video_num_columns * 2;
	video_num_lines = ORIG_VIDEO_LINES;
	video_page = ORIG_VIDEO_PAGE;
	video_erase_char = 0x0720;
	if (ORIG_VIDEO_MODE == 7){
		video_mem_start = 0xb0000;
		video_port_reg = 0x3b4;
		video_port_val = 0x3b5;
		if ((ORIG_VIDEO_EGA_BX & 0xff) != 0x10){
			video_type = VIDEO_TYPE_EGAM;
			video_mem_end = 0xb8000;
			display_desc = "EGAm";
		}else{
			video_type = VIDEO_TYPE_MDA;
			video_mem_end = 0xb2000;
			display_desc = "*MDA";
		}
	}else{
		video_mem_start = 0xb8000;
		video_port_reg = 0x3d4;
		video_port_val = 0x3d5;
		if ((ORIG_VIDEO_EGA_BX & 0xff) != 0x10){
			video_type = VIDEO_TYPE_EGAC;
			video_mem_end = 0xbc000;
			display_desc = "EGAc";
		}else{
			video_type = VIDEO_TYPE_CGA;
			video_mem_end = 0xba000;
			display_desc = "*CGA";
		}
	}
	video_mem_start = 0xb0000;	
	video_port_reg = 0x3b4;
	video_port_val = 0x3b5;

	display_ptr = ((char *)video_mem_start) + video_size_row - 8;
	while (*display_desc){
		*display_ptr++ = *display_desc++;
		display_ptr++;
	}
/*	video_mem_start = 0xb8000;
	video_port_reg = 0x3d4;
	video_port_val = 0x3d5;
	display_ptr = ((char *)video_mem_start) + video_size_row - 8;
	while (*display_desc_1){
		*display_ptr++ = *display_desc++;
		display_ptr++;
	}*/
}

