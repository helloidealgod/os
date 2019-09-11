#关于汇编中直接使用系统调用
#在汇编中使用系统调用，均通过0x80调用来实现。在具体编程时要将调用号放在eax中，
#其他的参数按定义的逆序入栈即可。这里用到两个调用号，一个是4号调用，即：sys_write，这个调用有三个参数：fd,msg,len;
#另一个调用是1号调用，好：sys_exit，这个调用只有一个参数，就是返回值。
.section .data
    msg: .string "In hello.s!\n"
    len=.-msg
.text
.global _start,myprint
_start:
#第一次显示In hello.s!
    movl $4,%eax
    movl $1,%ebx
    movl $msg,%ecx
    movl $len,%edx
    int $0x80
    
#模仿linux 0.11中的入栈动作    
    pushl $0
    pushl $0
    pushl $0
    
#调用c函数，显示hello, in hello.c!
    call main
    
#调用汇编函数，第二次显示In hello.s!
    call echohello
    movl $1,%eax
    movl $0,%ebx
    int $0x80
#
echohello:
    movl $4,%eax
    movl $1,%ebx
    movl $msg,%ecx
    movl $len,%edx
    int  $0x80
    ret
    
#c函数中显示字串使用的函数，其在c中的原型如下：
#myprint(char * msg,int len)
#由于使用ld直接连接，并没有连接标准库，所以c函数中不能使用printf等库函数。
myprint:
    movl 4(%esp),%ecx        #这里esp指向谁？想想上篇文章就明白了。
    movl 8(%esp),%edx        #参数的入栈是在c函数中完成的
    movl $1,%ebx
    movl $4,%eax
    int  $0x80
    ret
