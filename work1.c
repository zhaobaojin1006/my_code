/*************************************************************************
	> File Name: work.c
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年10月11日 星期日 18时38分33秒
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define X_MAX 7
#define X_MIN 0
#define Y_MAX 7
#define Y_MIN 0

struct Coor {
    int x ;
    int y ;
};

/*根据坐标判断是否越界*/
int coor_judge(struct Coor now_coor)
{
    printf("判断：（%d,%d）\n", now_coor.x, now_coor.y) ;sleep(0.8) ;
    if(now_coor.x > X_MAX || now_coor.x < X_MIN || now_coor.y > Y_MAX || now_coor.y < Y_MIN ) {
        return -1 ;
    }
    else {
        return 1 ;
    }
}


/*根据序号，获得坐标的下一个路径*/
int coor_get(struct Coor now_coor, struct Coor *next_coor, int num)
{
    switch(num) {
        case 1 : {
            next_coor -> x = now_coor.x + 1 ;
            next_coor -> y = now_coor.y - 2 ;
            return 1 ;
        }
        case 2 : {
            next_coor -> x = now_coor.x + 2 ;
            next_coor -> y = now_coor.y - 1 ;
            return 2 ;
        }
        case 3 : {
            next_coor -> x = now_coor.x + 2 ;
            next_coor -> y = now_coor.y + 1 ;
            return 3 ;
        }
        case 4 : {
            next_coor -> x = now_coor.x + 1 ;
            next_coor -> y = now_coor.y + 2 ;
            return 4 ;
        }
        case 5 : {
            next_coor -> x = now_coor.x - 1 ;
            next_coor -> y = now_coor.y + 2 ;
            return 5 ;
        }
        case 6 : {
            next_coor -> x = now_coor.x - 2 ;
            next_coor -> y = now_coor.y + 1 ;
            return 6 ;
        }
        case 7 : {
            next_coor -> x = now_coor.x - 2 ;
            next_coor -> y = now_coor.y - 1 ;
            return 7 ;
        }
        case 8 : {
            next_coor -> x = now_coor.x - 1 ;
            next_coor -> y = now_coor.y - 2 ;
            return 8 ;
        }
    }
    return 0 ;
}

/*初始化后继路径出度表*/
int path_next_init(struct Coor path_next[][8])
{
    int x = 0, y = 0, n = 0 ;
    int num = 0 ;
    struct Coor now_coor ;
    for(y = 0; y < 8; y++) {
        now_coor.y = y ;
        for(x = 0; x < 8; x++) {
            now_coor.x = x ;
            n = now_coor.y*8 + now_coor.x ;
            for(num = 1; num < 9; num++) {
                /*按顺时针顺序将后继路径权值表进行初始化*/
                if(coor_get(now_coor, &path_next[n][num-1], num) < 1) {
                    return -1 ;
                }
            }
        }
    }
    return 0 ;
}

/*根据坐标，计算其出度,若出度为0则返回9, 若该坐标越界，则返回10*/
int path_next_num(struct Coor now_coor, int map[][8])
{
    int num = 0 ;
    int count = 0 ;
    struct Coor next_coor ;
    /*若输入的坐标已经越界，则直接返回9*/
    if(coor_judge(now_coor) < 0 || map[now_coor.y][now_coor.x] > 0) {
        return 10 ;
    }
    for(count = 1; count < 9; count++) {
        /*依次获取该坐标的八个后继路径*/
        if(coor_get(now_coor, &next_coor, count) < 1) {
            return -1 ;
        }
        else {
            /*从后继路径中筛选出可选路径*/
            if(coor_judge(next_coor) > 0) {
                if(map[next_coor.y][next_coor.x] <= 0) {
                    num++ ;
                }
            }
        }
    }
    if(num == 0) {
        return 9 ;
    }
    else {
        return num ;
    }
}

/*对某坐标所对应的后继路径出度表进行排序，只对序号位于num之后的排序*/
int path_next_sort(struct Coor now_coor, int num, struct Coor path_next[][8], int map[][8])
{
    /*将二维坐标化为一维坐标*/
    int n = now_coor.y*8 + now_coor.x ;
    int count = 0 ;
    int flag = 1 ;
    int temp[8], num_temp = 0 ;
    struct Coor temp_coor ;
    /*遍历数组，将各个元素的出度保存到临时数组中*/
    for(count = num; count < 8; count++) {
        temp[count] = path_next_num(path_next[n][count], map) ;    
    }
    while(flag == 1) {
        flag = 0 ;
        for(count = num; count < 7; count++) {
            /*根据权值排序*/
            if( temp[count] > temp[count+1] ) {
                num_temp = temp[count] ;
                temp[count] = temp[count+1] ;
                temp[count+1] = num_temp ;
                memcpy(&temp_coor, &path_next[n][count], sizeof(struct Coor)) ;
                memcpy(&path_next[n][count], &path_next[n][count+1], sizeof(struct Coor)) ;
                memcpy(&path_next[n][count+1], &temp_coor, sizeof(struct Coor)) ;
                flag = 1 ;
            }
        }
    }
    return 0 ;
}

void map_print(int map[][8])
{
    int m, n ;printf("打印：\n") ;
    for(m = 0; m < 8; m++) {
        for(n = 0; n < 8; n++) {
            printf("%d\t", map[m][n]) ;
        }
        printf("\n") ;
    }
}

int Game(struct Coor first_coor)
{
    struct Coor now_coor = first_coor ;      /*当前位置*/
    struct Coor next_coor ;     /*下一步位置*/
    int map[8][8] ={0} ;     /*棋盘*/
    struct Coor path_stack[64] ;    /*路径栈*/
    struct Coor path_next[64][8] ;  /*后继路径出度表*/
    int step_count = 0 ;     /*记录步数*/
    int num = 0 ;
    int count = 0 ;

    map[first_coor.y][first_coor.x] = step_count + 1 ; map_print(map) ;
    /*初始化后继路径出度表*/
    path_next_init(path_next) ;

    while(step_count < 64) {
        if(step_count == 63) {
            map_print(map) ;
            sleep(1) ;
        }
        num = now_coor.y*8 + now_coor.x ;
        /*对当前坐标的出度表进行排序*/
        path_next_sort(now_coor, 0, path_next, map) ;

        if(path_next_num(path_next[num][0], map) < 10) {
            /*当前步入栈*/
            memcpy(&path_stack[step_count++], &path_next[num][0], sizeof(struct Coor)) ;
            /*更新当前步*/
            memcpy(&now_coor, &path_next[num][0], sizeof(struct Coor)) ;
            /*更新棋盘*/
            map[ path_next[num][0].y ][ path_next[num][0].x ] = step_count + 1 ;
        }
        else {
            while(step_count > 0) {
                /*退栈*/
                memcpy(&next_coor, &path_stack[--step_count], sizeof(struct Coor)) ;
                /*更新棋盘*/
                map[next_coor.y][next_coor.x] = 0 ;
                /*根据错误路径检查上一步的出度表*/
                num = path_stack[step_count-1].y*8 + path_stack[step_count-1].x ;
                for(count = 0; count < 8; count++) {
                    if(path_next[num][count].y == next_coor.y && path_next[num][count].x == next_coor.x ) {
                        break ;
                    }
                }
                if(count < 8) {
                    path_next_sort(path_stack[step_count-1], count + 1, path_next, map) ;
                    if(path_next_num(path_next[num][count+1], map) < 9) {
                        /*当前步入栈*/
                        memcpy(&path_stack[step_count++], &path_next[num][count+1], sizeof(struct Coor)) ;
                        /*更新棋盘*/
                        map[ path_next[num][count+1].y ][ path_next[num][count+1].x ] = step_count + 1 ;
                        break ;
                    }
                }
            }
        }
    }
    printf("完成！！\n") ;

    return 0 ;
}

int main(void)
{
    struct Coor first_coor = {0, 0} ;
    printf("请输入起始坐标(x,y)：") ;
    scanf("%d,%d", &first_coor.x, &first_coor.y) ;
    Game(first_coor) ;
    return 0 ;
}
