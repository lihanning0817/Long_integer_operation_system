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
void insertHead(BigInt*);

//插入低位（表尾）






