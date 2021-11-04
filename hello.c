#include "./include/unistd.h"

static inline _syscall3(int,write,unsigned int,fd,char *,buf,int,count)

int main(void){
	write(1,"hello world!\n",13);
	while(1);
	return 0;
}

