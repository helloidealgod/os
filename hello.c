#include "./include/unistd.h"

static inline int write(unsigned int fd,char * buff,int count) __attribute__((always_inline));

static inline _syscall3(int,write,unsigned int,fd,char *,buff,int,count)

int main(void){
	write(1,"hello world!\n",13);
	return 0;
}
