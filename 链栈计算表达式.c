/*************************************************************************
	> File Name: 链栈计算表达式.c
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2015年05月26日 星期二 19时34分54秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
struct n/*数字栈*/
{
	double num;
	struct n *next;
};
struct c/*字符栈*/
{
	char ch;
	struct c *next;
};
struct c *chpush(struct c *p,char ch)/*字符栈压栈函数*/
{
	struct c *pnew=NULL;
	pnew=(struct c *)malloc(sizeof(struct c));
	pnew->ch=ch;
	pnew->next=p;
	return pnew;
}
struct c *chpop(struct c *p)/*字符栈退栈函数，返回退栈后的栈顶节点*/
{
	struct c *pnext=NULL;
	pnext=p->next;
	free(p);
	return pnext;
}
struct n *numpush(struct n *p,double num)/*数字栈压栈函数*/
{
	struct n *pnew=NULL;
	pnew=(struct n *)malloc(sizeof(struct n));
	pnew->num=num;
	pnew->next=p;
	return pnew;
}
struct n *numpop(struct n *p)/*数字栈退栈函数，返回退栈后的栈顶节点*/
{
	struct n *pnext=NULL;
	pnext=p->next;
	free(p);
	return pnext;
}
double calculation(double a,char c,double b)/*计算函数*/
{
	double num=0;
	switch(c){
		case '+':
			num=a+b;
			break;
		case'-':
			num=a-b;
			break;
		case'*':
			num=a*b;
			break;
		case'/':
			num=a/b;
			break;
		case'^':
			num=pow(a,b);
			break;
	}
	return num;
}
int priority(char c)/*优先级函数，返回运算符的优先级*/
{
	int l;
	switch(c){
		case'+':
			l=0;
			break;
		case'-':
			l=0;
			break;
		case'*':
			l=1;
			break;
		case'/':
			l=1;
			break;
		case'^':
			l=2;
			break;
		case'(':
			l=-1;
			break;
		case')':
			l=-1;
			break;
	}
	return l;
}
double operation(char * a)/*操作函数，每次遇到数字直接压栈，遇到运算符先判断优先级*/
{
	struct n *numstacktop=NULL;/*数字栈*/struct n *numstack=NULL;
	struct c *chstacktop=NULL;/*字符栈*/
	char b[10];
	int i=0,n=0;
	double num1=0,num2=0,num=0;
	while(a[i]-'\0')
	{i++;}/*遍历至输入串结尾处*/
	a[i++]='!';/*在输入串末尾加上'！'作为结束标志*/
	a[i]='\0';/*重新加上字符串结束标志*/
	i=0;printf("%s\n",a);
	while(a[i]-'\0')
	{
		if(a[i]>='0'&&a[i]<='9')/*如果接收到的是数字*/
		{printf("N");
			b[n++]=a[i];
			b[n]='\0';/*每次在b数字串最后标注结束符号*/
		}
		else
		{
			if(n)/*若接收到非数字字符时，n不等于0，说明b数字串中有数字，先将其转换后压入数字栈*/
			{
				numstacktop=numpush(numstacktop,atof(b));printf("压入操作数为为：%lf\n：",numstacktop->num);numstack=numstacktop;
				while(numstack)
				{printf("数字栈中元素为：%lf\n",numstack->num);numstack=numstack->next;}
				n=0;/*b数字串归0*/
			}
			printf("\n接收字符为：%c\n",a[i]);
			if(!(a[i]-'+')||!(a[i]-'-')||!(a[i]-'*')||!(a[i]-'/')||!(a[i]-'^')||!(a[i]-'('))/*如果接收到的是运算符*/
			{
				if(!(a[i]-'('))/*若当前字符为前括号，直接压入字符栈，前括号优先级最低，在遇到其他运算符时不会被运算*/
				{chstacktop=chpush(chstacktop,a[i]);}
				else if(chstacktop)/*如果字符栈栈顶已有运算符，则比较当前运算符与栈顶运算符优先级*/
				{	
					if(priority(a[i])>priority(chstacktop->ch))/*若当前运算符优先级大于栈顶运算符优先级，则直接压入字符栈*/
					{chstacktop=chpush(chstacktop,a[i]);}
					else/*若当前运算符优先级小于或等于栈顶运算符，则从数字栈取出两个数，以栈顶运算符计算后存入数字栈*/
					{
						while((chstacktop)&&(priority(a[i])<=priority(chstacktop->ch)))/*当前运算符优先级大于栈顶或字符栈中全部退栈时，退出循环*/			
						{				 		
							num2=numstacktop->num;		
							numstacktop=numpop(numstacktop);						
							num1=numstacktop->num;					
							numstacktop=numpop(numstacktop);						
							num=calculation(num1,chstacktop->ch,num2);			 			
							numstacktop=numpush(numstacktop,num);/*将计算后的数压入数字栈*/printf("计算后结果压入字符栈%lf",numstacktop->num);
							chstacktop=chpop(chstacktop);
						}					
						chstacktop=chpush(chstacktop,a[i]);/*计算完后将当前运算符压入字符栈*/				
					}			
				}		
				else	
				{chstacktop=chpush(chstacktop,a[i]);}/*若字符栈栈顶未存储运算符，将当前运算符直接压入字符栈*/	
			}
			else if(!(a[i]-')'))/*若当前运算符为后括号*/
			{
				while(chstacktop&&chstacktop->ch- '(')/* 向栈底退栈到前括号时结束，将括号中所有表达式计算并将结果压入数字栈*/
				{
					num2=numstacktop->num;		
					numstacktop=numpop(numstacktop);						
					num1=numstacktop->num;					
					numstacktop=numpop(numstacktop);						
					num=calculation(num1,chstacktop->ch,num2);			 			
					numstacktop=numpush(numstacktop,num);/*将计算后的数压入数字栈*/
					chstacktop=chpop(chstacktop);
				}					
				if(!chstacktop)/*若退栈到栈底也未找到前括号，说明表达式错误*/
				{return 0;}
				if(!(chstacktop->ch-'('))/*退栈至前括号处，丢弃前括号与后括号*/
				{chstacktop=chpop(chstacktop);}
			}
			else if(!(a[i]-'!'))/*若结束字符是'！'标志，则将字符栈中所有字符全部退栈计算*/
			{	
				while(chstacktop)
				{
					num2=numstacktop->num;	
					numstacktop=numpop(numstacktop);		
					num1=numstacktop->num;	
					numstacktop=numpop(numstacktop);		
					numstacktop=numpush(numstacktop,calculation(num1,chstacktop->ch,num2));	
					chstacktop=chpop(chstacktop);		
				}	
			}
			else/*若接收到其他字符，表达式出错*/
			{printf("出错");return 0;}
		}	
		i++;
	}
	return numstacktop->num;
}
int main(void)
{
	char a[20];
	printf("请输入表达式：");
	scanf("%s",&a);
	printf("%lf",operation(a));
	return 0;
}
