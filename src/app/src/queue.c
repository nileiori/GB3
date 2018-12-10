
/*
*********************************************************************************************************
*
*                                               算法
*                                             循环队列
*
*
* Filename      : queue.c
* Version       : V1.00
* Programmer(s) : dxl,由标准算法略微改编而成
*********************************************************************************************************
*/	
/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "queue.h"
#include "string.h"
#include "VDR_Usart.h"
#include<stdlib.h>

/*
*********************************************************************************************************
*                                             define
*********************************************************************************************************
*/	

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      LOCAL GLOBAL VARIABLRS
*********************************************************************************************************
*/

extern u8	RadioProtocolRx1Buffer[];//连接1接收缓冲
extern u8	RadioProtocolRx2Buffer[];//连接1接收缓冲
extern u8	RadioProtocolRxSmsBuffer[];//短信接收缓冲
extern u8  VdrTxBuffer[];//VDR发送队列缓冲
extern u8 CanRxBuffer[];
/*
*********************************************************************************************************
*                                          ALGQueueIsEmpty()
*
* Description : 判断一个队列是否为空
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
int QueueIsEmpty( Queue Q )
{
    return Q->Size == 0;
}
/*
*********************************************************************************************************
*                                          ALGQueueIsFull()
*
* Description : 判断一个队列是否为满
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
int QueueIsFull( Queue Q )
{
    return Q->Size == Q->Capacity;
}
/*
*********************************************************************************************************
*                                          QueueCreate1
*
* Description : 创建连接1接收队列
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : 仅用于创建连接1接收队列，不能用于创建其它的队列
*********************************************************************************************************
*/
Queue QueueCreate1(int MaxElements)
{
    Queue Q;

    if(MaxElements < MIN_QUEUE_SIZE)
        return NULL;

    Q = (Queue)malloc( sizeof(struct QueueRecord));
    if( Q == NULL )
        return NULL;

    Q->Array = RadioProtocolRx1Buffer;
    
    if( Q->Array == NULL )
        return NULL;
    
    Q->Capacity = MaxElements;
    
    QueueMakeEmpty( Q );

    return Q;
}
/*
*********************************************************************************************************
*                                          QueueCreate2
*
* Description : 创建连接2接收队列
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : 仅用于创建连接2接收队列，不能用于创建其它的队列
*********************************************************************************************************
*/
Queue QueueCreate2(int MaxElements)
{
    Queue Q;

    if(MaxElements < MIN_QUEUE_SIZE)
        return NULL;

    Q = (Queue)malloc( sizeof(struct QueueRecord));
    if( Q == NULL )
        return NULL;

    Q->Array = RadioProtocolRx2Buffer;
    
    if( Q->Array == NULL )
        return NULL;
    
    Q->Capacity = MaxElements;
    
    QueueMakeEmpty( Q );

    return Q;
}
/*
*********************************************************************************************************
*                                          QueueCreate3
*
* Description : 创建短信接收队列
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : 仅用于创建短信接收队列，不能用于创建其它的队列
*********************************************************************************************************
*/
Queue QueueCreate3(int MaxElements)
{
    Queue Q;

    if(MaxElements < MIN_QUEUE_SIZE)
        return NULL;

    Q = (Queue)malloc( sizeof(struct QueueRecord));
    if( Q == NULL )
        return NULL;

    Q->Array = RadioProtocolRxSmsBuffer;
    
    if( Q->Array == NULL )
        return NULL;
    
    Q->Capacity = MaxElements;
    
    QueueMakeEmpty( Q );

    return Q;
}
/*
*********************************************************************************************************
*                                          QueueCreate4
*
* Description : 创建DVR发送队列
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : 仅用于创建短信接收队列，不能用于创建其它的队列
*********************************************************************************************************
*/
Queue QueueCreate4(int MaxElements)
{
    Queue Q;

    if(MaxElements < MIN_QUEUE_SIZE)
        return NULL;

    Q = (Queue)malloc( sizeof(struct QueueRecord));
    if( Q == NULL )
        return NULL;

    Q->Array = VdrTxBuffer;
    
    if( Q->Array == NULL )
        return NULL;
    
    Q->Capacity = MaxElements;
    
    QueueMakeEmpty( Q );

    return Q;
}
/*
*********************************************************************************************************
*                                          QueueCreate5
*
* Description : 创建CAN1接收队列
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : 仅用于创建CAN1 接收队列，不能用于创建其它的队列
*********************************************************************************************************
*/
Queue QueueCreate5(int MaxElements)
{
    Queue Q;

    if(MaxElements < MIN_QUEUE_SIZE)
        return NULL;

    Q = (Queue)malloc( sizeof(struct QueueRecord));
    if( Q == NULL )
        return NULL;

    Q->Array = CanRxBuffer;
    
    if( Q->Array == NULL )
        return NULL;
    
    Q->Capacity = MaxElements;
    
    QueueMakeEmpty( Q );

    return Q;
}
/*
*********************************************************************************************************
*                                          ALGQueueCreate()
*
* Description : 创建一个循环队列
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
Queue QueueCreate(int MaxElements)
{
    Queue Q;

    if(MaxElements < MIN_QUEUE_SIZE)
        return NULL;

    Q = (Queue)malloc( sizeof(struct QueueRecord));
    if( Q == NULL )
        return NULL;

    Q->Array = malloc( sizeof(ElementType) * MaxElements);
    
    if( Q->Array == NULL )
        return NULL;
    
    Q->Capacity = MaxElements;
    
    QueueMakeEmpty( Q );

    return Q;
}
/*
*********************************************************************************************************
*                                          ALGQueueMakeEmpty()
*
* Description : 使队列为空
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
void QueueMakeEmpty(Queue Q)
{
    Q->Size = 0;
    Q->Front = 1;
    Q->Rear = 0;
}
/*
*********************************************************************************************************
*                                          ALGQueueDispose()
*
* Description : 释放一个队列
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : 一般嵌入式应用中不要经常创建、释放，即不使用释放函数
*********************************************************************************************************
*/
void QueueDispose(Queue Q)
{
    if( Q != NULL )
    {
        free( Q->Array );
        free( Q );
    }
}
/*
*********************************************************************************************************
*                                          ALGQueueSucc()
*
* Description : value值加1
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
int QueueSucc(int Value, Queue Q)
{
    if( ++Value == Q->Capacity )
        Value = 0;
    return Value;
}
/*
*********************************************************************************************************
*                                          ALGQueueIn()
*
* Description : 向队列添加一个元素，即入队列
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
void QueueIn(ElementType X, Queue Q)
{
    if(QueueIsFull(Q))
    {
        return ;
    }
    else
    {
        Q->Size++;
        Q->Rear = QueueSucc( Q->Rear, Q );
        Q->Array[ Q->Rear ] = X;
     }
}
/*
*********************************************************************************************************
*                                          ALGQueueInBuffer()
*
* Description : 向队列一次添加多个元素
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
void QueueInBuffer(ElementType *pBuffer, int BufferLen, Queue Q)
{
    int i = 0;
    ElementType X;
    
    if((Q->Capacity - Q->Size) > (BufferLen))
    {
        for(i=0; i<BufferLen; i++)
        {
            X = *(pBuffer+i);
            Q->Size++;
            Q->Rear = QueueSucc( Q->Rear, Q );
            Q->Array[ Q->Rear ] = X;
        }
    } 
}
/*
*********************************************************************************************************
*                                          ALGQueueFront()
*
* Description : 读队列首元素
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
ElementType QueueFront(Queue Q)
{
    if(!QueueIsEmpty(Q))
    {
        return Q->Array[ Q->Front ];
    }
    else
    {
        return 0;  /* Return value used to avoid warning */
    }
}
/*
*********************************************************************************************************
*                                          ALGQueueOut()
*
* Description : 删除队列中的一个元素，即出队列
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
void QueueOut(Queue Q)
{
    if(QueueIsEmpty(Q))
    {
        return ;
    }
    else
    {
        Q->Size--;
        Q->Front = QueueSucc( Q->Front, Q );
    }
}
/*
*********************************************************************************************************
*                                          ALGQueueFrontAndOut()
*
* Description : 读取队列中的首元素，然后出队列
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
ElementType QueueFrontAndOut(Queue Q)
{
    ElementType X = 0;
    
    if(QueueIsEmpty(Q))
    {
                
    }
    else
    {
        Q->Size--;
        X = Q->Array[ Q->Front ];
        Q->Front = QueueSucc( Q->Front, Q );
    }
    return X;
}
/*
*********************************************************************************************************
*                                          ALGQueueFrontAndOut2()
*
* Description : 读取队列中的首元素，然后出队列，并返回是否帧尾的标志符
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : *pFlag == 1：当前读出字符是有效的；*pFlag == 0：当前读出字符是无效的；
*********************************************************************************************************
*/
ElementType QueueFrontAndOut2(Queue Q, u8 *pFlag)
{
    ElementType X = 0;
    
    if(QueueIsEmpty(Q))
    {
        *pFlag = 0;
        return X;
    }

    Q->Size--;
    X = Q->Array[ Q->Front ];
    Q->Front = QueueSucc( Q->Front, Q );
    
    *pFlag = 1;
    return X;
}
/*
*********************************************************************************************************
*                                          ALGQueueOutBuffer()
*
* Description : 向队列一次读取多个元素
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
int  QueueOutBuffer(ElementType *pBuffer, int BufferLen, Queue Q)
{
    int count = 0;
    ElementType *p = NULL;
    
    if(NULL != p)
    {
        return 0;
    }
    
    p = pBuffer;
    
    while(count < BufferLen)
    {
        if(QueueIsEmpty(Q))
        {
            break;
        }
        else
        {
            Q->Size--;
            *p++ = Q->Array[ Q->Front ];
            Q->Front = QueueSucc( Q->Front, Q );
            count++;
        }
        
    }
    return count;
}
/*
*********************************************************************************************************
*                                          QueueRemanentLength()
*
* Description : 队列剩余长度
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
int  QueueLength(Queue Q)
{
    if(Q->Size)
    {
        return Q->Size;
    }
    else
    {
        return 0;
    }
}