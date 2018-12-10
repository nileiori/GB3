#ifndef _JOINTECHOIL_H_
#define _JOINTECHOIL_H_


#include "stm32f10x.h"

#define DAMP_DEFAULT 60 //Ĭ�ϵ�����ֵ , ��λ:��  ȡֵ��Χ5~600

/***************   ö������ ������   ********************************/


typedef enum{

 STAGE_JOINTECHOIL_NULL = 0 , 
 STAGE_JOINTECHOIL_E_OILCURRENT ,//��ȡ��ǰ������Ϣ
 STAGE_JOINTECHOIL_H_DAMP , //��ѯ/��������
 STAGE_JOINTECHOIL_I_TEMPERATURE_COMPENSATION , //�����Ƿ����±겹��
 STAGE_JOINTECHOIL_R_CRITICALPARAMETERS , //���ؼ�����
 STAGE_JOINTECHOIL_MAX //��ͨ������λ���������� ״̬��������
} ENUM_STAGE_JOINTECHOIL ;


typedef enum{

 TASK_JOINTECHOIL_MAIN = 0 , //������
 TASK_JOINTECHOIL_SUB , //������
 TASK_JOINTECHOIL_MAX //
} ENUM_TASK_JOINTECHOIL ;

/***************   �ṹ�� ������   ********************************/

/* ���ھ�ͨ��λ������ JT606 Э�� �ṹ�� */

#define MAX_JOINTECH_DATA_LEN 40

typedef struct tagSTT_JOINTECH_OILCOST_PROTOCOL{
u8 id  ;
u8 cmd ;
u8 len ;
u8 data[ MAX_JOINTECH_DATA_LEN ];
u8 verify ;
}STT_JOINTECH_OILCOST_PROTOCOL;





/* ���ھ�ͨ��λ������ JT606 ����COM���ƽṹ��  */

#define MAX_COM_BUF_LEN  40
typedef struct tagSTT_JOINTECH_COM{
u16 buflen ;
u8  buf[ MAX_COM_BUF_LEN ];
}STT_JOINTECH_COM;




/* ���ھ�ͨ��λ������ JT606 �������ṹ�� */

typedef struct tagSTT_OILCOST{
u8  ID ;  //��ǰ��λ�������ı��
u8  iBaud; //ͨ�Ų����� ѡ�����
u16 TankVolume ; //��ǰ������� �ֱ���: 1L 
u16 TankZero; //��ǰ�ĵ���ֵ
u16 TankFull; //��ǰ�ĵ���ֵ
u16 LiquidLevelPercent ; //��ǰҺλ�߶���ֱȣ����ֵΪ10000;
u16 OilVolumeCur; //��ǰ����
u16 OilVolumeAdjust ; //��ǰ�����������λֵ , 0~4095

u8 iSetup ; //��װ���
char Version[10]; //�汾��
u16 Damp; //���� ��λ: �� ��Χ��5~600

u8  Temperature;         //�¶�ֵ ���ֱ���: 1Cel ƫ��: -40Cel
u8  IsTempCompensation ; //�Ƿ�����Ʈ���� 0:������ 1: ����

u16 SampleDamp; //���� ��λ: �� ��Χ��5~600
u16 SampleFrequence ; //�ɼ�Ƶ��ֵ , �ֱ���: 0.1kHz
u16 SampleTankZero ; //�ɼ�������ֵ
u16 SampleTankFull ; //�ɼ�������ֵ

u8  FlagErrorFrequency ; //JT606��������  0:����  1:���� 
u8  FlagErrorTankZero ; //��ֵ���  0:����  1:���� 
u8  FlagErrorTankFull ; //��ֵ���  0:����  1:���� 

}STT_JOINTECH_OILCOST ; 




/* ���ھ�ͨ��λ������ JT606 ����״̬�����ƽṹ�� */

typedef struct
{
  u8 StageMain;  //��״̬
  u8 StageSub ;  //��״̬
  u8 IsExist;    //�Ƿ����
  u8 ReCheckCnt ; //�ط�������
  u8 IsAckOK;   //�Ƿ��յ���ȷ��Ӧ��
  u8 ErrorCnt ; //ͳ�����������ܴ���
  u8 ErrorFlag ; //�����־
}STT_JOINTECH_OILCOST_CTRL ;


/***************   �ⲿ���� ������ ********************************/
FunctionalState JointechOiLCost_TimeTask(void);


/********************************************************************
* ���� : JointechOiLCost_CopyBuf
* ���� : �Ӵ��ڽ���Buf��������
* ˵��  
********************************************************************/
extern void  JointechOiLCost_CopyBuf( u8 * pInBuf, u16 InLength );

/*********************************************************************
*���� : JointechOiLCost_OilVolumeCur_Get
*���� : ��ȡ�����ڵ�ǰʣ������ֵ��
*��ע : �ֱ���: 0.01��/λ ,ƫ����: 0
*********************************************************************/
extern u16 JointechOiLCost_OilVolumeCur_Get( void );


/*********************************************************************
*���� : JointechOiLCost_OilVolumeCur_Get
*���� : ��ȡ�����ڵ�ǰҺλ�߶���ֱȣ� ���ֵΪ10000��
*��ע : �ֱ���: 0.01% ,ƫ����: 0
*********************************************************************/
extern u16 JointechOiLCost_OilLevelPercentCur_Get( void );

/*********************************************************************
*���� : JointechOiLCost_COM_Buf_Write
*���� : ���ھ�ͨ������λ������ COM ����д���ֽ���
*��ע :
*********************************************************************/
void JointechOiLCost_COM_Buf_Write(  u8 * pInBuf , u16 InLength );



/********************************************************************
* ���� : JointechOiLCost_Error_Set
* ���� : ��ͨ������λ���������ϱ�־ ��λ
* ˵��  
********************************************************************/
void  JointechOiLCost_Error_Set( void );


/********************************************************************
* ���� : JointechOiLCost_Error_Clr
* ���� : ��ͨ������λ���������ϱ�־ ���
* ˵��  
********************************************************************/
void  JointechOiLCost_Error_Clr( void );


/********************************************************************
* ���� : JointechOiLCost_Error_Set
* ���� : ��ͨ������λ���������ϱ�־ ��λ
* ˵��  
********************************************************************/
u8  JointechOiLCost_Error_Get( void );



/********************************************************************
* ���� : JointechOiLCost_Error_Set
* ���� : ��ͨ������λ������ ����
* ˵��  
********************************************************************/
void  JointechOiLCost_Online_Set( void );



/********************************************************************
* ���� : JointechOiLCost_Error_Clr
* ���� : ��ͨ������λ������  ����
* ˵��  
********************************************************************/
void  JointechOiLCost_Online_Clr( void );

/********************************************************************
* ���� : JointechOiLCost_Error_Set
* ���� : ��ѯ��ͨ������λ�������Ƿ� ����
* ˵��  
********************************************************************/
u8  JointechOiLCost_Online_Get( void );

/*********************************************************************
//��������	:JointechOiLCost_OilProtocolCheck
//����		:��ͨ�ͺ�Э����,��ȷ�Ļ�����ú���JointechOiLCost_CopyBuf,
//          :���ͺ�ָ�����g_sttJointechComRx.buf�У�Ȼ�����
//��ע		:,dxl,2014.5.12����,�ͺ���32PIN 485�ӿڸĵ�16PIN 232�ӿ�
//          :����ֵΪ1��ʾ���ж�Ϊ�ͺ�ָ�Ϊ0��ʾ�����ͺ�ָ��
*********************************************************************/
u8 JointechOiLCost_OilProtocolCheck(u8 *pBuffer, u16 BufferLen);

/*******************************************************************************
** ��������: JointechOiLCost_GetSubjoinInfoCurOilVolume
** ��������: ȡ�õ�ǰ����������Ϣ��(����������ϢID�����ȡ�������Ϣ��)
** ��ڲ���: ��Ҫ��������������Ϣ�����׵�ַ
** ���ڲ���: 
** ���ز���: ����������Ϣ�����ܳ���
*******************************************************************************/
unsigned char JointechOiLCost_GetSubjoinInfoCurOilVolume(unsigned char *data);
/*******************************************************************************
** ��������: JointechOiLCost_GetSubjoinInfoCurOilLevelPercent
** ��������: ȡ�õ�ǰ�����ٷֱȸ�����Ϣ��(����������ϢID�����ȡ�������Ϣ��)
** ��ڲ���: ��Ҫ���������ٷֱȸ�����Ϣ�����׵�ַ
** ���ڲ���: 
** ���ز���: �����ٷֱȸ�����Ϣ�����ܳ���
*******************************************************************************/
unsigned char JointechOiLCost_GetSubjoinInfoCurOilLevelPercent(unsigned char *data);

#endif
