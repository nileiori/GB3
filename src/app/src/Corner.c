/********************************************************************
//��Ȩ˵��	:Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����	:Corner.c		
//����		:�յ㲹��
//�汾��	:
//������	:dxl
//����ʱ��	:2014.7
//�޸���	:
//�޸�ʱ��	:
//�޸ļ�Ҫ˵��	:
//��ע		:ACC ON����ʻ����ǰʱ�̽Ƕ�-��һʱ�̽Ƕ� > �趨ֵ���ϱ�1��λ����Ϣ
***********************************************************************/ 
/*************************�ļ�����***********************/
#include "include.h"
#include "VDR.h"

/*************************�궨��***********************/

/*************************�ⲿ����***********************/
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ����Э����ģʽ

/*************************ȫ�ֱ���***********************/

static u32  CornerAngleMax = 0;//�յ�Ƕ�,��׼��4�ֽ�
/*************************���ر���***********************/

/*************************��������***********************/
/*********************************************************************
//��������	:Corner_UpdataPram
//����		:�յ�ģ����±���
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
//		:
*********************************************************************/
void Corner_UpdataPram(void)
{
    u8	TmpData[4] = {0};
	u8	PramLen = 0;

	//������ϵ��
	PramLen = EepromPram_ReadPram(E2_CORNER_REPORT_ID, TmpData);
	if(4 != PramLen)
	{
        CornerAngleMax = 5;
    }
    else
    {
        CornerAngleMax = 0;
		CornerAngleMax |= (TmpData[0] << 24);
		CornerAngleMax |= (TmpData[1] << 16);
		CornerAngleMax |= TmpData[2] << 8;
        CornerAngleMax |= TmpData[3];
    }
}
/*********************************************************************
//��������	:Corner_TimeTask
//����		:�յ㲹����ʱ����
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:1�����1��
*********************************************************************/
FunctionalState  Corner_TimeTask(void)
{
    u32 CornerAngle = 0;
    static u32 LastCornerAngle = 0;
    u8  Acc = 0;
    u8  RunFlag = 0;
    u8  Navigation = 0;
    
    Acc = Io_ReadStatusBit(STATUS_BIT_ACC);
    Navigation = Io_ReadStatusBit(STATUS_BIT_NAVIGATION);
    RunFlag = VDRDoubt_GetCarRunFlag();
    CornerAngle = Gps_ReadCourse();
    
    if((1 == Acc)&&(1 == Navigation)&&(1 == RunFlag))
    {
        if(CornerAngle >= LastCornerAngle)
        {
            if((CornerAngle - LastCornerAngle) >= 180)
            {
                if((360+LastCornerAngle - CornerAngle) >= CornerAngleMax)
                {
                    //SetEvTask(EV_REPORT); //λ���ϱ�
									  if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))
										{
										
										}
										else
										{
                        Report_UploadPositionInfo(CHANNEL_DATA_1);
                        Report_UploadPositionInfo(CHANNEL_DATA_2);
										}
                }
            }
            else
            {
                if((CornerAngle - LastCornerAngle) >= CornerAngleMax)
                {
									  if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))
										{
										
										}
										else
										{
                        //SetEvTask(EV_REPORT); //λ���ϱ�
                        Report_UploadPositionInfo(CHANNEL_DATA_1);
                        Report_UploadPositionInfo(CHANNEL_DATA_2);
										}
                }
            }
           
        }
        else
        {
            if((LastCornerAngle - CornerAngle) >= 180)
            {
                if((360+CornerAngle - LastCornerAngle) >= CornerAngleMax)
                {
									  if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))
										{
										
										}
										else
										{
                        //SetEvTask(EV_REPORT); //λ���ϱ�
                        Report_UploadPositionInfo(CHANNEL_DATA_1);
                        Report_UploadPositionInfo(CHANNEL_DATA_2);
										}
                }
            }  
            else
            {
                if((LastCornerAngle - CornerAngle) >= CornerAngleMax)
                {
									
				            if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))
										{
										
										}
										else
										{
                        //SetEvTask(EV_REPORT); //λ���ϱ�
                        Report_UploadPositionInfo(CHANNEL_DATA_1);
                        Report_UploadPositionInfo(CHANNEL_DATA_2);
										}
                }
            }
        }
    }
    LastCornerAngle = CornerAngle;
    
    return ENABLE;
}