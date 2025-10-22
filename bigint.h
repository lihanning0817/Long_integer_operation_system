//bigint.h头文件，包括存储长整数的数据结构声明和长整数处理的相关函数的声明；
//1. 长整数数据结构（结构体）
//2. 各类函数的声明
#ifndef BIGINT_H
#define BIGINT_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//链表节点
typedef struct LNode{
    int digit;//当前位
    struct LNode *next;  //下一位

}LNode,*LinkList;

//长整数结构体
typedef struct{
    LNode *head;
    int sign;//正负号
    int length;//数长度
}BigInt;

//函数

//开辟新结点
LinkList create_LNode(int digit);

//插入高位（表头）
void insertHead(BigInt*num,int digit);

//插入低位（表尾）
void insertTail(BigInt*num,int digit);

//释放长整数
void freeBigInt(BigInt*num);

//释放并清空长整数
void clearBigInt(BigInt*num);

//比较两个长整数
int compareBigInt(BigInt *a,BigInt *b);

//主要函数

//初始化
void InitBigInt(BigInt *num);

//读取
void readBigInt(BigInt *num);

//加
void addBigInt(BigInt *a,BigInt *b,BigInt *result);

//减
void subBigInt(BigInt *a,BigInt *b,BigInt *result);

//除
void divBigInt(BigInt *a,BigInt b,BigInt *quotient,BigInt *remainder);

//输出
void printbigint(BigInt num);

#endif BIGINT_H