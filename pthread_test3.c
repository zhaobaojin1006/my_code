/*************************************************************************
	> File Name: pthread_test3.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2015年07月28日 星期二 20时30分45秒
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>

pthread_mutex_t  number_mutex1;		/*互斥锁1*/
//pthread_mutex_t  number_mutex2;		/*互斥锁2*/

int globalnumber1 = 10;		/*所有线程共享的全局变量1*/
int globalnumber2 = 5;		/*所有线程共享的全局变量2*/

void thread2(void *arg);		/*线程函数2*/

void thread1(void *arg);		/*线程函数1*/

int main(void)
{
	int status;
	pthread_t thid1,thid2;
	pthread_mutex_init(& number_mutex1,PTHREAD_MUTEX_TIMED_NP);		/*初始化互斥锁1*/
//	pthread_mutex_init(& number_mutex2,PTHREAD_MUTEX_TIMED_NP);		/*初始化互斥锁2*/
	if(pthread_create(&thid1,NULL,(void *)thread1,NULL) != 0){		/*创建线程1*/
		printf("thid1 create failed\n");
	}
	if(pthread_create(&thid2,NULL,(void *)thread2,NULL) != 0){		/*创建线程2*/
		printf("thid2 create failed\n");
	}
	pthread_join(thid1,(void *)&status);		/*等待线程1结束*/
	pthread_join(thid2,(void *)&status);		/*等待线程2结束*/
	return 0;
}


/*线程函数2*/
void thread2(void *arg)
{
	int temp=0;

	sleep(2);
	pthread_mutex_lock(& number_mutex1);		/*对互斥锁1上锁*/

	globalnumber2++;
	temp=globalnumber2;

	pthread_mutex_unlock(& number_mutex1);		/*对互斥锁1解锁*/
	printf("i am thread2,my temp=%d\n",temp);
	pthread_exit(0);
}


/*线程函数1*/
void thread1(void *arg)
{
	pthread_mutex_lock(& number_mutex1);		/*对互斥锁1上锁*/

	printf("i am thread1,my globalnumber=%d\n",globalnumber1);
	globalnumber1++;		/*对全局变量1操作*/
	sleep(5);			/*线程1等待5秒*/

	pthread_mutex_unlock(& number_mutex1);		/*对互斥锁1解锁*/
	pthread_exit(0);
}

