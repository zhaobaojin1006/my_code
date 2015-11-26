/*************************************************************************
	> File Name: HuffmanTree.h
	> Author: zhaobaojin 
	> BLOG: blog.csdn.net/zhaobaojin1006 
	> Mail: 1279742553@qq.com 
	> Created Time: 2015年11月23日 星期一 17时10分40秒
 ************************************************************************/
#ifndef _HUFFMANTREE_H
#define _HUFFMANTREE_H
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<string.h>
#define ELEM_MAX 256

//哈夫曼树结点
struct HuffmanTree_Node {
    int weight ;    //权值
    int parent ;    //父结点
    int Lchild ;    //左孩子结点
    int Rchild ;    //右孩子结点
} ;

/******************************哈夫曼树类***************************************/
class HuffmanTree {
    private :
    int num ;   //哈夫曼树叶子结点个数
    struct HuffmanTree_Node Tree_Table[ 2*ELEM_MAX-1 ] ;   //哈夫曼树表
    char Code_Table[ELEM_MAX][ELEM_MAX] ;   //字符编码对照表

    public :
    HuffmanTree() ;     //构造函数
    int weight_set(char filename[]) ;     //扫描文件，获得字符频率表
    void weight_set(unsigned char ch, int n) ;    //将字符ch的权值置为num
    int weight_get(unsigned char ch) ;  //返回ch的权值
    int tree_min(void) ;   //返回n之前的结点中权值最小的一个结点的下标
    int tree_set(void) ;    //建立哈夫曼树
    void tree_show(void) ;
    int code_set(void) ;
    int code_get(unsigned char &ch, char Code[]) ;
} ;

//哈夫曼树类构造函数
HuffmanTree :: HuffmanTree()
{
    int count = 0 ;
    num = 0 ;
    for(count = 0; count < 2*ELEM_MAX-1; count++) {
        Tree_Table[count].weight = 0 ;
        Tree_Table[count].parent = -1 ;
        Tree_Table[count].Lchild = -1 ;
        Tree_Table[count].Rchild = -1 ;
    }
    for(count = 0; count < ELEM_MAX; count++) {
        Code_Table[count][0] = '\0' ;
    }
}

//获取文件中字符权值表
int HuffmanTree :: weight_set(char filename[])
{
    unsigned char ch = 0 ;
    int fd = 0 ;
    int filelength = 0 ;
    std :: cout << filename << std :: endl ;
    //打开文件
    if((fd = open(filename, O_RDONLY)) < 0) {
        std :: cout << "file open failed" << std :: endl ;
        return -1 ;
    }
    //扫描文件
    while(read(fd, &ch, sizeof(char)) == 1) {
        if(Tree_Table[ch].weight == 0) {    //记录文件中有多少种字符
            num++ ;
        }
        Tree_Table[ch].weight++ ;   //字符权值加1
        std :: cout << ch << " : " << Tree_Table[ch].weight << std :: endl ;
        filelength++ ;  //文件长度增加1
    }
    close(fd) ;
    return filelength ;
}

//将字符ch的权值置为num
void HuffmanTree :: weight_set(unsigned char ch, int n)
{
    Tree_Table[ch].weight = n ;
    num++ ;
}

//返回字符ch的权值
int HuffmanTree :: weight_get(unsigned char ch)
{
    return Tree_Table[ch].weight ;
}

int HuffmanTree :: tree_min(void)
{
    int min = 99999999 ;    //假定字符最大权值
    int min_num = -1 ;
    int count = 0 ;
    for(count = 0; count < 2*ELEM_MAX-1; count++) {
        if(Tree_Table[count].weight > 0) {  //只对权值大于0的元素操作
            if((Tree_Table[count].parent == -1) && (Tree_Table[count].weight < min)) {
                min = Tree_Table[count].weight ;
                min_num = count ;   //记录当前最小权值元素的下标
            }
        }
    }
    return min_num ;
}

//建立哈夫曼树
int HuffmanTree :: tree_set(void)
{
    int num1 = -1, num2 = -1 ;
    int count = ELEM_MAX ;

    for(count = ELEM_MAX; count < ELEM_MAX+num-1; count++) {
        //获取待选结点中权值最小的两个,将其作为当前结点的左右子树
        num1 = tree_min() ;
        Tree_Table[num1].parent = count ;
        num2 = tree_min() ;
        Tree_Table[num2].parent = count ;
        //更新当前子树
        Tree_Table[count].weight = Tree_Table[num1].weight + Tree_Table[num2].weight ;
        Tree_Table[count].Lchild = num1 ;
        Tree_Table[count].Rchild = num2 ;
    }
}

//打印哈夫曼树表
void HuffmanTree :: tree_show(void)
{
    int ch = 0 ;
    for(ch = 0; ch < 2*ELEM_MAX-1; ch++) {
        std :: cout << ch << std :: endl ;
        if(Tree_Table[ch].weight > 0) {
            std :: cout << ch << " : " << std :: endl ;
            std :: cout << "weight : " << Tree_Table[ch].weight << std :: endl ;
            std :: cout << "parent : " << Tree_Table[ch].parent << std :: endl ;
            std :: cout << "Lchild : " << Tree_Table[ch].Lchild << std :: endl ;
            std :: cout << "Rchild : " << Tree_Table[ch].Rchild << std :: endl ;
        }
    }
}

//产生哈夫曼编码
int HuffmanTree :: code_set(void)
{
    int count = 0 ;
    char Stack[ELEM_MAX] ;
    int Stack_Top = -1 ;
    int temp = 0 ;
    int now = 0 ;
    int now_parent = 0 ;

    //建立哈夫曼树
    if(tree_set() < 0) {
        return -1 ;
    }

    for(count = 0; count < ELEM_MAX; count++) {
        now = -1 ;
        now_parent = -1 ;
        temp = 0 ;
        if(Tree_Table[count].weight > 0) {
            now = count ;
            now_parent = Tree_Table[count].parent ;
            while(now_parent != -1) {
                if(Tree_Table[now_parent].Lchild == now) {
                    Stack[++Stack_Top] = '0' ;
                }
                else {
                    Stack[++Stack_Top] = '1' ;
                }
                now = Tree_Table[now].parent ;
                now_parent = Tree_Table[now_parent].parent ;
            }
            while(Stack_Top != -1) {
                Code_Table[count][temp++] = Stack[Stack_Top--] ;
            }
            Code_Table[count][temp] = '\0' ;
            std :: cout << "Code_Table[" << count << "] : " << Code_Table[count] << std :: endl ;
        }
    }
}

//获取字符ch的哈夫曼编码
int HuffmanTree :: code_get(unsigned char &ch, char Code[])
{
    if(Tree_Table[ch].weight <= 0) {
        return -1 ;
    }
    else {
        strcpy(Code, Code_Table[ch]) ;
        return 1 ;
    }
}

#endif

