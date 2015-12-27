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

int func(vector<int> &temp, vector<int> :: iterator index)
{
    if(index != temp.end()) {
        func(temp, index+1) ;
    }
    else {
        return 0 ;
    }
    cout << *index << endl ;
    return 0 ;
}

int main(int argc, char* argv[])
{
    vector<int> a({1,2,3,4,5,6,7,8,9,}) ;
    func(a, a.begin()) ;
    return 0 ;
}
