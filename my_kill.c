/*************************************************************************
	> File Name: my_kill.c
	> Author: zhaobaojin mZjin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月03日 星期一 08时50分03秒
 ************************************************************************/
/*支持-s参数*/
#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>

#define N 1
#define L 2
#define S 3

int cmddiscover(int argc, char *argv[], char *buf1, char *buf2) ;        /*解析命令*/

int cmdrun(int flag, char *buf1, char *buf2) ;        /*运行命令*/

int main(int argc, char *argv[])
{
    int i = 0 ;
    int flag = 0 ;
    char buf1[30], buf2[20] ;
    for(i = 0; i < argc; i++) {
        printf ("argv[%d]:%s\n", i, argv[i]) ;
    }
    if(argc == 1) {         /*无任何参数*/
        printf ("my_kill [-s 信号声明 | -n 信号编号 | -信号声明] 进程号 | 任务声明 ... 或 my_kill -l [信号声明]") ;
        exit (0) ;
    }
    else if(argc > 4) {
        printf ("命令过长，本程序不支持。\n") ;
    }
    if((flag = cmddiscover (argc, argv, buf1, buf2)) == -1) {
        printf ("命令格式错误。\n") ;
        exit (0) ;
    }
    printf ("flag=%d\n",flag) ;
    cmdrun (flag, buf1, buf2) ;
    return 0 ;
}

/*解析命令，返回参数标志*/
int cmddiscover(int argc, char *argv[], char buf1[30], char buf2[20])
{
    int i = 0 ;
    int flag = 0 ;
    for(i = 0; i < argc; i++) {
        if((argv[i][0] == '-') && (i < 2)) {
            switch(argv[i][1]) {
                case 's' :
                    flag = S ;
                    break ;
                case 'n' :
                    flag = N ;
                    break ;
                case 'l' :
                    flag = L ;
                    break ;
                default :
                    flag = -1 ;
                    break ;
            }
            break ;
        }
    }
    if(flag == N || flag == S) {
        if(argc == 4) {
        strcpy (buf1, argv[i+1]) ;      /*解析出信号编号*/
        strcpy (buf2, argv[i+2]) ;      /*解析出进程号*/
        }
        else {
            return -1 ;
        }
    }
    return flag ;
}


/*运行命令*/
int cmdrun(int flag, char buf1[30], char buf2[30])
{
    pid_t pid ;
    int sig;
    int t=S;
    if(flag == S || flag == N) {
        sig = atoi (buf1) ;
        pid = atoi (buf2) ;
        if(kill (pid, sig) < 0) {
            perror ("kill") ;
            exit (1) ;
        }
    }
    else if(flag == L) {
       // sigprint () ;
        return 0 ;
    }
    else {
        return -1 ;
    }
}

/*输出所有信号名
void sigprint (void)
{
    printf (" 1) SIGHUP     2) SIGINT    3) SIGQUIT    4) SIGILL    5) SIGTRAP\n");
    printf ("         6) SIGABRT    7) SIGBUS    8) SIGFPE     9) SIGKILL  10) SIGUSR1\n");
    printf ("        11) SIGSEGV   12) SIGUSR2  13) SIGPIPE   14) SIGALRM  15) SIGTERM\n");
    printf("         16) SIGSTKFLT 17) SIGCHLD  18) SIGCONT   19) SIGSTOP  20) SIGTSTP\n");
    printf("21) SIGTTIN22) SIGTTOU23) SIGURG24) SIGXCPU25) SIGXFSZ\n");
    printf("26) SIGVTALRM27) SIGPROF28) SIGWINCH29) SIGIO30) SIGPWR\n");
    printf("31) SIGSYS34) SIGRTMIN35) SIGRTMIN+1636) SIGRTMIN+2637) SIGRTMIN+3\n");
    printf("       38) SIGRTMIN+439) SIGRTMIN+5040) SIGRTMIN+641) SIGRTMIN+7.1942) SIGRTMIN+8\n");

     printf("       43) SIGRTMIN+944) SIGRTMIN+1045) SIGRTMIN+1146) SIGRTMIN+127974255347) SIGRTMIN+13\n
            48) SIGRTMIN+1449) SIGRTMIN+1550) SIGRTMAX-144951) SIGRTMAX-1352) SIGRTMAX-12\n
            53) SIGRTMAX-114654) SIGRTMAX-104555) SIGRTMAX-94456) SIGRTMAX-857) SIGRTMAX-7\n
            58) SIGRTMAX-64159) SIGRTMAX-5860) SIGRTMAX-4861) SIGRTMAX-3862) SIGRTMAX-2\n
            63) SIGRTMAX-10455564) SIGRTMAX\n") ;
}*/
