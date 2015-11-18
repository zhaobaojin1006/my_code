/*************************************************************************
	> File Name: HuffmanTree_test.cpp
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年11月18日 星期三 11时11分06秒
 ************************************************************************/
#include<iostream>
#include<stdlib.h>
#include<string.h>
using namespace std;

struct HuffmanTree_Node {   //哈夫曼树结点结构体
    int weight ;
    int child_left;
    int child_right ;
    int parent ;
} ;

int HuffmanTree_init(struct HuffmanTree_Node *HuffmanTree_table, int n)
{
    int count = 0 ;
    while(count < n) {
        std :: cout << "please input the elem's weight" << std :: endl ;
        std :: cin >> HuffmanTree_table[ count ].weight ;
        HuffmanTree_table[ count ].child_left = -1 ;
        HuffmanTree_table[ count ].child_right = -1 ;
        HuffmanTree_table[ count ].parent = -1 ;
        count++ ;
    }
    while(count < 2*n-1) {
        HuffmanTree_table[ count ].child_left = -1 ;
        HuffmanTree_table[ count ].child_right = -1 ;
        HuffmanTree_table[ count ].parent = -1 ;
        HuffmanTree_table[ count ].weight = 0 ;
        count ++ ;
    }
}

void HuffmanTree_print(struct HuffmanTree_Node *HuffmanTree_table, int n)
{
    int i = 0 ;
    while(i < 2*n-1) {
        std :: cout << "HuffmanTree_table[" << i << "]:weight = " << HuffmanTree_table[i].weight << "," ;
        std :: cout << "parent = " << HuffmanTree_table[i].parent << "," ;
        std :: cout << "child_left = " << HuffmanTree_table[i].child_left << "," ;
        std :: cout << "child_right = " << HuffmanTree_table[i].child_right << "," << std :: endl ;
        i++ ;
    }
}

int HuffmanTree_sort(struct HuffmanTree_Node *HuffmanTree_table, int n)    //根据权值排序
{
    int i = 0 ;
    char flag = '1' ;
    struct HuffmanTree_Node node_test = {0} ;
    while(flag == '1') {
        flag = '0' ;
        i = 0 ;
        while(i < n-1) {
            if(HuffmanTree_table[i].weight == 0) {  //叶子结点不能出现权值为0的情况
                return -1 ;
            }
            if(HuffmanTree_table[ i ].weight > HuffmanTree_table[ i+1 ].weight) {
                memcpy(&node_test, &HuffmanTree_table[ i ], sizeof(struct HuffmanTree_Node)) ;
                memcpy(&HuffmanTree_table[ i ], &HuffmanTree_table[ i+1 ], sizeof(struct HuffmanTree_Node)) ;
                memcpy(&HuffmanTree_table[ i+1 ], &node_test, sizeof(struct HuffmanTree_Node)) ;
                flag = '1' ;
            }
            i++ ;
        }
    }
    return 1 ;
}

int HuffmanTree_min(struct HuffmanTree_Node *HuffmanTree_table, int n)
{
    int t = 0 ;
    while(t < n) {
        if(HuffmanTree_table[ t ].parent == -1) {
            return t ;  //找到权值最小的待选结点，返回下标
        }
        t++ ;
    }
}

int HuffmanTree_set(struct HuffmanTree_Node *HuffmanTree_table, int n)
{
    int count = 0 ;
    int s1 = 0, s2 = 0 ;    //森林中最小两棵树的根结点下标
    HuffmanTree_sort(HuffmanTree_table, n) ;
    for(count = n; count < 2*n-1; count++) {
        s1 = HuffmanTree_min(HuffmanTree_table, count) ;    //在新结点以前的所有结点中找出权值最小的待选结点
        HuffmanTree_table[ s1 ].parent = count ;  //将新结点的下标赋值给s1的parent
        s2 = HuffmanTree_min(HuffmanTree_table, count) ;    //找出除s1外第二个权值最小的待选结点
        HuffmanTree_table[ s2 ].parent = count ;  //将新结点的下标赋值给s2的parent

        //更新新结点数据
        HuffmanTree_table[ count ].weight = HuffmanTree_table[ s1 ].weight + HuffmanTree_table[ s2 ].weight ;
        HuffmanTree_table[ count ].child_left = s1 ;
        HuffmanTree_table[ count ].child_right = s2 ;
    }
    return 0 ;
}

int main(void)
{
    int n = 0 ;
    struct HuffmanTree_Node *HuffmanTree_table = NULL ;
    std :: cout << "please input the HuffmanTree's elem num" <<std ::endl ;
    std :: cin >> n ;
    HuffmanTree_table = (struct HuffmanTree_Node *)malloc((2*n-1)*sizeof(struct HuffmanTree_Node)) ;    //申请动态空间
    HuffmanTree_init(HuffmanTree_table, n) ;
    HuffmanTree_sort(HuffmanTree_table, n) ;
    HuffmanTree_set(HuffmanTree_table, n) ;
    HuffmanTree_print(HuffmanTree_table, n) ;
    return 0 ;
}
