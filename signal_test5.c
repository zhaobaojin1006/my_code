/*************************************************************************
	> File Name: signal_test5.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月02日 星期日 20时37分51秒
 ************************************************************************/
#include<stdio.h>
#include<signal.h>
#include<setjmp.h>

jmp_buf env ;

void handler_sigrtmin15(int signo) ;        /*信号处理函数*/

void handler_sigrtmax14(int signo) ;

int main(void)
{
    struct sigaction act_sigrtmin15, act_sigrtmax14 ;
    /*设置跳转点*/
    switch(sigsetjmp(env, 2)) {
        case 0 :
        break ;
        case 1 :
        printf ("i get SIGRTMIN+15, and i jump to here\n") ;
        break ;
        case 2 :
        printf ("i get SIGRTMAX-14, and i jump to here\n") ;
        break ;
    }
    printf ("i am ready to get the sign\n") ;
    /*设置信号处理函数*/
    act_sigrtmin15.sa_handler = handler_sigrtmin15 ;
    act_sigrtmax14.sa_handler = handler_sigrtmax14 ;
    /*捕捉信号，安装信号处理函数*/
    sigaction (SIGRTMIN+15, &act_sigrtmin15, NULL) ;
    sigaction (SIGRTMAX-14, &act_sigrtmax14, NULL) ;

    while(1) {
        printf("i am running\n");
        sleep (1) ;
    }
    return 0;
}

void handler_sigrtmin15(int signo)
{
    printf ("i get the SIGRTMIN+15\n") ;
    sleep (3) ;
    siglongjmp (env, 1) ;
}

void handler_sigrtmax14(int signo)
{
    printf ("i get the SIGRTMAX-14\n") ;
    sleep (3) ;
    siglongjmp (env, 2) ;
}
