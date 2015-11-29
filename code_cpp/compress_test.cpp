/*************************************************************************
	> File Name: compress_test.cpp
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年11月24日 星期二 11时12分00秒
 ************************************************************************/
#include<iostream>
#include<stdio.h>
#include"HuffmanTree.h"
#define NAMEMAX 256
#define CODEMAX 256
using namespace std;

//整型数转换为二进制字符串
int myctob(unsigned char ch, char code[]) ;

int codesave(HuffmanTree T, int fd_to) ;

int compress(HuffmanTree T, char filename_from[], char filename_to[]) ;     //对文件内容进行编码替换

int main(void)
{
    char filename_from[NAMEMAX] ;
    char filename_to[NAMEMAX] ;
    HuffmanTree tree ;

    std :: cout << "please input the filename : " ;
    std :: cin >> filename_from ;
    //根据文件产生哈夫曼树
    if(tree.weight_set(filename_from) < 0) {
        return -1 ;
    }
    //生成哈夫曼编码
    if(tree.code_set() < 0) {
        return -1 ;
    }
    std :: cout << "please input the compressed filename : " ;
    std :: cin >> filename_to ;
    compress(tree, filename_from, filename_to) ;
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
    std :: cout << code << std :: endl ;
    return 1 ;
}


int codesave(HuffmanTree T, int fd_to)
{
    int num = 0 ;
    int count = 0 ;
    unsigned char ch = 0 ;
    for(count = 0; count < 256; count++) {
        ch = (unsigned char)count ;
        num = T.weight_get(ch) ;
        std :: cout << (int)ch << " : " << num << std :: endl ;
        //存储字符
        if(write(fd_to, &ch, sizeof(unsigned char)) < 0) {
            return -1 ;
        }
        //存储权值
        if(write(fd_to, &num, sizeof(int)) < 0) {
            return -1 ;
        }
        std :: cout << "ch = " << ch << "num = " << num << std :: endl ;
    }
    return 1 ;
}

int compress(HuffmanTree T, char filename_from[], char filename_to[])
{
    long filesize = 0 ;
    int fd_from = 0 ;
    int fd_to = 0 ;
    unsigned char step = 0 ;
    int count = 0 ;

    unsigned char ch = 0 ;
    unsigned char temp = 0 ;
    char code[CODEMAX] ;
    //打开源文件
    if((fd_from = open(filename_from, O_RDONLY)) < 0) {
        std :: cout << "file open failed " << std :: endl ;
    }
    //打开目标文件
    if((fd_to = open(filename_to, O_WRONLY | O_CREAT, 0744)) < 0) {
        std :: cout << "file open failed " << std :: endl ;
    }
    //预留存储源文件长度信息的位置 long型
    if(write(fd_to, &filesize, sizeof(long)) < 0) {
        return -1 ;
    }
    //添加哈夫曼树结点信息
    if(codesave(T, fd_to) < 0) {
        return -1 ;
    }
    //每次从源文件读取一个字符进行编码
    while(read(fd_from, &ch, sizeof(char)) == 1) {
        count = 0 ;
        T.code_get(ch, code) ;
        filesize++ ;   //文件长度加1

        while(code[count] != '\0') {
            //根据哈夫曼编码对temp变量进行位操作
            if(code[count] == '0') {
                temp = temp << 1 ;   //遇到'0'就将temp左移一位
            }
            else if(code[count] == '1') {
                temp = temp << 1 ;     //先将temp左移一位
                temp = temp|1 ;    //temp|1，相当于将temp末位变1
            }
            count++ ;
            //满8位则将字符temp存入目标文件，并将step归0
            if(++step == 8) {
                if(write(fd_to, &temp, sizeof(char)) < 1) {
                    return -1 ;
                }
                step = 0 ;
            }
        }
    }
    if(step != 0) {     //若最后的字符编码后不足8位，则在后面补0，并记录实际编码的位数
        while(++step <= 8) {
            temp = temp << 1 ;
        }
        if(write(fd_to, &temp, sizeof(char)) < 1) {
            return -1 ;
        }
    }
    //将文件指针移到文件开始位置
    if(lseek(fd_to, 0, SEEK_SET) < 0) {
        return -1 ;
    }
    if(write(fd_to, &filesize, sizeof(long)) < 0) {
        return -1 ;
    }
    //文件指针移回文件尾
    if(lseek(fd_to, 0, SEEK_END) < 0) {
        return -1 ;
    }
 
    std :: cout << "filesize = " << filesize << std :: endl ;
    close(fd_from) ;
    close(fd_to) ;
}
