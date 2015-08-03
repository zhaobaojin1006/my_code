/*************************************************************************
	> File Name: pthread_test7.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月03日 星期一 14时52分48秒
 ************************************************************************/
#include<stdio.h>
#include<pthread.h>

pthread_mutex_t mutex ;

void thread1(void *arg) ;       /*线程函数1*/

void thread2(void *arg) ;       /*线程函数2*/

int main(void)
{
    int *status1, *status2 ;
    pthread_t thid1 ,thid2;
    pthread_mutexattr_t mutexattr ;
    mutexattr.__align = PTHREAD_MUTEX_RECURSIVE_NP ;      /*将互斥锁初始化为嵌套锁*/
    //mutexattr.__align = PTHREAD_MUTEX_ADAPTIVE_NP ;       /*将互斥锁初始化为适应锁*/
    //mutexattr.__align = PTHREAD_MUTEX_ERRORCHECK_NP ;       /*将互斥锁初始化为检错锁*/
    pthread_mutex_init (&mutex, &mutexattr) ;
    //pthread_mutex_init (&mutex, NULL) ;         /*将互斥锁初始化为普通锁*/
    if(pthread_create(&thid1, NULL, (void *)thread1, NULL) != 0) {
        printf ("new thread create failed\n") ;
    }
    if(pthread_create(&thid2, NULL, (void *)thread2, NULL) != 0) {
        printf ("new thread create failed\n") ;
    }

    pthread_join (thid1, (void *)&status1) ;
    pthread_join (thid2, (void *)&status2) ;
    pthread_mutex_destroy (&mutex) ;
    return 0 ;
}

void thread1(void *arg)
{
    pthread_mutex_lock (&mutex) ;        /*给嵌套锁加锁*/
    sleep (10) ;
    pthread_mutex_unlock (&mutex) ;
    pthread_exit (0) ;
}

void thread2(void *arg)
{
    sleep (2) ;         /*等待线程thread1先给嵌套锁加锁*/
    if(pthread_mutex_trylock(&mutex) != 0) {        /*尝试给互斥锁加锁*/
        printf ("i can't get the lock\n") ;
    }
    if(pthread_mutex_unlock (&mutex) != 0) {
        printf ("i can't unlock the lock\n") ;
        pthread_exit (0) ;
    }
    else {
        printf ("i can unlock the lock\n") ;
        if(pthread_mutex_trylock (&mutex) != 0) {
            printf ("i can't get the lock\n") ;
        }
        else {
            printf ("i get the lock\n") ;
        }
    }
    pthread_exit (0) ;
}
