
#ifndef _RECORDER_H
#define _RECORDER_H

#include<stddef.h>
#include<stdio.h>
#include<string.h>

/**************************�ɼ������������б�*************************************/
#define CMD_GET_PERFORM_VERSION                 0x00 /*�ɼ���¼��ִ�б�׼�汾    */
#define CMD_GET_DRIVER_INFORMATION              0x01 /*�ɼ���ǰ��ʻԱ��Ϣ        */
#define CMD_GET_SYS_RTC_TIME                    0x02 /*�ɼ���¼��ʵʱʱ��        */
#define CMD_GET_ADD_MILEAGE                     0x03 /*�ɼ��ۼ���ʻ���          */
#define CMD_GET_IMPULSE_RATIO                   0x04 /*�ɼ���¼������ϵ��        */
#define CMD_GET_CAR_INFORMATION                 0x05 /*�ɼ�������Ϣ              */
#define CMD_GET_STATUS_SIGNAL_CONFIG            0x06 /*�ɼ���¼��״̬�ź�������Ϣ*/
#define CMD_GET_UNIQUE_ID                       0x07 /*�ɼ���¼��Ψһ�Ա��      */
#define CMD_GET_SPECIFY_RECORD_SPEED            0x08 /*�ɼ�ָ������ʻ�ٶȼ�¼    */
#define CMD_GET_SPECIFY_RECORD_POSITION         0x09 /*�ɼ�ָ����λ����Ϣ��¼    */
#define CMD_GET_SPECIFY_RECORD_ACCIDENT_DOUBT   0x10 /*�ɼ�ָ�����¹��ɵ��¼    */
#define CMD_GET_SPECIFY_RECORD_OVERTIME_DRIVING 0x11 /*�ɼ�ָ���ĳ�ʱ��ʻ��¼    */
#define CMD_GET_SPECIFY_RECORD_DRIVER_IDENTITY  0x12 /*�ɼ�ָ���ļ�ʻ����ݼ�¼  */
#define CMD_GET_SPECIFY_RECORD_EXTERN_POWER     0x13 /*�ɼ�ָ�����ⲿ�����¼    */
#define CMD_GET_SPECIFY_RECORD_CHANGE_VALUE     0x14 /*�ɼ�ָ���Ĳ����޸ļ�¼    */
#define CMD_GET_SPECIFY_LOG_STATUS_SPEED        0x15 /*�ɼ�ָ�����ٶ�״̬��־    */
/*16H~~~~17HԤ��                                                                 */


/**************************���ò����������б�*************************************/
#define CMD_SET_CAR_INFORMATION                 0x82 /*���ó�����Ϣ              */
#define CMD_SET_CAR_INIT_INSTALL_TIME           0x83 /*���ü�¼�ǳ��ΰ�װ����    */
#define CMD_SET_STATUS_SIGNAL_CONFIG            0x84 /*����״̬��������Ϣ        */
#define CMD_SET_RECORDER_TIME                   0xC2 /*���ü�¼��ʱ��            */
#define CMD_SET_IMPULSE_RATIO                   0xC3 /*���ü�¼������ϵ��        */
#define CMD_SET_INIT_MILEAGE                    0xC4 /*���ó�ʼ�����            */
/*0C5H~~0CFHԤ��                                                                 */


/**************************�����������б�*****************************************/
#define CMD_IDENTIFY_INTO                       0xE0 /*����򱣳ּ���״̬        */
#define CMD_IDENTIFY_MEASURE_MILEAGE            0xE1 /*�������������          */
#define CMD_IDENTIFY_MEASURE_IMPULSE_RATIO      0xE2 /*��������ϵ��������      */
#define CMD_IDENTIFY_MEASURE_SYS_RTC_TIME       0xE3 /*����ʵʱʱ��������      */
#define CMD_IDENTIFY_RETURN                     0xE4 /*������������״̬          */
/*0E5~~0EFHԤ��                                                                  */

/**************************�����������б�*****************************************/
#define CMD_NAME_VERSION                 ("ִ�б�׼�汾���  ")              
#define CMD_NAME_DRIVER_MSG              ("��ǰ��ʻ����Ϣ    ")
#define CMD_NAME_RTC                     ("ʵʱʱ��          ")
#define CMD_NAME_MILEAGE                 ("�ۼ���ʻ���      ")
#define CMD_NAME_IMPULSE_RATIO           ("����ϵ��          ")
#define CMD_NAME_CAR_MSG                 ("������Ϣ          ")
#define CMD_NAME_STATUS_MSG              ("״̬�ź�������Ϣ  ")
#define CMD_NAME_ONLY_ID                 ("��¼��Ψһ�Ա��  ")
#define CMD_NAME_SPEED                   ("��ʻ�ٶȼ�¼      ")
#define CMD_NAME_POSITION                ("λ����Ϣ��¼      ")
#define CMD_NAME_DOUBT                   ("�¹��ɵ��¼      ")
#define CMD_NAME_OVERTIME                ("��ʱ��ʻ��¼      ")
#define CMD_NAME_DRIVER                  ("��ʻ����ݼ�¼    ")
#define CMD_NAME_POWER                   ("�ⲿ�����¼      ")
#define CMD_NAME_VALUE                   ("�����޸ļ�¼      ")
#define CMD_NAME_SPEED_STA               ("�ٶ�״̬��־      ")


/**************************����***************************************************/


typedef struct _RECORDER_QUEUE_RX {                  /*��¼�ǽ��ն��нṹ��      */
	u16  in;
	u16  out;
	u8  nState;
	u8  nChannel;                          /*����ͨ�� 0��RS232ͨ��   ��0��GSMͨ��*/
	u8  nBuf[1024];
}RECORDER_QUEUE_RX;

typedef struct _RECORDER_QUEUE_TX {                  /*��¼�Ƿ��Ͷ��нṹ��      */
	u16 in;
	u16 out;
	u8  nState;
	u8  nBuf[1024];
}RECORDER_QUEUE_TX;

typedef  struct _ST_RECORDER_WIRE {                  /*��¼�����߲ɼ����ݽṹ��   */
    u8  DataBlock;      //���ݿ��־,0x03��AA 75,������AA 75
	u16 AckNum;         //Ӧ����ˮ��,��Ӧƽ̨�·���
	u8  Cmd;            //������
	u16 Cmd808;         //ƽ̨�·���ָ������жϴ�������
	TIME_T Time_start;  //��ʼʱ��
	TIME_T Time_end;    //����ʱ��
	u16 Max;            //ƽ̨�·���������
	u16 MaxCnt;         //������������
	u16 SendTotal;      //���߷���һ�����ݵ�������ݿ���
    u16 AllCnt;         //�ְ�����ʱ���ܰ���
    u16 AllRemainCnt;   //�ְ�����ʱ���ܰ������һ��������
    u16 AddOverflg;     //���������ݿ������ܿ���С������λ
    u16 AddCnt;         //�ְ�����ʱ�ĵ�ǰ����
    u16 BlockNum;       //ָ��ʱ����ڵ������ݿ���
    u8  All_07;         //һ����ȫ������0-7���ݱ�־��Ŀǰ�ñ�־ֻ��Ա�������ʹ��
    u16 Len;
	u8  Buf[1024];
	u8  PatchNum;//8003�·����ش�����
	u8  PatchCnt;//8003�·����ش���������
	u16 PatcBuf[50];//���֧��50������
#ifdef HUOYUN
    u16 HY_Cmd_SendNum;//���������ݿ��������ڷ��Ͷ���900�ֽڣ�����ƽ̨���ר��
    u16 HY_Cmd_SendRemain;//����900�������
#endif

}ST_RECORDER_WIRE;

typedef  struct _ST_RECORDER_HEAD {//��¼������ͷ������Э�����                 
    u8  acknum808H;//808Э���е�Ӧ����ˮ��
    u8  acknum808L;
    u8  cmd808;//808Э���е�������    
    u8  head1;//55
    u8  head2;//7a    
    u8  cmd;//������
    u8  lenH;//���ݿ鳤��
    u8  lenL;
    u8  back;//������
    u8  DataBuf[1024];//���ݿ�����

}ST_RECORDER_HEAD;

typedef  struct _ST_RECORDER_FLASH {//��¼������ͷ������Э�����                 
    u8  time[4];//32λʱ��
    u8  buf[250];//��flash�ж�ȡ������
    u8  ver;//У��

}ST_RECORDER_FLASH;

typedef  struct  {    

    u16  start[1];       //��ʼ�ַ�
    u16  time_ymd[6];    //������
    u16  separator_1[1]; //�ָ���
    u16  time_hm[4];     //ʱ��
    u16  separator_2[1]; //�ָ���
    u16  num[20];        //���ƺ���
    u16  ftype[4];       //�ļ����ͼ���׺��, .VDR
    
}RECORDER_FILE_VDR_NAME;//��¼������USB�ļ����ṹ��

typedef  struct  {    

    u8  cmd;       //���ݴ��뼴��¼��������
    u8  name[18];  //����
    u8  length[4]; //����
    u8  buf[700];  //����
    
}RECORDER_FILE_VDR_DATA;//��¼������USB�ļ����ṹ��

typedef  struct  {    

	u16	Sector  ; //����
	u16	Step    ; //����
    u16	StepLen ; //���� 
    
}RECORDER_FLASH_ADDR;//��¼��������flash�д洢�ĵ�ַ
/**************************�ⲿ����********************************************/

extern ST_RECORDER_WIRE   St_RecorderWire;

/*******************************************************************************
* Function Name  : RecorderWired_TimeTask
* Description    : ��¼���¼�����,100ms����һ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState RecorderWired_TimeTask(void);
/*********************************************************************
//��������	:RecorderCom_WirelessTimeTask
//����		:������ʻ��¼�Ƕ�ʱ����
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
FunctionalState RecorderWireless_TimeTask(void);
/*******************************************************************************
* Function Name  : Recorder_USBHandle
* Description    : ��ʻ��¼��USB���ݴ�����Ҫ�������ñ�־λ
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void Recorder_USBHandle(void);
/*******************************************************************************
* Function Name  : Recorder_MileageTest
* Description    : ��¼�Ǽ�������->������,��ÿ�����һ��
* Input          : None            
* Output         : None
* Return         : None
*******************************************************************************/
void Recorder_MileageTest(void);
/*********************************************************************
//��������	:Recorder_GetCarInformation
//����		:��ȡ������Ϣ����������VIN�����ƺ��룬���Ʒ���
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:������06H
*********************************************************************/
u8 Recorder_GetCarInformation(u8 *pBuffer);
/*******************************************************************************
* Function Name  : Recorder_Stop15MinuteSpeed
* Description    : ͣ��ǰ15�����ٶȣ����ݰ���VIN��ͣ��ʱ�䡢ͣ��ǰ15����ƽ���ٶ�
* Input          : *p  : ָ��15����ƽ���ٶ���Ϣ�������              
* Output         : None
* Return         : �������ݳ���
*******************************************************************************/
u8 Recorder_Stop15MinuteSpeed(u8 *p);
/**
* @brief  ���ݿ�ʼʱ�����ʱ�����ʱ������ܰ�������ȡָ�����ݰ�����
* @param  tpye:��ѯ����
* @param  Start:��ʼʱ��
* @param  End:����ʱ��
* @param  PackNum:Ҫ���ҵ����ݰ��ţ�ֻ��pData != NULLʱ����Ч
* @param  *pData:ָ����ȡ�����ݵ�ַ��
* @retval ��pData==NULLʱ�����ݰ��������򷵻����ݳ���
*/
u16 RecorderDataPack(u8 Type, TIME_T Start, TIME_T End,u16 PackNum,u8 *pData);
/**
* @brief  ���ݿ�ʼʱ�����ʱ�����ʱ������ܰ�������ȡָ�����ݰ�����,USBר��
* @param  tpye:��ѯ����
* @param  Start:��ʼʱ��
* @param  End:����ʱ��
* @param  PackNum:Ҫ���ҵ����ݰ��ţ�ֻ��pData != NULLʱ����Ч
* @param  *pData:ָ����ȡ�����ݵ�ַ��
* @retval ��pData==NULLʱ�����ݰ��������򷵻����ݳ���
*/
u16 RecorderDataPackUsb(u8 Type, TIME_T Start, TIME_T End,u16 PackNum,u8 *pData);
/**
* @brief  ��¼������ת��FLASH�洢����
* @param  tpye:��¼������
* @retval ����FLASH�洢����
*/
u8 RecorderCmdToRegister(u8 Type);
/*********************************************************************
//��������	:RecorderPatchGet
//����		:������ʻ��¼�ǽ���8003ָ������
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/

void RecorderPatchGet(u8 *pRec, u16 RecLen);
/*******************************************************************************
* Function Name  : Recorder_SendData
* Description    : ���ͼ�¼�Ƿ�������
* Input          : - *pData ��ָ��Ҫ�������ݵĵ�ַ
*                  - Len    :���͵����ݵĳ���             
* Output         : None
* Return         : �ɹ������� ���򷵻ؼ�
*******************************************************************************/
u8 Recorder_SendData(u8 *pData, u16 Len);

/**
* @brief  GPRS�������ݵ��ܰ���
* @param  tpye:��ѯ����
* @param  Start:��ʼʱ��
* @param  End:����ʱ��
* @retval �����ݰ���
*/
u16 RecorderWireSendNum(u8 Type, TIME_T Start, TIME_T End,u16 *pRemain);
u16 RecorderWireSend(u8 type,u16 Count,u8 *pData);
#endif



