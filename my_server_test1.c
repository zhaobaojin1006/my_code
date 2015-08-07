/*************************************************************************
	> File Name: my_server_test1.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月06日 星期四 16时02分41秒
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<poll.h>

#define POLLMAX 3

/*错误显示*/
void my_err (const char buf[50], int line) ;

/*产生监听套结字*/
int sock_set (int *sock_fd) ;

/*从监视列表数组中查找一个未使用的结点返回*/
struct pollfd *pollfd_get (struct pollfd *poll_fd) ;

/*从连接套结字描述符上获取数据*/
int serv_recv (const int *client_fd) ;

int main(void)
{
    int i ;
    int data_count = 0 ;        /*当前有事件发生的套结字描述符的个数*/
    int count = 0 ;         /*监视套结字描述符的个数*/
    int sock_fd ;
    int serv_fd ;
    int sockaddr_len ;
    struct pollfd poll_fd[3] ;
    struct pollfd *poll_fd_temp = NULL ;
    struct sockaddr_in serv_addr;
    memset (&poll_fd[0], 0, POLLMAX*sizeof (struct pollfd)) ;       /*监视列表清空*/

    /*产生监听套结字*/
    if(sock_set(&sock_fd) < 0) {
        exit (1) ;
    }
    printf ("poll_fd[0].fd : %d\n", poll_fd[0].fd) ;
    sleep (3) ;
    poll_fd[0].fd = sock_fd ;
    poll_fd[0].events = POLLRDNORM ;
    count++ ;
    memset (&serv_addr, 0, sizeof (struct sockaddr_in)) ;
    sockaddr_len = sizeof (struct sockaddr_in) ;
    /*监视套结字描述符*/
    while((data_count = poll (&poll_fd[0], count, -1)) != -1) {
        if(poll_fd[0].revents == POLLRDNORM) {         /*如果监听套结字上有可读数据，说明有用户请求连接*/
            if((serv_fd = accept (poll_fd[0].fd, (struct sockaddr *)&serv_addr, &sockaddr_len)) < 0) {
                my_err ("accept", __LINE__) ;
                exit (0) ;
            }
            if((poll_fd_temp = pollfd_get (&poll_fd[0])) == 0) {      /*找出未使用结点*/
                printf ("pollfd_get failed\n") ;
                exit (1) ;
            }
            poll_fd_temp->fd = serv_fd ;      /*添加新的连接套结字描述符到监视列表中*/
            poll_fd_temp->events = POLLRDNORM ;      /*监视事件为 有可读数据到达*/
            count++ ;
        }
        for(i = 1; i < POLLMAX; i++) {
            if(poll_fd[i].revents == POLLRDNORM) {      /*如果连接套结字上有可读数据到达，则读取该数据*/
                printf ("i get data\n") ;
                if(serv_recv (&poll_fd[i].fd) <= 0) {
                    close (poll_fd[i].fd) ;         /*关闭连接套结字*/
                    memset (&poll_fd[i].fd, 0, sizeof (struct pollfd)) ;
                }
                data_count-- ;
            }
            if(data_count <= 0) {       /*如果有事件发生的套结字被访问完*/
                break ;
            }
        }
    }

    return 0;
}

void my_err (const char buf[50], int line)
{
    perror (buf) ;
    fprintf (stderr, "line:%d\n", line) ;
}

int sock_set (int *sock_fd)
{
    struct sockaddr_in sock_fd_temp ;
    memset (&sock_fd_temp, 0, sizeof (struct sockaddr_in)) ;
    sock_fd_temp.sin_family = AF_INET ;
    sock_fd_temp.sin_port = htons (9527) ;
    sock_fd_temp.sin_addr.s_addr = INADDR_ANY ;
    /*创建套结字*/
    if((*sock_fd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        my_err ("socket", __LINE__) ;
        return -1 ;
    }
    /*套结字绑定IP和端口*/
    if(bind (*sock_fd, (struct sockaddr *)&sock_fd_temp, sizeof (struct sockaddr_in)) < 0) {
        my_err ("bind", __LINE__) ;
        return -1 ;
    }
    /*将套结字转换为监听套结字*/
    if(listen (*sock_fd, 10) < 0) {
        my_err ("listen", __LINE__) ;
        return -1 ;
    }
    return 0 ;
}

/*从监听列表中查找一个未使用的元素返回,如果全部被使用则返回NULL*/
struct pollfd *pollfd_get (struct pollfd *poll_fd)
{
    int i ;
    for(i = 1; i < POLLMAX; i++){
        printf ("poll_fd[%d].fd :%d\n", i, poll_fd[i].fd) ;
        if(poll_fd[i].fd <= 0) {
            return &poll_fd[i] ;
        }
    }
    return NULL ;
}

/*从连接套结字描述符上获取数据*/
int serv_recv (const int *client_fd)
{
    int buf_size = 0 ;
    char buf[256] ;
    if((buf_size = recv(*client_fd, buf, sizeof (buf), 0)) < 0) {
        my_err ("recv", __LINE__) ;
        return -1 ;
    }
    else if(buf_size == 0) {
        return buf_size ;
    }
    else {
        printf ("recv :%s\n",buf) ;
        memset (buf, 0, sizeof (buf)) ;
    }
    return buf_size ;
}

