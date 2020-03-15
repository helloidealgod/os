#ifndef _TERMIOS_H
#define _TERMIOS_H

#define NCC 8

struct termio {
	unsigned short c_iflag;
	unsigned short c_oflag;
	unsigned short c_cflag;
	unsigned short c_lflag;
	unsigned short c_line;
	unsigned short c_cc[NCC];
};

#define NCCS 17
struct termios {
    unsigned long c_iflag;        /* input mode flags   */
    unsigned long c_oflag;        /* output mode flags  */
    unsigned long c_cflag;        /* control mode flags */
    unsigned long c_lflag;        /* local mode flags   */
    unsigned char c_line;         /* line discipline    */
    unsigned char c_cc[NCCS];     /* control characters */ //可以更改的特殊字符
};

#endif
