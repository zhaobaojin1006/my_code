/*************************************************************************
	> File Name: pthread_test5.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年07月30日 星期四 14时38分59秒
 ************************************************************************/
#include<stdio.h>
#include<pthread.h>

pthread_key_t key ;      /*创建一个键*/

int func(void) ;

void thread1(void *arg) ;      /*线程函数*/

void thread2(void *arg) ;

int main(void)
{
    int status ;
    pthread_t thid1, thid2 ;
    pthread_key_create (&key, NULL) ;
    if (pthread_create (&thid1, NULL, (void *)thread1, NULL)  != 0 ) {       /*创建新线程1*/
        printf ("new thread create failed\n") ;
        return 0 ;
    }
    if (pthread_create (&thid2, NULL, (void *)thread2, NULL)  != 0 ) {       /*创建新线程2*/
        printf ("new thread create failed\n") ;
        return 0 ;
    }
    pthread_join (thid1, &status) ;
    pthread_join (thid2, &status) ;
    pthread_key_delete (key) ;
    return 0 ;
}


int func()
{
    /*新线程中调用函数打印key*/
    printf ("i am newthread:%u,i can read the key:%d\n", pthread_self(), pthread_getspecific (key)) ;

    return 0 ;
}


void thread1(void *arg)       /*线程1将一个线程函数中的局部变量跟key关联，调用func()打印跟key关联的值*/
{
    int tsd=5 ;
    pthread_setspecific( key , (void *)tsd ) ;      /*关联私有数据*/
    func();
    pthread_exit(0);
}


void thread2(void *arg)       /*线程2关联同一个key，并调用func()打印跟key关联的值*/
{
    int tsd=6 ;
    pthread_setspecific( key , (void *)tsd ) ;      /*关联私有数据*/
    func();
    pthread_exit(0);
}
