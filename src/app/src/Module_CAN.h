/*************************************************************************
* Copyright (c) 2013,�������������¼����������޹�˾
* All rights reserved.
* �ļ����� : Module_CAN.h
* ��ǰ�汾 : 1.0
* ������   : Shigle
* �޸�ʱ�� : 20113��6��10��
*************************************************************************/

#include "my_typedef.h"
#include "stm32f10x.h"

#ifndef _MODULE_CAN_H_
#define _MODULE_CAN_H_


/*****  �궨����   *****/



#define DEBUG_PRINT_CAN  0 //CAN������־��ӡ


#define CAN_PWR_ON()    GPIO_WriteBit(GPIOD, GPIO_Pin_15, Bit_SET)
#define CAN_PWR_OFF()   GPIO_WriteBit(GPIOD, GPIO_Pin_15, Bit_RESET)


#define iCAN1   0
#define iCAN2   1
#define CAN_CH_MAX 1 //CANͨ������� 1:ֻ����CAN1 2:ͬʱ����CAN1��CAN2

#define  CAN_MAX_SEND_LEN    3// 35
#define  CAN_MAX_RECE_NUM    160 //20// 20 //���ջ�����󲻳���40��.

#define  CAN_MAX_SEND_NMB    80// 35
#define MAX_NUM_FILTER_ID  2 //ÿ·CANͨ�� ���˵�ID��������




//---------- ����ΪBASICCAN SJA1000�Ĵ����� ----------
/*
#define     CAN_INTERFACE_0K    0                            //SJA1000�ӿ�����
#define     CAN_BUS_0K          0                          //CAN���߹�������
#define     CAN_INTERFACE_ERR   0XFF                         //SJA1000�ӿڴ���?
#define     CAN_QUITRESET_ERR   0XFD                         //SJA1000�����˳���λģʽ
#define     CAN_INITOBJECT_ERR  0XFC                         //��ʼ�����������˲�����
#define     CAN_INITBTR_ERR     0XFB                         //��ʼ�����߶�ʱ������
#define     CAN_INITOUTCTL_ERR  0XFA                         //��ʼ��������ƴ���
#define     CAN_INITSET_ERR     0XF9                         //��ʼ��ʱ�ӷ�Ƶ��������
#define     CAN_BUS_ERR         0XF8                         //SJA1000�������ݴ�

#define  CAN_SEND_OK    0
#define  CAN_SEND_ERR   1


*/


/*****  �ṹ�嶨����   *****/

/*****  ���� CAN�ع���ID�ṹ��   *****/

typedef struct {
	u8  iMaskID;
	u32 MaskID;
}CAN_MASK_ID;

#pragma pack(1)
typedef struct{
  u32 Can_ID;
  u8  Can_CtlCode;
  u8  CanChar[8];
}CAN_DATA;
#pragma pack()



typedef struct {
	u8 data[100];	//����
	u8 snd;			//�ѷ���
	u8 len;			//�ܳ���
}CAN_REC;



/*****  ���� CAN�����ṹ��   *****/

#pragma pack(1)
typedef struct{
  u8  CAN_ID_BYTE_3;
  u8  CAN_ID_BYTE_2;
  u8  CAN_ID_BYTE_1;
  
  u8  CAN_ID_BYTE_0: 5;     //28�ֽ�CAN����ID
  u8  CAN_Data_Original :1; //bit 29: ��ʾ���ݲɼ���ʽ: 0:ԭʼ���� 1: �ɼ�����ļ���ֵ
  u8  CAN_Frame_Type: 1; //bit 30: ��ʾ֡����: 0:��׼֡ 1:��չ֡
  u8  CAN_Channel: 1; //bit31: ��ʾCANͨ����, 0: CAN1 1: CAN2
   
  u32 CAN_Sample_Interval;

}PARAMETER_0110;
#pragma pack()






typedef struct tagPARAMETER_CAN{
u16 CAN_UploadPeriodxS; //CAN����ͨ��1�ϴ�ʱ����(s), 0��ʾ���ϴ�
u16  UploadTimeCnt;
u8   FlagUplodTimeArrived;
u32 CAN_SamplePeriodxMS; //CAN����ͨ��1�ɼ�ʱ����(ms) , 0��ʾ���ɼ�
u16  SampleTimeCnt;
u8   FlagSampleTimeArrived;
u32 CAN_Filter_ID_Index;

}PARAMETER_CAN , * PPARAMETER_CAN;


typedef struct tagPARAMETER_CAN_SINGLE{
PARAMETER_0110  CAN_Sample_Single[ MAX_NUM_FILTER_ID ];
u8    CAN_Sample_Single_Num;
}PARAMETER_CAN_SINGLE , * PPARAMETER_CAN_SINGLE;


#pragma pack(1)
typedef struct tagCAN_ITEM{
   u8  CAN_ID_BYTE_3: 5;  //28�ֽ�CAN����ID
  u8  CAN_Data_Original :1; //bit 29: ��ʾ���ݲɼ���ʽ: 0:ԭʼ���� 1: �ɼ�����ļ���ֵ
   u8  CAN_Frame_Type: 1; //bit 30: ��ʾ֡����: 0:��׼֡ 1:��չ֡
 u8  CAN_Channel: 1; //bit31: ��ʾCANͨ����, 0: CAN1 1: CAN2

  
  u8  CAN_ID_BYTE_2;
  u8  CAN_ID_BYTE_1;
  u8  CAN_ID_BYTE_0;

  u8  CAN_Data[8];
  u8  CAN_Time[5];
  
}CAN_ITEM;
#pragma pack()




typedef struct
{


u8  read;

u8 write;
  
u8   receIndex;        //����ָ��; ÿ�ν�����һ����������Ϣ���߽���Ϣȡ�� �����1;
u8   UnreadIndex;      //���յ������ݵ�δ��ָʾ;
u8   receNum;          //�������ݰ�����(��λ:13�ı���)
u8   sendLen;         //���͵����ݳ���(��λ:13�ı���)
u8   sendIndex;     //�������ݵ�����(��λ:13�ı���)
u8   sendBuffer[CAN_MAX_SEND_LEN][13];	//���� ���������ݵĻ���

CAN_ITEM  receBuffer[CAN_MAX_RECE_NUM];

}CAN_ISR_DATA;






/*****  ���� ������   *****/


/********************************************************************
* ���� : DecodeCan_EvTask
* ���� : ����CAN���յ����ݰ�
********************************************************************/
extern void DecodeCan_EvTask(void);

/********************************************************************
* ���� : Time_Now_Get_BCD
* ���� : ��ȡ��ǰ��RTCʱ��,ת����Э��Ҫ���BCD��ʽ
* ��� : ����BCDʱ���ָ��
********************************************************************/
extern void Time_Now_Get_BCD( u8 * pTimer );

/********************************************************************
* ���� : CAN_Isr_Rx
* ���� : �ɼ�CAN�����жϵ�����
* ���� :  * CANx  : STEM32 CAN�ṹ��ָ��
********************************************************************/

extern void CAN_Isr_Rx( CAN_TypeDef* CANx );

/*******************************************************************
* ���� : CAN1_Sample_Sw
* ���� : CAN ��������
* ���� : 
*       iCANChannel:    0: CAN1ͨ��   1: CAN2ͨ��
*            bSwCtr:    0: �ر�CAN�����ж�  1: ����CAN1�����ж�   
*******************************************************************/
extern void CAN_Sample_Sw(  u8 iCANChannel , u8 bSwCtr );

/*******************************************************************
* ���� : Can_Data_Package
* ���� : ���CAN����
* ���� : 
*       iCANCh:    0: CAN1ͨ��   1: CAN2ͨ��
* ��� :
*        pdata:    ���հ����ݵ�ָ��
*******************************************************************/
extern u16 Package_Can_Data( u8 iCANCh,  u8 *pdata );


/*******************************************************************
* ���� : ����ID0x0110~0x01FF  ����ID �����ɼ���������
* ���� : CAN ����ID �����ɼ�����
* ���� :  CANParmamerID : CAN����ID  Ŀǰֻ��ȡֵ0x0110�� 0x0111
*******************************************************************/
extern void UpdataOnePram_Sample_Single(  u32 CANParmamerID  );


/********************************************************************
* ���� : Can_TimeTask
* ���� : ��ʼ��ϵͳ����֮ CAN
********************************************************************/
extern void CAN_TimeTask_Init(void);


/********************************************************************
* ���� : Can_TimeTask
* ���� : ϵͳ����֮ CAN
********************************************************************/
extern FunctionalState  Can_TimeTask(void);

/*******************************************************************
* ���� : ����ID0x0110~0x01FF  ����ID �����ɼ���������
* ���� : CAN ����ID �����ɼ�����
* ���� :  CANParmamerID : CAN����ID  Ŀǰֻ��ȡֵ0x0110�� 0x0111
*******************************************************************/
extern  void UpdataOnePram_Sample_Single(  u32 CANParmamerID  );
/*******************************************************************
* ���� : Get_Buff_Item_Length
* ���� : ��û��λ��������ݳ���
* ���� : 
*       
* ��� :
*        
*******************************************************************/
u8 Get_Buff_Item_Length(void);

/********************************************************************
* ���� : Can_Data_Struct_Iinit
* ���� : ��ʼ��can���ݽṹ
********************************************************************/
void Can_Data_Struct_Iinit(void);
/********************************************************************
* ���� : UpdataOnePram_SamplePeriod_CAN1
* ���� : CAN����ͨ��1 �ɼ�ʱ��������ˢ��
* ˵�� : ����ID0x0100  CAN����ͨ��1 �ɼ�ʱ����(ms)��0 ��ʾ���ɼ� 
********************************************************************/
void UpdataOnePram_SamplePeriod_CAN1( void );
/********************************************************************
* ���� : UpdataOnePram_UploadPeriod_CAN1
* ���� : CAN����ͨ��1 �ϴ�ʱ����(s)����ˢ��
* ˵�� : ����ID0x0101  CAN ����ͨ��1 �ϴ�ʱ����(s)��0 ��ʾ���ϴ�
********************************************************************/
void UpdataOnePram_UploadPeriod_CAN1( void );
/*******************************************************************
* ���� : Check_Filter_Frame
* ���� : �ҳ����˵������ݶ���
* ���� : 
*       iCANCh:    0: CAN1ͨ��   1: CAN2ͨ��
* ��� :
*       
*******************************************************************/

u8 Check_Filter_Frame(u8 iCANCh);


#endif
