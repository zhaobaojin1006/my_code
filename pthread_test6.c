/*************************************************************************
	> File Name: pthread_test6.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年07月31日 星期五 09时22分32秒
 ************************************************************************/
/*主线程不断激活等待条件变量的线程，而副线程则不断对条件变量进行判断*/
#include<stdio.h>
#include<pthread.h>

pthread_mutex_t mutex ;     /*定义互斥锁*/

pthread_cond_t cond ;       /*定义条件变量*/

void thread1(void *arg) ;

void thread2(void *arg) ;

int main(void)
{
    int count = 6;
    pthread_t thid1, thid2 ;
    pthread_mutex_init (&mutex, PTHREAD_MUTEX_TIMED_NP) ;       /*初始化线程互斥锁为普通锁*/
    pthread_cond_init (&cond, NULL) ;       /*初始化条件变量*/
    if(pthread_create (&thid1, NULL, (void *)thread1, NULL)) {        /*创建新线程1*/
        printf ("thread1 create failed\n") ;
    }
    if(pthread_create (&thid2, NULL, (void *)thread2, NULL)) {        /*创建新线程2*/
        printf ("thread2 create failed\n") ;
    }
    while(count-- > 0) {
        sleep(1);
        pthread_cond_signal (&cond) ;         /*循环触发条件变量，激活等待条件变量的线程*/
    }

    return 0 ;
}

void thread1(void *arg)
{
    int count= 3 ;
    pthread_cleanup_push (pthread_mutex_unlock, &mutex) ;

    while(count-- > 0) {
        printf ("thread1 is runing\n") ;
        pthread_mutex_lock (&mutex) ;       /*给互斥锁上锁*/
        pthread_cond_wait (&cond, &mutex) ;         /*等待条件变量被触发*/
        printf ("thread1 get the condition\n") ;
        pthread_mutex_unlock (&mutex) ;         /*给互斥锁解锁*/
    }
    pthread_mutex_lock (&mutex) ;

    pthread_mutex_unlock (&mutex) ;

    pthread_cleanup_pop (0) ;
}

void thread2(void *arg)
{
    int count = 2 ;
    pthread_cleanup_push (pthread_mutex_unlock, &mutex) ;

    printf ("thread2 is runing\n") ;
    while(count-- > 0) {
        pthread_mutex_lock (&mutex) ;       /*给互斥锁上锁*/
        pthread_cond_wait (&cond, &mutex) ;         /*等待条件变量被触发*/
        printf ("thread2 get the condition\n") ;
        pthread_mutex_unlock (&mutex) ;         /*给互斥锁解锁*/
    }

    pthread_cleanup_pop (0) ;
}
