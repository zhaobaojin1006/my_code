/*************************************************************************
	> File Name: pthread_test2.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2015年07月28日 星期二 17时30分02秒
 ************************************************************************/

#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>


void thread(void *arg);

int main(void)
{
	int i=0;
	pid_t pid[4];
	for(i=0;i<4;i++){
		if((pid[i] = fork()) < 0){		/*创建子进程*/
			printf("child process create failed\n");
		}
		else if(pid[i] == 0){		/*判断操作子进程*/
			int status[2];
			int j=0;
			pthread_t thid[2];
			for(j=0;j<2;j++){
				printf("i am child process %d,i set child pthread%d.\n",getpid(),j+1);
				pthread_create(&thid[j],NULL,(void *)thread,NULL);		/*在子进程中创建线程*/
				pthread_join(thid[j],(void *)&status[j]);		/*子进程等待创建的线程退出*/
			}
			exit(0);
		}
		else{		/*操作父进程使其等待一秒*/
			sleep(1);
		}
	}
	return 0;
}

void thread(void *arg)
{
	printf("i am new thread\n");
	pthread_exit(0);
}
