/*************************************************************************
	> File Name: signal_test1.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月02日 星期日 10时01分25秒
 ************************************************************************/
#include<stdio.h>
#include<signal.h>


void handler_sigint(int signo) ;        /*处理信号的函数是一个返回值为void，且带有一个int型参数的函数*/

int main(void)
{
    int count = 0 ;
    signal (SIGINT, handler_sigint) ;       /*安装SIGINT信号的处理函数handler_sigint*/
    printf ("i am run:count=%d\n", ++count) ;       /*测试 如果进程接收到信号是否重新执行signal函数之后的语句*/
   // while(1) {      /*无限循环，等待接收信号*/
   //     ;
   // }
   // sleep (10) ;        /*使用进程睡眠来等待信号*/
    while(1) {
        printf("\ni am run ,count=%d\n", ++count);      /*测试接收信号时是否会中断当前执行的语句。*/
        sleep (3) ;
    }
    return 0 ;
}

void handler_sigint(int signo)
{
    printf ("\ni get SIGINT!!\n") ;
}
