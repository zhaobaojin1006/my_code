/*************************************************************************
	> File Name: my_server_test.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月05日 星期三 20时01分08秒
 ************************************************************************/
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#include<unistd.h>
#include<sys/select.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<pthread.h>
#include<string.h>


void thread_serv (int *client_fd) ;       /*响应客户端的线程函数*/

void thread_send (int *client_fd) ;      /*发送消息的线程函数*/

void thread_recv (int *client_fd) ;      /*接收消息的线程函数*/

int main(void)
{
    int sock_fd ;
    int client_fd ;         /*保存连接的套结字描述符*/
    int client_len ;
    int option_value = 1 ;
    struct sockaddr_in client_addr ;
    struct sockaddr_in serv_addr ;
    pthread_mutex_init (&mutex, NULL) ;         /*初始化线程互斥锁*/
    pthread_t thid ;
    memset (&serv_addr, 0, sizeof (struct sockaddr_in)) ;       /*将serv_addr清0*/
    serv_addr.sin_family = AF_INET ;         /*地址类型*/
    serv_addr.sin_port = htons (9527) ;      /*设置端口号为9527*/
    serv_addr.sin_addr.s_addr = htonl (INADDR_ANY) ;         /*设置ip地址*/

    /*创建套结字*/
    if((sock_fd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror ("socket") ;
        exit (1) ;
    }

    /*绑定端口*/
    if(bind (sock_fd, (struct sockaddr *)&serv_addr, sizeof (struct sockaddr_in)) == -1) {
        perror ("bind") ;
        exit (1) ;
    }

    /*将套结字转换为监听套结字,最大连接队列长度为10*/
    if(listen (sock_fd, 10) == -1) {
        perror ("listen") ;
        exit (1) ;
    }

    /*设置socket属性*/
    if(setsockopt (sock_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof (option_value)) == -1) {
        perror ("setsockopt") ;
        exit (1) ;
    }
    client_len = sizeof (struct sockaddr_in) ;
    /*等待客户端连接*/
    while((client_fd = accept (sock_fd, (struct sockaddr *)&client_addr, &client_len)) != -1) {
        if(pthread_create (&thid, NULL, (void *)thread_serv , &client_fd) != 0) {       /*创建新线程以响应连接的客户端*/
            perror ("pthread_create") ;
            exit (1) ;
        }
    }
    if(client_fd < 0) {
        perror ("accept") ;
        exit (1) ;
    }
    close (sock_fd) ;       /*关闭监听套结字*/
    return 0 ;
}

/*处理连接客户端的线程函数*/
void thread_serv (int *client_fd)
{
    int *status ;
    pthread_t thid ;
    int serv_fd = *(int *)client_fd ;

    printf ("i get a client") ;
    if(pthread_create (&thid, NULL, (void *)thread_send, &serv_fd) != 0) {
        perror ("pthread_create") ;
        pthread_exit (0) ;
    }
    pthread_join (thid, (void *)&status) ;      /*等待发送数据线程结束*/
    close (serv_fd) ;       /*断开连接*/
}

void thread_send (int *client_fd)
{
    pthread_t thid ;
    int *status ;
    char buf[256] ;
    int send_fd = *client_fd ;
    if(pthread_create (&thid, NULL, (void *)thread_recv, &send_fd)) {
        perror ("pthread_create") ;
        pthread_exit (0) ;
    }
    do {
        printf ("请输入：") ;
        scanf ("%s", buf) ;
        if(strcmp (buf, "quit") == 0) {
            pthread_cancel (thid) ;         /*关闭接收接收数据的线程*/
            pthread_join (thid, (void *)&status) ;      /*等待接收数据线程结束*/
            pthread_exit (0) ;
        }
    }while(send(send_fd, buf, sizeof (buf), 0) != -1) ;
}

void thread_recv (int *client_fd)
{
    char buf[256] ;
    int recv_fd = *client_fd ;
    while(recv (recv_fd, buf, sizeof (buf), 0) != -1) {
        printf ("recv :%s",buf) ;
        memset (buf, 0, sizeof(buf)) ;
    }
}

