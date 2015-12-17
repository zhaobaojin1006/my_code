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
using namespace std ;
using std :: vector ;
using std :: cin ;
using std :: cout ;
using std :: endl ;
using std :: string ;

int main(int argc, char* argv[])
{
    string str ;
    string str_temp ;
    vector<string> v ;
    int count = 0 ;
    for(count = 0; count < argc; count++) {
        str = argv[count] ;
        v.push_back(str) ;
    }
    for(count = 0; count < argc; count++) {
        str = str + v[count] ;
    }
    cout << str << endl ;
    return 0 ;
}
