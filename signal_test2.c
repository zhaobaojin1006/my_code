/*************************************************************************
	> File Name: signal_test2.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月02日 星期日 10时22分48秒
 ************************************************************************/
#include<stdio.h>
#include<signal.h>

void handler_sigint(int signo) ;        /*信号处理函数返回值为void，带有一个int型的参数*/

void signal_wait(void) ;

int main(void)
{
    signal (SIGINT, handler_sigint) ;       /*安装SIGINT信号处理函数*/
    signal_wait () ;        /*循环等待SIGINT信号*/
    return 0;
}

void handler_sigint(int signo)
{
    printf ("\ni get the SIGINT\n") ;
}

void signal_wait(void)
{
    int count = 1 ;
    while(1) {
        printf ("now i am waiting %d seconds\n", count++) ;
        sleep (1) ;
    }
}
