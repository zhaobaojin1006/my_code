/*************************************************************************
	> File Name: signal_test3.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月02日 星期日 14时48分31秒
 ************************************************************************/
#include<stdio.h>
#include<signal.h>

int temp = 0 ;

void handler_sigint(int signo) ;        /*只有一个参数的信号处理函数，该参数为信号编号*/

int main(void)
{
    struct sigaction act ;      /*signal函数参数结构体*/
    act.sa_handler = handler_sigint ;       /*将act的sa_handler成员设置为信号处理函数*/
    /*如果不设置为SA_NOMASK，则不可靠信号不支持排队，只会在处理一个信号的同时再阻塞一个相同信号，更多的相同信号将被丢弃*/
    act.sa_flags = SA_NOMASK ;      /*将信号处理的相关操作设置为信号处理结束前，允许该信号再次递送*/
    if(sigaction(SIGINT, &act, NULL) != 0) {
        printf ("signal create failed\n") ;
    }
    
    while(1) {      /*循环等待信号*/
        printf ("i am running\n") ;
        sleep (1) ;
    }
    return 0;
}

void handler_sigint(int signo)
{
    printf ("i get the SIGINT\n") ;
    temp++ ;
    printf ("temp = %d\n", temp) ;
    sleep (5) ;         /*延长信号处理时间，测试多个信号同时接收*/
    printf ("after sleep\n") ;
}
