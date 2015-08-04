/*************************************************************************
	> File Name: signal_test7.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月03日 星期一 21时44分16秒
 ************************************************************************/
#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>


void handler_sigint(int signo) ;            /*信号SIGINT的处理函数*/

void handler_sigrtmin15(int signo) ;          /*信号SIGMIN+15的处理函数*/

int main(void)
{
    sigset_t newmask, oldmask, pendmask ;         /*定义信号集*/
    struct sigaction act ;

    sigemptyset (&newmask) ;        /*将newmask信号集初始化为空集*/
    sigaddset (&newmask, SIGINT) ;      /*将SIGINT信号加入newmask信号集中*/
    act.sa_flags = SA_NOMASK ;
    act.sa_handler = handler_sigint ;

    /*安装信号处理函数*/
    if(sigaction(SIGINT, &act, NULL) == -1) {
        perror ("signal") ;
        exit (1) ;
    }
    act.sa_handler = handler_sigrtmin15 ;
    if(sigaction(SIGRTMIN+15, &act, NULL) == -1) {
        perror ("signal") ;
        fprintf (stderr, "line:%d\n", __LINE__) ;
        exit (1) ;
    }

    sleep (5) ;         /*此时进程还可以接收到SIGINT*/
    /*屏蔽信号SIGINT*/
    if(sigprocmask (SIG_BLOCK, &newmask, &oldmask) == -1) {         /*从此时之后 此进程对SIGINT信号屏蔽*/
        perror ("sigprocmask") ;
        exit (1) ;
    }
    else {
        printf ("SIGINT blocked\n") ;
    }
    sleep (5) ;         /*此时SIGINT信号被屏蔽，但未决信号中并没有SIGINT(因为没有对其发出SIGINT信号，故等待5秒接收SIGINT)*/

    /*获取当前未决信号队列*/
    if(sigpending(&pendmask) == -1) {
        perror ("sigpending") ;
        exit (1) ;
    }

    /*检查当前未决信号队列中是否有 SIGINT*/
    switch(sigismember(&pendmask, SIGINT)) {
        case 0 :
            printf ("pendmask haven't SIGINT\n") ;
            break ;
        case 1 :
            printf ("SIGINT in pendmask\n") ;
            break ;
        case -1 :
            perror ("sigismember") ;
            fprintf (stderr, "line:%d\n", __LINE__) ;
            exit (1) ;
            break ;
        default :
            break ;
    }

    while(1) {      /*循环等待信号*/
        ;
    }
    return 0 ;
}

void handler_sigint(int signo)
{
    printf ("i get the SIGINT\n") ;
}

void handler_sigrtmin15(int signo)
{
    printf ("i get the SIGRTMIN+15\n") ;
    sigset_t mask ;
    /*获取当前未决信号队列*/
    if(sigpending(&mask) == -1) {
        perror ("sigpending") ;
        fprintf (stderr, "line:%d\n", __LINE__) ;
        exit (1) ;
    }
    
    /*从mask中删去SIGINT*/
    if(sigdelset(&mask, SIGINT) == -1) {
        perror ("sigdelset") ;
        fprintf (stderr, "line:%d\n", __LINE__) ;
        exit (1) ;
    }

    /*重新设置进程的信号屏蔽码*/
    if(sigprocmask(SIG_SETMASK, &mask, NULL) == -1) {
        perror ("sigprocmask") ;
        fprintf (stderr, "line:%d\n", __LINE__) ;
        exit (1) ;
    }
}
