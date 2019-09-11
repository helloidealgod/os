int main()
{
    _asm{
        .section .data
        msg: .string "In hello.s!\n"
        len=.-msg
        .text
        .global _start
        _start:
        #第一次显示In hello.s!
            movl $4,%eax
            movl $1,%ebx
            movl $msg,%ecx
            movl $len,%edx
            int $0x80
    }
    return 0;
}
