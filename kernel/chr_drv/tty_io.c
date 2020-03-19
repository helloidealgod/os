#include "../../include/tty.h"

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
	if ((tty->read_q).head != (tty->read_q).tail){
		GETCH(tty->read_q,c);
		printkc(c);
	}
/*	while (!EMPTY(tty->read_q) && !FULL(tty->secondary)){
		GETCH(tty->read_q,c);
		PUTCH(c,tty->write_q);
		printk("tty->write\n");
		tty->write(tty);
	}*/
}

void do_tty_interrupt(int tty){
	copy_to_cooked(tty_table);
}

