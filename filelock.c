/*************************************************************************
	> File Name: shiyan1.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2015年07月19日 星期日 14时52分41秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>

int main(void)
{
	char a[5];
	int fd;
	struct flock lock;
	pid_t pid;
	if((fd=open("123.txt",O_CREAT|O_WRONLY)) == -1){
		close(fd);
		return -1;;
	}
	lock.l_whence=SEEK_SET;
	lock.l_start=0;
	lock.l_len=3;

	/*测试能否写入写锁*/
	lock.l_type=F_WRLCK;
	if(fcntl(fd,F_GETLK,&lock) == 0){   /*测试锁执行成功*/
		if(lock.l_type == F_UNLCK){   /*能被写入锁*/
			printf("可以被写入锁。\n");
		}
		else{
			printf("不能被写入锁。\n");
		}
	}
	else{
		close(fd);
		return -1;
	}

	/*设置写锁*/
	lock.l_type=F_WRLCK;
	if(fcntl(fd,F_SETLK,&lock) == 0){
		printf("写锁设置成功！\n");
		getchar();
	}
	else{
		printf("写锁设置失败。\n");
		close(fd);
		return -1;
	}

	pid=fork();
	if(pid == 0){
		printf("i am child process,my id=%d",getpid());
		/*测试能否写入写锁*/
		lock.l_type=F_WRLCK;
		if(fcntl(fd,F_GETLK,&lock) == 0){
			if(lock.l_type == F_UNLCK){
				printf("child process can  set the file lock\n");
			}
			else{
				printf("child process can't set the file lock\n");;
			}
		}
		else{
			exit(0);
		}
		/*写入写锁*/
		lock.l_type=F_WRLCK;
		if(fcntl(fd,F_SETLK,&lock) == 0){
			printf("write lock set success\n");
			exit(0);
		}
		else{
			printf("write lock set fail\n");
		}
	}
	else{
		wait();
		printf("i am parent process,my id=%d\n",getpid());
		printf("请输入四个字符");
		scanf("%s",a);
		if(write(fd,a,4) != -1){
			printf("写入成功。\n");
			getchar();
		}
	}
	close(fd);
	return 0;
}
