/********************************************************************************
 * File Name:			public.h 
 * Function Describe:	public.h declare module
 * Relate Module:		No File
 * Writer:				Joneming
 * Date:				  2011-09-08
 * Rewriter: 			
 * Date:				
 *******************************************************************************/
#ifndef __LZMPUBLIC__H__
#define __LZMPUBLIC__H__

//*********************ͷ�ļ�********************************
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "rtc.h"
#include "SysTickApp.h"
/****************************************************/
/*                                                  */
/* Included files                                   */
/*                                                  */
/****************************************************/

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif


/****************************************************/
/*                                                  */
/* Structures                                       */
/*                                                  */
/****************************************************/

#define PUBLIC_CHECKBIT(X,N) ((X) & (1ul << (N)))	   //����ĳһλ
#define PUBLIC_SETBIT(X,N) (X)=((X) | (1ul<<(N)))       //����ĳһλ
#define PUBLIC_CLRBIT(X,N) (X)=((X) & (~(1ul<<(N))))    //���ĳһλ
////////////////////////////////////
#define PUBLIC_CHECKBIT_EX(X,N) (PUBLIC_CHECKBIT(X,N)?1:0)	   //����ĳһλ,����ֵֻ��0��1
//////////////////////////////////////
#define USE_SYS_QUEUE		1//�Ƿ�ʹ��ϵͳ����: 0:��ʹ��  
//////////////���в�������//////////////////////
typedef unsigned char  QueueData;  //���е���������
typedef unsigned short  Qu16;      //���в�����������
#define CIRCULAR_QUEUE	0		//���ζ���
#define QUEUE_TYPE		CIRCULAR_QUEUE//ʹ�õĶ�������
//��������
#if(QUEUE_TYPE == CIRCULAR_QUEUE)
typedef struct
{
  //////////////////////////////////////////////////////
    Qu16 datalen;				//���г���
    Qu16 front;					//����ͷ
    Qu16 rear;					//����β
    Qu16 count;					//�������ݸ���
    QueueData *data;		//��������	
//////////////////////////////////////////////////////
}ST_QUEUE;
#endif

//////////////////////////////////////////////////
#define USE_STACK    0   //�Ƿ�ʹ��ջ 0:��ʹ��  1:ʹ��

typedef unsigned short  StackData;  //ջ����������
typedef unsigned char  SDus8;       //ջ����������
#define STACKMAXNUM 			12	//Ϊ�˶�������

typedef struct
{
	StackData data[STACKMAXNUM];    //ջ����
	SDus8 top;					    //ջ��
    SDus8 bottom;					//ջ��
	SDus8 count;					//ջ���ݸ���
	SDus8 pad;					    //����
}ST_STACK;
/////////////////ջ������/////////////////
void LZM_StackClear(ST_STACK *ps);
StackData LZM_StackEmpty(ST_STACK *ps);
StackData LZM_StackFull(ST_STACK *ps);
StackData LZM_StackTop(ST_STACK *ps); 
StackData LZM_StackBottom(ST_STACK *ps);
void LZM_StackPush(ST_STACK *ps,StackData sdata);
///////////////////////////////////////////
//////////////////////////////////////////
//��ʱ���������Ͷ���///////////////////////////
typedef unsigned char   LZM_RET;                     //����ķ��ز�������//
typedef unsigned short  LZM_TIMER16;                 //��ʱ����������//
typedef unsigned long   LZM_TIMER32;                 //��ʱ����������//

/////////////��ʱ�������//////////////////////
#define TIMER_TRIGGER	0                           //��ʱ��������Ϣֵ����ʱ��Ϊ����ʱ������ TIMER_TRIGGER ������Ϣ��	
#define ONCE			TIMER_TRIGGER               //���ζ�ʱ��־ֵ����ʱ��Ϊ����ʱ������ TIMER_TRIGGER ������Ϣ��	
#define TIMER_ENABLE	0xBA                        //������ʱ��//���Ϊ����ֵ,��ʹû�г�ʼ��,һ��Ҳ�����Ǹ�ֵ	
#define TIMER_DISENABLE	0x10		                //�رն�ʱ��
//ʱ�ӽṹ
typedef struct sttimer
{
    LZM_TIMER32 counter;	                        //��ʱʱ�䣨��ʱ��Ϊ����ʱ��
    LZM_TIMER32 interval;	                        //��װ��ֵ
    LZM_RET     enabled;	                        //ʱ�ӿ�����־ 0:�ر�  1:����    
    void (*operate)(void);                          //handle independtly funtion
}LZM_TIMER;

////////////////////////////////////
#define LZM_AT_ONCE         1//���ִ��
#define LZM_TIME_BASE       1//����ֻ���ж϶�ʱ���Ƿ��Ѿ����˵�Ƶ��
/////////////////////////////////////////////
#define PUBLICSECS(x)	(LZM_TIMER32)((x) * SYSTICK_1SECOND)  //
/////////////////////

/****************************************************/
/*                                                  */
/* Definitions                                      */
/*                                                  */
/****************************************************/
/****************************************************/
/*                                                  */
/* Function declarations                            */
/*                                                  */
/****************************************************/
/////////////////���д�����/////////////////
/************************************************************
** ��������: LZM_QueueClear()initialize the queue
** ��������: ������У���ʼ���� queue is initialized to being empty 
** ��ڲ���: pq points to a queue 
** ���ڲ���:
************************************************************/ 
void LZM_QueueClear(ST_QUEUE *pq);
/************************************************************
** ��������: LZM_QueueFull()
** ��������: �������Ƿ���
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
QueueData LZM_QueueFull(ST_QUEUE *pq);
/************************************************************
** ��������: LZM_QueueInsert()
** ��������: �����ݲ������
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void LZM_QueueInsert(ST_QUEUE *pq,QueueData ucdata);
/************************************************************
** ��������: LZM_QueueDataBatchInsert
** ��������: �����������
** ��ڲ���: ST_QUEUE:������Ӷ���,dest:Ŀ���ַ,datalen:���ݳ���
** ���ڲ���: ��ӵ����ݳ���,
************************************************************/ 
Qu16 LZM_QueueDataBatchInsert(ST_QUEUE *pq,unsigned char *dest,unsigned short datalen);
/************************************************************
** ��������: LZM_QueueDelete()
** ��������: �Ӷ���ȡ������
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
QueueData LZM_QueueDelete(ST_QUEUE *pq);
/************************************************************
** ��������: LZM_QueueDataBatchDelete
** ��������: ������������
** ��ڲ���: ST_QUEUE:�������Ӷ���,dest:Ŀ���ַ,maxcount:�����,��ֹdest���
** ���ڲ���: ���ӵ����ݳ���,
************************************************************/ 
Qu16 LZM_QueueDataBatchDelete(ST_QUEUE *pq,unsigned char *dest,unsigned short maxcount);
/************************************************************
** ��������: LZM_QueueEmpty()
** ��������: �������Ƿ�Ϊ��
** ��ڲ���:
** ���ڲ���: 0:   �ǿ�
			 ��0����
************************************************************/ 
QueueData LZM_QueueEmpty(ST_QUEUE *pq);
//////////���б������ݵ�����ռ��ʼ��/////////////////////////////////////
/************************************************************
** ��������: LZM_QueueClear()initialize the queue
** ��������: ������У���ʼ���� queue is initialized to being empty 
** ��ڲ���: pq points to a queue 
** ���ڲ���:
************************************************************/ 
void LZM_QueueDataInit(ST_QUEUE *pq,QueueData *data,int dataLen);
//////////////////
/************************************************************
** ��������: LZM_QueueDataCount
** ��������: ��ȡ���е�ǰ�����ܳ���
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
Qu16 LZM_QueueDataCount(ST_QUEUE *pq);
/////////////////////////
/************************************************************
** ��������: LZM_QueueDataFreeSpace
** ��������: ��ȡ���е�ǰ����ʣ��ռ�
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
Qu16 LZM_QueueDataFreeSpace(ST_QUEUE *pq);
//////////////���г�ʼ��//////////////////////
void LZM_QueueInit(void);

#if (USE_SYS_QUEUE==1)
extern ST_QUEUE g_sutSysQueue;		                    //ϵͳ��Ϣ����(��Ҫ���ڸ�������л�)
//////////////���������//////////////////////
#define Clear_SYSQueue()\
		LZM_QueueClear(&g_sutSysQueue)
//////////////����к�////////////////////////
#define Insert_SYSQueue(message)\
		LZM_QueueInsert(&g_sutSysQueue,message)
/////////////�����к�////////////////////////
#define Delete_SYSQueue()\
		LZM_QueueDelete(&g_sutSysQueue)
//////////////�ж϶����Ƿ�Ϊ�պ�/////	
#define Empty_SYSQueue()\
		LZM_QueueEmpty(&g_sutSysQueue)

/////////////IntQueue//////////////////////////
extern ST_QUEUE g_sutIntQueue;		                    //�ж���Ϣ����(��Ҫ�����жϵ��л�)
//////////////���������//////////////////////
#define Clear_INTQueue()\
		LZM_QueueClear(&g_sutIntQueue)
//////////////����к�////////////////////////
#define Insert_INTQueue(message)\
		LZM_QueueInsert(&g_sutIntQueue,message)
/////////////�����к�////////////////////////
#define Delete_INTQueue()\
		LZM_QueueDelete(&g_sutIntQueue)
//////////////�ж϶����Ƿ�Ϊ�պ�/////	
#define Empty_INTQueue()\
		LZM_QueueEmpty(&g_sutIntQueue)
#endif


/*************************************************************
 *                        ��ʱ����غ���
 *************************************************************/

/************************************************************
** ��������: LZM_PublicSetCycTimer
** ��������: �ظ�(ѭ��)��ʱ
** ��ڲ���: stTimer:��ʱ������
             value:��ʱֵ
             function:��Ҫִ�еĺ���
** ���ڲ���:
************************************************************/ 
void LZM_PublicSetCycTimer(LZM_TIMER *stTimer,LZM_TIMER32 value,void (*function)(void));
/************************************************************
** ��������: LZM_PublicSetOnceTimer
** ��������: ���ζ�ʱ
** ��ڲ���: stTimer:��ʱ������
             value:��ʱֵ
             function:��Ҫִ�еĺ���
** ���ڲ���:
************************************************************/ 
void LZM_PublicSetOnceTimer(LZM_TIMER *stTimer,LZM_TIMER32 value,void (*function)(void));
/************************************************************
** ��������: LZM_PublicKillTimer
** ��������: �ر�ָ����ʱ��
** ��ڲ���: stTimer:��ʱ������
** ���ڲ���:
************************************************************/ 
void LZM_PublicKillTimer(LZM_TIMER *stTimer);
/************************************************************
** ��������: LZM_PublicGetTimerEnable
** ��������: ָ����ʱ���Ƿ���Ч
** ��ڲ���: stTimer:��ʱ��ָ��
** ���ڲ���:
************************************************************/ 
LZM_RET LZM_PublicGetTimerEnable(LZM_TIMER *stTimer);
/************************************************************
** ��������: LZM_PublicKillTimerAll
** ��������: �ر�����ָ����ʱ��
** ��ڲ���: stTimes:��ʱ������
             maxtimes:��ʱ�������
** ���ڲ���:
************************************************************/ 
void LZM_PublicKillTimerAll(LZM_TIMER *stTimers,unsigned char maxtimes);
/************************************************************
** ��������: LZM_PublicTimerHandler
** ��������: Ӧ�ó���ʱ�Ӵ�����
** ��ڲ���: stTimers:��ʱ������
             maxtimes:��ʱ�������
** ���ڲ���:
************************************************************/ 
void LZM_PublicTimerHandler(LZM_TIMER *stTimers,unsigned char maxtimes);
/*************************************************************
** ��������: LZM_PublicTimerTask
** ��������: ������ʱ��������(��ʱ��Ҫ���Ǻܸߵ���ִ��ռ�õ�ʱ�䲻��ܳ�)
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
FunctionalState LZM_PublicTimerTask(void);

//////////////////////////////////////////////////////////////
/*************************************************************
 ********����Ϊ��ʱ����غ���,�����޸�****
 *************************************************************/
////////////////////////////////////////////////////////////////

/*************************************************************
** ��������: Public_ConvertStrToASC
** ��������: ���ַ�����ÿһλ(0~F)��һ��Asc���ʾ
** ��ڲ���: desĿ�����ݵ��׵�ַ,srcԴ���ݵ��׵�ַ,len��Ҫת����Դ���ݳ���
** ���ڲ���: desĿ�����ݵ��׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: ע��ռ����
*************************************************************/	
void Public_ConvertStrToASC(char * des,const char * src, unsigned short len);
/*************************************************************
** ��������: Public_ConvertAscToVal
** ��������: ���ַ�'0'~'F'ת��0~15����ֵ
** ��ڲ���: val:��Ҫת��������
** ���ڲ���: ��
** ���ز���: =0xffת������,������Ϊת�����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_ConvertAscToVal(unsigned char val);
/*************************************************************OK
** ��������: Public_GetSumVerify
** ��������: �����ݿ��ۼ���ͣ�ֻȡ���ֽ�
** ��ڲ���: Pdata���ݿ��׵�ַ,datalen����		 
** ���ڲ���: 
** ���ز���: У����
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Public_GetSumVerify(const unsigned char *Pdata,unsigned short datalen);
/*************************************************************
** ��������: Public_GetXorVerify
** ��������: �����ݿ�������ֻȡ���ֽ�
** ��ڲ���: Pdata���ݿ��׵�ַ,datalen����		 
** ���ڲ���: 
** ���ز���: У����
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Public_GetXorVerify(const unsigned char *Pdata,unsigned short datalen);
/*************************************************************
** ��������: Public_ConvertLongToBuffer
** ��������: ��unsigned longת��4λunsigned char,��λ��ǰ
** ��ڲ���: value:��Ҫת��������
** ���ڲ���: buf:ת��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertLongToBuffer(unsigned long value,unsigned char *buf);
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
unsigned long Public_ConvertBufferToLong(unsigned char *buf);
/*************************************************************
** ��������: Public_ConvertBufferToShort
** ��������: ��2λunsigned char,��λ��ǰ������ת��unsigned short;
** ��ڲ���: buf:��Ҫת�������ݵ��׵�ַ
** ���ڲ���: 
** ���ز���: ת�����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned short Public_ConvertBufferToShort(unsigned char *buf);
/*************************************************************
** ��������: Public_ConvertShortToBuffer
** ��������: ��unsigned shortת��2λunsigned char,��λ��ǰ
** ��ڲ���: value:��Ҫת��������
** ���ڲ���: buf:ת��������׵�ַ
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertShortToBuffer(unsigned short value,unsigned char *buf);
/*************************************************************
** ��������: Public_ConvertAscToBCD
** ��������: ��asciii��ת��BCD��,(�������ֽڵ�asciii��ת��һ���ֽڵ�BCD��)
** ��ڲ���: destinĿ�����ݵ��׵�ַ,sourceԴ���ݵ��׵�ַ,len��Ҫת����Դ���ݳ���
** ���ڲ���: destinĿ�����ݵ��׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertAscToBCD(unsigned char * destin,const unsigned char* source, unsigned char len);
/*************************************************************
** ��������: Public_ConvertBcdToAsc
** ��������: ��BCD���ַ���ÿһ���ֽ�ת��������AscII����ַ���
** ��ڲ���: pAscת��֮��������׵�ַ,pBcd��Ҫת����BCD�����׵�ַ��BcdLen:��Ҫת�������ݳ���
** ���ڲ���: pAscת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: ע��ռ����
*************************************************************/	
void Public_ConvertBcdToAsc(unsigned char *pAsc, const unsigned char *pBcd, unsigned char BcdLen);
/*************************************************************
** ��������: Public_ConvertBcdToValue
** ��������: ��BCD���ַ���ת��������ֵ���ַ���
** ��ڲ���: pValueת��֮��������׵�ַ,pBcd��Ҫת����BCD�����׵�ַ��BcdLen:��Ҫת�������ݳ���
** ���ڲ���: pValueת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertBcdToValue(unsigned char *pValue,const unsigned char *pBcd, unsigned char BcdLen);
/*************************************************************
** ��������: Public_ConvertTimeToBCD
** ��������: ��ʱ��ת��BCD��(7λ)
** ��ڲ���: stTime:��Ҫת����ʱ��ṹ��,����BCD����׵�ַ
** ���ڲ���: data:ת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertTimeToBCD(TIME_T stTime,unsigned char * data);
/*************************************************************
** ��������: Public_ConvertTimeToBCDEx
** ��������: ��ʱ��ת��BCD��(6λ)
** ��ڲ���: stTime:��Ҫת����ʱ��ṹ��,����BCD����׵�ַ
** ���ڲ���: data:ת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertTimeToBCDEx(TIME_T stTime,unsigned char * data);
/*************************************************************
** ��������: Public_ConvertNowTimeToBCD
** ��������: �ѵ�ǰʱ��ת��BCD��(7λ)
** ��ڲ���: ����BCD����׵�ַ
** ���ڲ���: data:ת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertNowTimeToBCD(unsigned char * data);
/*************************************************************
** ��������: Public_ConvertNowTimeToBCDEx
** ��������: �ѵ�ǰʱ��ת��BCD��(6λ)
** ��ڲ���: ����BCD����׵�ַ
** ���ڲ���: data:ת��֮��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertNowTimeToBCDEx(unsigned char * data);
/*************************************************************
** ��������: Public_CheckTimeStruct
** ��������: ���ʱ��ṹ���ڵ�ֵ�Ƿ���ȷ
** ��ڲ���: ���ʱ��ṹ����׵�ַ
** ���ڲ���: 
** ���ز���: ��ȷ������1�����󷵻�0
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_CheckTimeStruct(TIME_T *tt);
/*************************************************************
** ��������: Public_ConvertBCDToTime
** ��������: ��BCD��(6λ)ת��ʱ���ʽ
** ��ڲ���: BCD����׵�ַ
** ���ڲ���: stTime:ת��֮��Ľ��(ת���ɹ��Ÿ�ֵ)
** ���ز���: 1ת���ɹ�,0����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_ConvertBCDToTime(TIME_T *stTime,unsigned char * data);
/*************************************************************
** ��������: Public_ConvertBCDToTimeEx
** ��������: ��BCD��(6λ)ת��ʱ���ʽ
** ��ڲ���: BCD����׵�ַ
** ���ڲ���: stTime:ת��֮��Ľ��,���жϽ��
** ���ز���:  
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertBCDToTimeEx(TIME_T *stTime,unsigned char * data);
/*************************************************************
** ��������: Public_HEX2BCD
** ��������: Byte��ת��ΪBCD��    
** ��ڲ���: val:��Ҫת��������
** ���ڲ���: ��
** ���ز���: ת�����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Public_HEX2BCD(unsigned char val);    //B��ת��ΪBCD��
/*************************************************************
** ��������: Public_BCD2HEX
** ��������: BCDת��ΪByte        
** ��ڲ���: val:��Ҫת��������
** ���ڲ���: ��
** ���ز���: ת�����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Public_BCD2HEX(unsigned char val);    //BCDת��ΪByte
/*************************************************************
** ��������: Public_CheckStrIsBCD
** ��������: �ж��ַ��Ƿ�ΪBCD�룬����,�򷵻���Ӧ����ֵ,������:�򷵻�0xff
** ��ڲ���: val:��Ҫת��������
** ���ڲ���: ��
** ���ز���: =0xffת������,������Ϊת�����
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_CheckStrIsBCD(unsigned char val);
/*************************************************************
** ��������: Public_ConvertTwoAscToOneChar
** ��������: �������ַ��ϳ���ֵ���ַ�'0'~'F'ת��0~15����ֵ
** ��ڲ���: src:�����ַ����׵�ַ
** ���ڲ���: result:ת�����
** ���ز���: 0:ת������,1ת���ɹ�
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_ConvertTwoAscToOneChar(const unsigned char *src,unsigned char *result);
/*************************************************************
** ��������: Public_ConvertShortToBCD
** ��������: ��short������ת��BCD(2λBCD,�����ǰ�油0)
** ��ڲ���: val���ݲ�����9999,buffer�׵�ַ
** ���ڲ���: bufferת��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/		
void Public_ConvertShortToBCD(unsigned short val,unsigned char *buffer);
/*************************************************************
** ��������: Public_ConvertShortToBCDEx
** ��������: ��short������ת��BCD(3λBCD,�����ǰ�油0)
** ��ڲ���: val����,bufferת��������׵�ַ
** ���ڲ���: bufferת��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertShortToBCDEx(unsigned short val,unsigned char *buffer);
/*************************************************************
** ��������: Public_ConvertLongToBCD
** ��������: ��long������ת��BCD(4λBCD,�����ǰ�油0)
** ��ڲ���: val���ݲ�����99999999,bufferת��������׵�ַ
** ���ڲ���: bufferת��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
void Public_ConvertLongToBCD(unsigned long val,unsigned char *buffer);
/*************************************************************
** ��������: Public_ConvertLongToBCDEx
** ��������: ��long������ת��BCD(5λBCD,�����ǰ�油0)
** ��ڲ���: val����,bufferת��������׵�ַ
** ���ڲ���: bufferת��������׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void Public_ConvertLongToBCDEx(unsigned long val,unsigned char *buffer);
/*************************************************************
** ��������: Public_ConvertBCDToLong
** ��������: ��4λBCDת��long������
** ��ڲ���: buffer��Ҫת�����ݵ��׵�ַ
** ���ڲ���: ת�����
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned long Public_ConvertBCDToLong(const unsigned char *buffer);
/*************************************************************
** ��������: Public_CheckArrayValIsEqual
** ��������: �ж����������Ƿ����,
** ��ڲ���: array��Ҫ�жϵ�����1�׵�ַ,array1��Ҫ�жϵ�����2�׵�ַ,len��Ҫ�жϵĳ���
** ���ڲ���: ��
** ���ز���: 0���;1����� 
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_CheckArrayValIsEqual(const unsigned char *array,const unsigned char *array1,unsigned short len);
/*************************************************************
** ��������: Public_WriteDataToFRAM
** ��������: ��������д����,������ĳβ����У���ֽ�
** ��ڲ���: address:�����ַ;data:�����׵�ַ;datalen:д����ֽ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: FRAM_BufferWrite
*************************************************************/
void Public_WriteDataToFRAM(unsigned short address,const unsigned char *data, unsigned short datalen);
/*************************************************************
** ��������: Public_ReadDataFromFRAM
** ��������: �������������
** ��ڲ���: address:�����ַ;data:Ŀ�������׵�ַ;datalen:�������ֽ���
** ���ڲ���: data
** ���ز���: ʵ�ʶ������ֽ���
** ȫ�ֱ���: ��
** ����ģ��: FRAM_BufferRead
*************************************************************/
unsigned short Public_ReadDataFromFRAM(unsigned short address,unsigned char *data, unsigned short datalen);
/*************************************************************
** ��������: Public_GetCurTotalMile
** ��������: ��ȡ�����(�ѿ������ٶ����ͣ��������ж��ٶ�������)
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: ��λ��0.1����
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned long Public_GetCurTotalMile(void);
/*************************************************************
** ��������: Public_Mymemcpy
** ��������: src�����ݿ���des��,nΪ��������,flagΪ����(1:����,0:˳��)
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void *Public_Mymemcpy(void *des,  const void *src, unsigned short len, unsigned char flag);
/*************************************************************OK
** ��������: Public_GetCurBasicPositionInfo
** ��������: ��õ�ǰ����λ����Ϣ(�ϱ���ʽ)
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: Report_GetBasicPositionInformation
*************************************************************/
void Public_GetCurBasicPositionInfo(unsigned char *data);
/*************************************************************
** ��������: Public_GetCurPositionInfoDataBlock
** ��������: ȡ�õ�ǰλ����Ϣ(γ�ȡ����ȡ��߶�:��A.20 λ����Ϣ���ݿ��ʽ)
** ��ڲ���: �������ݵ��׵�ַ
** ���ڲ���: �������ݵ��׵�ַ
** ���ز���: ���ݳ���
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Public_GetCurPositionInfoDataBlock(unsigned char *buffer);
/*************************************************************
** ��������: Public_PlayTTSVoiceStr
** ��������: �ֶβ�������,��ͨ��Ϣ
** ��ڲ���: ��Ҫ���ŵ������׵�ַ
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Public_PlayTTSVoiceStr(const u8 *StrPtr);
/*************************************************************
** ��������: Public_PlayTTSVoiceStr
** ��������: �ֶβ�������,������Ϣ
** ��ڲ���: ��Ҫ���ŵ������׵�ַ
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
//void Public_PlayTTSVoiceStr(const u8 *StrPtr);
/*************************************************************
** ��������: Public_PlayE2ParamAlarmVoice
** ��������: ���ű�����E2�����еı�����Ϣ����
** ��ڲ���: ����ID
** ���ڲ���: 
** ���ز���: 0:������ȡ����,1������ȡ��ȷ
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Public_PlayE2ParamAlarmVoice(unsigned short ParamID);
/*************************************************************OK
** ��������: Public_ShowTextInfo
** ��������: ��ʱ��ʾ�ı���Ϣ(��ʾ������,����),��ʱ������ʾ֮ǰ�Ľ��� 
** ��ڲ���: msg�ַ����׵�ַ,time:��ʾʱ��
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: LcdShowMsgEx
*************************************************************/
void Public_ShowTextInfo(char *msg,unsigned long timeVal);
/*************************************************************
** ��������: Public_ShowSaveDataProBar
** ��������: ��ʾ�洢����
** ��ڲ���: �ٷֱ�(��0��ʼ,1.0������ʾ,ȡֵ:0.0~1.0)
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Public_ShowSaveDataProBar(float persent);
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
void Public_SetCarLoadStatus(unsigned char status);
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
void Public_QuickOpenAccount(void);
/*************************************************************
** ��������: Public_ConvertTimeToLong
** ��������: ��ʱ��ṹ����ݿ�����ת��long��
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),stTime��Ҫת����ʱ��ṹ��
** ���ڲ���: 
** ���ز���: ת�����(��0,Ϊ��Public_ConvertBCDTimeToLong�����ֳ���) 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned long Public_ConvertTimeToLong(unsigned short ctrl,TIME_T stTime);
/*************************************************************
** ��������: Public_ConvertBCDTimeToLong
** ��������: ��BCD��(6λ)���ݿ�����ת��long��
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),data��Ҫת����BCD���׵�ַ
** ���ڲ���: 
** ���ز���: 0:��ʾת���������Ҫת��,��0:������ת����� 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned long Public_ConvertBCDTimeToLong(unsigned short ctrl,unsigned char *data);
/*************************************************************
** ��������: Public_CheckTimeRangeIsValid
** ��������: �ж�ָ��ʱ�䷶Χ�Ƿ���Ч
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),startTime��Χ��ʼʱ���(����Public_ConvertTimeToLongת����ֵ),endTime��Χ����ʱ���(����Public_ConvertTimeToLongת����ֵ)
** ���ڲ���: 
** ���ز���: 1:��Ч��0:��Ч
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
unsigned char Public_CheckTimeRangeIsValid(unsigned short ctrl,unsigned long startTime,unsigned long endTime);
/*************************************************************
** ��������: Public_CheckTimeIsInRange
** ��������: �ж�ָ��ʱ���Ƿ���ָ����Χ��
** ��ڲ���: curTimeָ��ʱ��,startTime��ʼʱ��,endTime����ʱ��
** ���ڲ���: 
** ���ز���: 1:�ڷ�Χ�ڣ�0:���ڷ�Χ�� 
** ȫ�ֱ���: ��
** ����ģ��: 
*************************************************************/
unsigned char Public_CheckTimeIsInRange(unsigned long curTime,unsigned long startTime,unsigned long endTime);
/*************************************************************
** ��������: Public_CheckCurTimeIsInTimeRange
** ��������: �жϵ�ǰʱ���Ƿ���ָ��ʱ�䷶Χ��
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),startTime��Χ��ʼʱ���(����Public_ConvertTimeToLongת����ֵ),endTime��Χ����ʱ���(����Public_ConvertTimeToLongת����ֵ)
** ���ڲ���: 
** ���ز���: 1:�ڷ�Χ�ڣ�0:���ڷ�Χ�� 
** ȫ�ֱ���: ��
** ����ģ��: Public_ConvertTimeToLong,RTC_GetCurWeekDay
*************************************************************/
unsigned char Public_CheckCurTimeIsInTimeRange(unsigned short ctrl,unsigned long startTime,unsigned long endTime);
/*************************************************************
** ��������: Public_CheckTimeIsInTimeRange
** ��������: �ж�ָ��ʱ���Ƿ���ָ��ʱ�䷶Χ��
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),checkTime��Ҫ�жϵ�ʱ���(�ĸ��ֽڵ�ʱ��),startTime(����Public_ConvertTimeToLongת����ֵ),endTime��Χ����ʱ���(����Public_ConvertTimeToLongת����ֵ)
** ���ز���: 1:�ڷ�Χ�ڣ�0:���ڷ�Χ�� 
** ȫ�ֱ���: ��
** ����ģ��: Public_ConvertTimeToLong,RTC_CacWeekDay
*************************************************************/
unsigned char Public_CheckTimeIsInTimeRange(unsigned short ctrl,unsigned long checkTime,unsigned long startTime,unsigned long endTime);
/*************************************************************
** ��������: Public_CheckCurTimeIsInEarlyAlarmTimeRange
** ��������: �жϵ�ǰʱ���Ƿ���ָ��ʱ�����ǰ��Χ��
** ��ڲ���: ctrl:���ڿ�����(���������ʽ),earlyTime��ǰʱ��,startTime��Χ��ʼʱ���(����Public_ConvertTimeToLongת����ֵ),endTime��Χ����ʱ���(����Public_ConvertTimeToLongת����ֵ)
** ���ڲ���: 
** ���ز���: 1:�ڷ�Χ�ڣ�0:���ڷ�Χ�� 
** ȫ�ֱ���: ��
** ����ģ��: Public_CheckTimeIsInTimeRange
*************************************************************/
unsigned char Public_CheckCurTimeIsInEarlyAlarmTimeRange(unsigned short ctrl,unsigned long earlyTime,unsigned long startTime,unsigned long endTime);
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
void Public_ShowValue(unsigned char x,unsigned char y,char *format ,unsigned long value);
/*************************************************************
** ��������: Public_CheckIsPhoneServerRunStatus
** ��������: �ж��Ƿ�Ϊ�绰�������ʻ״̬
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 1Ϊ�绰�������ʻ״̬,0:�ǵ绰�������ʻ״̬
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Public_CheckIsPhoneServerRunStatus(void);
/*************************************************************
** ��������: Public_CheckPhoneIsSpecialNumber
** ��������: �ж��Ƿ�Ϊ�������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 0:���������,��0:Ϊ�������,
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Public_CheckPhoneIsSpecialNumber(unsigned char *phone);
/*************************************************************
** ��������: Public_CheckPhoneInformation
** ��������: ���绰����,�Ѹ���ͨ��ʱ���жϵ绰������ĺ���,(�����绰���롢���ߵ绰���벻���κ�����)
** ��ڲ���: phone����Ҫ���ĵ绰�����׵�ַ�� 
** ���ڲ���: name�����ص���ϵ������
** ���ز���: 0����ֹ����/������1: ���룻2: ������3������/����
** ȫ�ֱ���: ��
** ����ģ��: CheckPhoneInformation,
*************************************************************/
unsigned char Public_CheckPhoneInformation(unsigned char *phone,unsigned char *name);
/*************************************************************
** ��������: Public_DisposePhoneStatus
** ��������: ����绰״̬����
** ��ڲ���: phone:�绰����,status:�绰״̬��0��Ϊ�Ҷ�;1:��ͨ��2:������
** ���ڲ���: ��
** ���ز���: ����ǵ绰����״̬Ϊ������ʱ,����ֵ:0:ֱ�Ӿܽ�����;1:���ڵȴ��û���������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Public_DisposePhoneStatus(unsigned char *phone,unsigned char status);
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
void Public_LcdShowTime(unsigned char y,TIME_T stTime);
/*************************************************************
** ��������: Public_ParameterInitialize
** ��������: ������ʼ��
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Public_ParameterInitialize(void);
/************************************************************
** ��������: Public_SetEIExpandTimer
** ��������: 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void Public_SetEIExpandTimer(void (*function)(void),LZM_TIMER32 time);
/************************************************************
** ��������: Public_SetTestTimer
** ��������: 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void Public_SetTestTimer(void (*function)(void),LZM_TIMER32 time);
/************************************************************
** ��������: Public_SetOperateTimer
** ��������: 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void Public_SetOperateTimer(void (*function)(void),LZM_TIMER32 time);
/************************************************************
** ��������: Public_SetTaskTimer
** ��������: 
** ��ڲ���:
** ���ڲ���:
************************************************************/ 
void Public_SetTaskTimer(void (*function)(void),LZM_TIMER32 time);
/*************************************************************
** ��������: Public_CheckSystemLogoAndVerSion
** ��������: �����ն˲�Ʒ���ơ���Ʒ�汾���Ƿ���ȷ(2�ֽڹ�˾��־,8�ֽ��ͺ�,2�ֽ�PCB�汾,6�ֽڰ汾��,4�ֽ�У����)
** ��ڲ���: ������Ϣ�׵�ַ,crc
** ���ڲ���: ��
** ���ز���:  1:������ȷ,0:�������
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_CheckSystemLogoAndVerSion(unsigned char *pBuffer,unsigned long *checkCRC);
/*************************************************************
** ��������: Public_CheckTerminalAndPeripheralVerify
** ��������: �����նˡ������Ʒ���ơ���Ʒ�汾���Ƿ���ȷ(2�ֽڹ�˾��־,8�ֽ��ͺ�,2�ֽ�PCB�汾,6�ֽڰ汾��,4�ֽ�У����)
** ��ڲ���: ������Ϣ�׵�ַ,checkCRC����ֵ,type�������ͷ���ֵ
** ���ڲ���: ��
** ���ز���: 1:������ȷ,0:�������
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char Public_CheckTerminalAndPeripheralVerify(unsigned char *pBuffer,unsigned long *checkCRC,unsigned char *type);
/*************************************************************
** ��������: ShowStartLogo
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
void Public_SaveCurSIMICcidNumberToE2Param(void);
/*************************************************************
** ��������: Play_TTSVoiceStr
** ��������: �ֶβ�������,��ͨ��Ϣ
** ��ڲ���: ��Ҫ���ŵ������׵�ַ
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
** ��ע: ���ַ������������
*************************************************************/
void Play_TTSVoiceStr(void);

//////////////////////////////////////////////////
/****************************************************/
/*                                                  */
/* Structures                                       */
/*                                                  */
/****************************************************/
////////////////////////////////////////////////////////
#ifndef PublicConvertValueToBuffer
/////////////////////////////////
#define PublicConvertStrToASC(des,src,len)   Public_ConvertStrToASC(des,src,len)
#define PublicGetSumVerify(Pdata,datalen)   Public_GetSumVerify(Pdata,datalen)
#define PublicGetXorVerify(Pdata,datalen)   Public_GetXorVerify(Pdata,datalen)
#define PublicConvertValueToBuffer(value,buf) Public_ConvertLongToBuffer(value,buf)
#define PublicConvertBufferToLong(buf)   Public_ConvertBufferToLong(buf)
#define PublicConvertAscToBCD(destin,source, len)   Public_ConvertAscToBCD(destin,source, len)
#define PublicConvertNowTimeToBCD(data)   Public_ConvertNowTimeToBCD(data)
#define CheckStrIsBCD(val)   Public_CheckStrIsBCD(val)
#define PublicConvertAscToVal(val)  Public_ConvertAscToVal(val)
#define PublicConvertTwoAscToOneChar(val,result)  Public_ConvertTwoAscToOneChar(val,result)
#define HEX2BCD(val)   Public_HEX2BCD(val)
#define BCD2HEX(val)   Public_BCD2HEX(val)
////////////////////
#endif
/********************************************************************************
 *                            End of File
 *******************************************************************************/
#endif//__LZMPUBLIC__H__
/********************************************************************************
 *                            End of File
 *******************************************************************************/
