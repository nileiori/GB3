/*******************************************************************************
 * File Name:			usb_app.h 
 * Function Describe:	
 * Relate Module:		
 * Writer:				joneming
 * Date:				2012-05-15
 * Rewriter:
 * Date:		
 *******************************************************************************/
#ifndef __USB_APP_H_
#define __USB_APP_H_
///////////////////////////
#include "stm32f10x.h"

//ģ�鹤��ģʽ
typedef enum {
	USB_WORK_STATUS_EMPTY = 0,
	USB_WORK_STATUS_INSERT,  //����U��
	USB_WORK_STATUS_ERROR,  //��װU��ʧ��
	USB_WORK_STATUS_ENUM,    //ö�ٳɹ�
	USB_WORK_STATUS_READLIST,//����ļ��б�
	USB_WORK_STATUS_DEL,     //�Ƴ�U��	
	USB_WORK_STATUS_UPDATA,  //������
	USB_WORK_STATUS_UPDATA_FINISH,//�������
	USB_WORK_STATUS_UPDATA_ERROR,  //��������
	USB_WORK_STATUS_PROTECTED,//U�̱���
	USB_WORK_STATUS_NO_U_DISK,//û��U��
	USB_WORK_STATUS_READ_FILE_ERROR,//���ļ�����
	USB_WORK_STATUS_START_SAVE,//���ڱ�������
	USB_WORK_STATUS_SAVE_FINISH,//�����ļ��ɹ�
	USB_WORK_STATUS_SAVE_ERROR,//�����ļ�����
	USB_WORK_STATUS_MAX
}USBWORKSTATUS;
/////////////////////////////////////
/*************************************************************
** ��������: USB_ClearUsbUpdataFlag
** ��������: ���U��������ϱ�־
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
*************************************************************/
void USB_ClearUsbUpdataFlag(void);
/*************************************************************
** ��������: USB_SetUsbUpdataFlag
** ��������: ��ΪU��������ϱ�־
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
*************************************************************/
void USB_SetUsbUpdataFlag(void);
/*************************************************************
** ��������: USB_GetReadUsbUpdataFlag
** ��������: 
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 1:ΪU��������ϱ�־,0:����
*************************************************************/
unsigned char USB_GetReadUsbUpdataFlag(void);
/*************************************************************
** ��������: USB_AppInitialize
** ��������: 
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void USB_AppInitialize(void);
/*************************************************************
** ��������: Usb_FindFileAndSaveToUdisk
** ��������: 
** ��ڲ���: 	 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Usb_FindSDcardFileAndSaveToUdisk(unsigned char type,unsigned char *startTime,unsigned char *endTime);
/*************************************************************
** ��������: Usb_AppTimeTask
** ��������: 
** ��ڲ���: 	 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
FunctionalState Usb_AppTimeTask(void);
/*************************************************************
** ��������: Usb_Exti1Isr
** ��������: USB�������,��EXTI1_IRQHandler�ж��е���
** ��ڲ���: 	 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Usb_Exti1Isr(void);
#endif//__USB_APP_H_
/*******************************************************************************
 *                             end of module
 *******************************************************************************/
