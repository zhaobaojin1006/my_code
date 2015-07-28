/*************************************************************************
	> File Name: myshell.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2015年07月20日 星期一 09时25分32秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<dirent.h>
#include<sys/wait.h>


#define LENMAX 255

struct command
{
	char cmd[50];
	struct command *next;
	struct command *last;
};

int mygets(char *buf);

struct command *command_add(struct command *phead,struct command *padd);	/*在命令链表中添加节点*/

struct command *command_resolve(struct command *phead,char *buf);	/*解析命令*/

void c(struct command *phead);	/*输出命令链表*/

struct command *myshell_print(void);	/*输出表示符及接收命令*/

int command_find(char *command);	/*查找命令*/

int command_analyse(struct command *phead);	 /*分析命令链表*/

int main(void)
{
	struct command *phead=NULL;
	phead=myshell_print();
	if(command_analyse(phead) == -1){
		printf("出错\n");
	}
	/*c(phead);*/
	return 0;
}


/*输出提示符及接受命令*/

struct command *myshell_print(void)
{
	struct command *phead=NULL;
	char a[LENMAX];
	phead=(struct command *)malloc(sizeof(struct command));
	phead->next=phead;
	phead->last=phead;
	printf("myshell$$:");
	mygets(a);
	phead=command_resolve(phead,a);
	return phead;
}


/*自制gets*/

int mygets(char *buf)
{
	char c;
	int m=0;
	while((c=getchar()) != '\n'){
		buf[m++]=c;
		if(m >= LENMAX){
			return -1;
		}
	}
	buf[m]='\0';
	return m;
}


/*在命令链表中添加节点*/

struct command *command_add(struct command *phead,struct command *padd)
{
	struct command *ptemp=NULL;
	padd->next=NULL;
	padd->last=NULL;
	ptemp=phead;
	while(ptemp->next != phead){  /*遍历至链表尾*/
		ptemp=ptemp->next;
	}
	padd->next=phead;
	padd->last=ptemp;
	ptemp->next=padd;
	phead->last=padd;
	return phead;
}


/*将输入的初始命令分解并存入命令链表中*/

struct command *command_resolve(struct command *phead,char *buf)
{
	struct command *pnew=NULL;
	char buftemp[256];
	int m=0,n=0;
	pnew=(struct command *)malloc(sizeof(struct command));
	pnew->next=NULL;
	pnew->last=NULL;
	while(buf[m++] != '\0'){;}
	buf[m-1]='#';
	buf[m]='\0';
	m=0;
	do{
		if(m > 255){
			return NULL;
		}
		if((buf[m] == ' ') || (buf[m] == '#')){
			if(n != 0){    /*n==0 说明有连续空格*/
				n=0;
				strcpy(pnew->cmd,buftemp);
				phead=command_add(phead,pnew);
				pnew=(struct command *)malloc(sizeof(struct command));
				pnew->next=NULL;
				pnew->last=NULL;
			}
		}
		else{
			buftemp[n++]=buf[m];
			buftemp[n]='\0';
		}
		m++;
	}while(buf[m] != '\0');
	return phead;
}



/*遍历命令链表*/

void c(struct command *phead)
{
	struct command *ptemp=NULL;
	ptemp=phead->next;
	while(ptemp != phead){
		printf("%s\n",ptemp->cmd);
		ptemp=ptemp->next;
	}

}



/*根据参数寻找命令*/
int command_find(char *command)
{
	int i;
	DIR *dir;
	struct dirent *pdir=NULL;
	char *path[4]={"./","/bin","/usr/bin",NULL};
	i=0;
	while(path[i] != NULL){
		if((dir=opendir(path[i++])) == NULL){
			return -1;
		}
		while((pdir=readdir(dir)) != NULL){
			if(strcmp(pdir->d_name,command) == 0){
				closedir(dir);
				return 1;
			}
		}
		closedir(dir);
	}
	return 0;
}




/*分析命令链表*/

int command_analyse(struct command *phead)
{
	int count=0;	/*记录参数个数*/
	int flag=0;		/*记录命令中是否含有重定向符或管道符*/
	int whence=0;
	char **argv1,**argv2;
	char which,file[20];
	int n=0,status,status2;
	int fd,fd2;		/*用来操作标准输入和标准输出*/
	pid_t pid,pid2;
	struct command *ptemp=NULL;
	ptemp=phead->next;
	while(ptemp != phead){
		if((strcmp(ptemp->cmd,">") == 0) || (strcmp(ptemp->cmd,"<") == 0) || (strcmp(ptemp->cmd,"|") == 0)){		/*如果含有重定向符或管道符*/
			if(!strcmp(ptemp->cmd,">")){
				which='>';
			}
			else if(!strcmp(ptemp->cmd,"<")){
				which='<';
			}
			else if(!strcmp(ptemp->cmd,"|")){
				which='|';
			}
			whence=count;	/*记录重定向符或管道符之前的命令个数 (whence是前半部分的命令,count-whence-1是后半部分的命令)*/
			flag++;
			if(ptemp->next == phead){	/*重定向符或管道符之后直接结束，格式错误*/
				return -1;
			}
			if(flag > 1){		/*命令中含有超过多个重定向符或管道符*/
				return -1;
			}
		}
		count++;
		ptemp=ptemp->next;
	}
	printf("%d %d\n",count,whence);

	if((pid=fork()) < 0){		/*产生子进程处理命令*/
		printf("fork error\n");
		return -1;
	}
	if(pid == 0){
		if(flag == 1){		/*存储命令参数并查找命令执行*/
			switch(which){		/*根据符号种类存储命令参数*/
				case '>':
					/*存储命令*/
					argv1=(char **)malloc((whence+1)*sizeof(char *));
					ptemp=phead->next;
					for(n=0;n<whence;n++){
						argv1[n]=(char *)malloc(30);		/*每个命令参数长度为30*/
						strcpy(argv1[n],ptemp->cmd);
						ptemp=ptemp->next;
					}
					argv1[n] = NULL;		/*命令行参数以NULL结尾*/
					ptemp=ptemp->next;		/*越过重定向符*/
					strcpy(file,ptemp->cmd);		/*记录重定向符后的文件名*/
					/*执行命令*/
					if(command_find(argv1[0]) != 1){		/*没有此命令，退出子进程*/
						printf("command not find");
						exit(0);
					}
					if((fd=open(file,O_WRONLY | O_CREAT,0644)) == -1){		/*打开或创建目标文件*/
						printf("file open fail\n");
						exit(0);
					}
					dup2(fd,1);		/*用文件描述符fd替代标准输出*/
					execvp(argv1[0],argv1);
					exit(0);
					break;

				case '<':
					/*存储命令*/
					argv1=(char **)malloc(whence*sizeof(char *));
					ptemp=phead->next;
					for(n=0;n<whence;n++){
						argv1[n]=(char *)malloc(30);		/*每个命令参数长度为30*/
						strcpy(argv1[n],ptemp->cmd);
						ptemp=ptemp->next;
					}
					ptemp=ptemp->next;		/*越过重定向符*/
					strcpy(file,ptemp->cmd);		/*记录重定向符后的文件名*/
					/*执行命令*/
					if(command_find(argv1[0]) != 1){
						printf("command not find");
						exit(0);
					}
					fd=open(file,O_RDONLY | O_CREAT,0644);
					dup2(fd,0);		/*用文件描述符fd替代标准输入*/
					execvp(argv1[0],argv1);
					exit(0);
					break;

				case '|':
					/*存储命令*/
					argv1=(char **)malloc(whence*sizeof(char *));
					argv2=(char **)malloc((count-whence-1)*sizeof(char *));
					ptemp=phead->next;
					for(n=0;n<whence;n++){
						argv1[n]=(char *)malloc(30);
						strcpy(argv1[n],ptemp->cmd);
						ptemp=ptemp->next;
					}
					ptemp=ptemp->next;		/*越过管道符*/
					for(n=0;n<(count-whence-1);n++){		/*把管道符后面的命令存入参数数组*/
						argv2[n]=(char *)malloc(30);
						strcpy(argv2[n],ptemp->cmd);
						ptemp=ptemp->next;
					}
					/*执行命令*/
					if(command_find(argv1[0]) != 1 ||command_find(argv2[0]) != 1){
						printf("command not find\n");
						exit(0);
					}
					if((pid2=fork()) < 0){		/*创建新的子进程，执行管道符前面的命令*/
						printf("fork error\n");
						exit(0);
					}
					else if(pid2 == 0){			
						fd=open("/home/zhaobaojin/000.txt",O_WRONLY | O_CREAT);
						dup2(fd,1);
						execvp(argv1[0],argv1);
						exit(0);
					}
					if(waitpid(pid2,&status2,0) == -1){
						printf("wait for child process error\n");
						exit(0);
					}
					fd2=open("/home/zhaobaojin/000.txt",O_RDONLY);
					dup2(fd2,0);
					execvp(argv2[0],argv2);
					exit(0);
			}
		}
		else{		/*没有特殊符号*/
			argv1=(char **)malloc((count+1)*sizeof(char *));		/*总共需要存储count+1个参数，argv[]需要以NULL结尾*/
			ptemp=phead->next;
			for(n=0;n<count;n++){
				argv1[n]=(char *)malloc(30);
				strcpy(argv1[n],ptemp->cmd);
				ptemp=ptemp->next;
			}
			argv1[n]=NULL;
			if(command_find(argv1[0]) != 1){		/*没有此命令，退出子进程*/
				printf("command not find");
				exit(0);
			}
			for(n=0;n < count;n++){
				printf("argv1[%d]:%s\n",n,argv1[n]);
			}
			execvp(argv1[0],argv1);
			exit(0);
		}
	}
	if(waitpid(pid,&status2,0) == -1){
		printf("child process wait fail\n");
		return -1;
	}
	return 0;
}
