#ifndef _UNISTD_H
#define _UNISTD_H

#define _NR_setup 0
#define _NR_exit 1
#define _NR_fork 0
#define _NR_read 3
#define _NR_write 4
#define _NR_open 5
#define _NR_close 6

#define _syscall0(type,name) \
type name(void){ \
long _res;\
__asm__ volatile ("int $0x80" \
	:"=a" (_res) \
	:"0" (_NR_##name)); \
if (_res >= 0) \
	return (type)_res; \
return -1;\
}
#endif
