#ifndef __VDR_USB_H
#define __VDR_USB_H

//#include "stm32f2xx.h"
#include "stm32f10x.h"
#include <integer.h>

#define VDR_USB_READ_BUFFER_SIZE  1024

/**************************************************************************
//��������VDRUsb_CreatVdrFile
//���ܣ�����*.VDR�ļ�
//���룺��
//�������
//����ֵ����
//��ע��VDR�ļ��ĸ�ʽ��GB/T19056-2012��¼B
//�ú�������USB_USR_ReadDataForUpdataApp��������
***************************************************************************/
void VDRUsb_CreatVdrFile(void);
void Vdr_Usb_UniNm2(WCHAR *p_name);
#endif