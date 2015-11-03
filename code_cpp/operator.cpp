/*************************************************************************
	> File Name: operator.cpp
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年11月03日 星期二 11时06分54秒
 ************************************************************************/
#include<iostream>

#define STR_MAX 100

using namespace std;

class STRing {
    private :
    char str[STR_MAX] = {0} ;

    public :
    STRing(char string[]) ;
    STRing operator +(STRing &temp) ;
    void print(void) ;
} ;

STRing :: STRing(char string[])
{
    int i = 0 ;
    while(string[i] != '\0') {
        str[i] = string[i] ;
        i++ ;
    }
}

STRing STRing :: operator +(STRing &temp)
{
    int i = 0, j = 0 ;
    char str_temp[2*STR_MAX+2] ;
    while(str[i] != '\0') {
        str_temp[i] = str[i] ;
        i++ ;
    }

    do {
        str_temp[i++] = temp.str[j++] ;
    }while(temp.str[j] != '\0') ;
    return STRing(str_temp) ;
}

void STRing :: print(void)
{
    std :: cout << str << std :: endl ;
}

void func(const int &i)
{
    i++ ;
}

int main(void)
{int i = 0 ;
    STRing s1("1231465"), s2("777777") ;
    STRing s3("0") ;
    s1.print() ;
    s2.print() ;
    s3 = s1 + s2 ;
    s3.print() ;
    func(i) ;
    return 0 ;
}
