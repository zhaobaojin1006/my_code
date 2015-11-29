/*************************************************************************
	> File Name: decompress_test.cpp
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年11月25日 星期三 21时53分56秒
 ************************************************************************/
#include<iostream>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include"HuffmanTree.h"

#define NAMEMAX 256
#define SIZEMAX 256

using namespace std;

int filemessage(HuffmanTree &T, int fd_from, long &filesize) ;

int decompress(HuffmanTree &T, int fd_from, int fd_to, long filesize) ;

//整型数转换为二进制字符串
int myctob(unsigned char ch, char code[]) ;

int main(void)
{
    long filesize = 0 ;
    int fd_from = 0 ;
    int fd_to = 0 ;
    char filename_from[NAMEMAX] ;
    char filename_to[NAMEMAX] ;
    HuffmanTree T ;

    std :: cout << "please input the file from : " ;
    std :: cin >> filename_from ;
    if((fd_from = open(filename_from, O_RDONLY)) < 0) {
        std :: cout << "file open failed " << std :: endl ;
        return -1 ;
    }
    //读取文件头信息
    filemessage(T, fd_from, filesize) ;

    std :: cout << "please input the file to " ;
    std :: cin >> filename_to ;
    if((fd_to = open(filename_to, O_WRONLY | O_CREAT, 0744)) < 0) {
        std :: cout << "file open failed " << std :: endl ;
        return -1 ;
    }
    //解压文件
    decompress(T, fd_from, fd_to, filesize) ;

    close(fd_from) ;
    close(fd_to) ;
    return 0 ;
}

//字符值转换为二进制字符串
int myctob(unsigned char ch, char code[])
{
    //位运算的基准数，二进制为10000000
    unsigned char temp = 128 ;
    unsigned char flag = 0 ;
    int count = 0 ;
    //将字符的8位全部对比
    for(count = 0; count < 8; count++) {
        flag = ch & temp ;
        if(flag > 0) {  //与运算结果大于0说明该位为1
            code[count] = '1' ;
        }
        else {
            code[count] = '0' ;
        }
        temp = temp >> 1 ;  //temp右移一位
    }
    code[count] = '\0' ;
    return 1 ;
}


//读取文件头信息
int filemessage(HuffmanTree &T, int fd_from, long &filesize)
{
    int n = 0 ;
    int num = 0 ;
    unsigned char ch = 0 ;

    //读取文件头信息
    if(read(fd_from, &filesize, sizeof(long)) < 0) {
        return -1 ;
    }
    std :: cout << filesize << std :: endl ;
    //读取文件字符频率并输入哈夫曼树类
    for(n = 0; n < 256; n++) {
        if(read(fd_from, &ch, sizeof(unsigned char)) < 1) {
            return -1 ;
        }
        if(read(fd_from, &num, sizeof(int)) < 4) {
            return -1 ;
        }
        std :: cout << "the ch = " << (int)ch << std :: endl ;
        std :: cout << "the num = " << num << std ::endl ;
        //输入哈夫曼树类
        T.weight_set(ch, num) ;
    }
    //生成哈夫曼编码
    if(T.code_set() < 0) {
        return -1 ;
    }

    T.tree_show() ;
    return 1 ;
}

int decompress(HuffmanTree &T, int fd_from, int fd_to, long filesize)
{
    unsigned char ch = 0 ;
    unsigned char c = 0 ;
    char code[SIZEMAX] ;
    char code_temp[9] ;
    int step = 0 ;
    int count = 0 ;
    int n = 0 ;
    long num = 0 ;  //记录当前翻译了多少字符

    std :: cout << "filesize = " << filesize << std :: endl ;
    //每次从源文件中读取字符进行翻译
    while(read(fd_from, &ch, sizeof(unsigned char)) == 1) {
        //获取二进制字符串
        myctob(ch, code_temp) ;
       
        //对二进制字符串进行译码
        for(count = 0; count < 8; count++) {
            code[step++] = code_temp[count] ;
            code[step] = '\0' ;
            if(T.code_change(code, c) > 0) { 
                //译码成功
                if(write(fd_to, &c, sizeof(unsigned char)) < 0) {   //将字符c写入目标文件
                    return -1 ;
                }
                if(++num == filesize) {   //翻译的字符数等于源文件字符数
                    std :: cout << "num = " << num << std :: endl ;
                    break ;
                }
                step = 0 ;  //code重新开始记录编码
            }
        }
    }
    return 1 ;
}
