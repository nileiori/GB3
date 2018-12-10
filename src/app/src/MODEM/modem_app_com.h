/*
********************************************************************************
*
*                                        
*                          
*
*                        
*
* Filename      : modem_app_com.h
* Version       : V1.00
* Programmer(s) : miaoyahan
* @date         : 2014-10-24 
********************************************************************************
*/

/*
********************************************************************************
*                               MODULE
*
* Note(s) : 
********************************************************************************
*/
#include "stm32f10x.h"

#include "my_typedef.h"
#include "modem_lib.h"
#ifndef MODEM_APP_COM_H
#define MODEM_APP_COM_H


/*
********************************************************************************
*                               INCLUDES
********************************************************************************
*/


/*
********************************************************************************
*                               DEFINES
********************************************************************************
*/
#ifndef true
#define true                       1
#endif

#ifndef false
#define false                      0
#endif

#ifndef bool
#define bool unsigned char
#endif

#define ICAUC_TIME_TICK            1*SYSTICK_0p1SECOND//����ά������ִ��ʱ��

#define  UPDATE_START_SECTOR       FLASH_UPDATE_START_SECTOR
#define  UPDATE_BASE_ADDRESS       (UPDATE_START_SECTOR * SECTOR_SIZE)

#define  SECTOR_SIZE  		       0x1000ul //�����ֽ�4096	
#define  SPI_FLASH_PageSize        0x100	//�ļ�ҳ��С512�ֽ�



/*
********************************************************************************
*                                 ENUM
********************************************************************************
*/
typedef enum {
    CHANNEL_SMS    = 0x01,//����ͨ��,0ֵ����
    CHANNEL_DATA_1 = 0x02,//����ͨ��1
    CHANNEL_DATA_2 = 0x04,//����ͨ��2
}SND_TYPE;//�������ݵ�����

typedef enum {
	PHONE_NORMAL = 0,			//��������
	PHONE_MONITOR,				//����
	PHONE_TALK,					//����
	PHONE_END
}CALL_TYPE;//�绰����

typedef enum{
	COMMUNICATE_STATE = 0, 		//ͨ��ģ��״̬��0��GSM��1��GPRS
	SIGN_STRENTH,				//�ź�ǿ��
}GPRSSTATE;

typedef enum {
    GSM_PHONE_IDLE = 0,   	    //����
    GSM_PHONE_CALL_IN,   	    //�绰����
    GSM_PHONE_TALK,   		    //ͨ��
}GSM_PHONE_STATUS;//�绰״̬

//��������״ֵ̬
typedef enum{
    ICAUC_RET_SUCCED = 0,               //���سɹ�
    ICAUC_RET_FAIL,                     //����,���������Լ�ʧ��
    ICAUC_RET_CONN_FAIL,                //����,TCP����δ��
    ICAUC_RET_OPENLNK_AUTH_FAIL,        //����,������ʧ��,δ��ü�Ȩ��
    ICAUC_RET_OPENLNK_RE,               //����,�����ظ���,�ϴ���������ִ�С�
    ICAUC_RET_SENDBUF_BUSY,             //����,���ݷ��ͻ�����æ,�ϴη��Ϳ��ܻ�δ���.
    ICAUC_RET_SENDBUF_NO_CMD,           //����,ģ�鲻�ڴ�����״̬,�����ڴ�绰��������
    
      
}ICAUC_RetType;

typedef enum
{
  DOWN_IPOP,//��ǰIP
  DOWN_IPMN,//��IP  
  DOWN_IPSW,//�л�IP
  DOWN_IPTO,//ָ��IP

}APP_TYPE_DOWN;//������������

typedef enum
{
  VAL_IPMAIN=0,//��IP
  VAL_IPBACK,//����IP
  VAL_IPTO,//ָ��IP
  VAL_HEART,//���� 

}APP_TYPE_VAL;//��������

/*
********************************************************************************
*                                STRUCT
********************************************************************************
*/
typedef struct
{
    u8 status;
    u8 telephone[23];      //���е绰����
}CALL_RECORD;//�绰��ؽṹ��



typedef struct
{
    SM_PARAM msgParam;	   // ��Ϣ��������	
}SMS_RECORD;//���Ų������


typedef struct {
	u8	rec_chanel;		   // 1 : GPRS;0:����

}UART2_RECEIVE;


typedef  void (*ICAUC_FUN_READ)(u8*,u16);  //������պ���ָ��
/*
********************************************************************************
*                          EXTERN  VALUE
********************************************************************************
*/
extern  CALL_RECORD     gPhone;         //ͨ�����
extern  SMS_RECORD      gSmsRecord;     //�������
extern  UART2_RECEIVE	uart2_recdat;   //����2����

/*
********************************************************************************
*                               FUNCTIONS
********************************************************************************
*/
void GSM_PWR_ON(void);
void GSM_PWR_OFF(void);


FunctionalState Communication_TimeTask(void);
FunctionalState Communication_CloseGprsTimeTask(void);
void Communication_TaskSet(u8 state);
void Communication_Init(void); 
u8   communication_CallPhone (u8 *pCode, CALL_TYPE type);
u16  Communication_SndMsg2Svr(u8 channel, u8 * srcdat, u16 srclen, u8 *phone);
u16  communicatio_GetMudulState(GPRSSTATE type);
u8   communicatio_GetSignalIntensity(void);
u8   communicatio_NetState(u8 channel);


void Communication_Close(void);
void Communication_Open(void);
void Communication_Close2(void);


#if 0
void MOD_InitCRC(void);
void MOD_StopCRC(void);
u32  MOD_CalcCRC(u8 *pBuffer, u32 BufferLength);
#endif
void MOD_CloseMainIp(void);
void MOD_GetIccidBcdValue(u8 *Bcdbuff);


void SetTelVolumeIncr(void);
void SetTelVolumeDecr(void);
u8   GetTelVolume(void);

void SetModTtsVolumeIncr(void);
void SetModTtsVolumeDecr(void);
u8   GetModTtsVolume(void);
/*
FunctionalState ICAUC_TimeTask(void);
ICAUC_RetType   ICAUC_OpenLnk(void);
ICAUC_RetType   ICAUC_SendBuff(u8* buff,u16 len);
void ICAUC_RegReadFun(ICAUC_FUN_READ pFun);

u8   ICAUC_GetLnkStatus(void);*/


#endif

