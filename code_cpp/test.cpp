/*************************************************************************
	> File Name: test.cpp
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年11月18日 星期三 21时03分31秒
 ************************************************************************/
#include<iostream>
#include<stdio.h>
#include<cctype>
#include<vector>
#include<initializer_list>
using namespace std ;
using std :: vector ;
using std :: cin ;
using std :: cout ;
using std :: endl ;
using std :: string ;

int &func(int *a, int index)
{
    return a[index] ;
}

int main(int argc, char* argv[])
{
    int a[10] ;
    for(int i = 0; i < 10; i++) {
        func(a, i) = i ;
        cout << a[i] << endl ;
    }
    return 0 ;
}
