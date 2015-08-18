/*************************************************************************
	> File Name: my_client_test.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年08月10日 星期一 21时40分19秒
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<arpa/inet.h>
#include<termio.h>
#include<pthread.h>


#define IDMAX 5              /*ID最大长度*/
#define PASSWARDMAX 40      /*用户密码最大长度*/
#define NAMEMAX 20          /*用户名最大长度*/
#define BUFMAX 256      /*接收缓冲区最大长度*/
#define DATAMAX 1024        /*发送数据最大长度*/
#define ALL "0"       /*表示群发或者服务器发来的消息*/
#define MESSAGE 1       /*发送消息*/
#define REGISTER 2      /*注册数据*/
#define LOGIN 3         /*登陆数据*/
#define ONLINE 4        /*在线用户信息*/
#define FILENAME 5      /*欲发送的文件名*/
#define FILEDATA 6      /*文件数据*/

/*在线用户列表*/
struct online_list {
    char user_name[NAMEMAX] ;
    char user_ID[IDMAX] ;
    struct online_list  *next ;
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
struct online_list  *online_list_PHEAD = NULL ;
struct online_list  online_list_self  ;
pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER ;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER ;
short login_state = 0 ;
short register_state = 0 ;



/*****************链表操作函数*************/
/*在online_list链表中增加结点*/
struct online_list  *online_list_add (struct online_list *phead, struct online_list *padd) ;


/*从online_list中删除结点*/
struct online_list  *online_list_del (struct online_list *phead, struct online_list *pdel) ;


/*从online_list中查找某个结点*/
struct online_list  *online_list_discover (struct online_list *phead, char user_ID[IDMAX]) ;




/**************发送消息函数**************/
/*按任意键继续函数*/
void client_pause (void) ;

/*根据ID查询聊天记录，参数为"0"时表示查询群聊记录*/  
int client_history_read (char user_ID[IDMAX]) ;


/*获得用户输入的命令*/
void my_gets (char buf[BUFMAX]) ;


/*发送命令包函数*/
int client_send (int fd, char buf[BUFMAX], char cmd_from[IDMAX], char cmd_to[IDMAX], short flag) ;


/*合并字符串函数,在buf1和buf2之间加上'#'，存入buf中*/
int client_cmddata_cat (char buf[BUFMAX], char buf1[BUFMAX], char buf2[BUFMAX]) ;


/*拆分字符串函数*/
int client_cmddata_cut (char buf[BUFMAX], char buf1[BUFMAX], char buf2[BUFMAX]) ;


/*注册用户函数*/
int client_register (int fd) ;


/*用户登陆函数*/
int client_login (int fd) ;

/*私聊消息发送*/
int client_chat_person (int sock_fd, char data[DATAMAX]) ;

/*聊天函数*/
int client_chat (int sock_fd) ;

/*********************接收命令包函数*********************/
/*处理MESSAGE消息包*/
int client_message (struct cmd  cmd_temp) ;


/*处理ONLINE在线用户信息包*/
int client_online (struct cmd  cmd_temp) ;


/*处理LOGIN登陆反馈信息包*/
int client_login_message (struct cmd cmd_temp) ;


/*处理REGISTER注册反馈消息包*/
int client_register_message (struct cmd  cmd_temp) ;


/*接收命令包，根据flag分配函数*/
int client_recv (int *sock_fd) ;





/***************控制函数*****************/
/*错误信息输出*/
void my_err (const char buf[BUFMAX], int line) ;


/*获得套结字描述符*/
int sock_get (int * const sock_fd) ;


/*登陆/注册界面*/
int interface_first (void) ;





int main(void)
{
    pthread_t recv_thid ;
    int fd ;
    /*获取已经连接的套结字描述符*/
    if(sock_get (&fd) < 0) {
        exit (1) ;
    }
    /*使用互斥锁保证主线程先等待条件变量*/
    pthread_mutex_lock (&mutex) ;
    /*创建接收消息线程*/
    if(pthread_create (&recv_thid, NULL, (void *)client_recv, &fd) < 0) {
        my_err ("pthread_create", __LINE__) ;
        exit (1) ;
    }
    /*登陆/注册*/
    switch(interface_first ()) {
        case 1 :
            /*注册成功退出程序*/
            if(client_register (fd) < 0) {
                exit (1) ;
            }
            break ;
        case 2 :
            /*登陆成功进入聊天界面*/
            if(client_login (fd) < 0) {
                exit (1) ;
            }
            else {
                printf ("输入任意键继续。\n") ;
                client_pause () ;
                printf ("\033[2J") ;
                /*聊天界面*/
                client_chat (fd) ;
            }
            
            break ;
        default:
            return 0 ;
            break ;
    }

    return 0 ;
}



/***************链表操作函数*******************/
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
struct online_list  *online_list_discover (struct online_list *phead, char user_ID[IDMAX])
{
    struct online_list *ptemp = phead ;
    if(phead == NULL) {
        printf ("no online list\n") ;
    }
    while(ptemp != NULL) {
        if(strcmp (ptemp -> user_ID, user_ID)) {
            return ptemp ;
        }
        ptemp = ptemp -> next ;
    }
    return NULL ;
}



/********************发送消息函数************************/
/*按任意键继续函数*/
void client_pause (void)
{
    /*取消回显*/
    system ("stty -echo") ;
    /*无缓冲模式，接收到字符立即返回*/
    system ("stty -icanon") ;
    getchar () ;
    system ("stty echo") ;
    system ("stty icanon") ;
  
}




/*获取密码的函数*/
void passwd_gets (char passwd[PASSWARDMAX])
{
    char buf[PASSWARDMAX] ;
    char c ;
    int i = 0 ;
    /*取消回显*/
    system ("stty -echo") ;
    /*无缓冲模式，接收到字符立即返回*/
    system ("stty -icanon") ;
    do{
        c = getchar () ;
        /*127是删除键的键值*/
        if(c == 127) {
            /*如果i>0 说明已有输入字符，数组下标-1*/
            if(i > 0) {
                printf ("\b \b") ;
                i-- ;
            }
        }
        /*如果输入的是普通字符*/
        else if(c != '\n'){
            /*输入不能超过密码字符个数上限*/
            if(i >= PASSWARDMAX) {
                continue ;
            }
            printf ("*") ;
            buf[i++] = c ;
        }
    }while(c != '\n') ;
    system ("stty echo") ;
    system ("stty icanon") ;
    buf[i] = '\0' ;
    /*回退光标后， buf中有冗余数据*/
    strcpy (passwd, buf) ;
}



/*根据参数大小接收字符串*/
void client_gets (char data[DATAMAX], int size)
{
    char buf[DATAMAX] ;
    char c ;
    int i = 0 ;
    /*取消回显*/
    system ("stty -echo") ;
    /*无缓冲模式，接收到字符立即返回*/
    system ("stty -icanon") ;
    do{
        c = getchar () ;
        /*127是删除键的键值*/
        if(c == 127) {
            /*如果i>0 说明已有输入字符，数组下标-1*/
            if(i > 0) {
                printf ("\b \b") ;
                i-- ;
            }
        }
        /*如果输入的是普通字符*/
        else if(c != '\n'){
            /*输入不能超过密码字符个数上限*/
            if(i >= size) {
                continue ;
            }
            printf ("%c", c) ;
            buf[i++] = c ;
        }
    }while(c != '\n') ;
    system ("stty echo") ;
    system ("stty icanon") ;
    buf[i] = '\0' ;
    /*回退光标后， buf中有冗余数据*/
    strcpy (data, buf) ;

}





/*存储聊天历史记录，源ID，目标ID，消息数据*/
 int client_history_write (char from[IDMAX], char to[IDMAX], char data[DATAMAX])
 { 
     struct history  history_temp ;
     char path[BUFMAX], time_temp[BUFMAX], data_temp[DATAMAX] ;
     FILE *fp =NULL ;
     printf ("history_from :%s\nhistory_to :%s\ndata :%s\n", from, to, data) ;
     if(strcmp (to, ALL) == 0) {
         memset (&history_temp, 0, sizeof (struct history)) ;
         strcpy (path, online_list_self.user_ID) ;
         strcat (path, "_group_chat_history.txt") ;
         if((fp = fopen (path, "ab+")) == NULL) {
             my_err ("fopen", __LINE__) ;
             /*创建记录文件*/
             if((fp = fopen (path, "w")) == NULL) {
                 my_err ("fopen", __LINE__) ;
                 return -1 ;
             }
             fclose (fp) ;
             if((fp = fopen (path, "ab+")) == NULL) {
                 my_err ("fopen", __LINE__) ;
                 return -1 ;
             }
         }
         client_cmddata_cut (data, time_temp, data_temp) ;printf ("time:%s\ndata:%s\n", time_temp, data_temp) ;
         strcpy (history_temp.history_from, from) ;
         strcpy (history_temp.history_to, to) ;
         strcpy (history_temp.history_time, time_temp) ;
         strcpy (history_temp.history_data, data_temp) ;
         if(fwrite (&history_temp, sizeof (struct history), 1, fp) <= 0) {
             my_err ("fwrite", __LINE__) ;
             return -1 ;
         }printf ("group_client write success\n") ;
         fclose (fp) ;
         return 0 ;
     }
     /*发送或者接收消息时，记录聊天记录进用户聊天历史文件中*/
     strcpy (path, online_list_self.user_ID) ;
     strcat (path, "_chat_history.txt") ;
     memset (&history_temp, 0, sizeof (struct history)) ;
     if((fp = fopen (path, "ab+")) == NULL) {
         my_err ("fopen", __LINE__) ;
         /*创建记录文件*/
         if((fp = fopen (path, "w")) == NULL) {
             my_err ("fopen", __LINE__) ;
             return -1 ;
         }
         fclose (fp) ;
         if((fp = fopen (path, "ab+")) == NULL) {
             my_err ("fopen", __LINE__) ;
             return -1 ;
         }
     }
     client_cmddata_cut (data, time_temp, data_temp) ;
     strcpy (history_temp.history_from, from) ;
     strcpy (history_temp.history_to, to) ;
     strcpy (history_temp.history_time, time_temp) ;
     strcpy (history_temp.history_data, data_temp) ;
     if(fwrite (&history_temp, sizeof (struct history), 1, fp) <= 0) {
         my_err ("fwrite", __LINE__) ;
         return -1 ;
     }
     fclose (fp) ;
     printf ("history write succsee\n") ;
 }


/*根据ID查询聊天记录，参数为"0"时表示查询群聊记录*/  
int client_history_read (char user_ID[IDMAX])
{
    struct history  history_temp ;
    int fd ;
    char path[BUFMAX] ;
    int len = 1;
    /*查询群聊记录*/
    if(strcmp (user_ID, ALL) == 0) {
        strcpy (path, online_list_self.user_ID) ;
        strcat (path, "_group_chat_history.txt") ;
        if((fd = open (path, O_RDONLY | O_CREAT, 0644)) < 0) {
            my_err ("open", __LINE__) ;
            return -1 ;
        }
        while(len != 0) {
            memset (&history_temp, 0, sizeof (struct history)) ;
            if((len = read (fd, &history_temp, sizeof (struct history))) < 0) {
                my_err ("read", __LINE__) ;
                return -1 ;
            }
            if(strcmp (history_temp.history_to, ALL) == 0) {
                printf ("时间：%s\n%s：%s\n", history_temp.history_time, history_temp.history_from, history_temp.history_data) ;
            }
        }
    }
    /*查询私聊记录*/
    else {
        strcpy (path, online_list_self.user_ID) ;
        strcat (path, "_chat_history.txt") ;
        if((fd = open (path, O_RDONLY)) < 0) {
            printf ("无此用户聊天记录\n") ;
            return -1 ;
        }
        while(len != 0) {
            memset (&history_temp, 0, sizeof (struct history)) ;
            if((len = read (fd, &history_temp, sizeof (struct history))) < 0) {
                my_err ("read", __LINE__) ;
                return -1 ;
            }
            else if(len == 0) {
                printf ("无更多聊天记录\n按任意键继续...\n") ;
                getchar () ;
                client_pause () ;
                printf ("\033[2J") ;
            }
            else {
                /*筛选出与目标用户的聊天记录*/
                if(strcmp (history_temp.history_from ,user_ID) == 0) {
                    printf ("时间：%s\n%s @ you：%s\n\n", history_temp.history_time, user_ID, history_temp.history_data) ;
                }
                else if(strcmp (history_temp.history_to, user_ID) == 0) {
                    printf ("时间：%s\nyou @ %s：%s\n\n", history_temp.history_time, user_ID, history_temp.history_data) ;
                }
            }
        }
    }
    return 0 ;
}







/*获得用户输入的命令*/
void my_gets (char buf[BUFMAX])
{
    int i = 0 ;
    while((buf[i++] = getchar ()) != '\n') {
        if(i >= (BUFMAX - 100)) {
            break ;
        }
    }
    buf[i-1] = '\0' ;
}



/*发送命令包函数*/
int client_send (int fd, char data[DATAMAX], char cmd_from[IDMAX], char cmd_to[IDMAX], short flag)
{
    struct cmd  cmdtemp ;
    memset (&cmdtemp, 0, sizeof (struct cmd)) ;
    strcpy (cmdtemp.cmd_from, cmd_from) ;
    strcpy (cmdtemp.cmd_to, cmd_to) ;
    strcpy (cmdtemp.cmd_data, data) ;
    cmdtemp.cmd_flag = flag ;printf ("%s\n%s\n%s\n%d\n", data, cmd_from, cmd_to, flag) ;
    if(send (fd, &cmdtemp, sizeof (struct cmd), 0) < 0) {
        my_err ("send", __LINE__) ;
        return -1 ;
    }
    /*消息类命令包，聊天记录*/
    if(flag == MESSAGE) {
        if(client_history_write (cmd_from, cmd_to, data) < 0) {
            my_err ("client_history_write", __LINE__) ;
            return -1 ;
        }
    }
    printf ("send success\n") ;
}


/*合并字符串函数,在buf1和buf2之间加上'#'，存入buf中*/
int client_cmddata_cat (char buf[BUFMAX], char buf1[BUFMAX], char buf2[BUFMAX])
{
    int i = 0 ;
    for(i = 0; i < NAMEMAX; i++) {
        if(buf1[i] == '\0') {
            buf1[i] = '#' ;
            buf1[i+1] = '\0' ;
            break ;
        }
    }
    strcpy (buf, buf1) ;
    strcat (buf, buf2) ;
    return 0 ;
}


/*拆分字符串函数*/
int client_cmddata_cut (char buf[BUFMAX], char buf1[BUFMAX], char buf2[BUFMAX])
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



/*注册用户函数*/
int client_register (int fd)
{
    int i = 0 ;
    char user_name[NAMEMAX], user_passward[PASSWARDMAX], buf[BUFMAX] ;
    getchar () ;
    printf ("请输入昵称(最长10个字符)：\n") ;
    client_gets (user_name, NAMEMAX-1) ;
    printf ("\n") ;
    printf ("请输入密码(最长20个字符)：\n") ;
    passwd_gets (user_passward) ;
    client_cmddata_cat (buf, user_name, user_passward) ;
    if(client_send (fd, buf, "0", "0", REGISTER) < 0) {
        return -1 ;
    }
    /*等待条件变量*/
    pthread_cond_wait (&cond, &mutex) ;
    /*注册成功*/
    if(register_state == 1) {
        /*解开互斥锁*/
        pthread_mutex_unlock (&mutex) ;
        return 0 ;
    }
    else {
        /*解开互斥锁*/
        pthread_mutex_unlock (&mutex) ;
        return -1 ;
    }
}


/*用户登陆函数*/
int client_login (int fd)
{
    char user_ID[IDMAX], user_passward[IDMAX], buf[BUFMAX] ;
    getchar () ;
    printf ("请输入用户ID：") ;
    client_gets (user_ID, IDMAX-1) ;
    printf ("\n") ;
    printf ("请输入用户密码:") ;
    passwd_gets (user_passward) ;
    client_cmddata_cat (buf, user_ID, user_passward) ;
    if(client_send (fd, buf, "0", "0", LOGIN) < 0) {
        return -1 ;
    }
    /*等待条件变量*/
    pthread_cond_wait (&cond, &mutex) ;
    /*登陆成功*/
    if(login_state == 1) {
        /*创建该用户文件夹*/
        mkdir (user_ID, 0744) ;
        /*将用户文件夹作为工作目录*/
        if(chdir (user_ID) < 0) {
            my_err ("chdir", __LINE__) ;
            return -1 ;
        }
        /*解开互斥锁*/
        pthread_mutex_unlock (&mutex) ;
        return 0 ;
    }
    /*登陆失败*/
    else {
        /*解开互斥锁*/
        pthread_mutex_unlock (&mutex) ;
        return -1 ;
    }
}




/*从在线用户列表中找出目标用户信息*/
struct online_list  *client_chat_discover (struct online_list  *online_list_phead, const char user[NAMEMAX])
{
    struct online_list  *online_list_ptemp = online_list_phead ;
    if(online_list_ptemp == NULL) {
        printf ("no online_list \n") ;
        return NULL ;
    }
    while(online_list_ptemp != NULL) {
        if(strcmp (online_list_ptemp -> user_ID, user) == 0 || strcmp (online_list_ptemp -> user_name, user) == 0) {
            break ;
        }
        online_list_ptemp = online_list_ptemp -> next ;
    }
    return online_list_ptemp ;
}






/*分割用户输入私聊字符串*/
int client_chat_cut (const char buf[BUFMAX], char user[NAMEMAX], char data[DATAMAX])
{
    int i = 1, n = 0 ;
    int data_state = 0 ;
    while(buf[i] != '\0') {
        if(buf[i] == ':') {
            user[n] = '\0' ;
            data_state = 1 ;
            n = 0 ;
            i++ ;
            continue ;
        }
        if(data_state == 1) {
            data[n++] = buf[i] ;
        }
        else {
            user[n++] = buf[i] ;
        }
        i++ ;
    }
    /*字符串中没有':'，格式错误*/
    if(data_state == 0) {
        return -1 ;
    }
    else {
        data[n] = '\0' ;
        return 0 ;
    }
}


/*获取时间字符串*/
void client_time_get (char buf[BUFMAX])
{
    struct tm *t ;
    time_t t_now ;
    time (&t_now) ;
    t = localtime (&t_now) ;
    sprintf (buf, "%.4d-%.2d-%.2d-%.2d-%.2d-%.2d", t -> tm_year+1900, t -> tm_mon+1, t -> tm_mday, t -> tm_hour, t -> tm_min, t -> tm_sec) ;
}


/*私聊消息发送*/
int client_chat_person (int sock_fd, char data[DATAMAX])
{   
    time_t  t ;
    int i = 0, n = 0 ;
    struct online_list  *online_list_ptemp = NULL ;
    char user[NAMEMAX], buf[DATAMAX], time_temp[BUFMAX], cmd_data[DATAMAX];
    /*从命令中解析出目标昵称或ID，和欲发送的字符串*/
    if(client_chat_cut (data, user, buf) < 0) {
        my_err ("client_chat_cut", __LINE__) ;
        printf ("命令格式错误") ;
        return -1  ;
    }
    else {
        /*从在线列表中找出目标用户信息*/
        if((online_list_ptemp = client_chat_discover (online_list_PHEAD, user)) == NULL) {
            printf ("此用户不在线或无此用户信息\n") ;
            return 1 ;
        }
        else {
            client_time_get (time_temp) ;
            client_cmddata_cat (cmd_data, time_temp, buf) ;
            if(client_send (sock_fd, cmd_data, online_list_self.user_ID, online_list_ptemp -> user_ID, MESSAGE) < 0) {
                printf ("发送失败\n") ;
                return -1 ;
            }
            return 0 ;
        }
    }
}





/*聊天函数*/
int client_chat (int sock_fd)
{
    char buf[BUFMAX], user[NAMEMAX], data[DATAMAX], time_temp[BUFMAX], cmd_data[DATAMAX];
    while(1) {
        memset (buf, 0, sizeof (buf)) ;
        memset (data, 0, sizeof (data)) ;
        printf (">>>") ;
        my_gets (buf) ;
        /*向某人发送的私聊消息*/
        if(buf[0] == '@') {
            if(client_chat_person (sock_fd, buf) < 0) {
                my_err ("client_chat_person", __LINE__) ;
                continue ;
            }
        }
        /*打印在线用户*/
        else if(strcmp (buf, "online") == 0) {
            struct online_list  *online_list_ptemp = online_list_PHEAD ;
            while(online_list_ptemp != NULL) {
                printf ("在线用户：\nID：%s\n昵称:%s\n", online_list_ptemp -> user_ID, online_list_ptemp -> user_name) ;
                online_list_ptemp = online_list_ptemp -> next ;
            }
        }
        /*根据ID，打印聊天记录*/
        else if(strcmp (buf, "history") == 0) {
            char user_ID[IDMAX] ;
            printf ("请输入要查询的ID\n") ;
            scanf ("%s", user_ID) ;
            client_history_read (user_ID) ;
        }
        /*群聊消息*/
        else if(buf[0] == ':') {
            memset (time_temp, 0, sizeof (time_temp)) ;
            client_time_get (time_temp) ;
            client_cmddata_cat (cmd_data, time_temp, &buf[1]) ;
            if(client_send (sock_fd, cmd_data, online_list_self.user_ID, "0", MESSAGE) < 0) {
                printf ("发送失败\n") ;
            }
            continue ;
        }
        /*退出命令*/
        else if(strcmp (buf, "quit") == 0) {
            close (sock_fd) ;
            return 0 ;
        }
    }
}


/*********************接收命令包函数*********************/
/*处理MESSAGE消息包*/
int client_message (struct cmd  cmd_temp)
{
    char time_temp[BUFMAX], data_temp[BUFMAX] ;
    struct online_list *online_list_ptemp = NULL ;
    if(client_cmddata_cut (cmd_temp.cmd_data, time_temp, data_temp) < 0) {
        my_err ("client_cmddata_cut", __LINE__) ;
        return -1 ;
    }
    /*消息来源是服务器,系统消息*/
    if(strcmp (cmd_temp.cmd_from, ALL) == 0) {
        fprintf (stdout, "服务器：%s\n", cmd_temp.cmd_data) ;
    }
    /*消息目标为群发,群消息*/
    else if(strcmp(cmd_temp.cmd_to, ALL) == 0) {

        fprintf (stdout, "%s : %s\n", cmd_temp.cmd_from, data_temp) ;
        if(client_history_write (cmd_temp.cmd_from, cmd_temp.cmd_to, cmd_temp.cmd_data) < 0) {
            return -1 ;
        }

    }
    /*消息目标为自己，私聊消息*/
    else {
        fprintf (stdout, "%s @ you :%s\n", cmd_temp.cmd_from, data_temp) ;
        if(client_history_write (cmd_temp.cmd_from, cmd_temp.cmd_to, cmd_temp.cmd_data) < 0) {
            return -1 ;
        }

    }
    return 0 ;
}


/*处理ONLINE在线用户信息包*/
int client_online (struct cmd  cmd_temp)
{
    struct online_list  *online_list_ptemp ;
    online_list_ptemp = (struct online_list *)malloc (sizeof (struct online_list)) ;
    memset (online_list_ptemp, 0, sizeof (struct online_list)) ;
    if(client_cmddata_cut (cmd_temp.cmd_data, online_list_ptemp -> user_ID, online_list_ptemp -> user_name) < 0) {
        my_err ("client_cmddata_cut", __LINE__) ;
        return -1 ;
    }
    /*把在线用户信息添加到在线用户链表中*/
    online_list_PHEAD =online_list_add (online_list_PHEAD, online_list_ptemp) ;
    return 0 ;
}


/*处理LOGIN登陆反馈信息包*/
int client_login_message (struct cmd cmd_temp)
{
    /*加互斥锁*/
    pthread_mutex_lock (&mutex) ;
    /*如果消息包的目标是ALL，说明登陆失败*/
    if(strcmp (cmd_temp.cmd_to, ALL) == 0) {
        login_state = 0 ;
        fprintf (stdout, "系统消息：%s\n", cmd_temp.cmd_data) ;
    }
    /*如果目标是具体ID，则说明登陆成功，将自己的ID和密码存储到全局变量中*/
    else {
        login_state = 1 ;
        fprintf (stdout, "系统消息：登陆成功\n") ;
        /*分割信息包数据，解析出登陆成功的用户的ID和昵称*/
        client_cmddata_cut (cmd_temp.cmd_data, online_list_self.user_ID, online_list_self.user_name) ;
    }
    /*解开互斥锁，并激活条件变量*/
    pthread_mutex_unlock (&mutex) ;
    pthread_cond_signal (&cond) ;
    return 0 ;
}


/*处理REGISTER注册反馈消息包*/
int client_register_message (struct cmd  cmd_temp)
{ 
    /*加互斥锁*/
    pthread_mutex_lock (&mutex) ;
    /*如果消息包的目标是ALL，说明注册失败*/
    if(strcmp (cmd_temp.cmd_to, ALL) == 0) {
        register_state = 0 ;
        fprintf (stdout, "系统消息：注册失败\n") ;
    }
    /*如果目标是具体ID，则说明注册成功*/
    else {
        fprintf (stdout, "系统消息：注册成功，您的ID号为%s，请牢记\n", cmd_temp.cmd_data) ;
    }
    /*解开互斥锁，并激活条件变量*/
    pthread_mutex_unlock (&mutex) ;
    pthread_cond_signal (&cond) ;
    return 0 ;
}




/*接收命令包，根据flag分配函数*/
int client_recv (int *sock_fd)
{
    struct cmd  cmd_temp ;
    while(recv (*sock_fd, &cmd_temp, sizeof (struct cmd), 0) > 0) {printf ("recv bag flag = %d\n", cmd_temp.cmd_flag) ;
        switch(cmd_temp.cmd_flag) {
            case MESSAGE :
                client_message (cmd_temp) ;
                break ;
            case ONLINE :
                client_online (cmd_temp) ;
                break ;
            /*客户端收到REGISTER消息包，说明登陆是否失败*/
            case LOGIN :
                client_login_message (cmd_temp) ;
                break ;
            case REGISTER :
                client_register_message (cmd_temp) ;
                break ;
        }
    }
    printf ("与服务器断开连接\n") ;
    close (*sock_fd) ;
}




/*****************控制函数******************/
/*错误信息输出*/
void my_err (const char buf[BUFMAX], int line)
{
    perror (buf) ;
    fprintf (stderr, "line :%d\n", line) ;
}



/*获得套结字描述符*/
int sock_get (int * const sock_fd)
{
    struct in_addr  inp ;
    struct sockaddr_in serv_addr ;
    memset (&serv_addr, 0, sizeof (struct sockaddr_in)) ;
    /*创建套结字*/
    if((*sock_fd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        my_err ("socket", __LINE__) ;
        return -1 ;
    }
    serv_addr.sin_family = AF_INET ;
    serv_addr.sin_port = htons (9527) ;
    if(serv_addr.sin_addr.s_addr = inet_aton ("127.0.0.1", &inp) == 0) {
        my_err ("inet_aton", __LINE__) ;
        return -1 ;
    }
    /*连接服务器端*/
    if(connect (*sock_fd, (struct sockaddr *)&serv_addr, sizeof (struct sockaddr)) != 0) {
        my_err ("connect", __LINE__) ;
        return -1 ;
    }
    return 0 ;
}


/*注册/登陆界面*/
int interface_first (void)
{
    int choise = 0 ;
    printf ("1.注册。\n2.登陆。\n0.退出。\n请选择：") ;
    scanf ("%d", &choise) ;
    return choise ;
}


