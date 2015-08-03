/*************************************************************************
	> File Name: signal_test6.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月03日 星期一 20时28分40秒
 ************************************************************************/
#include<stdio.h>
#include<sys/time.h>
#include<signal.h>

void handler_sigtime(int signo) ;       /*信号处理函数*/

int main(void)
{
    struct sigaction act ;
    struct itimerval value ;
    act.sa_flags = SA_NOMASK ;
    act.sa_handler = handler_sigtime ;

    /*安装信号处理函数*/
    sigaction (SIGALRM, &act, NULL) ;
    sigaction (SIGPROF, &act, NULL) ;

    /*初始化时间结构体*/
    value.it_value.tv_sec = 2 ;         /*第一次2秒触发信号*/
    value.it_value.tv_usec = 0 ;
    value.it_interval.tv_sec = 3 ;      /*第二次开始每次3秒触发信号*/
    value.it_interval.tv_usec = 0 ;

    /*设置定时器*/
    setitimer (ITIMER_REAL, &value, NULL) ;
    setitimer (ITIMER_PROF, &value, NULL) ;

    while(1) {      /*循环等待信号*/
        ;
    }
    return 0 ;
}

void handler_sigtime(int signo)
{
    switch(signo) {
        case SIGALRM :
            printf ("i get the SIGALRM\n") ;
            break ;
        case SIGPROF :
            printf ("i get the SIGPROF\n") ;
            break ;
        default :
            break ;
    }
}
