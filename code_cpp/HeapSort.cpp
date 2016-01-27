/*************************************************************************
	> File Name: HeapSort.cpp
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2016年01月27日 星期三 13时54分15秒
 ************************************************************************/
#include<iostream>
#include<vector>
using namespace std;

/*对以front为根结点的完全二叉树进行大顶堆调整
* 末尾结点为rear*/
void HeapAdjust(int List[], int front, int rear)
{
    /*f为当前遍历结点，i为其孩子结点*/
    int f = front ;
    int i = 2*front ;
    int temp = List[front] ;

    for( ; i <= rear; i = i*2) {
        /*将两个孩子结点中较大的结点赋给i*/
        if(i < rear && List[i] < List[i+1]) {
            i++ ;
        }
        /*把根结点与其较大的孩子结点进行比较
        * 根结点大则说明符合性质，否则将根结
        * 点与较大孩子结点进行交换*/
        if(temp >= List[i]) {
            break ;
        }
        else {
            List[f] = List[i] ;
            /*进入较大孩子结点，持续向下筛选*/
            f = i ;
        }
    }
    /*插入根结点*/
    List[f] = temp ;
}

/*对长度为num的序列进行堆排序*/
void HeapSort(int List[], int num)
{
    int step = num/2 ;  /*根据完全二叉树的性质，结点总数的1/2正好是其末尾叶子结点的双亲结点编号*/
    int temp = 0 ;

    /*从最末尾的子树开始向上进行堆调整
    * 将初始序列调整为大顶堆*/
    for( ; step > 0; step--) {
        HeapAdjust(List, step, num) ;
    }

    /*依次取出堆顶*/
    for(step = num+1; step > 0; step--) {
        /*将堆顶元素与末尾元素交换*/
        temp = List[step] ;
        List[step] = List[1] ;
        List[1] = temp ;

        /*对剩余元素进行堆调整*/
        HeapAdjust(List, 1, step-1) ;
    }
}

int main(void)
{
    int j = 0 ;
    int i[10] = {0, 90, 80, 70, 60, 50, 40, 30, 20, 10} ;
    HeapSort(i, 9) ;
    for(j = 1; j <10; j++) {
        std :: cout << i[j] << std :: endl ;
    }
    return 0 ;
}
