#ifndef _UNISTD_H
#define _UNISTD_H

#define _syscall0(type,name) \
type name(void){ \
long _res;\
__asm__ volatile ("int $0x80" \
	:"=a" (_res) \
	:"0" (_NR_##name)); \
if (_res >= 0) \
	return (type)_res; \
error = -_res;\
return -1;\
}
#endif
