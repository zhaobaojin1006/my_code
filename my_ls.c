/*************************************************************************
	> File Name: my_ls.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2015年06月19日 星期五 17时41分48秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>

#define CMDNONE 0
#define CMD_a 1
#define CMD_l 2
#define CMD_al 3
#define CMD_R 4

struct cmand
{
	int flag;	/*记录执行该cmd的参数*/
	char filename[30];		/*记录文件名*/
	char type;		/*记录文件类型*/
	struct cmand *next;
};

struct dir
{
	char dirname[256];
	struct dir *next;
};

void my_gets(char *c);/*获取输入的命令*/

struct cmand *listadd(struct cmand *phead,struct cmand *padd);

struct cmand *cmdcut(char *command);/*分割命令*/

int dirprint(const char *path);/*打印path参数指定目录下的文件*/

int getfilestatus(const char *name);/*获取文件属性*/

int dirallfile(const char *filename);/*打印filename参数指定的文件属性*/

/*判断参数是否重复*/
int cmdcmp(char const *buf,char flag);

/*判断参数文件类型*/
char FileFind(char const *filename);

struct dir *diradd(struct dir *phead,char *dirname);	/*在目录链表中增加节点*/

int dirdiscover(const char *path);	/*递归显示目录中的文件属性*/

int CmdRun(struct cmand *phead);





int main(void)
{
	struct cmand *phead=NULL,*ptemp=NULL;
	char command[256];
	printf("请输入命令：");
	my_gets(command);
	phead=cmdcut(command);
	if(phead == NULL){
		printf("command failed");
		return -1;
	}
	if(CmdRun(phead) == -1){
		printf("command run fail");
	}
	return 0;
}



void my_gets(char *c)
{
	int i=0;
	do{
		c[i]=getchar();
	}
	while((c[i++] != '\n') && (i<256));
	c[i-1]='\0';
}

/*命令链表增加结点*/
struct cmand *listadd(struct cmand *phead,struct cmand *padd)
{
	struct cmand *ptemp=NULL;
	padd->next=NULL;
	ptemp=phead;
	if(phead == NULL){
		printf("头结点为空");
		return NULL;
	}
	while(ptemp->next != NULL){/*遍历到链表最后一个节点*/
		ptemp=ptemp->next;
	}
	ptemp->next=padd;
	return phead;
}


/*判断参数是否重复*/
int cmdcmp(char const *buf,char flag)
{
	int m=0;
	while(buf[m] != '\0'){
		if(buf[m] == flag){
			return 1;
		}
		m++;
	}
	return 0;
}


/*判断参数文件类型*/
char FileFind(char const *filename)
{
	char type;
	struct stat buf;
	if(stat(filename,&buf) == -1){
		return 0;
	}

	if(S_ISLNK(buf.st_mode)){
		type='l';
	}
	else if(S_ISREG(buf.st_mode)){
		type='-';
	}
	else if(S_ISDIR(buf.st_mode)){
		type='d';
	}
	else if(S_ISCHR(buf.st_mode)){
		type='c';
	}
	else if(S_ISBLK(buf.st_mode)){
		type='b';
	}
	else if(S_ISFIFO(buf.st_mode)){
		type='f';
	}
	else if(S_ISSOCK(buf.st_mode)){
		type='s';
	}

	return type;
}

/*解析命令*/
struct cmand *cmdcut(char *command)
{
	struct cmand *ptemp,*phead=NULL;
	int flag=0,which=0;
	int m=0,n=0,show=0,count=0;
	char buf[10];	/*记录连续参数*/
	char temp[20];
	char type;
	if(command[0] != 'l' && command[1] != 's'){
		printf("无此命令。\n");
		return NULL;
	}
	phead=(struct cmand *)malloc(sizeof(struct cmand));
	phead->next=NULL;
	while(command[m++] != '\0'){;}		/*遍历到command的结尾*/
	command[m-1]=' ';		/*在command字符串的结尾加上空格*/
	command[m]='\0';
	m=0;
	while(command[m] != '\0'){
		if(command[m] == ' '){		/*遇到空格就把temp字符串取出分析*/
			n=0;
			if(! strcmp(temp,"ls")){		/*将命令略过*/
				m++;
				continue;
			}
			if(temp[0] == '-'){		/*解析出的是选项参数*/
				if(show == 1){		/*如果在路径参数之后遇到选项参数，就将flag归0*/
					show=0;
					flag=0;
				}
				count=1;
				while(temp[count] != '\0'){
					switch(temp[count]){		/*根据参数类型设置flag*/
						case 'a':
							if(cmdcmp(buf,temp[count]) == 1){		/*有重复的参数*/
								break;
							}
							strcat(buf,"a");
							flag=flag+CMD_a;
							break;
						case 'l':
							if(cmdcmp(buf,temp[count]) == 1){		/*有重复的参数*/
								break;
							}
							strcat(buf,"l");
							flag=flag+CMD_l;
							break;
						case 'R':
							if(cmdcmp(buf,temp[count]) == 1){		/*有重复的参数*/
								break;
							}
							strcat(buf,"R");
							flag=flag+CMD_R;
							break;
						default:
							printf("无此参数。\n");
							return NULL;
							break;
					}
					count++;
				}
			}
			else{		/*如果不是选项参数，就将其视为路径或文件名存入链表*/
				show=1;
				which=1;		/*用which标记所有参数中是否有文件参数，如果没有文件参数，则需要自动添加当前目录.*/

				if((type=FileFind(temp)) == 0){		/*如果无法找到参数指定的文件，说明参数错误*/
					printf("找不到此文件：%s\n",temp);
					m++;
					continue;
				}

				ptemp=(struct cmand *)malloc(sizeof(struct cmand));
				ptemp->flag=flag;
				ptemp->type=type;
				strcpy(ptemp->filename,temp);
				ptemp->next=NULL;
				phead=listadd(phead,ptemp);
			}
		}
		else{
			temp[n++]=command[m];		/*记录分割的命令*/
			temp[n]='\0';
		}
		m++;
	}
	if(which == 0){		/*如果没有文件名参数，自动添加"."*/
		ptemp=(struct cmand *)malloc(sizeof(struct cmand));
		ptemp->next=NULL;
		ptemp->flag=flag;
		strcpy(ptemp->filename,".");
		ptemp->type='d';
		phead=listadd(phead,ptemp);
	}
	return phead;
}


/*打印指定路径下所有文件名*/
int dirprint(const char *path)
{
	DIR *dir;
	struct dirent *ptr;
	if((dir=opendir(path))==NULL){
		return -1;
	}
	while((ptr=readdir(dir))!=NULL){
		printf("%s\n",ptr->d_name);
	}
	closedir(dir);
	return 0;
}


/*打印文件的属性*/
int getfilestatus(const char *name)
{
	char buf_time[30];/*存储文件时间*/
	struct stat buf;/*存储文件属性*/
	struct passwd *psd=NULL;/*存储用户的数据*/
	struct group *grp=NULL;/*存储用户所属组的数据*/
	char filename[30];
	int m=0,n=0;

	while(name[m] != '\0'){		/*从完整路径名中解析出文件名*/
		if(name[m] == '/'){
			n=0;
		}
		else{
			filename[n++]=name[m];
			filename[n]='\0';
		}
		m++;
	}

	if(lstat(name,&buf)==-1){
		printf("无法读取这个文件：%s\n",filename);
		return 0;
	}

	/*判断并打印文件类型*/
	if(S_ISLNK(buf.st_mode)){
		printf("l");
	}
	else if(S_ISREG(buf.st_mode)){
		printf("-");
	}
	else if(S_ISDIR(buf.st_mode)){
		printf("d");
	}
	else if(S_ISCHR(buf.st_mode)){
		printf("c");
	}
	else if(S_ISBLK(buf.st_mode)){
		printf("b");
	}
	else if(S_ISFIFO(buf.st_mode)){
		printf("f");
	}
	else if(S_ISSOCK(buf.st_mode)){
		printf("s");
	}

	/*打印文件所有者对该文件的操作权限*/
	if(buf.st_mode & S_IRUSR){/*判断文件所有者是否有读取权限*/
		printf("r");
	}
	else{
		printf("-");
	}
	if(buf.st_mode & S_IWUSR){/*判断文件所有者是否具有写入权限*/
		printf("w");
	}
	else{
		printf("-");
	}
	if(buf.st_mode & S_IXUSR){/*判断文件所有者是否具有运行权限*/
		printf("x");
	}
	else{
		printf("-");
	}

	/*打印文件所有者同用户组的用户对该文件的操作权限*/
	if(buf.st_mode & S_IRGRP){
		printf("r");
	}
	else{
		printf("-");
	}
	if(buf.st_mode & S_IWGRP){
		printf("w");
	}
	else{
		printf("-");
	}
	if(buf.st_mode & S_IXGRP){
		printf("x");
	}
	else{
		printf("-");
	}

	/*打印其他用户对该文件的操作权限*/
	if(buf.st_mode & S_IROTH){
		printf("r");
	}
	else{
		printf("-");
	}
	if(buf.st_mode & S_IWOTH){
		printf("w");
	}
	else{
		printf("-");
	}
	if(buf.st_mode & S_IXOTH){
		printf("x");
	}
	else{
		printf("-");
	}
	
	printf("  ");
	printf("%d  ",buf.st_nlink);/*打印文件的链接数*/

	/*打印文件所有者及文件所有者所属的组*/
	psd=getpwuid(buf.st_uid);/*getpwuid函数根据用户id获取用户信息*/
	grp=getgrgid(buf.st_gid);/*getgrgid函数根据用户组id获取用户所属用户组的信息*/
	printf("%s  ",psd->pw_name);/*打印文件所有者的名字*/
	printf("%s  ",grp->gr_name);/*打印文件所属组的名字*/

	printf("%d  ",buf.st_size);/*打印文件的大小*/
	strcpy(buf_time,ctime(&buf.st_mtime));/*ctime函数转换的字符串末尾是\n\0*/
	buf_time[strlen(buf_time)-1]='\0';/*将字符串最后的\n替换为\0*/
	printf("%s  ",buf_time);/*打印文件最近修改的时间*/
	printf("%s\n",filename);/*打印文件名*/
	return 0;
}

/*打印指定路径下所有文件的属性*/
int dirallfile(const char *path)
{
	DIR *dir;
	struct dirent *ptr;
	char filename[50];
	if((dir=opendir(path)) == NULL){
		return -1;
	}
	while((ptr = readdir(dir)) != NULL){
		strcpy(filename,path);		/*把文件名写成完整路径*/
		strcat(filename,"/");
		strcat(filename,ptr->d_name);
		if(getfilestatus(filename) == -1){
			return -1;
		}
	}
	return 0;
}


/*目录链表中添加节点*/
struct dir *diradd(struct dir *phead,char *dirname)
{
	struct dir *ptemp=NULL;
	struct dir *padd=NULL;
	ptemp=phead;
	while(ptemp->next != NULL){		/*遍历到链表末尾*/
		ptemp=ptemp->next;
	}
	padd=(struct dir *)malloc(sizeof(struct dir));
	strcpy(padd->dirname,dirname);
	padd->next=NULL;
	ptemp->next=padd;
	return  phead;
}


/*递归查看目录中的文件*/
int dirdiscover(const char *path)
{
	DIR *dir;
	struct dirent *ptr;
	char filename[256];
	char type;
	struct dir *pdirhead=NULL,*pdirtemp=NULL;
	pdirhead=(struct dir *)malloc(sizeof(struct dir));
	pdirhead->next=NULL;

	if((dir=opendir(path)) == NULL){
		return -1;
	}
	printf("\n%s:\n",path);
	while((ptr = readdir(dir)) != NULL){
		strcpy(filename,path);		/*把文件名写成完整路径*/
		strcat(filename,"/");
		strcat(filename,ptr->d_name);

		/*检查读取的文件是否为目录，如果是目录则将完整目录名保存进目录链表中，在打印完所有文件属性后递归查看目录链表*/
		if((FileFind(filename) == 'd') && (strcmp(ptr->d_name,".") != 0) && (strcmp(ptr->d_name,".." ) != 0)){		
			pdirhead=diradd(pdirhead,filename);
		}

		/*打印读取的文件属性*/
		if(getfilestatus(filename) == -1){
			return -1;
		}
	}
	pdirtemp=pdirhead->next;
	while(pdirtemp != NULL){		/*遍历目录链表，并递归显示目录中的内容*/
		dirdiscover(pdirtemp->dirname);
		pdirtemp=pdirtemp->next;
	}
	while(pdirhead != NULL){		/*销毁链表*/
		pdirtemp=pdirhead;
		pdirhead=pdirhead->next;
		free(pdirtemp);
	}
	return 0;
}


/*根据命令链表执行命令*/
int CmdRun(struct cmand *phead)
{
	struct cmand *ptemp=NULL;
	if(phead->next == NULL){
		return -1;
	}
	ptemp=phead->next;
	while(ptemp != NULL){
		switch(ptemp->flag){
			case 0:
				printf("%s\n",ptemp->filename);
				break;
			case CMD_a:
				if(ptemp->type != 'd'){
					printf("%s\n",ptemp->filename);
				}
				else{
					if(dirprint(ptemp->filename) == -1){
						printf("找不到这个文件：%s\n",ptemp->filename);
					}
				}
				break;
			case CMD_l:
				if(ptemp->type == 'd'){
					if(dirallfile(ptemp->filename) == -1){
						printf("读取文件信息错误\n");
					}
				}
				else{
					if(getfilestatus(ptemp->filename) == -1){
						printf("查找不到这个文件：%s\n",ptemp->filename);
					}
				}
				break;
			case CMD_al:
				if(ptemp->type == 'd'){
					if(dirallfile(ptemp->filename) == -1){
						printf("读取文件信息错误。\n");
					}
				}
				else{
					if(getfilestatus(ptemp->filename) == -1){
						printf("查找不到这个文件：%s\n",ptemp->filename);
					}
				}
				break;
			case CMD_R:
				if(ptemp->type == 'd'){
					dirdiscover(ptemp->filename);
				}
				else{
					printf("%s\n",ptemp->filename);
				}
				break;
			default:
				printf("参数错误。\n");
				break;
		}
		ptemp=ptemp->next;
	}
}
