
/*
*********************************************************************************************************
*
*                                               �㷨
*                                             ѭ������
*
*
* Filename      : queue.c
* Version       : V1.00
* Programmer(s) : dxl,�ɱ�׼�㷨��΢�ı����
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

extern u8	RadioProtocolRx1Buffer[];//����1���ջ���
extern u8	RadioProtocolRx2Buffer[];//����1���ջ���
extern u8	RadioProtocolRxSmsBuffer[];//���Ž��ջ���
extern u8  VdrTxBuffer[];//VDR���Ͷ��л���
extern u8 CanRxBuffer[];
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
int QueueIsEmpty( Queue Q )
{
    return Q->Size == 0;
}
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
int QueueIsFull( Queue Q )
{
    return Q->Size == Q->Capacity;
}
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
* Description : ����һ��ѭ������
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
* Description : ��������һ��Ԫ�أ��������
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
* Description : �����һ����Ӷ��Ԫ��
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
* Description : ��ȡ�����е���Ԫ�أ�Ȼ������У��������Ƿ�֡β�ı�־��
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : *pFlag == 1����ǰ�����ַ�����Ч�ģ�*pFlag == 0����ǰ�����ַ�����Ч�ģ�
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
* Description : ����ʣ�೤��
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