#ifndef _UNISTD_H
#define _UNISTD_H

#define _NR_setup 0
#define _NR_exit 1
#define _NR_fork 0
#define _NR_read 3
#define _NR_write 4
#define _NR_open 5
#define _NR_close 6
#define _NR_sprintk 1
#define _NR_pause 2

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

#define _syscall1(type,name,atype,a)\
type name(atype a){\
long _res;\
__asm__ volatile ("int $0x80"\
	:"=a" (_res)\
	:"0" (_NR_##name),"b" ((long)(a)));\
if (_res >= 0)\
	return (type) _res;\
return -1;\
}

#define _syscall2(type,name,atype,a,btype,b)\
type name(atype a,btype b){\
long _res;\
__asm__ volatile ("int $0x80"\
	:"=a" (_res)\
	:"0" (_NR_##name),"b" ((long)(a)),"c" ((long)(b)));\
if (_res >= 0)\
	return (type) _res;\
return -1;\
}

#define _syscall3(type,name,atype,a,btype,b,ctype,c)\
type name(atype a,btype b,ctype c){\
long _res;\
__asm__ volatile ("int $0x80"\
	:"=a" (_res)\
	:"0" (_NR_##name),"b" ((long)(a)),"c"((long)(b)),"d"((long)(c)));\
if (_res >= 0)\
	return (type) _res;\
return -1;\
}

#endif
