/*************************************************************************
	> File Name: my_server_test2.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月07日 星期五 10时52分11秒
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<poll.h>

#define IDMAX 5              /*ID最大长度*/
#define PASSWARDMAX 40      /*用户密码最大长度*/
#define NAMEMAX 20          /*用户名最大长度*/
#define POLLMAX 100         /*监听列表的最大个数*/
#define LISTEN_MAX 12       /*请求队列最大长度*/
#define BUFMAX 256      /*接收缓冲区最大长度*/

/*连接客户端*/
struct link_list {
    int sock_fd ;
    struct sockaddr_in sockaddr ;
    struct link_list  *next ;
} ;

/*在线用户*/
struct oline_list {
    int sock_fd ;
    char user_name[NAMEMAX] ;
    char user_ID[IDMAX] ;
    struct sockaddr_in sockaddr ;
    struct oline_list  *next ;
} ;

/*用户信息*/
struct user_data {
    char user_ID[IDMAX] ;
    char user_name[NAMEMAX] ;
    char user_passward[PASSWARDMAX]
} ;

/*命令包*/
struct cmd {
    char cmd_from[IDMAX] ;
    char cmd_to[IDMAX] ;
    char cmd_flag ;
    char cmd_data[BUFMAX] ;
} ;



/*全局变量*/
struct link_list  *link_list_PHEAD ;          /*连接客户端链表头指针*/
struct oline_list  *oline_list_PHEAD ;         /*在线用户链表头指针*/



/*函数声明*/

/********************链表操作函数*****************/
/*给link_list链表增加结点*/
struct link_list  *link_list_add (struct link_list *phead, struct link_list *padd) ;


/*从link_list中删除结点*/
struct link_list  *link_list_del (struct link_list *phead, struct link_list *pdel) ;


/*从link_list中查找某个结点*/
struct link_list  *link_list_discover (struct link_list *phead, int sock_fd) ;


/*在oline_list链表中增加结点*/
struct oline_list  *oline_list_add (struct oline_list *phead, struct oline_list *padd) ;


/*从oline_list中删除结点*/
struct oline_list  *oline_list_del (struct oline_list *phead, struct oline_list *pdel) ;


/*从oline_list中查找某个结点*/
struct oline_list  *oline_list_discover (struct oline_list *phead, int sock_fd) ;




/*******************命令执行函数*******************/
/*接受客户端的连接*/
int server_accept (int *serv_fd, struct pollfd *poll_fd) ;


/*获取用户最大ID，加1后分配给新注册用户*/
int user_ID_get (void) 


/*用户注册*/
int server_register (struct cmd *pcmd) ;


/*用户信息核对*/
int user_data_cmp (char user_ID[5], char user_passward[PASSWARDMAX], char user_name[NAMEMAX])


/*用户登陆*/
int server_login (struct cmd *pcmd) ;





/*****************底层控制函数********************/
/*错误信息输出*/
void my_err (const char buf[50], int line) ;


/*创建侦听套结字*/
int sock_get (int * const sock_fd) ;


/*从监视列表中寻找一个未使用的元素*/
struct pollfd *poll_get (struct pollfd *poll_fd) ;


/*接收客户端的命令包*/
int server_recv (struct pollfd *poll_fd) ;


/*通过poll函数循环监视套结字*/
int poll_server (int *sock_fd) ;






int main(void)
{
    int sock_fd ;

    /*将sock_fd创建为侦听套结字*/
    if(sock_get (&sock_fd) < 0) {
        fprintf (stderr, "sock_get error\n") ;
        exit (1) ;
    }

    poll_server (&sock_fd) ;
    return 0 ;
}



void my_err (const char buf[50], int line)
{
    perror (buf) ;
    fprintf (stderr, "line : %d\n", line) ;
}


/********************链表操作函数*******************/
/*给link_list链表增加结点*/
struct link_list  *link_list_add (struct link_list *phead, struct link_list *padd)
{
    padd -> next = NULL ;
    /*头插法*/
    if(phead != NULL) {
        padd -> next = phead ;
    }
    return padd ;
}


/*从link_list中删除 pdel参数指向的结点*/
struct link_list  *link_list_del (struct link_list *phead, struct link_list *pdel)
{
    struct link_list *ptemp = phead ;
    if(phead == pdel) {
        if(phead -> next == NULL) {
            free (pdel) ;
            return NULL ;
        }
        ptemp = phead -> next ;
        free (pdel) ;
        return ptemp ;
    }
    while(ptemp != NULL) {
        if(ptemp -> next == pdel) {
            ptemp -> next = ptemp -> next -> next ;
            free (pdel) ;
            break ;
        }
        ptemp = ptemp -> next ;
    }
    return phead ;
}


/*从link_list中查找某个结点*/
struct link_list  *link_list_discover (struct link_list *phead, int sock_fd)
{
    struct link_list *ptemp = phead ;
    while(ptemp != NULL) {
        if(ptemp -> sock_fd == sock_fd) {
            return ptemp ;
        }
        ptemp = ptemp -> next ;
    }
    return NULL ;
}


/*在oline_list链表中增加结点*/
struct oline_list  *oline_list_add (struct oline_list *phead, struct oline_list *padd)
{
    padd -> next = NULL ;
    /*头插法*/
    if(phead != NULL) {
        padd -> next = phead ;
    }
    return padd ;
}

/*从oline_list中删除 pdel指向的结点*/
struct oline_list  *oline_list_del (struct oline_list *phead, struct oline_list *pdel)
{  
    struct oline_list *ptemp = phead ;
    if(phead == pdel) {
        if(phead -> next == NULL) {
            free (pdel) ;
            return NULL ;
        }
        ptemp = phead -> next ;
        free (pdel) ;
        return ptemp ;
    }
    while(ptemp != NULL) {
        if(ptemp -> next == pdel) {
            ptemp -> next = ptemp -> next -> next ;
            free (pdel) ;
            break ;
        }
        ptemp = ptemp -> next ;
    }
    return phead ;
}


/*从oline_list中查找某个结点*/
struct oline_list  *oline_list_discover (struct oline_list *phead, int sock_fd)
{
    struct oline_list *ptemp = phead ;
    while(ptemp != NULL) {
        if(ptemp -> sock_fd == sock_fd) {
            return ptemp ;
        }
        ptemp = ptemp -> next ;
    }
    return NULL ;
    
}



/******************命令执行函数**********************/
/*接受客户端的连接*/
int server_accept (int *serv_fd, struct pollfd *poll_fd)
{
    struct link_list  *link_list_ptemp ;
    struct sockaddr_in sockaddr ;
    int sockaddr_len = sizeof (struct sockaddr_in) ;
    int client_fd ;
    if((client_fd = accept (*serv_fd, (struct sockaddr *)&sockaddr, &sockaddr_len)) < 0) {
        my_err ("accept", __LINE__) ;
        return -1 ;
    }
    
    printf ("new client_fd : %d\n", client_fd) ;

    /*将新套结字描述符加入监视列表*/
    poll_fd -> fd = client_fd ;
    poll_fd -> events = POLLRDNORM ;
    /*将此连接套结字加入到连接用户链表中*/
    link_list_ptemp = (struct link_list  *)malloc (sizeof (struct link_list)) ;
    link_list_ptemp -> sock_fd = client_fd ;
    link_list_ptemp -> sockaddr.sin_family = sockaddr.sin_family ;
    link_list_ptemp -> sockaddr.sin_port = sockaddr.sin_port ;
    link_list_ptemp -> sockaddr.sin_addr.s_addr = sockaddr.sin_addr.s_addr ;

    link_list_PHEAD = link_list_add (link_list_PHEAD, link_list_ptemp) ;
    return 0 ;
}



/*获取用户最大ID，加1后分配给新注册用户*/
int user_ID_get (void) 
{
    struct user_data  user_data_ptemp ;
    int fd_ID ;
    FILE *fp =NULL ;
    memset (&user_data_ptemp, 0, sizeof (user_data_ptemp)) ;
    /*以可读形式打开一个二进制文件,若没有该文件则新建一个并将初始ID1000写入*/
    if((fp = fopen ("./user_data.txt", "rb+")) == NULL) {
        my_err ("fopen", __LINE__) ;
        if((fp = fopen ("./user_data.txt", "wb+")) == NULL) {
            my_err ("fopen", __LINE__) ;
            return -1 ;
        }
        strcpy (user_data_ptemp.user_ID, "1000") ;
        if(fwrite (&user_data_ptemp, sizeof (struct user_data), 1, fp) == 0) {
            my_err ("fwrite", __LINE__) ;
            return -1 ;
        }
        fclose (fp) ;
        memset (&user_data_ptemp, 0, sizeof (struct user_data)) ;
        if((fp = fopen ("./user_data.txt", "rb+")) == NULL) {
            my_err ("fopen", __LINE__) ;
            return -1 ;
        }
    }
    /*读取二进制文件第一个数据块，分配当前新注册用户的ID*/
    if(fread (&user_data_ptemp, sizeof (struct user_data), 1, fp) == 0) {
        my_err ("fread", __LINE__) ;
        return -1 ;
    }
    fd_ID = atoi (user_data_ptemp.user_ID) ;
    printf ("fd_ID :%d\n", fd_ID) ;
    fd_ID++ ;
    /*把分配后的最大ID重新储存到文件开头*/
    if(sprintf (user_data_ptemp.user_ID, "%d", fd_ID) <= 0) {
        my_err ("sprintf", __LINE__) ;
        return -1 ;
    }
    /*文件指针指向文件开头，覆盖更新最大ID号*/
    if(fseek (fp, 0, SEEK_SET) < 0) {
        my_err ("fseek", __LINE__) ;
        return -1 ;
    }
    if(fwrite (&user_data_ptemp, sizeof (struct user_data), 1, fp) == 0) {
        my_err ("fwrite", __LINE__) ;
        return -1 ;
    }
    fclose (fp) ;
    return fd_ID ;
}




/*用户注册*/
int server_register (struct cmd *pcmd)
{
    FILE *fp = NULL;
    int fd_ID ;
    struct user_data  user_data_ptemp ;
    char user_name[NAMEMAX], user_passward[PASSWARDMAX] ;
    int i = 0, n = 0, flag = 0 ;
    buf = pcmd -> cmd_data ;
    /*解析出用户名和密码*/
    for(i = 0; i < 256 && buf[i] != '\0'; i++) {
        if(buf[i] == '#') {
            n = 0 ;
            flag = 1 ;
            continue ;
        }
        if(flag == 0) {
            user_name[n++] = buf[i] ;
            user_name[n] = '\0' ;
        }
        else {
            user_passward[n++] = buf[i] ;
            user_passward[n] = '\0' ;
        }
    }
    printf ("user_name:%s\nuser_passward:%s\n", user_name, user_passward) ;
    /*分配新用户ID*/
    memset (&user_data_ptemp, 0, sizeof (struct user_data)) ;
    if((fd_ID = user_ID_get()) == -1) {
        return -1 ;
    }
    if(sprintf (user_data_ptemp.user_ID, "%d", fd_ID) <= 0) {
        my_err ("sprintf", __LINE__) ;
        return -1 ;
    }
    strcpy (user_data_ptemp.user_name, user_name) ;
    strcpy (user_data_ptemp.user_passward, user_passward) ;
    /*存储新用户信息，完成注册*/
    if((fp = fopen ("./user_data.txt", "ab+")) == NULL) {
        my_err ("fopen", __LINE__) ;
        return -1 ;
    }
    if(fwrite (&user_data_ptemp, sizeof (struct user_data), 1, fp) == 0) {
        my_err ("fwrite", __LINE__) ;
        return -1 ;
    }
    fclose (fp) ;
    return 0 ;
}




/*用户信息核对*/
int user_data_cmp (char user_ID[5], char user_passward[PASSWARDMAX], char user_name[NAMEMAX])
{ 
    struct user_data  *user_data_ptemp ;
    FILE *FP = NULL ;
    int user_id, user_id_max;
    
    memset (&user_data_ptemp, 0, sizeof (struct user_data)) ;
    user_id = atoi (user_ID) ;
    if((fp = fopen ("./user_data.txt", "rb+")) == NULL) {
        my_err ("fopen", __LINE__) ;
        return -1 ;
    }
    if(fread (&user_data_ptemp, sizeof (struct user_data), 1, fp) <= 0) {
        my_err ("fread", __LINE__) ;
        return -1 ;
    }
    user_id_max = atoi (user_data_ptemp.user_ID) ;
    /*如果欲登陆的ID不存在，返回登陆失败信息，用户ID或密码错误*/
    if(user_id < 1000 || user_id >= user_id_max) {
        return 0 ;
    }
    else {
        /*count为ID所在位置偏移量*/
        count = user_id - 999 ;
        if(fseek (fp, count*sizeof (struct user_data), SEEK_SET) < 0) {
            my_err ("fseek", __LINE__) ;
            return -1 ;
        }
        if(fread (&user_data_ptemp, sizeof (struct user_data), 1, fp) <= 0) {
            my_err ("fread", __LINE__) ;
            return -1 ;
        }
        /*核对文件中的用户信息，ID与密码同时验证通过时返回1，若未通过，返回0*/
        if((strcmp (user_data_ptemp.user_ID, user_ID) == 1) && (strcmp (user_data_ptemp.user_passward, user_passward) == 1)) {
            strcpy (user_name, user_data_ptemp.user_name) ;
            return 1 ;
        }
        else {
            return 0 ;
        }
    }

}




/*用户登陆*/
int server_login (int sock_fd, struct cmd *pcmd)
{
    struct link_list  *link_list_ptemp ;
    char user_ID[5], user_passward[PASSWARDMAX], user_name[NAMEMAX] ;
    int i = 0, n = 0, flag = 0 ;
    buf = pcmd -> cmd_data ;
    /*解析出用户ID和密码*/
    for(i = 0; i < BUFMAX && buf[i] != '\0'; i++) {
        if(buf[i] == '#') {
            n = 0 ;
            flag = 1 ;
            continue ;
        }
        if(flag == 0) {
            user_ID[n++] = buf[i] ;
            user_ID[n] = '\0' ;
        }
        else {
            user_passward[n++] = buf[i] ;
            user_passward[n] = '\0' ;
        }
    }

    /*验证通过，加入在线用户*/
    if(user_data_cmp (user_ID, user_passward, user_name) == 1) {
        if((link_list_ptemp = link_list_discover (link_list_PHEAD, sock_fd)) == NULL) {
            return -1 ;
        }
        oline_list_ptemp = (struct oline_list *)malloc (sizeof (struct oline_list)) ;
        oline_list_ptemp -> sock_fd = link_list_ptemp -> sock_fd ;
        strcpy (oline_list_ptemp -> user_name, user_name) ;
        strcpy (oline_list_ptemp -> user_ID, user_ID) ;
        oline_list_ptemp -> sockaddr = link_list_ptemp -> sockaddr ;

        oline_list_PHEAD = oline_list_add (oline_list_PHEAD, oline_list_ptemp) ;
        return 1 ;
    }
    else {
        return 0 ;
    }
}




/****************底层控制函数******************/
/*错误信息输出*/
void my_err (const char buf[50], int line)
{
    perror (buf) ;
    fprintf (stderr, "line : %d\n", line) ;
}



/*创建侦听套结字的函数*/
int sock_get (int * const sock_fd)
{
    struct sockaddr_in sockaddr ;
    /*创建套结字*/
    if((*sock_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        my_err ("socket", __LINE__) ;
        return -1 ;
    }

    /*绑定套结字*/
    memset (&sockaddr, 0, sizeof (struct sockaddr_in)) ;
    sockaddr.sin_family = AF_INET ;     /*设置地址类型*/
    sockaddr.sin_port = htons (9527) ;      /*设置短口号*/
    sockaddr.sin_addr.s_addr = INADDR_ANY ;         /*设置IP地址为本机*/
    if(bind (*sock_fd, (struct sockaddr *)&sockaddr, sizeof (struct sockaddr_in)) < 0) {
        my_err ("bind", __LINE__) ;
        return -1 ;
    }

    /*将主动套结字转换为侦听套结字,最大请求连接队列长度为 LISTEN_MAX*/
    if(listen (*sock_fd, LISTEN_MAX) < 0) {
        my_err ("listen", __LINE__) ;
        return -1 ;
    }
    return 0 ;
}




/*从监视列表中寻找一个未使用的元素*/
struct pollfd *poll_get (struct pollfd *poll_fd)
{
    int n = 0 ;
    for(n = 0; n < POLLMAX; n++) {
        if(poll_fd[n].fd <= 0) {
            return &poll_fd[n];
        }
    }
    return NULL ;
}




/*服务端接收客户端命令，并进行解析*/
int server_recv (struct pollfd  *poll_fd)
{
    struct cmd *cmdtemp ;
    struct link_list  *link_list_ptemp = NULL ;
    struct oline_list  *oline_list_ptemp = NULL ;
    int recv_len = 0 ;
    if((recv_len = recv ()) < 0) {
        close () ;
        my_err ("recv", __LINE__) ;
        return -1 ;
    }
    /*如果该套结字断开连接*/
    else if(recv_len == 0) {
        printf ("a client qut\n") ;
        /*从连接链表和在线用户链表中查找此套结字描述符,删去该结点*/
        if((link_list_ptemp = link_list_discover (link_list_PHEAD, poll_fd -> fd)) != NULL) {
            link_list_PHEAD = link_list_del (link_list_PHEAD, link_list_ptemp) ;
        }
        else if((oline_list_ptemp = oline_list_discover (oline_list_PHEAD, poll_fd -> fd)) != NULL) {
            oline_list_PHEAD = oline_list_del (oline_list_PHEAD, oline_list_ptemp) ;
        }
        close (poll_fd -> fd) ;
    }

}




/*通过poll函数循环监视套结字*/
int poll_server (int *sock_fd)
{
    int n ;
    int poll_num = 0 ;      /*记录poll函数返回 发生事件的套结字的个数*/
    int count = 0 ;         /*需要监视的套结字个数*/
    struct pollfd poll_fd[POLLMAX] ;        /*监视列表数组*/
    struct pollfd *poll_fd_temp ;

    memset (&poll_fd[0], 0, POLLMAX*sizeof (struct pollfd)) ;       /*监视列表数组全部清0*/
    /*将侦听套结字存入poll_fd[0]元素中,监听事件为有普通可读数据就绪*/
    poll_fd[0].fd = *sock_fd ;
    poll_fd[0].events = POLLRDNORM ;
    count = 1 ;

    /*循环监视事件*/
    while((poll_num = poll (&poll_fd[0], count, -1)) >= 0) {
        if(poll_fd[0].revents == POLLRDNORM) {      /*侦听套结字上有可读数据时，说明有客户端请求连接*/
            /*从监视列表中寻找第一个未使用的元素*/
            if((poll_fd_temp = poll_get (&poll_fd[0])) == NULL) {
                printf ("too many client\n") ;
                break ;
            }
            if(server_accept (&poll_fd[0].fd, poll_fd_temp) < 0) {
                break ;
            }
            count++ ;
        }
        
        /*将整个监视列表查看一遍，对发生事件的套结字进行处理*/
        for(n = 1; n < POLLMAX; n++) {
            if(poll_fd[n].revents == POLLRDNORM) {

                if(server_recv (&poll_fd[n]) < 0) {
                    continue ;
                }

                poll_fd[n].revents = 0 ;        /*重新将发生的事件置为0，以便下一次监视*/
                poll_num-- ;
            }
            if(poll_num <= 0) {         /*发生事件的套结字处理完，直接退出循环*/
                break ;
            }
        }
    }
    return 0 ;
}
