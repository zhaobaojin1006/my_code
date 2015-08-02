/*************************************************************************
	> File Name: signal_test4.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月02日 星期日 16时32分15秒
 ************************************************************************/
#include<stdio.h>
#include<setjmp.h>
#include<signal.h>

jmp_buf env ;       /*保存希望跳转位置的栈信息*/

void handler_sigrtmin15(int signo) ;

void handler_sigrtmax14(int signo) ;

int main(void)
{
    /*设置返回点*/
    switch(setjmp(env)) {
        case 0:         /*setjmp函数返回值为0，说明是直接调用，而非跳转*/
        break;
        case 1:
        printf ("SIGRTMIN+15 jump to here\n") ;
        break;
        case 2:
        printf ("SIGRTMAX-14 jump to here\n") ;
        break;
        default:
        break;
    }

    /*捕捉信号，安装信号处理函数*/
    signal (SIGRTMIN+15, handler_sigrtmin15) ;
    signal (SIGRTMAX-14, handler_sigrtmax14) ;
    while(1) {
        ;
    }
    return 0 ;
}

/*SIGRTMIN+15信号的处理函数*/
void handler_sigrtmin15(int signo)
{
    printf ("i get SIGRTMIN+15\n") ;
    longjmp (env, 1) ;      /*跳转函数，跳转到setjmp函数的位置，第二个参数为完成跳转时，setjmp的返回值*/
}

/*SIGRTMAX-14信号的处理函数*/
void handler_sigrtmax14(int signo)
{
    printf ("i get SIGRTMAX-14\n") ;
    longjmp (env, 2) ;      /*跳转函数*/
}
