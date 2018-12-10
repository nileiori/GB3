/*******************************************************
Copyright (C),E-EYE Tech. Co.,Ltd.
Author: WYF
Version:V1.0
Finish Date:2012-6-12
Description:TTS����
*******************************************************/ 
#include "include.h"

u8  TtsPowerFlag = 0;//dxl,TTS��Դ�����־,0Ϊ�ϵ�,1Ϊ����,��������ǰ���ȼ��ñ�־�Ƿ�Ϊ0,��Ϊ0��ʾ���ڲ���
/*******************************************************
Description:���ڽ�������
Input:
Output:
Return:
*******************************************************/
void TTSDev_UartInit(void) 
{
	/* dxl,2016.9.1����
    GpioInInit(TTS_RDY); 
    GpioOutInit(TTS_P);      //��GPS���ʹ��ڹ���,TTS_P������GPS���ʹ���ʧЧ.
	*/
}      
/*******************************************************
Description:������������������,��������   
Input: �������ݵ�ַ,���ͳ���
Output:
Return: 0:���Է��� 1:����ʧ��
*******************************************************/
u8 TTSDerv_SendUartData(u8 *Ptr,u16 Len)
{
    //Gps_SendData(Ptr,Len);dxl,2016.9.1����
    return 1;
}
/*******************************************************
Description:TTS��Դ����
Input: S_OFF_TTS_POWER \ S_ON_TTS_POWER
Output:
Return: 0:���Է��� 1:����ʧ��
*******************************************************/
void TTSDerv_CtrlTTSPower(u8 Val) 
{
    if(Val)
    {
        TtsPowerFlag =1;
        //GpioOutOn(TTS_P);dxl,2016.9.1����
    }
    else
    {
        TtsPowerFlag =0;
        //GpioOutOff(TTS_P);dxl,2016.9.1����
    }
}
//-----------------------------------------------
//����:��λTTS
//����:
//���:
//��ע:
//-----------------------------------------------
void TTSDev_ResetTTS(void) 
{
    vu16 i;
    //GpioOutOff(TTS_P);       //��λTTSdxl,2016.9.1����
    for(i=0; i<0XFFFF; i++); //Delay 
    //GpioOutOn(TTS_P);dxl,2016.9.1����
}
/******************************************************************************
**                            End Of File
******************************************************************************/