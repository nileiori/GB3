/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*
*           (2) This file and its dependencies requires IAR v6.20 or later to be compiled.
*
*********************************************************************************************************
*/
#ifndef __QUEUE_H
#define __QUEUE_H


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "stm32f10x.h"

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/
typedef u8 ElementType;
struct QueueRecord
        {
            int Capacity;
            int Front;
            int Rear;
            int Size;
            ElementType *Array;
        };
typedef struct QueueRecord *    Queue;



/*
*********************************************************************************************************
*                                             define
*********************************************************************************************************
*/
#define MIN_QUEUE_SIZE  ( 10 )
#define MAX_QUEUE_SIZE  ( 2000 )

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                          ALGQueueIsEmpty()
*
* Description : �ж�һ�������Ƿ�Ϊ��
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
int QueueIsEmpty( Queue Q );
/*
*********************************************************************************************************
*                                          ALGQueueIsFull()
*
* Description : �ж�һ�������Ƿ�Ϊ��
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
int QueueIsFull( Queue Q );
/*
*********************************************************************************************************
*                                          QueueCreate1
*
* Description : ��������1���ն���
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : �����ڴ�������1���ն��У��������ڴ��������Ķ���
*********************************************************************************************************
*/
Queue QueueCreate1(int MaxElements);
/*
*********************************************************************************************************
*                                          QueueCreate2
*
* Description : ��������2���ն���
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : �����ڴ�������2���ն��У��������ڴ��������Ķ���
*********************************************************************************************************
*/
Queue QueueCreate2(int MaxElements);
/*
*********************************************************************************************************
*                                          QueueCreate3
*
* Description : �������Ž��ն���
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : �����ڴ������Ž��ն��У��������ڴ��������Ķ���
*********************************************************************************************************
*/
Queue QueueCreate3(int MaxElements);
/*
*********************************************************************************************************
*                                          QueueCreate4
*
* Description : ����DVR���Ͷ���
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : �����ڴ������Ž��ն��У��������ڴ��������Ķ���
*********************************************************************************************************
*/
Queue QueueCreate4(int MaxElements);
/*
*********************************************************************************************************
*                                          QueueCreate5
*
* Description : ����CAN1���ն���
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : �����ڴ���CAN1 ���ն��У��������ڴ��������Ķ���
*********************************************************************************************************
*/
Queue QueueCreate5(int MaxElements);
/*
*********************************************************************************************************
*                                          ALGQueueCreate()
*
* Description : ����һ��ѭ������
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : ע��Ŀǰ������Э����ջ���,�����ط����ܵ��øú���,����ģ��Ҫ�������д���Ƶ�һ������
*********************************************************************************************************
*/
Queue QueueCreate(int MaxElements);
/*
*********************************************************************************************************
*                                          ALGQueueMakeEmpty()
*
* Description : ʹ����Ϊ��
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void QueueMakeEmpty(Queue Q);
/*
*********************************************************************************************************
*                                          ALGQueueDispose()
*
* Description : �ͷ�һ������
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : һ��Ƕ��ʽӦ���в�Ҫ�����������ͷţ�����ʹ���ͷź���
*********************************************************************************************************
*/
void QueueDispose(Queue Q);
/*
*********************************************************************************************************
*                                          ALGQueueSucc()
*
* Description : valueֵ��1
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
int QueueSucc(int Value, Queue Q);
/*
*********************************************************************************************************
*                                          ALGQueueIn()
*
* Description : ���������һ��Ԫ�أ��������
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void QueueIn(ElementType X, Queue Q);
/*
*********************************************************************************************************
*                                          ALGQueueInBuffer()
*
* Description : �����һ�����Ӷ��Ԫ��
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void QueueInBuffer(ElementType *pBuffer, int BufferLen, Queue Q);
/*
*********************************************************************************************************
*                                          ALGQueueFront()
*
* Description : ��������Ԫ��
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
ElementType QueueFront(Queue Q);
/*
*********************************************************************************************************
*                                          ALGQueueOut()
*
* Description : ɾ�������е�һ��Ԫ�أ���������
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void QueueOut(Queue Q);
/*
*********************************************************************************************************
*                                          ALGQueueFrontAndOut()
*
* Description : ��ȡ�����е���Ԫ�أ�Ȼ�������
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
ElementType QueueFrontAndOut(Queue Q);
/*
*********************************************************************************************************
*                                          ALGQueueFrontAndOut2()
*
* Description : ��ȡ�����е���Ԫ�أ�Ȼ������У��������Ƿ�֡β�ı�־��
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : *pFlag == 1����ǰ�����ַ���֡β��*pFlag == 0����ǰ�����ַ�����֡β��
*********************************************************************************************************
*/
ElementType QueueFrontAndOut2(Queue Q, u8 *pFlag);
/*
*********************************************************************************************************
*                                          ALGQueueOutBuffer()
*
* Description : �����һ�ζ�ȡ���Ԫ��
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
int  QueueOutBuffer(ElementType *pBuffer, int BufferLen, Queue Q);
/*
*********************************************************************************************************
*                                          QueueLength()
*
* Description : ���г���
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/
int  QueueLength(Queue Q);
#endif