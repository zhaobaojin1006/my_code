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
#include<sys/stat.h>
#include<netinet/in.h>
#include<string.h>
#include<poll.h>

#define IDMAX 5              /*ID最大长度*/
#define PASSWARDMAX 40      /*用户密码最大长度*/
#define NAMEMAX 20          /*用户名最大长度*/
#define POLLMAX 100         /*监听列表的最大个数*/
#define LISTEN_MAX 12       /*请求队列最大长度*/
#define BUFMAX 256      /*接收缓冲区最大长度*/
#define DATAMAX 1024        /*数据最大长度*/
#define ALL "0"           /*表示对所有在线用户发送消息*/
#define MESSAGE 1       /*发送消息*/
#define REGISTER 2      /*注册数据*/
#define LOGIN 3         /*登陆数据*/
#define ONLINE 4        /*在线用户信息*/
#define FILENAME 5      /*欲发送的文件名*/
#define FILEDATA 6      /*文件数据*/


/*连接客户端*/
struct link_list {
    int sock_fd ;
    struct sockaddr_in sockaddr ;
    struct link_list  *next ;
} ;

/*在线用户*/
struct online_list {
    int sock_fd ;
    char user_name[NAMEMAX] ;
    char user_ID[IDMAX] ;
    struct sockaddr_in sockaddr ;
    struct online_list  *next ;
} ;

/*用户信息*/
struct user_data {
    char user_ID[IDMAX] ;
    char user_name[NAMEMAX] ;
    char user_passward[PASSWARDMAX] ;
} ;

/*命令包*/
struct cmd {
    char cmd_from[IDMAX] ;
    char cmd_to[IDMAX] ;
    short cmd_flag ;
    char cmd_data[BUFMAX] ;
} ;

/*聊天记录*/
struct history {
    char history_from[IDMAX] ;
    char history_to[IDMAX] ;
    char history_time[BUFMAX] ;
    char history_data[DATAMAX] ;
} ;



/*全局变量*/
struct link_list  *link_list_PHEAD ;          /*连接客户端链表头指针*/
struct online_list  *online_list_PHEAD ;         /*在线用户链表头指针*/



/*函数声明*/

/********************链表操作函数*****************/
/*给link_list链表增加结点*/
struct link_list  *link_list_add (struct link_list *phead, struct link_list *padd) ;


/*从link_list中删除结点*/
struct link_list  *link_list_del (struct link_list *phead, struct link_list *pdel) ;


/*从link_list中查找某个结点*/
struct link_list  *link_list_discover (struct link_list *phead, int sock_fd) ;


/*在online_list链表中增加结点*/
struct online_list  *online_list_add (struct online_list *phead, struct online_list *padd) ;


/*从online_list中删除结点*/
struct online_list  *online_list_del (struct online_list *phead, struct online_list *pdel) ;


/*从online_list中查找某个结点*/
struct online_list  *online_list_discover (struct online_list *phead, int sock_fd) ;




/*******************命令执行函数*******************/
/*接受客户端的连接*/
int server_accept (int *serv_fd, struct pollfd *poll_fd) ;


/*合并字符串*/
int server_bag_cat (char buf[BUFMAX], const char buf1[BUFMAX], const char buf2[BUFMAX]) ;


/*分拆字符串*/
int server_bag_cut (char buf[DATAMAX], char buf1[BUFMAX], char buf2[BUFMAX]) ;


/*命令包打包*/
struct cmd  *server_bag (char cmd_from[IDMAX], char cmd_to[IDMAX], char buf[BUFMAX], short flag) ;


/*存储聊天记录,from和to用户必须存在*/
int server_history_write (char from[IDMAX], char to[IDMAX], char data[DATAMAX]) ;


/*向命令包指定的ID发送命令包*/
int server_send (struct cmd *cmdtemp) ;


/*获取用户最大ID，加1后分配给新注册用户*/
int user_ID_get (void) ;


/*用户注册*/
int server_register (struct cmd *pcmd, int sock_fd) ;


/*检查ID有效性*/
int user_ID_cmp (char user_ID[IDMAX]) ;


/*用户信息核对*/
int user_data_cmp (char user_ID[5], char user_passward[PASSWARDMAX], char user_name[NAMEMAX]) ;


/*用户登陆*/
int server_login (int sock_fd, struct cmd *pcmd) ;





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


/*在online_list链表中增加结点*/
struct online_list  *online_list_add (struct online_list *phead, struct online_list *padd)
{
    padd -> next = NULL ;
    /*头插法*/
    if(phead != NULL) {
        padd -> next = phead ;
    }
    return padd ;
}

/*从online_list中删除 pdel指向的结点*/
struct online_list  *online_list_del (struct online_list *phead, struct online_list *pdel)
{  
    struct online_list *ptemp = phead ;
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


/*从online_list中查找某个结点*/
struct online_list  *online_list_discover (struct online_list *phead, int sock_fd)
{
    struct online_list *ptemp = phead ;
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


/*合并字符串*/
int server_bag_cat (char buf[BUFMAX], const char buf1[BUFMAX], const char buf2[BUFMAX])
{
    int i = 0, n = 0 ;
    strcpy (buf, buf1) ;
    while(buf1[i] != '\0') {
        buf[n++] = buf1[i++] ;
    }
    buf[n] = '#' ;
    buf[n+1] = '\0' ;
    strcat (buf, buf2) ;
    return 0 ;
}


/*分拆字符串*/
int server_bag_cut (char buf[DATAMAX], char buf1[BUFMAX], char buf2[BUFMAX])
{   
    int flag = 0 ;
    int i = 0, n = 0 ;
    while(buf[i] != '\0') {
        if(buf[i] == '#') {
            buf1[n] = '\0' ;
            flag = 1 ;
            n = 0 ;
            i++ ;
            continue ;
        }
        switch(flag) {
            case 0 :
                buf1[n++] = buf[i] ;
                break ;
            case 1 :
                buf2[n++] = buf[i] ;
                break ;
            default :
                return -1 ;
                break ;
        }
        i++ ;
    }
    buf2[n] = '\0' ;
    return 0 ;
}


/*命令包打包*/
struct cmd  *server_bag (char cmd_from[IDMAX], char cmd_to[IDMAX], char buf[BUFMAX], short flag)
{
    struct cmd  *cmdtemp ;
    cmdtemp = (struct cmd *)malloc (sizeof (struct cmd)) ;
    memset (cmdtemp, 0, sizeof (struct cmd)) ;
    strcpy (cmdtemp -> cmd_from, cmd_from) ;
    strcpy (cmdtemp -> cmd_to, cmd_to) ;
    strcpy (cmdtemp -> cmd_data, buf) ;
    cmdtemp -> cmd_flag = flag ;
    return cmdtemp ;
}



/*存储聊天记录,from和to用户必须存在*/
int server_history_write (char from[IDMAX], char to[IDMAX], char data[DATAMAX])
{
    FILE *fp =NULL ;
    char path[BUFMAX], path_temp[BUFMAX] ;
    char time_temp[BUFMAX], data_temp[DATAMAX] ;
    struct history  history_temp ;
    printf ("from :%s\nto :%s\ndata:%s\n", from, to, data) ;
    /*群消息储存*/
    if(strcmp (to, ALL) == 0) {
        server_bag_cut (data, time_temp, data_temp) ;printf ("time_temp:%s\ndata_temp:%s\n", time_temp, data_temp) ;
        strcpy (history_temp.history_from, from) ;
        strcpy (history_temp.history_to, to) ;
        strcpy (history_temp.history_time, time_temp) ;
        strcpy (history_temp.history_data, data_temp) ;
        /*存储聊天记录到群历史消息文件*/
        if((fp = fopen ("group_chat_history_server.txt", "ab+")) == NULL) {
            my_err ("fopen", __LINE__) ;
            if((fp = fopen (path_temp, "w")) == NULL) {
                my_err ("fopen", __LINE__) ;
                return -1 ;
            }
            fclose (fp) ;
        }
        if(fwrite (&history_temp, sizeof (struct history), 1, fp) < 0) {
            my_err ("fwrite", __LINE__) ;
            return -1 ;
        }
        fclose (fp) ;
        return 0 ;
    }
    /*保存当前工作目录*/
    if(getcwd (path, sizeof (path)) == NULL) {
        my_err ("getcwd", __LINE__) ;
        return -1 ;
    }
    /*创建用户目录*/
    mkdir (from, 0744) ;
    /*进入用户目录*/
    if(chdir (from) < 0) {
        my_err ("chdir", __LINE__) ;
        return -1 ;
    }
    server_bag_cut (data, time_temp, data_temp) ;
    strcpy (history_temp.history_from, from) ;
    strcpy (history_temp.history_to, to) ;
    strcpy (history_temp.history_time, time_temp) ;
    strcpy (history_temp.history_data, data_temp) ;
    /*存储聊天记录到from用户文件*/
    strcpy (path_temp, from) ;
    strcat (path_temp, "_chat_history_server.txt") ;
    if((fp = fopen (path_temp, "ab+")) == NULL) {
        my_err ("fopen", __LINE__) ;
        if((fp = fopen (path_temp, "w")) == NULL) {
            my_err ("fopen", __LINE__) ;
            return -1 ;
        }
        fclose (fp) ;
    }
    if(fwrite (&history_temp, sizeof (struct history), 1, fp) < 0) {
        my_err ("fwrite", __LINE__) ;
        return -1 ;
    }
    fclose (fp) ;
    /*工作目录返回主目录*/
    if(chdir (path) < 0) {
        my_err ("chdir", __LINE__) ;
        return -1 ;
    }
    /*创建用户目录*/
    mkdir (to, 0744) ;
    /*进入用户目录*/
    if(chdir (to) < 0) {
        my_err ("chdir", __LINE__) ;
        return -1 ;
    }

    /*存储聊天记录到to用户文件*/
    memset (path_temp, 0, sizeof (path_temp)) ;
    strcpy (path_temp, to) ;
    strcat (path_temp, "_chat_history_server.txt") ;
    if((fp = fopen (path_temp, "ab+")) == NULL) {
        my_err ("fopen", __LINE__) ;
        if((fp = fopen (path_temp, "w")) == NULL) {
            my_err ("fopen", __LINE__) ;
            return -1 ;
        }
        fclose (fp) ;
    }
    if(fwrite (&history_temp, sizeof (struct history), 1, fp) < 0) {
        my_err ("fwrite", __LINE__) ;
        return -1 ;
    }
    fclose (fp) ;
}




/*向命令包指定的ID发送命令包*/
int server_send (struct cmd *cmdtemp)
{
    struct online_list  *online_list_ptemp = online_list_PHEAD ;
    /*发送目标ID为ALL*/
    if(strcmp (cmdtemp -> cmd_to, ALL) == 0) {
        /*只有MESSAGE类型命令包才转发*/
        if(cmdtemp -> cmd_flag == MESSAGE) {
            online_list_ptemp = online_list_PHEAD ;
            /*遍历在线用户列表，向每个用户发送命令包*/
            while(online_list_ptemp != NULL) {
                /*不给发送者重复发送命令包*/
                if(strcmp (cmdtemp -> cmd_from, online_list_ptemp -> user_ID) == 0) {
                    online_list_ptemp = online_list_ptemp -> next ;
                    continue ;
                }
                if(send (online_list_ptemp -> sock_fd, cmdtemp, sizeof (struct cmd), 0) < 0) {
                    my_err ("send", __LINE__) ;
                    return -1 ;
                }
                online_list_ptemp = online_list_ptemp -> next ;
            }
            /*群消息记录存储*/
            if(cmdtemp -> cmd_flag == MESSAGE) {
                if(server_history_write (cmdtemp -> cmd_from, cmdtemp -> cmd_to, cmdtemp -> cmd_data) < 0) {
                    my_err ("server_history_write", __LINE__) ;
                    return -1 ;
                }
            }
            return 0 ;
        }
        return 0 ;
    }
    else {
        /*目标ID无效，无人注册*/
        if(user_ID_cmp (cmdtemp -> cmd_to) <= 0) {
            return -1 ;
        }
        while(online_list_ptemp != NULL) {
            if(strcmp(online_list_ptemp -> user_ID, cmdtemp -> cmd_to) == 0) {
                break ;
            }
            online_list_ptemp = online_list_ptemp -> next ;
        }
        /*目标用户在线*/
        if(online_list_ptemp != NULL) {
            /*向目标ID发送消息*/
            if(send (online_list_ptemp -> sock_fd, cmdtemp, sizeof (struct cmd), 0) < 0) {
                my_err ("send", __LINE__) ;
                return -1 ;
            }
            /*发送成功，记录聊天历史*/
            if(cmdtemp -> cmd_flag == MESSAGE) {
                if(server_history_write (cmdtemp -> cmd_from, cmdtemp -> cmd_to, cmdtemp -> cmd_data) < 0) {
                    my_err ("server_history_write", __LINE__) ;
                    return -1 ;
                }
            }
        }
        /*目标用户不在线*/
        else {
            /*离线消息机制*/
        }
        return 0 ;
    }
}



/*获取用户最大ID分配给新注册用户*/
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
    /*把分配后的最大ID重新储存到文件开头*/
    if(sprintf (user_data_ptemp.user_ID, "%d", fd_ID+1) <= 0) {
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
int server_register (struct cmd *pcmd, int sock_fd)
{
    char *buf ;
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
    struct cmd  *ptemp = NULL ;
    ptemp = server_bag ("0", user_data_ptemp.user_ID, user_data_ptemp.user_ID, REGISTER) ;
    if(send (sock_fd, ptemp, sizeof (struct cmd), 0) < 0) {
        my_err ("send", __LINE__) ;
        return -1 ;
    }
    printf ("i send end\n") ;
    return 0 ;
}



/*检查ID有效性*/
int user_ID_cmp (char user_ID[IDMAX])
{
    struct user_data  user_data_ptemp ;
    FILE *fp = NULL ;
    int user_id_max, user_id ;
    
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
    if(user_id < 1000 || user_id >= user_id_max) {
        return 0 ;
    }
    else {
        return 1 ;
    }
}



/*用户信息核对*/
int user_data_cmp (char user_ID[5], char user_passward[PASSWARDMAX], char user_name[NAMEMAX])
{ 
    struct user_data  user_data_ptemp ;
    FILE *fp = NULL ;
    int user_id, user_id_max ;
    int count = 0 ;
    
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
    if(user_id < 1000 || user_id >=  user_id_max) {
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
        if((strcmp (user_data_ptemp.user_ID, user_ID) == 0) && (strcmp (user_data_ptemp.user_passward, user_passward) == 0)) {
            strcpy (user_name, user_data_ptemp.user_name) ;
            return 1 ;
        }
        else {
            return 0 ;
        }
    }

}


/*向新登陆的用户发送在线用户链表的信息，并向所有在线用户发送有新用户登陆的信息*/
int server_login_send (int sock_fd, char user_name[NAMEMAX], char user_ID[IDMAX])
{
    struct cmd  *cmdptemp = NULL ;
    struct online_list  *online_list_ptemp = online_list_PHEAD ;
    char buf[BUFMAX] ;
    /*把新用户登陆信息发送给所有在线用户*/
    memset (buf, 0, sizeof (buf)) ;
    server_bag_cat (buf, user_ID, user_name) ;
    cmdptemp = server_bag ("0", "0", buf, ONLINE) ;
    if(server_send (cmdptemp) < 0) {
        my_err ("server_send", __LINE__) ;
        printf ("server_login_send end\n") ;
        return -1 ;
    }
   /*遍历在线用户列表,把所有在线用户信息发送给新登陆用户*/
    while(online_list_ptemp != NULL) {
        if(strcmp (online_list_ptemp -> user_ID, user_ID) == 0) {
            online_list_ptemp = online_list_ptemp -> next ;
            continue ;
        }
        memset (buf, 0, sizeof (buf)) ;
        server_bag_cat (buf, online_list_ptemp -> user_ID, online_list_ptemp -> user_name) ;
        cmdptemp = server_bag ("0", user_ID, buf, ONLINE) ;
        if(send (sock_fd, cmdptemp, sizeof (struct cmd), 0) < 0) {
            my_err ("send", __LINE__) ;
            return -1 ;
        }
        online_list_ptemp = online_list_ptemp -> next ;
    }
        printf ("server_login_send right end\n") ;
    return 0 ;
}




/*用户登陆*/
int server_login (int sock_fd, struct cmd *pcmd)
{
    char *buf ;
    struct online_list  *online_list_ptemp = NULL;
    struct link_list  *link_list_ptemp = NULL;
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
        /*若一个用户已经登陆过，则不允许再次登陆*/
        online_list_ptemp = online_list_PHEAD ;
        while(online_list_ptemp != NULL) {
            if(strcmp (online_list_ptemp -> user_ID, user_ID) == 0) {
                struct cmd  *cmdptemp = NULL ;
                cmdptemp = server_bag ("0", "0", "此用户已经登陆", LOGIN) ;
                if(send (sock_fd, cmdptemp, sizeof (struct cmd), 0) < 0) {
                    my_err ("send", __LINE__) ;
                }
                printf ("login failed\n") ;
                return 0 ;
            }
        }

        printf ("login start\n") ;
        online_list_ptemp = (struct online_list *)malloc (sizeof (struct online_list)) ;
        online_list_ptemp -> sock_fd = link_list_ptemp -> sock_fd ;
        strcpy (online_list_ptemp -> user_name, user_name) ;
        strcpy (online_list_ptemp -> user_ID, user_ID) ;
        online_list_ptemp -> sockaddr = link_list_ptemp -> sockaddr ;
        /*将用户加入在线用户列表*/
        online_list_PHEAD = online_list_add (online_list_PHEAD, online_list_ptemp) ;
        /*将用户从连接用户列表中删除*/
        link_list_PHEAD = link_list_del (link_list_PHEAD, link_list_ptemp) ;
        /*给新登陆用户发送登陆成功信息*/
        struct cmd  *cmdptemp = NULL ;
        char user_data [BUFMAX] ;
        server_bag_cat (user_data, user_ID, user_name) ;
        cmdptemp = server_bag ("0", user_ID, user_data, LOGIN) ;
        if(server_send (cmdptemp) < 0) {
            printf ("server_send failed\n") ;
        }
        /*有新登陆用户，所有在线用户更新在线列表*/
        if(server_login_send (sock_fd, user_name, user_ID) < 0) {
            printf ("server_login_send failed\n") ;
        }
        return 1 ;
    }
    else {
        struct cmd  *cmdptemp = NULL ;
        cmdptemp = server_bag ("0", "0", "登陆失败", LOGIN) ;
        if(send (sock_fd, cmdptemp, sizeof (struct cmd), 0) < 0) {
            my_err ("send", __LINE__) ;
        }
        printf ("login failed\n") ;
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




/*服务端接收客户端命令，并进行解析,处理完毕返回0，返回1说明该套结字断开连接*/
int server_recv (struct pollfd  *poll_fd)
{
    struct cmd  cmdtemp ;
    struct link_list  *link_list_ptemp = NULL ;
    struct online_list  *online_list_ptemp = NULL ;
    int recv_len = 0 ;
    memset (&cmdtemp, 0, sizeof (struct cmd)) ;printf ("i get a bag\n") ;
    /*接收命令包*/
    if((recv_len = recv (poll_fd -> fd, &cmdtemp,  sizeof (struct cmd), 0)) < 0) {
        close (poll_fd -> fd) ;
        my_err ("recv", __LINE__) ;
        return -1 ;
    }
    /*如果该套结字断开连接,返回1则提示该套结字断开连接*/
    else if(recv_len == 0) {
        printf ("a client quit\n") ;
        /*从连接链表和在线用户链表中查找此套结字描述符,删去该结点*/
        if((link_list_ptemp = link_list_discover (link_list_PHEAD, poll_fd -> fd)) != NULL) {
            link_list_PHEAD = link_list_del (link_list_PHEAD, link_list_ptemp) ;
        }
        else if((online_list_ptemp = online_list_discover (online_list_PHEAD, poll_fd -> fd)) != NULL) {
            online_list_PHEAD = online_list_del (online_list_PHEAD, online_list_ptemp) ;
        }
        close (poll_fd -> fd) ;
        return 1 ;
    }
    /*接收正常的命令包，对其进行解析*/
    else {
        switch(cmdtemp.cmd_flag) {
            case MESSAGE :
                if(server_send (&cmdtemp) < 0) {
                    return -1 ;
                }
                break ;
            case LOGIN :
                if(server_login (poll_fd -> fd, &cmdtemp) <= 0) {
                    return -1 ;
                }
                printf ("login end \n") ;
                break ;
            case REGISTER :
                if(server_register (&cmdtemp, poll_fd -> fd) < 0) {
                    return -1 ;
                }
                break ;
            default :
                break ;
        }
        return 0 ;
    }
}




/*通过poll函数循环监视套结字*/
int poll_server (int *sock_fd)
{
    int server_recv_return = 0 ;
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
    while((poll_num = poll (&poll_fd[0], count, -1)) >= 0) {printf ("start deal\n") ;
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
                /*接收命令包*/
                if((server_recv_return = server_recv (&poll_fd[n])) < 0) {
                    continue ;
                }
                /*如果该套结字断开连接，将poll_fd数组中该元素清空*/
                else if(server_recv_return == 1) {
                    memset (&poll_fd[n], 0, sizeof (struct pollfd)) ;
                    printf ("a client gone\n") ;
                }
                else {
                    poll_fd[n].revents = 0 ;        /*重新将发生的事件置为0，以便下一次监视*/
                }
                poll_num-- ;
            }
            if(poll_num <= 0) {         /*发生事件的套结字处理完，直接退出循环*/
                break ;
            }
        }
        printf ("deal end\n") ;
    }
    return 0 ;
}
