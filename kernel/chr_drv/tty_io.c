#include "../../include/tty.h"
#include "../../include/system.h"

extern void con_write(struct tty_struct * tty);

struct termios termios;

struct tty_struct tty_table[] = {
	{0,0,con_write,{0,0,0,0,""},
	{0,0,0,0,""},{0,0,0,0,""}}
};

struct tty_queue table_list[]={
	&tty_table[0].read_q,
	&tty_table[0].write_q
};

void copy_to_cooked(struct tty_struct * tty){
	signed char c;
	while (!EMPTY(tty->read_q) && !FULL(tty->secondary)){
		GETCH(tty->read_q,c);
		if(13 == c){
			//enter
			PUTCH('\0',tty->secondary);
			wake_up(&tty->secondary.proc_list);
		}else if(127 == c){
			//bs
			if(!EMPTY(tty->secondary) && '\0' != LAST(tty->secondary)){
				DEC(tty->secondary.head);
			}
		}else{
			PUTCH(c,tty->secondary);
		}
		PUTCH(c,tty->write_q);
		tty->write(tty);
	}
}

void do_tty_interrupt(int tty){
	copy_to_cooked(tty_table);
}

int tty_read(char * buf, int nr){
//	printk("tty_read\n");
	struct tty_struct * tty;
	char c,*b=buf;
	tty = tty_table;
	while(nr>0){
//		printk("while nr=%d\n",nr);
		cli();
		if(EMPTY(tty->secondary)){
//			printk("before sleep_on\n");
			interruptible_sleep_on(&tty->secondary.proc_list);
			sti();
//			printk("sleep_on\n");
			continue;
		}
		sti();
		do{
			GETCH(tty->secondary,c);
//			printk("tty_read: %c ",c);
			if('\0' == c){
				put_fs_byte('\0',b++);
				break;
			}else{
				put_fs_byte(c,b++);
				if(!--nr){
					break;
				}
			}
		}while(nr>0 && !EMPTY(tty->secondary));
		break;
	}
//	printk("tty_read end\n");
	return (b-buf);
}
