/********************************************************************************
 * File Name:			public.c
 * Function Describe:	
 * Relate Module:	public.h
 * Writer:				Joneming 
 * Date:				  2013-01-18
 * Rewriter: 			
 * Date:				
 *******************************************************************************/
/****************************************************/
/*                                                  */
/* Included files                                   */
/*                                                  */
/****************************************************/
#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"
#include <string.h>
#include <stdio.h>
#include "Public.h"
#include "Rtc.h"
#include "stm32f10x_rtc.h"
#include "spi_flash_app.h"
#include "spi_flash.h"
#include "fm25c160.h"
#include "SysTick.h"
#include "EepromPram.h"
#include "Rtc.h"
/****************************************************/
/*                                                  */
/* Local Definitions and macros                     */
/*                                                  */
/****************************************************/
enum 
{
    PUBLIC_TIMER_EIEXPAND,					//
    PUBLIC_TIMER_TASK,                      //  
    PUBLIC_TIMER_TEST,						//  
    PUBLIC_TIMER_OPERATE,					//      
    PUBLIC_TIMERS_MAX
}STPUBLICTIME;
static LZM_TIMER s_stPUBLICTimer[PUBLIC_TIMERS_MAX];
///////////////////////////
//static unsigned char s_ucNewCallPhoneFlag = 0;//�������־
//////////////////////////////////
#if (USE_SYS_QUEUE == 1) 
#define QUEUE_DATALEN_SYS     40  //�������ݳ���
ST_QUEUE g_sutSysQueue;
static QueueData s_QDSysData[QUEUE_DATALEN_SYS];
//�ж���Ϣ����(��Ҫ�����жϵ��л�,�����е�ֵΪ�ж����ͣ���ϵͳ�г���Ϊ��Ϣ)
#define QUEUE_DATALEN_INT       40
ST_QUEUE g_sutIntQueue;
static QueueData s_QDIntData[QUEUE_DATALEN_INT];
#endif
/************************************************************
** ��������: LZM_QueueClear()initialize the queue
** ��������: ������У���ʼ���� queue is initialized to being empty 
** ��ڲ���: pq points to a queue 
** ���ڲ���:
************************************************************/ 
void LZM_QueueClear(ST_QUEUE *pq)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    pq->front   = 0;
    pq->rear    = 0;
    pq->count   = 0; 
    #endif
}

/************************************************************
** ��������: LZM_QueueEmpty()
** ��������: �������Ƿ�Ϊ��(����������ˣ���ô�졣��������)
** ��ڲ���:
** ���ڲ���: 0:   �ǿ�
			 ��0����
************************************************************/ 
QueueData LZM_QueueEmpty(ST_QUEUE *pq)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    if(pq->rear == pq->front||(0==pq->count))
    {
        pq->count = 0;
        return 1;
    }
    return 0;
    #endif
}

/************************************************************
** ��������: LZM_QueueFull()
** ��������: �������Ƿ���
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
QueueData LZM_QueueFull(ST_QUEUE *pq)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    return (pq->count >= pq->datalen);
    #endif
}
/************************************************************
** ��������: LZM_QueueInsert()
** ��������: �����ݲ������
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_QueueInsert(ST_QUEUE *pq,QueueData qdata)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    if(!LZM_QueueFull(pq))
    {  
        pq->data[pq->rear++] = qdata;
        if(pq->rear >= pq->datalen)
            pq->rear = 0;
        pq->count++;   
    }
    #endif
}

/************************************************************
** ��������: LZM_QueueDataBatchInsert
** ��������: �����������
** ��ڲ���: ST_QUEUE:������Ӷ���,dest:Ŀ���ַ,datalen:���ݳ���
** ���ڲ���: ��ӵ����ݳ���,
************************************************************/ 
Qu16 LZM_QueueDataBatchInsert(ST_QUEUE *pq,unsigned char *dest,unsigned short datalen)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    Qu16 count,count1;
    if(!LZM_QueueFull(pq))
    {
        if(pq->count + datalen > pq->datalen)
        {
            count = pq->datalen - pq->count;
        }
        else
        {
            count = datalen;
        }
        ////////////////////////////
        if(pq->rear + count > pq->datalen)
        {
            count1 =pq->datalen-pq->rear;
            memcpy(&pq->data[pq->rear],dest,count1);
            memcpy(&pq->data[0],&dest[count1],count-count1);
            pq->rear = count-count1;
        }
        else
        {
            memcpy(&pq->data[pq->rear],dest,count);
            pq->rear +=count;
        }
        //////////////////
        pq->count +=count;
        //////////////////////
        return count;
    }
    return 0;
    #endif
}
/************************************************************
** ��������: LZM_QueueDelete()
** ��������: �Ӷ���ȡ������
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
QueueData LZM_QueueDelete(ST_QUEUE *pq)
{   
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    QueueData data;
    data = 0;
    if(!LZM_QueueEmpty(pq))   
    {   
        ///////////////////////////////
        data = pq->data[pq->front];
        /////////////////////////
        if(pq->count)pq->count--;
        pq->front++;
        if(pq->front >= pq->datalen)
            pq->front = 0;
    } 
    ////////////////////////
    return data;
    #endif
}

/************************************************************
** ��������: LZM_QueueDataBatchDelete
** ��������: ������������
** ��ڲ���: ST_QUEUE:�������Ӷ���,dest:Ŀ���ַ,maxcount:�����,��ֹdest���
** ���ڲ���: ���ӵ����ݳ���,
************************************************************/ 
Qu16 LZM_QueueDataBatchDelete(ST_QUEUE *pq,unsigned char *dest,unsigned short maxcount)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    Qu16 count,count1;
    if(!LZM_QueueEmpty(pq))
    {
        if(pq->rear >= pq->front)
        {
            count = pq->rear -pq->front;
        }
        else
        {
            count = pq->datalen+ pq->rear - pq->front;
        }
        pq->count = count;
        /////////////////////////
        if(count>maxcount)count = maxcount;
        /////////////////////////
        pq->count -= count;
        if(pq->front + count > pq->datalen)
        {
            count1 = pq->datalen - pq->front;
            memcpy(dest,&pq->data[pq->front],count1);
            pq->front = count-count1;
            memcpy(&dest[count1],&pq->data[0],pq->front);
        }
        else
        {
            memcpy(dest,&pq->data[pq->front],count);
            pq->front += count;
        }
        //////////////////////
        return count;
    }
    return 0;
    #endif
}
/************************************************************
** ��������: LZM_QueueBackUpParam
** ��������: ������ض��в�������
** ��ڲ���: srcԴ����,bak�ݴ����
** ���ڲ���:
************************************************************/ 
void LZM_QueueBackUpParam(ST_QUEUE *src,ST_QUEUE *bak)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    bak->datalen    = src->datalen;
    bak->count      = src->count; 
    bak->front      = src->front;
    bak->rear       = src->rear;    
    #endif
}
/************************************************************
** ��������: LZM_QueueRecoveryParam
** ��������: �ָ���ض��в�������(ע��:����ʵ�����ݸ���ʵ�����,���û�и��ǵ�����ǰ������,���ɻָ�)
** ��ڲ���: desĿ�����,bak�ݴ����
** ���ڲ���:
************************************************************/ 
void LZM_QueueRecoveryParam(ST_QUEUE *des,ST_QUEUE *bak)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    des->datalen    = bak->datalen;
    des->count      = bak->count; 
    des->front      = bak->front;
    des->rear       = bak->rear;    
    #endif
}
/************************************************************
** ��������: LZM_QueueDataCount
** ��������: ��ȡ���е�ǰ�����ܳ���
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
Qu16 LZM_QueueDataCount(ST_QUEUE *pq)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    return   pq->count; 
    #endif
}

/************************************************************
** ��������: LZM_QueueDataFreeSpace
** ��������: ��ȡ���е�ǰ����ʣ��ռ�
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
Qu16 LZM_QueueDataFreeSpace(ST_QUEUE *pq)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    return   (pq->datalen - pq->count); 
    #endif
}
/************************************************************
** ��������: LZM_QueueClear()initialize the queue
** ��������: ������У���ʼ���� queue is initialized to being empty 
** ��ڲ���: pq points to a queue 
** ���ڲ���:
************************************************************/ 
void LZM_QueueDataInit(ST_QUEUE *pq,QueueData *data,int dataLen)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    pq->data= data;
    pq->datalen = dataLen;
    LZM_QueueClear(pq);
    #endif
}

/************************************************************
** ��������: LZM_QueueInit()
** ��������: 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_QueueInit(void)
{
    #if(QUEUE_TYPE == CIRCULAR_QUEUE)
    #if (USE_SYS_QUEUE==1)
    LZM_QueueDataInit(&g_sutSysQueue,s_QDSysData,QUEUE_DATALEN_SYS);
    LZM_QueueDataInit(&g_sutIntQueue,s_QDIntData,QUEUE_DATALEN_INT);
    #endif
    #endif
}

#if(USE_STACK == 1)
/************************************************************
** ��������: LZM_StackClear()initialize the stack
** ��������: ���ջ����ʼ���� stack is initialized to being empty 
** ��ڲ���: ps points to a stack 
** ���ڲ���:
************************************************************/ 
void LZM_StackClear(ST_STACK *ps)
{
    ps->top = 0;    
    ps->count = 0;
    ps->bottom = 0;
}

/************************************************************
** ��������: LZM_StackEmpty()
** ��������: ���ջ�Ƿ�Ϊ��(����������ˣ���ô�졣��������)
** ��ڲ���:
** ���ڲ���: 0:   �ǿ�
			 ��0����
************************************************************/ 
StackData LZM_StackEmpty(ST_STACK *ps)
{
    return(0 == ps->count); 
}


/************************************************************
** ��������: LZM_StackFull()
** ��������: ���ջ�Ƿ���
** ��ڲ���:
** ���ڲ���: 0:   ����
			 ��0����
************************************************************/ 
StackData LZM_StackFull(ST_STACK *ps)
{
    return (ps->count >= (STACKMAXNUM - 1));
}	  

/************************************************************
** ��������: LZM_StackTop
** ��������: �����ݴ�ջ����ջ
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
StackData LZM_StackTop(ST_STACK *ps)
{
    StackData data;
    data = 0;
    if(!LZM_StackEmpty(ps))   
    {
        if(ps->top == 0)
            ps->top = STACKMAXNUM;
        ps->top--;
        ps->count--;
        data = ps->data[ps->top];
    }
    return data;
}

/************************************************************
** ��������: LZM_StackBottom()
** ��������: �����ݴ�ջ�׳�ջ
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
StackData LZM_StackBottom(ST_STACK *ps)
{
    StackData data;
    data =0;
    if(!LZM_StackEmpty(ps))   
    { 
        data = ps->data[ps->bottom];
        ps->bottom++;
        if(ps->bottom == STACKMAXNUM)
            ps->bottom = 0;
        ps->count--;
    }
    /////////////////////
    return data;
}
/************************************************************
** ��������: LZM_StackPush()
** ��������: ��������ջ
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_StackPush(ST_STACK *ps,StackData sdata)
{
    if(!LZM_StackFull(ps))   
    { 
        ps->data[ps->top++] = sdata;  
        if(ps->top >= STACKMAXNUM) 
            ps->top = 0;   
        ps->count++;
    }
}
#endif

/*****************************************************
*
*       Public  code
*
*****************************************************/

/************************************************************
** ��������: LZM_PublicSetCycTimer
** ��������: �ظ�(ѭ��)��ʱ
** ��ڲ���: stTimer:��ʱ��ָ�����
             value:��ʱֵ
             function:��Ҫִ�еĺ���
** ���ڲ���:
************************************************************/ 
void LZM_PublicSetCycTimer(LZM_TIMER *stTimer,LZM_TIMER32 value,void (*function)(void))
{
    stTimer->enabled  = TIMER_ENABLE;
    stTimer->counter  = value+SysTick_GetCounter();
    stTimer->interval = value;
    stTimer->operate = function;
}
/************************************************************
** ��������: LZM_PublicSetOnceTimer
** ��������: ���ζ�ʱ
** ��ڲ���: stTimer:��ʱ��ָ�����
             value:��ʱֵ
             function:��Ҫִ�еĺ���
** ���ڲ���:
************************************************************/ 
void LZM_PublicSetOnceTimer(LZM_TIMER *stTimer,LZM_TIMER32 value,void (*function)(void))
{
    stTimer->enabled  = TIMER_ENABLE;
    stTimer->counter  = value+SysTick_GetCounter();
    stTimer->interval = ONCE;
    stTimer->operate = function;
}
/************************************************************
** ��������: LZM_PublicKillTimer
** ��������: �ر�ָ����ʱ��
** ��ڲ���: stTimer:��ʱ��ָ��
** ���ڲ���:
************************************************************/ 
void LZM_PublicKillTimer(LZM_TIMER *stTimer)
{
    stTimer->enabled  = TIMER_DISENABLE;
}
/************************************************************
** ��������: LZM_PublicGetTimerEnable
** ��������: ָ����ʱ���Ƿ���Ч
** ��ڲ���: stTimer:��ʱ��ָ��
** ���ڲ���:
************************************************************/ 
LZM_RET LZM_PublicGetTimerEnable(LZM_TIMER *stTimer)
{
    return (TIMER_ENABLE == stTimer->enabled)? 1 : 0;
}
/************************************************************
** ��������: LZM_PublicKillTimerAll
** ��������: �ر�����ָ����ʱ��
** ��ڲ���: stTimes:��ʱ������
             maxtimes:��ʱ�������
** ���ڲ���:
************************************************************/ 
void LZM_PublicKillTimerAll(LZM_TIMER *stTimers,unsigned char maxtimes)
{
    unsigned char i;
    for(i = 0; i < maxtimes; i++)
    {
        stTimers[i].enabled = TIMER_DISENABLE;
    }
}
/************************************************************
** ��������: LZM_PublicTimerHandler
** ��������: Ӧ�ó���ʱ��������
** ��ڲ���: stTimes:��ʱ������
             maxtimes:��ʱ�������
** ���ڲ���:
************************************************************/ 
void LZM_PublicTimerHandler(LZM_TIMER *stTimers,unsigned char maxtimes)
{
    unsigned char i;
    unsigned long timerVal;
    timerVal=SysTick_GetCounter();
    for(i = 0; i < maxtimes; i++)
    {
        if(TIMER_ENABLE == stTimers[i].enabled)
        {
            if(stTimers[i].counter <= timerVal)//
            {
                if(ONCE == stTimers[i].interval)//����
                {
                    stTimers[i].enabled = TIMER_DISENABLE;
                }
                else//�ظ���ʱ��
                {
                    stTimers[i].counter += stTimers[i].interval;
                    if(stTimers[i].counter< timerVal)
                        stTimers[i].counter = timerVal;
                }
                //�����ڸú��������ظ�����ö�ʱ��
                if(NULL != stTimers[i].operate)
                stTimers[i].operate();
            }
        }
    }
}

/*************************************************************
** ��������: Public_ConvertStrToASC
** ��������: ���ַ�����ÿһ���ֽ�������16��������ʾ(��ƽ̨��Ӧ)
** ��ڲ���: desĿ�����ݵ��׵�ַ,srcԴ���ݵ��׵�ַ,len��Ҫת����Դ���ݳ���
** ���ڲ���: desĿ�����ݵ��׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: ע��ռ����
*************************************************************/	
void Public_ConvertStrToASC(char * des,const char * src, unsigned short len)
{
    unsigned short i;
    for(i = 0; i < len; i++)
    {
        sprintf(&des[2*i],"%2X",src[i]);
    }
}
/*************************************************************
** ��������: Public_ConvertAscToVal
** ��������: ���ַ�'0'~'F'ת��0~15����ֵ
** ��ڲ���: val:��Ҫת��������
** ���ڲ���: ��
** ���ز���: =0xffת������,������Ϊת�����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_ConvertAscToVal(unsigned char val)
{
    unsigned char temp;
    temp = 0xff;
    if(val>=0x30&&val<=0x39)
    {
        temp = val-0x30;
    }
    else
    if(val>=0x41&&val<=0x46)
    {
        temp = val-0x41+10;
    }
    return temp;
}
/*************************************************************OK
** ��������: Public_GetSumVerify
** ��������: �����ݿ��ۼ���ͣ�ֻȡ���ֽ�
** ��ڲ���: Pdata���ݿ��׵�ַ,datalen����		 
** ���ڲ���: 
** ���ز���: У����
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Public_GetSumVerify(const unsigned char *Pdata,unsigned short datalen)
{
    //�����ݿ��ۼ���ͣ�ֻȡ���ֽ�
    unsigned char sum = 0;
    unsigned short  i;
    IWDG_ReloadCounter();//ι��
    for(i = 0; i < datalen; i++)
    {
        sum += Pdata[i];
    }
    IWDG_ReloadCounter();//ι��
    return sum;	  
}
/*************************************************************
** ��������: Public_GetXorVerify
** ��������: �����ݿ�������ֻȡ���ֽ�
** ��ڲ���: Pdata���ݿ��׵�ַ,datalen����		 
** ���ڲ���: 
** ���ز���: У����
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Public_GetXorVerify(const unsigned char *Pdata,unsigned short datalen)
{
    //�����ݿ�������ֻȡ���ֽ�
    unsigned char Verify ;
    unsigned short  i;
    //////////////////
    Verify= Pdata[0];
    IWDG_ReloadCounter();//ι��
    for(i = 1; i < datalen; i++)
    {
        Verify ^= Pdata[i];
    }
    IWDG_ReloadCounter();//ι��
    return Verify;	  
}

/*************************************************************
** ��������: Public_ConvertAscToBCD
** ��������: ��asciii��ת��BCD��,(�������ֽڵ�asciii��ת��һ���ֽڵ�BCD��)
** ��ڲ���: destinĿ�����ݵ��׵�ַ,sourceԴ���ݵ��׵�ַ,len��Ҫת����Դ���ݳ���
** ���ڲ���: destinĿ�����ݵ��׵�ַ
** ���ز���: ��
** ȫ�ֱ���: destinʹ��ʱע��destin�Ŀռ����
** ����ģ��: 
*************************************************************/	
void Public_ConvertAscToBCD(unsigned char * destin,const unsigned char* source, unsigned char len)
{
    unsigned char i;
    unsigned char chtemp,chhigh,chlow;
    for(i = 0; i < len; i++)
    {
        chtemp = *source++;
        chhigh = chtemp & 0x0F;
        chtemp = *source++;
        chlow = chtemp & 0x0F;
        chhigh <<= 4; 
        chhigh |= chlow;
        *destin++ = chhigh;
    }
}
/*************************************************************
** ��������: Public_ConvertBcdToAsc
** ��������: ��BCD���ַ���ÿһ���ֽ�ת��������AscII����ַ���
** ��ڲ���: pAscת��֮��������׵�ַ,pBcd��Ҫת����BCD�����׵�ַ��BcdLen:��Ҫת�������ݳ���
** ���ڲ���: pAscת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: ע��ռ����
*************************************************************/	
void Public_ConvertBcdToAsc(unsigned char *pAsc, const unsigned char *pBcd, unsigned char BcdLen)
{
    unsigned char	i;
    for(i=0; i<BcdLen; i++)
    {
        *(pAsc+2*i) = (*(pBcd+i) >> 4)+0x30;
        *(pAsc+2*i+1) = (*(pBcd+i) & 0x0f)+0x30;
    }
}
/*************************************************************
** ��������: Public_ConvertBcdToValue
** ��������: ��BCD���ַ���ת��������ֵ���ַ���
** ��ڲ���: pValueת��֮��������׵�ַ,pBcd��Ҫת����BCD�����׵�ַ��BcdLen:��Ҫת�������ݳ���
** ���ڲ���: pValueת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: ע��ռ����
*************************************************************/	
void Public_ConvertBcdToValue(unsigned char *pValue, const unsigned char *pBcd, unsigned char BcdLen)
{
    unsigned char	i;
    for(i=0; i<BcdLen; i++)
    {
        *(pValue+i) = (*(pBcd+i) >> 4)*10+(*(pBcd+i) & 0x0f);
    }
}
/*************************************************************
** ��������: Public_ConvertTimeToBCD
** ��������: ��ʱ��ת��BCD��(7λ)
** ��ڲ���: stTime:��Ҫת����ʱ��ṹ��,����BCD����׵�ַ
** ���ڲ���: data:ת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertTimeToBCD(TIME_T stTime,unsigned char * data)
{
    unsigned char buffer[20];
    unsigned char buffer1[20];
    sprintf((char *)buffer,"20%02d%02d%02d%02d%02d%02d",stTime.year,stTime.month,stTime.day,stTime.hour,stTime.min,stTime.sec);
    Public_ConvertAscToBCD(buffer1,buffer,strlen((char *)buffer));
    memcpy(data,buffer1,7);
}
/*************************************************************
** ��������: Public_ConvertTimeToBCDEx
** ��������: ��ʱ��ת��BCD��(6λ)
** ��ڲ���: stTime:��Ҫת����ʱ��ṹ��,����BCD����׵�ַ
** ���ڲ���: data:ת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertTimeToBCDEx(TIME_T stTime,unsigned char * data)
{
    unsigned char buffer[20];
    unsigned char buffer1[20];
    sprintf((char *)buffer,"%02d%02d%02d%02d%02d%02d",stTime.year,stTime.month,stTime.day,stTime.hour,stTime.min,stTime.sec);
    Public_ConvertAscToBCD(buffer1,buffer,strlen((char *)buffer));
    memcpy(data,buffer1,6);
}
/*************************************************************
** ��������: Public_ConvertNowTimeToBCD
** ��������: �ѵ�ǰʱ��ת��BCD��(7λ)
** ��ڲ���: ����BCD����׵�ַ
** ���ڲ���: data:ת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertNowTimeToBCD(unsigned char * data)
{
    TIME_T now_time;
    RTC_ReadTime(&now_time);
    Public_ConvertTimeToBCD(now_time,data);
}
/*************************************************************
** ��������: Public_ConvertNowTimeToBCDEx
** ��������: �ѵ�ǰʱ��ת��BCD��(6λ)
** ��ڲ���: ����BCD����׵�ַ
** ���ڲ���: data:ת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertNowTimeToBCDEx(unsigned char * data)
{
    TIME_T now_time;
    RTC_ReadTime(&now_time);
    Public_ConvertTimeToBCDEx(now_time,data);
}
/*************************************************************
** ��������: Public_CheckTimeStruct
** ��������: ���ʱ��ṹ���ڵ�ֵ�Ƿ���ȷ
** ��ڲ���: ���ʱ��ṹ����׵�ַ
** ���ڲ���: 
** ���ز���: ��ȷ������1�����󷵻�0
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_CheckTimeStruct(TIME_T *tt)
{
    //����ꡢ�¡��ա�ʱ���֡���ʱ�����
    if((tt->year > END_YEAR)||(tt->year < 0)||
    (tt->month > 12)||(tt->month < 1)||
    (tt->day > 31)||(tt->day < 1)||
    (tt->hour > 23)||(tt->hour < 0)||
    (tt->min > 59)||(tt->min < 0)||
    (tt->sec > 59)||(tt->sec < 0))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
/*************************************************************
** ��������: Public_ConvertBCDToTime
** ��������: ��BCD��(6λ)ת��ʱ���ʽ
** ��ڲ���: BCD����׵�ַ
** ���ڲ���: stTime:ת��֮��Ľ��(ת���ɹ��Ÿ�ֵ)
** ���ز���: 1ת���ɹ�,0����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_ConvertBCDToTime(TIME_T *stTime,unsigned char * data)
{
    unsigned char buffer[20];
    Public_ConvertBcdToValue(buffer,data,6);
    if(Public_CheckTimeStruct((TIME_T *)buffer))
    {
        memcpy(stTime, buffer,6);
        return 1;
    }
    return 0;
}
/*************************************************************
** ��������: Public_ConvertBCDToTimeEx
** ��������: ��BCD��(6λ)ת��ʱ���ʽ
** ��ڲ���: BCD����׵�ַ
** ���ڲ���: stTime:ת��֮��Ľ��,���жϽ��
** ���ز���:  
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertBCDToTimeEx(TIME_T *stTime,unsigned char * data)
{
    unsigned char buffer[20];
    Public_ConvertBcdToValue(buffer,data,6);
    memcpy(stTime, buffer,6);
}
/*************************************************************
** ��������: Public_ConvertLongToBuffer
** ��������: ��unsigned longת��4λunsigned char,��λ��ǰ
** ��ڲ���: value:��Ҫת��������
** ���ڲ���: buf:ת��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertLongToBuffer(unsigned long value,unsigned char *buf)
{
    unsigned char i;
    for(i= 0; i<4; i++)
    {
        buf[3-i]=value>>(i<<3);
    }
}
/*************************************************************
** ��������: Public_ConvertBufferToLong
** ��������: ��4λunsigned char,��λ��ǰ������ת��unsigned long;
             (��Public_ConvertLongToBuffer�����෴)
** ��ڲ���: buf:��Ҫת�������ݵ��׵�ַ
** ���ڲ���: 
** ���ز���: ת�����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned long Public_ConvertBufferToLong(unsigned char *buf)
{
    unsigned long value;
    value = 0;
    value |= buf[0]<<24;
    value |= buf[1]<<16;
    value |= buf[2]<<8;
    value |= buf[3];
    return value;
}
/*************************************************************
** ��������: Public_ConvertBufferToShort
** ��������: ��2λunsigned char,��λ��ǰ������ת��unsigned short;
** ��ڲ���: buf:��Ҫת�������ݵ��׵�ַ
** ���ڲ���: 
** ���ز���: ת�����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned short Public_ConvertBufferToShort(unsigned char *buf)
{
    unsigned short value;
    value = 0;
    value |= buf[0]<<8;
    value |= buf[1];
    return value;
}
/*************************************************************
** ��������: Public_ConvertShortToBuffer
** ��������: ��unsigned shortת��2λunsigned char,��λ��ǰ
** ��ڲ���: value:��Ҫת��������
** ���ڲ���: buf:ת��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertShortToBuffer(unsigned short value,unsigned char *buf)
{
    buf[0] = value>>8;
    buf[1] = value&0xff;
}
/*************************************************************
** ��������: Public_BCD2HEX
** ��������: BCDת��ΪByte        
** ��ڲ���: val:��Ҫת��������
** ���ڲ���: ��
** ���ز���: ת�����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Public_BCD2HEX(unsigned char val)    //BCDת��ΪByte
{
    unsigned char temp;
    temp=val&0x0f;
    val>>=4;
    val&=0x0f;
    val*=10;
    temp+=val;
   
    return temp;
}
/*************************************************************
** ��������: Public_HEX2BCD
** ��������: Byte��ת��ΪBCD��    
** ��ڲ���: val:��Ҫת��������
** ���ڲ���: ��
** ���ز���: ת�����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Public_HEX2BCD(unsigned char val)    //Byte��ת��ΪBCD��
{
    unsigned char i,j,k;
    i=val/10;
    j=val%10;
    k=j+(i<<4);
    return k;
}
/*************************************************************
** ��������: Public_ConvertTwoAscToOneChar
** ��������: �������ַ��ϳ���ֵ(���ַ�'0'~'F'ת��0~15����ֵ)
** ��ڲ���: src:�����ַ����׵�ַ
** ���ڲ���: result:ת�����
** ���ز���: 0:ת������,1ת���ɹ�
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_ConvertTwoAscToOneChar(const unsigned char *val,unsigned char *result)
{
    unsigned char temp,temp1;
    temp1=Public_ConvertAscToVal(val[0]);
    if(temp1==0xff)return 0;
    temp=Public_ConvertAscToVal(val[1]);
    if(temp==0xff)return 0;
    /////////////////
    temp1=temp1*16+temp;
    *result=temp1;
    return 1;
}
/*************************************************************
** ��������: Public_CheckStrIsBCD
** ��������: �ж��ַ��Ƿ�ΪBCD�룬����,�򷵻���Ӧ����ֵ,������:�򷵻�0xff
** ��ڲ���: val:��Ҫת��������
** ���ڲ���: ��
** ���ز���: =0xffת������,������Ϊת�����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_CheckStrIsBCD(unsigned char val)
{
    unsigned char temp,temp1;
    temp=val/16;
    temp1=val%16;
    if(temp>9||temp1>9)
    {
        return 0xff;
    }
    else
    {
        return (temp*10+temp1);
    }
}
/*************************************************************
** ��������: Public_ConvertShortToBCD
** ��������: ��short������ת��BCD(2λBCD,�����ǰ�油0)
** ��ڲ���: val���ݲ�����9999,bufferת��������׵�ַ
** ���ڲ���: bufferת��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertShortToBCD(unsigned short val,unsigned char *buffer)
{
    unsigned char temp[20],temp1[20];  
    if(val>9999) val = 9999;
    sprintf((char *)temp,"%04d",val);
    Public_ConvertAscToBCD(temp1,temp,4);
    memcpy(buffer,temp1,2);
}
/*************************************************************
** ��������: Public_ConvertShortToBCDEx
** ��������: ��short������ת��BCD(3λBCD,�����ǰ�油0)
** ��ڲ���: val����,bufferת��������׵�ַ
** ���ڲ���: bufferת��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertShortToBCDEx(unsigned short val,unsigned char *buffer)
{
    unsigned char temp[20],temp1[20];  
    sprintf((char *)temp,"%06d",val);
    Public_ConvertAscToBCD(temp1,temp,6);
    memcpy(buffer,temp1,3);
}
/*************************************************************
** ��������: Public_ConvertBCDToShort
** ��������: ��2λBCDת��Short������
** ��ڲ���: buffer�׵�ַ
** ���ڲ���: ��
** ���ز���: ת��֮�����ֵ
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned short Public_ConvertBCDToShort(unsigned char *buffer)
{
    unsigned char data[20];
    unsigned short val;
    Public_ConvertBcdToValue(buffer,data,4);
    val=100ul*data[0]+data[1];
    return val;
}
/*************************************************************
** ��������: Public_ConvertLongToBCD
** ��������: ��long������ת��BCD(4λBCD,�����ǰ�油0)
** ��ڲ���: val���ݲ�����99999999,bufferת��������׵�ַ
** ���ڲ���: bufferת��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertLongToBCD(unsigned long val,unsigned char *buffer)
{
    unsigned char temp[20],temp1[20];
    if(val>99999999) val = 99999999;
    sprintf((char *)temp,"%08d",val);
    Public_ConvertAscToBCD(temp1,temp,8);
    memcpy(buffer,temp1,4);
}
/*************************************************************
** ��������: Public_ConvertLongToBCDEx
** ��������: ��long������ת��BCD(5λBCD,�����ǰ�油0)
** ��ڲ���: val����,buffer�׵�ַ
** ���ڲ���: bufferת��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertLongToBCDEx(unsigned long val,unsigned char *buffer)
{
    unsigned char temp[20],temp1[20];
    sprintf((char *)temp,"%010d",val);
    Public_ConvertAscToBCD(temp1,temp,10);
    memcpy(buffer,temp1,5);
}
/*************************************************************
** ��������: Public_ConvertBCDToLong
** ��������: ��4λBCDת��long������
** ��ڲ���: buffer��Ҫת�����ݵ��׵�ַ
** ���ڲ���: ת�����
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned long Public_ConvertBCDToLong(const unsigned char *buffer)
{
    unsigned char data[20];
    unsigned long val;
    Public_ConvertBcdToValue(data,buffer,4);
    val=1000000ul*data[0]+10000ul*data[1]+100ul*data[2]+data[3];
    return val;
}
/*************************************************************
** ��������: Public_CheckArrayValIsEqual
** ��������: �ж����������Ƿ����,
** ��ڲ���: array��Ҫ�жϵ�����1�׵�ַ,array1��Ҫ�жϵ�����2�׵�ַ,len��Ҫ�жϵĳ���
** ���ڲ���: ��
** ���ز���: 0���,1�����;
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_CheckArrayValIsEqual(const unsigned char *array,const unsigned char *array1,unsigned short len)
{
    unsigned short i;
    for(i =0; i <len; i++)
    {
        if(array[i]!=array1[i])return 1;
    }
    return 0;
}

/*************************************************************
** ��������: Public_PlayTTSVoiceAlarmStr
** ��������: �ֶβ�������,������Ϣ
** ��ڲ���: ��Ҫ���ŵ������׵�ַ
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
/* dxl,2014.8.8 �����蹤�ύ�޸�
void Public_PlayTTSVoiceAlarmStr(const u8 *StrPtr)
{
    if(Modem_Api_Tts_IsSpt())//dxl,2014.6.11��������ttsʱ�޸�
    {
        Modem_Api_Tts_Play(0,(u8 *)StrPtr,0);
    }
    else
    {
        PlayMulTTSVoiceAlarmStr((u8 *)StrPtr);
    }
}
*/
/*
void Public_PlayTTSVoiceAlarmStr(const u8 *StrPtr)
{
    PlayMulTTSVoiceAlarmStr((u8 *)StrPtr);
}
*/
/*************************************************************
** ��������: Public_PlayE2ParamAlarmVoice
** ��������: ���ű�����E2�����еı�����Ϣ����(�洢������������50���ֽ�)
** ��ڲ���: ����ID
** ���ڲ���: 
** ���ز���: 0:������ȡ����,1������ȡ��ȷ
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
/*dxl,2015.8
unsigned char Public_PlayE2ParamAlarmVoice(unsigned short ParamID)
{
    unsigned char len;
    unsigned char buffer[51];
    len=EepromPram_ReadPram(ParamID, buffer);
    if(len)
    {
        buffer[len]=0;
        Public_PlayTTSVoiceAlarmStr(buffer);
        return 1;
    }
    return 0;
}
*/
/*************************************************************
** ��������: Public_PlayTTSVoiceStr
** ��������: �ֶβ�������,��ͨ��Ϣ
** ��ڲ���: ��Ҫ���ŵ������׵�ַ
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
/* dxl,2014.8.8�����蹤�ύ���޸�
void Public_PlayTTSVoiceStr(const u8 *StrPtr)
{
    if(Modem_Api_Tts_IsSpt())//dxl,2014.6.11��������ttsʱ�޸�
    {
        Modem_Api_Tts_Play(0,(u8 *)StrPtr,0);
    }
    else
    {
        PlayMulTTSVoiceStr((u8 *)StrPtr);
    }
}
*/
/*
void Public_PlayTTSVoiceStr(const u8 *StrPtr)
{
    PlayMulTTSVoiceStr((u8 *)StrPtr);
}
*/
/*************************************************************
** ��������: Public_WriteDataToFRAM
** ��������: ��������д����,������ĳβ����У���ֽ�
** ��ڲ���: address:�����ַ;data:�����׵�ַ;datalen:д����ֽ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: FRAM_BufferWrite
*************************************************************/
void Public_WriteDataToFRAM(unsigned short address,const unsigned char *data, unsigned short datalen)
{
    FRAM_BufferWrite(address,(unsigned char *)data,datalen);
}
/*************************************************************
** ��������: Public_ReadDataFromFRAM
** ��������: �������������
** ��ڲ���: address:�����ַ;data:Ŀ�������׵�ַ;datalen:�������ֽ���
** ���ڲ���: data
** ���ز���: ʵ�ʶ������ֽ���
** ȫ�ֱ���: ��
** ����ģ��: FRAM_BufferRead
*************************************************************/
unsigned short Public_ReadDataFromFRAM(unsigned short address,unsigned char *data, unsigned short datalen)
{
    return FRAM_BufferRead(data,datalen,address);
}
/*************************************************************OK
** ��������: Public_GetCurBasicPositionInfo
** ��������: ��õ�ǰ����λ����Ϣ(�ϱ���ʽ)
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: Report_GetBasicPositionInformation
*************************************************************/
/*
void Public_GetCurBasicPositionInfo(unsigned char *data)
{
    Report_GetPositionBasicInfo(data); //����λ����Ϣ
}
*/
/*************************************************************
** ��������: Public_GetCurTotalMile
** ��������: ��ȡ�����(�ѿ������ٶ����ͣ��������ж��ٶ�������)
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: ��λ��0.1����
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
/*
unsigned long Public_GetCurTotalMile(void)
{
	
    unsigned long CurrentMile;
    if(SpeedMonitor_GetCurSpeedType())
    {
        CurrentMile = MileMeter_GetTotalMile();        
    }
    else
    {
        CurrentMile = Pulse_GetTotalMile();
    }
    CurrentMile /=10;
    return CurrentMile;
}
*/
/*************************************************************
** ��������: Public_Mymemcpy
** ��������: src�����ݿ���des��,nΪ��������,flagΪ����(1:����,0:˳��)
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void *Public_Mymemcpy(void *des,  const void *src, unsigned short len, unsigned char flag)
{
    //copy char src[len] to des[len] in any order //
    char *su1 = (char *)des;
    const char *su2 = (const char *)src;
    unsigned short i;
    IWDG_ReloadCounter();//ι��
    if(flag==0)
    {
        for (; 0 < len; ++su1, ++su2, --len)
            *su1 = *su2;
    }
    else
    {
        for(i=0 ;i<len; i++)
        {
            *(su1+len-1-i) = *(su2+i);
        }	
    }
    IWDG_ReloadCounter();//ι��
    return (des);
}
/*************************************************************
** ��������: Public_GetCurPositionInfoDataBlock
** ��������: ȡ�õ�ǰλ����Ϣ(γ�ȡ����ȡ��߶�:��A.20 λ����Ϣ���ݿ��ʽ)
** ��ڲ���: �������ݵ��׵�ַ
** ���ڲ���: �������ݵ��׵�ַ
** ���ز���: ���ݳ���
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
/*
unsigned char Public_GetCurPositionInfoDataBlock(unsigned char *buffer)
{
    unsigned char len;
    signed long slVal;
    GPS_STRUCT stTmpGps;
    Gps_CopygPosition(&stTmpGps);
    //////////////////////////////
    len = 0;
    //////////////////////////
    slVal=stTmpGps.Longitue_D*600000ul+stTmpGps.Longitue_F*10000ul+stTmpGps.Longitue_FX;
    if(!stTmpGps.East)slVal *= -1;
    buffer[len++] = slVal>>24;
    buffer[len++] = slVal>>16;
    buffer[len++] = slVal>>8;
    buffer[len++] = slVal;
    /////////////////
    slVal=stTmpGps.Latitue_D*600000ul+stTmpGps.Latitue_F*10000ul+stTmpGps.Latitue_FX;
    if(!stTmpGps.North)slVal *= -1;   
    buffer[len++] = slVal>>24;
    buffer[len++] = slVal>>16;
    buffer[len++] = slVal>>8;
    buffer[len++] = slVal;
    
    buffer[len++] = stTmpGps.High>>8;
    buffer[len++] = stTmpGps.High; 
    return len;
}
*/
/*************************************************************OK
** ��������: Public_ShowTextInfo
** ��������: ��ʱ��ʾ�ı���Ϣ(��ʾ������,����),��ʱ������ʾ֮ǰ�Ľ��� 
** ��ڲ���: msg�ַ����׵�ַ,time:��ʾʱ��
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: LcdShowMsgEx
*************************************************************/
void Public_ShowTextInfo(u8 *msg,u32 timeVal)
{
    #ifdef NEW_LCD_DRIVER_GDI
    LcdShowMsgEx(msg,timeVal);
    #else
    #ifdef USE_NAVILCD
    NaviLcdShowTempText((unsigned char*)msg);
    #endif
    #endif
}
/*************************************************************
** ��������: Public_ShowSaveDataProBar
** ��������: ��ʾ�洢����
** ��ڲ���: �ٷֱ�(��0��ʼ,1.0������ʾ,ȡֵ:0.0~1.0)
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Public_ShowSaveDataProBar(float persent)
{
    #ifdef NEW_LCD_DRIVER_GDI
    char buffer[20];
    unsigned char val;
    static unsigned char x0,x;
    if(persent==0.0)
    {
        x0=x=12;
        LcdCopyScreen();
        LcdClearScreen();
        LcdShowCaption("����ת����", 2);
        LcdShowWindow(10, 22, 115, 31);
        LcdShowWindow(12, 24, 112, 29);
        LcdShowStr(48, 36, "  0%", 0);
        return;
    }
    ////////////////////////////
    val =(unsigned char)(persent*100.0);
    if(val > 100)val=100;
    x= 12 + val;
    LcdShowHorLine(x0+1, x+1, 26, 0);  
    LcdShowHorLine(x0+1, x+1, 27, 0);    
    x0=x;
    sprintf(buffer,"%3d",val);    
    LcdShowStr(48, 36, buffer, 0);
    if(val>=100)LcdPasteScreen();
    #endif
}

/*******************************************************************************
**  ��������  : Public_SetCarLoadStatus
**  ��������  : �����ػ�״̬ 
**  ��    ��  : status: 0�����ء�1�����ء�2������
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : SetEvTask(EV_REPORT);Io_WriteStatusBit
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
/*
void Public_SetCarLoadStatus(unsigned char status)
{
    if(0==status)
    {
        Io_WriteStatusBit(STATUS_BIT_FULL_LOAD1,RESET);
        Io_WriteStatusBit(STATUS_BIT_FULL_LOAD2,RESET);
    }
    else
    if(1==status)
    {
        Io_WriteStatusBit(STATUS_BIT_FULL_LOAD1,SET);
        Io_WriteStatusBit(STATUS_BIT_FULL_LOAD2,RESET);
    }
    else
    if(2==status)
    {
        Io_WriteStatusBit(STATUS_BIT_FULL_LOAD1,SET);
        Io_WriteStatusBit(STATUS_BIT_FULL_LOAD2,SET);
    }
    //SetEvTask(EV_REPORT);
    Report_UploadPositionInfo(CHANNEL_DATA_1);
    Report_UploadPositionInfo(CHANNEL_DATA_2);
}
*/
/*******************************************************************************
**  ��������  : Public_QuickOpenAccount
**  ��������  : ���ٿ���
**  ��    ��  : 
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
/*
void Public_QuickOpenAccount(void)
{
    ///����������ӿ��ٿ���ʵ�ʷ��ͺ���/////////    
    #ifdef USE_QUICK_OPEN_ACCOUNT
    OpenAccount_CheckAndDisposeLockStatus();
    #endif
}
*/
/*************************************************************
** ��������: Public_ConvertTimeToLong
** ��������: ��ʱ��ṹ����ݿ�����ת��long��
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),stTime��Ҫת����ʱ��ṹ��
** ���ڲ���: 
** ���ز���: ת�����(0:���ڿ�����Ϊ0,��������;��0,������) 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned long Public_ConvertTimeToLong(unsigned short ctrl,TIME_T stTime)
{
    unsigned long Temp=0;
    ///////////////////////
    if(0==ctrl)return 0;////
    ///////////////////////////
    if(PUBLIC_CHECKBIT(ctrl, 0))//����ʱ�䷶Χ
    {
        Temp = stTime.year; //�����
        Temp = Temp*12 + stTime.month; //�����
        Temp = Temp*31 + stTime.day; //�����
        Temp = Temp*24 + stTime.hour; //���ʱ
        Temp = Temp*60 + stTime.min; //��÷�
        Temp = Temp*60 + stTime.sec; //�����
    }
    else
    if(PUBLIC_CHECKBIT(ctrl, 1))//���ڷ�Χ
    {
        Temp = stTime.year; //�����
        Temp = Temp*12 + stTime.month; //�����
        Temp = Temp*31 + stTime.day; //�����
    }
    else
    if(PUBLIC_CHECKBIT(ctrl, 2))//ʱ�䷶Χ
    {
        Temp = Temp*24 + stTime.hour; //���ʱ
        Temp = Temp*60 + stTime.min; //��÷�
        Temp = Temp*60 + stTime.sec; //�����
    }
    else
    if(PUBLIC_CHECKBIT(ctrl, 3))//���·�Χ
    {
        Temp = Temp*12 + stTime.month; //�����
        Temp = Temp*31 + stTime.day; //�����
    }
    else
    if(PUBLIC_CHECKBIT(ctrl, 4))//����
    {
    }
    else
    if(PUBLIC_CHECKBIT(ctrl, 5))//��
    {
        Temp = Temp*31 + stTime.day; //�����
    }
    else
    if(PUBLIC_CHECKBIT(ctrl, 6))//��
    {
        Temp = Temp*12 + stTime.month; //�����
    }
    else
    if(PUBLIC_CHECKBIT(ctrl, 7))//��
    {
        Temp = stTime.year; //�����
    }
    /////////////////
    Temp++;//��֤���ؽ������Ϊ0
    ///////////////////
    return Temp;
}
/*************************************************************
** ��������: Public_ConvertBCDTimeToLong
** ��������: ��BCD��(6λ)���ݿ�����ת��long��
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),data��Ҫת����BCD���׵�ַ
** ���ڲ���: 
** ���ز���: 0:��ʾת���������Ҫת��,��0:������ת����� 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned long Public_ConvertBCDTimeToLong(unsigned short ctrl,unsigned char *data)
{
    TIME_T stTime;
    if(0==ctrl)return 0;
    Public_ConvertBCDToTimeEx(&stTime,data);
    return Public_ConvertTimeToLong(ctrl,stTime);//���ؽ����0
}
/*************************************************************
** ��������: Public_CheckTimeRangeIsValid
** ��������: �ж�ָ��ʱ�䷶Χ�Ƿ���Ч
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),startTime��Χ��ʼʱ���(����Public_ConvertTimeToLongת����ֵ),endTime��Χ����ʱ���(����Public_ConvertTimeToLongת����ֵ)
** ���ڲ���: 
** ���ز���: 1:��Ч��0:��Ч
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
unsigned char Public_CheckTimeRangeIsValid(unsigned short ctrl,unsigned long startTime,unsigned long endTime)
{
    if((0==startTime)||(0==endTime))return 0;
    if(startTime==endTime)
    {
        if(PUBLIC_CHECKBIT(ctrl, 0)||PUBLIC_CHECKBIT(ctrl, 2))//����ʱ�䷶Χ
        return 0;
    }
    return 1;
}
/*************************************************************
** ��������: Public_ConvertWeekDay
** ��������: ת������
** ��ڲ���: flagת����־,0:ĳ������Чʱ,ҹ��ʱ��η�ΧΪĳ�������Ϻ�����,������ʱ���,������
                          1:ĳ������Чʱ,ҹ��ʱ��η�ΧΪĳ�������ϼ��ڶ��������,Ϊһ��������ʱ���
             week����,startTime��Χ��ʼʱ���(����Public_ConvertTimeToLongת����ֵ),endTime��Χ����ʱ���(����Public_ConvertTimeToLongת����ֵ)
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
unsigned char Public_ConvertWeekDay(unsigned char flag,unsigned char week,TIME_T stTime,unsigned long startTime,unsigned long endTime)
{
    unsigned char result;
    result=week;
    /////���ݱ�־������һ�ַ���/////////////
    if(0==flag)return result;////ĳ������Чʱ,ҹ��ʱ��η�ΧΪĳ�������Ϻ�����,������ʱ���,������
    /////////////////
    if(startTime>endTime&&stTime.hour<12)//ĳ������Чʱ,ҹ��ʱ��η�ΧΪĳ�������ϼ��ڶ��������,Ϊһ��������ʱ���
    {
        if(1==result)
        {
            result=7;
        }
        else
        {
           result=week-1;
        }
    }
    return result;
}
/*************************************************************
** ��������: Public_CheckTimeIsInRange
** ��������: �ж�ָ��ʱ���Ƿ���ָ����Χ��
** ��ڲ���: curTimeָ��ʱ��,startTime��ʼʱ��,endTime����ʱ��
** ���ڲ���: 
** ���ز���: 1:�ڷ�Χ�ڣ�0:���ڷ�Χ�� 
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
unsigned char Public_CheckTimeIsInRange(unsigned long curTime,unsigned long startTime,unsigned long endTime)
{
    if(startTime<endTime)//С��
    {
        if(startTime<=curTime&&curTime<=endTime)return 1;
    }
    else//����
    if(startTime>endTime)//
    {
        if(startTime<=curTime||curTime<=endTime)return 1;
    }
    else
    if(startTime==endTime)//����
    {
        if(startTime==curTime)return 1;
    }
    return 0;
}
/*************************************************************
** ��������: Public_CheckCurTimeIsInTimeRange
** ��������: �жϵ�ǰʱ���Ƿ���ָ��ʱ�䷶Χ��
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),startTime��Χ��ʼʱ���(����Public_ConvertTimeToLongת����ֵ),endTime��Χ����ʱ���(����Public_ConvertTimeToLongת����ֵ)
** ���ڲ���: 
** ���ز���: 1:�ڷ�Χ�ڣ�0:���ڷ�Χ�� 
** ȫ�ֱ���: ��
** ����ģ��: Public_ConvertTimeToLong,RTC_GetCurWeekDay
*************************************************************/
/*
unsigned char Public_CheckCurTimeIsInTimeRange(unsigned short ctrl,unsigned long startTime,unsigned long endTime)
{
    unsigned char temp,week;
    unsigned long curTime;
    TIME_T stTime;
    if(!Public_CheckTimeRangeIsValid(ctrl,startTime,endTime))return 0;
    RTC_ReadTime(&stTime);
    temp=(ctrl & 0xff00)>>8;
    if(temp)//��Ҫ�������
    {
        week = RTC_GetCurWeekDay();
        week = Public_ConvertWeekDay(0,week,stTime,startTime,endTime);
        if(!PUBLIC_CHECKBIT(temp, week-1))return 0;//
    }
    ////////////////////////////    
    curTime=Public_ConvertTimeToLong(ctrl,stTime);
    return Public_CheckTimeIsInRange(curTime,startTime,endTime);
}
*/
/*************************************************************
** ��������: Public_CheckTimeIsInTimeRange
** ��������: �ж�ָ��ʱ���Ƿ���ָ��ʱ�䷶Χ��
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),checkTime��Ҫ�жϵ�ʱ���(�ĸ��ֽڵ�ʱ��),startTime(����Public_ConvertTimeToLongת����ֵ),endTime��Χ����ʱ���(����Public_ConvertTimeToLongת����ֵ)
** ���ز���: 1:�ڷ�Χ�ڣ�0:���ڷ�Χ�� 
** ȫ�ֱ���: ��
** ����ģ��: Public_ConvertTimeToLong,RTC_CacWeekDay
*************************************************************/
/*
unsigned char Public_CheckTimeIsInTimeRange(unsigned short ctrl,unsigned long checkTime,unsigned long startTime,unsigned long endTime)
{
    unsigned char temp,week;
    unsigned long curTime;
    TIME_T stTime;    
    if(!Public_CheckTimeRangeIsValid(ctrl,startTime,endTime))return 0;
    //////////////////////////
    Gmtime(&stTime,checkTime);
    /////////////////////////
    temp=(ctrl & 0xff00)>>8;
    if(temp)//��Ҫ�������
    {
        week = RTC_CacWeekDay(stTime.year,stTime.month,stTime.day);
        week = Public_ConvertWeekDay(0,week,stTime,startTime,endTime);
        if(!PUBLIC_CHECKBIT(temp, week-1))return 0;//
    }
    ////////////////////////////
    curTime=Public_ConvertTimeToLong(ctrl,stTime);
    /////////////////////////
    return Public_CheckTimeIsInRange(curTime,startTime,endTime);
}
*/
/*************************************************************
** ��������: Public_CheckCurTimeIsInEarlyAlarmTimeRange
** ��������: �жϵ�ǰʱ���Ƿ���ָ��ʱ�����ǰ��Χ��
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),earlyTime��ǰʱ��,startTime��Χ��ʼʱ���(����Public_ConvertTimeToLongת����ֵ),endTime��Χ����ʱ���(����Public_ConvertTimeToLongת����ֵ)
** ���ڲ���: 
** ���ز���: 1:�ڷ�Χ�ڣ�0:���ڷ�Χ�� 
** ȫ�ֱ���: ��
** ����ģ��: Public_CheckTimeIsInTimeRange
*************************************************************/
/*
unsigned char Public_CheckCurTimeIsInEarlyAlarmTimeRange(unsigned short ctrl,unsigned long earlyTime,unsigned long startTime,unsigned long endTime)
{
    unsigned long checktime;
    checktime=RTC_GetCounter();
    checktime += earlyTime;
    return Public_CheckTimeIsInTimeRange(ctrl,checktime,startTime,endTime);
}
*/
/*******************************************************************************
**  ��������  : Public_ShowValue
**  ��������  : ��ʾ��ֵ
**  ��    ��  : x,y,format��ʽ,value��ֵ
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void Public_ShowValue(unsigned char x,unsigned char y,char *format ,unsigned long value)
{
    #ifdef NEW_LCD_DRIVER_GDI
    char buffer[30];
    sprintf(buffer,format,value);
    LcdShowStr(x,y, buffer, 0);
    #endif
}
/*******************************************************************************
**  ��������  : Public_LcdShowTime
**  ��������  : ��ʾʱ��
**  ��    ��  : y����,stTimeʱ��ṹ��
**  ��    ��  : 
**  ȫ�ֱ���  : 
**  ���ú���  : 
**  �ж���Դ  : 
**  ��    ע  :
*******************************************************************************/
void Public_LcdShowTime(unsigned char y,TIME_T stTime)
{
    #ifdef NEW_LCD_DRIVER_GDI
    char buffer[20];
    sprintf(buffer,"%02d-",stTime.year);
    LcdShowStr(0,y,buffer,0);
    sprintf(buffer,"%02d-",stTime.month);
    LcdShowStr(23,y,buffer,0);
    sprintf(buffer,"%02d",stTime.day);
    LcdShowStr(46,y,buffer,0);
    sprintf(buffer,"%02d:%02d:%02d",stTime.hour,stTime.min,stTime.sec);
    LcdShowStr(67,y,buffer,0);
    #endif
}
/*************************************************************
** ��������: Public_CheckIsPhoneServerRunStatus
** ��������: �ж��Ƿ�Ϊ�绰�������ʻ״̬
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 1Ϊ�绰�������ʻ״̬,0:�ǵ绰�������ʻ״̬
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
/*
unsigned char Public_CheckIsPhoneServerRunStatus(void)
{
    #ifdef E2_RUN_FORBID_CALL_SWITCH_ID 
    unsigned char flag;
    if(EepromPram_ReadPram(E2_RUN_FORBID_CALL_SWITCH_ID,&flag))
    {
        if(1==flag)
        {
            if((Gps_ReadSpeed()>5)||(Pulse_GetSecondSpeed()>5))
            {
                return 1;//
            }
        }
    }
    #else
    #if (MOD_TYPE == CDMA_MODULE_MC8331) 
    if((Gps_ReadSpeed()>5)||(Pulse_GetSecondSpeed()>5))
    {
        return 1;//
    }
    #endif
    #endif
    return 0;
}
*/
/*************************************************************
** ��������: Public_CheckPhoneIsSpecialNumber
** ��������: �ж��Ƿ�Ϊ�������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 0:���������,��0:Ϊ�������,
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
/*
unsigned char Public_CheckPhoneIsSpecialNumber(unsigned char *phone)
{
    unsigned char buffer[30];
    unsigned char len;
    if(!strlen((char *)phone))return 0;
    len = EepromPram_ReadPram(E2_MONITOR_SERVER_PHONE_ID,buffer);
    if(len)
    {
        buffer[len]='\0';
        if(!strcmp((char *)phone,(char *)buffer))
        {
            return 1;//����/����
        }
    }
    /////�����绰����////////////
    len = EepromPram_ReadPram(E2_MONITOR_PHONE_ID,buffer);
    if(len)
    {
        buffer[len]='\0';
        if(!strcmp((char *)phone,(char *)buffer))
        {
            return 2;//����/����
        }
    }
    /////////////////////
    len = EepromPram_ReadPram(E2_TERMINAL_RESET_PHONE_ID,buffer);//�ն˸�λ�绰����
    if(len)
    {
        buffer[len]='\0';
        if(!strcmp((char *)phone,(char *)buffer))
        {
            return 3;
        }
    }
    len = EepromPram_ReadPram(E2_TERMINAL_DEFAULT_SET_PHONE_ID,buffer);//�ָ��������õ绰����
    if(len)
    {
        buffer[len]='\0';
        if(!strcmp((char *)phone,(char *)buffer))
        {
            return 4;
        }
    }
    return 0;
}
*/
/*************************************************************
** ��������: Public_CheckPhoneInformation
** ��������: ���绰����,�Ѹ���ͨ��ʱ���жϵ绰������ĺ���,(�����绰���롢���ߵ绰���벻���κ�����)
** ��ڲ���: phone����Ҫ���ĵ绰�����׵�ַ�� 
** ���ڲ���: name�����ص���ϵ������
** ���ز���: 0:��ֹ����/������1: ���룻2: ������3������/���� 
** ȫ�ֱ���: ��
** ����ģ��: CheckPhoneInformation,
*************************************************************/
/*
unsigned char Public_CheckPhoneInformation(unsigned char *phone,unsigned char *name)
{
    unsigned char flag;
    flag=Public_CheckPhoneIsSpecialNumber(phone);
    #ifdef USE_PHONE
    Phone_SetPhoneSpecialFlag(flag);
    #endif
    if(1 == flag)//���ƽ̨�绰����
    {
        strcpy((char *)name,"�ͷ�����");
        return 3;//����/����
    }
    else
    if(2 == flag)//�����绰
    {
        strcpy((char *)name,"�����绰");
        return 3;//����/����
    }
    else
    if(3 == flag)//���ն˸�λ�绰����
    {
        strcpy((char *)name,"��λ�绰");
        if(s_ucNewCallPhoneFlag)
        Public_SetOperateTimer(NVIC_SystemReset,PUBLICSECS(3));
        return 3;//����/����
    }
    else
    if(4 == flag)//�ָ��������õ绰����
    {
        strcpy((char *)name,"�ָ�����");
        if(s_ucNewCallPhoneFlag)
        Public_SetOperateTimer(EepromPram_DefaultSet,PUBLICSECS(2));
        return 3;//����/����
    }
    ///////����Ƿ�������ʻ�н�ֹͨ��////////////
    if(Public_CheckIsPhoneServerRunStatus())
    {
        return 0;//
    }
    //////���ͨ��ʱ��///////////////////
    #ifdef USE_PHONE 
    /////////ÿ��ͨ��ʱ��////////////
    if(0==Phone_GetEachMaxTalkTime())
    {
        return 0;//��ֹ����/����
    }
    ////////////ÿ��ͨ��ʱ��/////////////// 
    if(Phone_GetOverMonthTalkTimeFlag())//��ó�������ͨ��ʱ���־
    {
        return 0;//��ֹ����/����
    }
    #else
    unsigned char buffer[30];
    /////////ÿ��ͨ��ʱ��////////////
    if(EepromPram_ReadPram(E2_MAX_PHONE_TIME_EACH_ID,buffer))
    {
        if(0==Public_ConvertBufferToLong(buffer))
        {
            return 0;//��ֹ����/����
        }
    }
    ////////////ÿ��ͨ��ʱ��/////////////// 
    if(EepromPram_ReadPram(E2_MAX_PHONE_TIME_MONTH_ID,buffer))
    {
        if(0==Public_ConvertBufferToLong(buffer))
        {
            return 0;//��ֹ����/����
        }
    }
    #endif
    ///////��û�е绰�������������ͨ��/////////////////
    if(0==Message_GetPhoneBookTotalNum())
    {
        flag = 3;
        strcpy((char *)name,"δ֪");
        return flag;//����/���� 
    }
    ////////�ɵ绰�����Ծ���////////////////
    return CheckPhoneInformation(phone,name);//0��û�иõ绰��������ϣ�1: ���룻2: ������3������/����
}
*/
/*************************************************************
** ��������: Public_DisposePhoneStatus
** ��������: ����绰״̬����
** ��ڲ���: phone:�绰����,status:�绰״̬��0��Ϊ�Ҷ�;1:��ͨ;2:������;
** ���ڲ���: ��
** ���ز���: ����ǵ绰����״̬Ϊ������ʱ,����ֵ:0:ֱ�Ӿܽ�����;1:���ڵȴ��û���������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
/*
unsigned char Public_DisposePhoneStatus(unsigned char *phone,unsigned char status)
{
    unsigned char result;
    result = 0;
    s_ucNewCallPhoneFlag = 0;
    if(2 == status)s_ucNewCallPhoneFlag = 1;
    #ifdef USE_PHONE_BOOK
    #ifdef USE_NAVILCD
    if(NaviLcd_GetCurOnlineStatus())
    result = NaviLcd_DisposePhoneStatus(phone,status);
    #endif
    result = PhoneBook_DisposePhoneStatus(phone,status);
    #else
    #ifdef USE_NAVILCD
    result = NaviLcd_DisposePhoneStatus(phone,status); 
    #endif 
    #endif
    #ifdef USE_PHONE
    Phone_DisposePhoneStatus(phone,status);
    #endif
    s_ucNewCallPhoneFlag = 0;
    return result;
}
*/
/*************************************************************
** ��������: ShowStartLogo
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
/*
void Public_SaveCurSIMICcidNumberToE2Param(void)
{
    unsigned char data[20]={0}; 
    MOD_GetIccidBcdValue(data);
    EepromPram_WritePram(E2_SIM_ICCID_NUMBER_ID,data,E2_SIM_ICCID_NUMBER_ID_LEN);
}
*/
/*************************************************************
** ��������: Public_CheckSystemLogoAndVerSion
** ��������: �����ն˲�Ʒ���ơ���Ʒ�汾���Ƿ���ȷ(2�ֽڹ�˾��־,8�ֽ��ͺ�,2�ֽ�PCB�汾,6�ֽڰ汾��,4�ֽ�У����)
** ��ڲ���: ������Ϣ�׵�ַ,crc
** ���ڲ���: ��
** ���ز���:  1:������ȷ,0:�������
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
/*dxl,2015.8
unsigned char Public_CheckSystemLogoAndVerSion(unsigned char *pBuffer,unsigned long *checkCRC)
{
    char readLogoModel[11];  
    char readVersion[7]; 
    unsigned char len;
    ///////////2�ֽڹ�˾��־,8�ֽ��ͺ�,2�ֽ�PCB�汾,6�ֽڰ汾��,4�ֽ�У���� 
    ///////////////////
    memcpy(readLogoModel,pBuffer,10);//EIEGS702
    memcpy(readVersion,pBuffer+12,6);//021101  
    /////////////
    *checkCRC = Public_ConvertBufferToLong(&pBuffer[18]); 
    /////////////////////////////////
    #ifdef __VERSION_H_//EGS702
    char curLogoModel[11];  
    char curVersion[7];    
    ///////////////////
    len=Version_GetEquipmentCompany(curLogoModel);
    Version_GetProductSeriesName(&curLogoModel[len]);
    Version_GetEquipmentSoftVer(curVersion);
    //�ȽϹ�˾LOGO�Ͳ�Ʒ�ͺ�
    if(0 != strncmp(readLogoModel,curLogoModel,8))
    {
        return 0;//        
    }
    //////////////////
    if(0 != strncmp(readVersion,curVersion,4))
    {
        if(0 != strncmp(readVersion,"0220",4))return 0;
    }
    #else//EGS701
    if(0 != strncmp(readLogoModel+2,PRODUCT_MODEL+14,6))
    {
        return 0;//        
    }
    #endif
    ////////////////////
    //д��������,dxl,2014.6.14
    #ifdef FRAM_FIRMWARE_UPDATA_TYPE_ADDR
    len = 0;
    FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_TYPE_ADDR, &len, FRAM_FIRMWARE_UPDATA_TYPE_LEN);
    #endif
    return 1;
}
*/
/*************************************************************
** ��������: Public_CheckPeripheralUpdataType
** ��������: ���������Ʒ�����Ƿ���ȷ(2�ֽڹ�˾��־,8�ֽ��ͺ�,2�ֽ�PCB�汾,6�ֽڰ汾��,4�ֽ�У����
** ��ڲ���: ������Ϣ�׵�ַ,crc
** ���ڲ���: ��
** ���ز���: 1:������ȷ,0:�������
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_CheckPeripheralUpdataType(unsigned char *pBuffer,unsigned char *type)
{
    #ifndef __VERSION_H_
    if(0 == strncmp((char const *)pBuffer+2,"EICZ    ",4))//����Ʒ�ͺ�dxl,2014.6.14
    {
        *type = 0x09;
        #ifdef FRAM_FIRMWARE_UPDATA_TYPE_ADDR
        FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_TYPE_ADDR, type, FRAM_FIRMWARE_UPDATA_TYPE_LEN);
        #endif
        return 1;
    }
    #endif    
    return 0;
}
/*************************************************************
** ��������: Public_CheckTerminalAndPeripheralVerify
** ��������: �����նˡ������Ʒ���ơ���Ʒ�汾���Ƿ���ȷ(2�ֽڹ�˾��־,8�ֽ��ͺ�,2�ֽ�PCB�汾,6�ֽڰ汾��,4�ֽ�У����)
** ��ڲ���: ������Ϣ�׵�ַ,checkCRC����ֵ,type�������ͷ���ֵ
** ���ڲ���: ��
** ���ز���: 1:������ȷ,0:�������
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
/*
unsigned char Public_CheckTerminalAndPeripheralVerify(unsigned char *pBuffer,unsigned long *checkCRC,unsigned char *type)
{    
    *type = 0;
    ///////�Ƿ����ն�����///////////////////
    if(Public_CheckSystemLogoAndVerSion(pBuffer,checkCRC))
    {
        return 1;
    }
    ///////�Ƿ�����������///////////////////
    if(Public_CheckPeripheralUpdataType(pBuffer,type))
    {
        return 1;
    }
    ///////////////////
    return 0;
}
*/
/*************************************************************
** ��������: Public_ParameterInitialize
** ��������: ������ʼ��
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
/*
void Public_ParameterInitialize(void)
{
    LZM_PublicKillTimerAll(s_stPUBLICTimer,PUBLIC_TIMERS_MAX);  
    ///////////////////////
    SetTimerTask(TIME_PUBLIC,LZM_TIME_BASE);
    ////////////////////////
    //Public_SetOperateTimer(Public_SaveCurSIMICcidNumberToE2Param,PUBLICSECS(50));
    //////////////////////////  
    #ifdef USE_PHONE
    Phone_ParamInitialize();
    #endif
    ///////////////////////
}
*/
/************************************************************
** ��������: Public_SetOperateTimer
** ��������: 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void Public_SetOperateTimer(void (*function)(void),LZM_TIMER32 time)
{
    LZM_PublicSetOnceTimer(&s_stPUBLICTimer[PUBLIC_TIMER_OPERATE],time,function);
}
/************************************************************
** ��������: Public_SetEIExpandTimer
** ��������: 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void Public_SetEIExpandTimer(void (*function)(void),LZM_TIMER32 time)
{
    LZM_PublicSetOnceTimer(&s_stPUBLICTimer[PUBLIC_TIMER_EIEXPAND],time,function);
}
/************************************************************
** ��������: Public_SetTestTimer
** ��������: 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void Public_SetTestTimer(void (*function)(void),LZM_TIMER32 time)
{
    LZM_PublicSetOnceTimer(&s_stPUBLICTimer[PUBLIC_TIMER_TEST],time,function);
}

/************************************************************
** ��������: Public_SetTaskTimer
** ��������: 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void Public_SetTaskTimer(void (*function)(void),LZM_TIMER32 time)
{
    LZM_PublicSetOnceTimer(&s_stPUBLICTimer[PUBLIC_TIMER_TASK],time,function);
}
/*************************************************************
** ��������: LZM_PublicTimerTask
** ��������: ������ʱ��������(��ʱ��Ҫ���Ǻܸߵ���ִ��ռ�õ�ʱ�䲻��ܳ�)
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
FunctionalState LZM_PublicTimerTask(void)
{
    LZM_PublicTimerHandler(s_stPUBLICTimer,PUBLIC_TIMERS_MAX);
    #ifdef USE_PHONE
    Phone_TimeTask();
    #endif
    return ENABLE;
}

/********************************************************************************
 *                            End of File
 *******************************************************************************/
