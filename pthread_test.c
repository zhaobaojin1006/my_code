/*************************************************************************
	> File Name: pthread_test2.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2015年07月28日 星期二 11时23分23秒
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>

int flag=10;

void thread1(void *arg);		/*线程函数1*/

void thread2(void *arg);		/*线程函数2*/

int main(void)
{
	int status;
	int a=5;
	pthread_t thid2;
	printf("i am main thread,my a=%d\n",a);		/*第一次打印main thread中的a*/
	if(pthread_create(&thid2,NULL,(void *)thread2,NULL) != 0){		/*创建线程2*/
		printf("thread2 create failed");
	}
	pthread_join(thid2,(void *)&status);		/*等待线程2结束*/
	printf("i am main thread,my a=%d\n",a);		/*第二次打印main thread中的a*/
	return 0;
}


void thread1(void *arg)
{
	int a=3;
	printf("i am thread1,my a=%d\n",a);	/*打印线程1中的a*/
	pthread_exit(0);
}

void thread2(void *arg)
{
	int a=4;
	int b=4;
	int c=5;
	int status;
	pthread_t thid1;
	printf("i am thread2,my a=%d\n",a);		/*第一次打印线程2中的a*/
	printf("i am thread2,my b=%d\n",b);		/*第一次打印线程2中的b*/
	printf("i am thread2,my c=%d\n",c);		/*第一次打印线程2中的c*/
	if(pthread_create(&thid1,NULL,(void *)thread1,NULL) != 0 ){		/*创建线程1*/
		printf("thread1 create failed\n");
	}
	pthread_join(thid1,(void *)&status);		/*等待线程1结束*/
	printf("i am thread2,my a=%d\n",a);		/*第二次打印线程2中的a*/
	printf("i am thread2,my b=%d\n",b);		/*第二次打印线程2中的b*/
	printf("i am thread2,my c=%d\n",c);		/*第二次打印线程2中的c*/
	pthread_exit(0);
}
