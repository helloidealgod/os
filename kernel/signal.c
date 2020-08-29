#include "../include/signal.h"
#include "../include/sched.h"
#include "../include/asm/segment.h"

extern struct task_struct * current;

//获取当前任务信号屏蔽位图（屏蔽码或阻塞码）
int sys_sgetmask(){
    return current->blocked;
}

//设置新的信号屏蔽位图，SIGKILL不能被屏蔽，返回的是原屏蔽位图
int sys_ssetmask(int newmask){
    int old = current->blocked;
    current->blocked = newmask & ~(1<<(SIGKILL-1));
    return old;
}

//复制sigaction数据到fs数据段to处，即从内核空间复制到用户空间数据段处
static inline void save_old(char * from, char * to){
    int i;
    //首先验证to处的空间是否足够大，然后把一个sigaction结构数据复制到fs处
    //verify(to,sizeof(struct sigaction));
    for (i=0; i < sizeof(struct sigaction); i++){
        put_fs_byte(*from,to);
        from++;
        to++;
    }
}

//把sigaction数据从fs数据段from位置复制到to处，即从用户数据段复制到内核空间数据段
static inline void get_new(char * from, char * to){
    int i;
    for (i=0; i < sizeof(struct sigaction); i++){
        *(to++) = get_fs_byte(from++);
    }
}

int sys_signal(int signum, long handler, long restorer){
    struct sigaction tmp;
    if (signum < 1 || signum > 32 || signum == SIGKILL)
        return -1;
    tmp.sa_handler = (void (*)(int))handler;
    tmp.sa_mask = 0;
    tmp.sa_flags = SA_ONESHOT | SA_NOMASK;
    tmp.sa_restorer = (void(*)(void))restorer;
    handler = (long)current->sigaction[signum-1].sa_handler;
    current->sigaction[signum-1] = tmp;
    return handler;
}

int sys_sigaction(int signum, const struct sigaction * action, struct sigaction * oldaction){
    struct sigaction tmp;
    if (signum < 1 || signum > 32 || signum == SIGKILL)
        return -1;
    tmp = current->sigaction[signum-1];
    get_new((char *)action, (char *)(signum-1+current->sigaction));
    if (oldaction)
        save_old((char *)&tmp,(char *)oldaction);
    if (current->sigaction[signum-1].sa_flags & SA_NOMASK)
        current->sigaction[signum-1].sa_mask = 0;
    else
        current->sigaction[signum-1].sa_mask |= (1<<(signum-1));
    return 0;

}

void do_signal(long signr,long eax,long ebx,long ecx,long edx,long fs,
	long es,long ds,long eip,long cs,long eflags,unsigned long * esp,long ss){

    unsigned long sa_handler;
    long old_eip = eip;
    struct sigaction * sa = current->sigaction + signr -1;
    int longs;
    unsigned long * tmp_esp;

    sa_handler = (unsigned long) sa->sa_handler;
    if (sa_handler == 1)
        return;
    if (!sa_handler){
        if (signr == SIGCHLD)
            return;
        else{
         //   do_exit(1<<(signr-1));
	}
    }
    if (sa->sa_flags & SA_ONESHOT)
        sa->sa_handler = NULL;
    *(&eip) = sa_handler;
    longs = (sa->sa_flags & SA_NOMASK) ? 7:8;
    *(&esp) -= longs;
 //   verify_area(esp,longs * 4);
    tmp_esp = esp;
    put_fs_long((long)sa->sa_restorer,tmp_esp++);
    put_fs_long(signr,tmp_esp++);
    if (!(sa->sa_flags & SA_NOMASK))
        put_fs_long(current->blocked,tmp_esp++);
    put_fs_long(eax,tmp_esp++);
    put_fs_long(ecx,tmp_esp++);
    put_fs_long(edx,tmp_esp++);
    put_fs_long(eflags,tmp_esp++);
    put_fs_long(old_eip,tmp_esp++);
    current->blocked |= sa->sa_mask;
}














































