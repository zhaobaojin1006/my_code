/*************************************************************************
	> File Name: signal_test8.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月05日 星期三 16时39分55秒
 ************************************************************************/
#include<stdio.h>
#include<signal.h>
#include<pthread.h>
#include<stdlib.h>


void handler_sigrtmax14 (int signo) ;       /*信号SIGRTMAX-14处理函数*/

void handler_sigrtmax14_new(int signo) ;

void thread (void *arg) ;       /*线程函数*/

int main(void)
{
    int *status ;
    pthread_t thid ;
    if(pthread_create (&thid, NULL, (void *)thread, NULL) != 0) {         /*创建新线程*/
        perror ("pthread_create") ;
        exit (1) ;
    }

    signal (SIGRTMAX-14, handler_sigrtmax14) ;      /*安装信号处理函数*/

    pthread_join (thid, (void *)&status) ;      /*等待新线程结束*/
    return 0 ;
}

void handler_sigrtmax14 (int signo)
{
    printf ("i am parent thread,i get the SIGRTMAX-14\n") ;
}

void handler_sigrtmax14_new (int signo)
{
    printf ("i am new thread,i get the SIGRTMAX-14\n") ;
}

void thread (void *arg)
{
    signal (SIGRTMAX-14, handler_sigrtmax14_new) ;      /*安装信号处理函数*/

    sleep (10) ;
}
