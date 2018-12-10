/************************************************************************
//�������ƣ�Lock2.c
//���ܣ�ʵ������������
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2016.4
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1�����������ܣ�
//1.��������ʹ�ܱ�־������IDΪ0xF28C������Ϊʹ�ܣ�1��ʱ��δ��������������־Ϊ0���ſ����ù��ܣ�
//2.����������״̬ʱ���������ɵ�LCD����ʾ��ʹ��ǰ���������������޷�Ӧ��ͨ��ģ�����ӹرգ�
//3.ֻ�е�����������������Ϊjt1.gghypt.net�����ݷ�������������Ϊjt2.gghypt.netǰ���£����յ��˵������Ŀ���ָ��
//��ʱʱ�䣨2���ӣ����˲Żᷢ�Ϳ��ٿ���ָ�ƽ̨��ʵ�ֽ������ܣ�������־��1��
//4.��������ʹ�ܱ�־����Ϊ0����ֹ��ʱ����˳���ѿ�����־��0���Ա���һ��ʹ��������ʱ�����·��Ϳ��ٿ���ָ��

*************************************************************************/

/********************�ļ�����*************************/
#include <stdio.h>
#include "Lock2.h"
#include "modem_app_first.h"
#include "modem_app_second.h"
#include "taskschedule.h"
#include "Gdi.h"
#include "EepromPram.h"
#include "Lcd.h"
/********************���ر���*************************/
static u8 OpenAccountFlag = 0;//���ٿ�����־��1Ϊ�ѿ�����0Ϊδ����������ֵ��ͬ��δ����
static u8 Lock2EnableFlag = 0;//ʹ�ܱ�־��1Ϊʹ�ܣ�0Ϊ��ʹ�ܣ�����ֵΪ������ʱ���رոù��ܣ���ͬ�ڲ�ʹ�ܣ�
static u8 Lock2Flag = 0;//��������־��1Ϊ�����������˿��ٿ������̣���0Ϊδ����
static u8 InitFlag = 0;//��ʼ����־
static u8 OpenAccountCmdRxFlag = 0;//���ٿ���ָ����ձ�־��1Ϊ�ѽ��գ�0Ϊδ���ա�ָ���������������Ŀ��ٿ���ָ��
static u8 OpenAccountOnline = 0;//���ٿ������߱�־��1Ϊ�������ߣ�0Ϊ���Ӳ�����
static u8 OpenAccountRequestFlag = 0;//���ٿ��������־��1:������0:������
/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
extern TIME_TASK MyTimerTask[MAX_TIMETASK];

/********************���غ�������*********************/
//static u8 Lock2_ParameterIsRight(void);
static void Lock2_Init(void);

/********************��������*************************/

/**************************************************************************
//��������Lock2_TimeTask
//���ܣ�ʵ������������
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע����������ʱ����1���ӽ���1�Σ������������Ҫ���ô˺���
***************************************************************************/
FunctionalState Lock2_TimeTask(void)
{
	
   static u8 state = 0;
   static u32 count = 0;
	
  // u8 Buffer[5];	
	 
	
   if(0 == InitFlag)
   {
       state = 0;
       count = 0;
		   Lock2Flag = 0;
       InitFlag = 1;
       Lock2_Init();
   }
   else
   {
       if((1 == Lock2EnableFlag)&&(1 != OpenAccountFlag))//����������������
       {
				   if(0 == state)//�������������IP
           {
						 /*
					     if(0 == Lock2_ParameterIsRight())//����������IP��������ȷ
					     {
					         count++;
						       if(1 == count)//�ȹر�����
							     {
							         Net_First_Close();
										   LcdClearScreen();
                       LcdShowStr(10,20,"����ʹ��ǰ������",0);
							     }
									 else if(count >= 10)//��ʾ������ȷ������
									 {
									     LcdClearScreen();
                       LcdShowStr(10,20,"��������ȷ�������Ͷ˿ں�",0);
									 }

					    }
							else
							{
							*/
								  LcdClearScreen();
                  LcdShowStr(10,20,"����ʹ��ǰ������",0);
								  OpenAccountCmdRxFlag = 0;
								  ClrTimerTask(TIME_COMMUNICATION);
                  ClrTimerTask(TIME_MENU);
									Lock2Flag = 1;
							    state++;
							//}
					
					 } 
           else if(1 == state)//����Ƿ��յ����ٿ���ָ��
           {
							 if(1 == OpenAccountCmdRxFlag)//�յ��˵��������Ŀ��ٿ���ָ��
							 {
								   LcdClearScreen();
                   LcdShowStr(10,20,"�յ���������������",0);
								   OpenAccountCmdRxFlag = 0;
								   OpenAccountRequestFlag = 1;
								   OpenAccountOnline = 0;
								   count = 0;
							     Communication_Init();
									 SetTimerTask(TIME_COMMUNICATION,1);
								   state++;
							 }
							 else
							 {
								   LcdClearScreen();
                   LcdShowStr(10,20,"ʹ��ǰ������",0);
							     if(1 == BBGNTestFlag)
									 {
									     count++;
										   if(count >= 1200)
											 {
												    LcdClearScreen();
                            LcdShowStr(10,20,"�յ���������������",0);
											      OpenAccountCmdRxFlag = 0;
								            OpenAccountRequestFlag = 1;
								            OpenAccountOnline = 0;
								            count = 0;
							              Communication_Init();
									          SetTimerTask(TIME_COMMUNICATION,1);
								            state++;
											 }
									 }
							 }
           }
           else if(2 == state)//���Ϳ��ٿ���ָ��
           {
							 count++;
							 if(count >= 1800)//30���Ӻ��λ
							 {
							     count = 0;
								   InitFlag = 0;
							 }
							 else if(1 == OpenAccountOnline)
							 {
							     if(0 == count%30)
									 {
								       RadioProtocol_OpenAccount(CHANNEL_DATA_1);
										   LcdClearScreen();
                       LcdShowStr(10,20,"���ٿ���ָ���ѷ���",0);
									 }
							 }
           }
					 else
					 {
					     Lock2EnableFlag = 0;
					 }	 
       }
       else
       { 
           count = 0;
           state = 0;
				   if(1 == Lock2Flag)
           {
						   Lock2Flag = 0;
						   if(0 == Lock1_GetLock1Flag())
							 {
						      Lcd_SetMainRedrawFlag();
                  Communication_Init();
                  SetTimerTask(TIME_COMMUNICATION,1);
                  SetTimerTask(TIME_MENU,1);
								  LcdClearScreen();
                  LcdShowStr(10,20,"����������ͨ�ź���ʾ����",0);
							 }
           }
       }
   }
    
		
    return ENABLE;      
}
/**************************************************************************
//��������Lock2_UpdataParameter
//���ܣ�������ز���
//���룺��
//�������
//����ֵ����
//��ע�������ò������������������ݷ�������������ʹ�ܣ�ʱ��Ҫ���øú���
***************************************************************************/
void Lock2_UpdataParameter(void)
{
    InitFlag = 0;
}
/**************************************************************************
//��������Lock2_OpenAccountAck
//���ܣ�������ٿ���Ӧ��
//���룺��
//�������
//����ֵ����
//��ע�����յ�ƽ̨���ٿ���Ӧ��ʱ���øú���
***************************************************************************/
void Lock2_OpenAccountAck(u8 *pBuffer, u16 BufferLen)
{
	  u16 Addr;
	  u16 AckCmd;
	  u8 *p = NULL;
	
	  
	  p = pBuffer;
	  p += 2;
	  AckCmd = 0;
	  AckCmd |= *p++ << 8;
	  AckCmd |= *p++;
	  if(((0x0110 == AckCmd)&&(0 == *p))||((0x0110 == AckCmd)&&(5 == *p)))//��һ�ο���ע��ɹ�Ӧ��0���Ժ�ע��Ӧ��5
		{
	      OpenAccountRequestFlag = 0;
        OpenAccountFlag = 1;
	      Addr = FRAM_QUICK_ACCOUNT_ADDR;
        FRAM_BufferWrite(Addr, &OpenAccountFlag, FRAM_QUICK_ACCOUNT_LEN);
			  //������Ȩ�룬���������Ա���ע������
			  ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
		}
}
/**************************************************************************
//��������Lock2_OpenAccountOnline
//���ܣ���λOpenAccountOnline
//���룺��
//�������
//����ֵ��1����ǰΪ���ٿ������ӣ�0����ǰΪ�������ӣ�����Ӧ��ʱ�����������
//��ע����ͨ��ģ�������ɹ�������ô˺���
***************************************************************************/
void Lock2_OpenAccountOnline(void)
{
    OpenAccountOnline = 1; 
}
/**************************************************************************
//��������Lock2_GetOpenAccountFlag
//���ܣ���ȡ���ٿ�����־
//���룺��
//�������
//����ֵ��1:Ϊ�ѿ�����0:δ����
***************************************************************************/
u8 Lock2_GetOpenAccountFlag(void)
{
    return OpenAccountFlag; 
}
/**************************************************************************
//��������Lock2_GetLock2Flag
//���ܣ���ȡ��������־
//���룺��
//�������
//����ֵ��1:Ϊ������0:Ϊδ����
***************************************************************************/
u8 Lock2_GetLock2Flag(void)
{
    return Lock2Flag; 
}
/**************************************************************************
//��������Lock2_GetEnableFlag
//���ܣ���ȡ������ʹ�ܱ�־
//���룺��
//�������
//����ֵ��1:Ϊʹ��;0:Ϊ��ʹ��
***************************************************************************/
u8 Lock2_GetEnableFlag(void)
{
    return Lock2EnableFlag; 
}
/**************************************************************************
//��������Lock2_GetOpenAccountRequestFlag
//���ܣ���ȡOpenAccountRequestFlag
//���룺��
//�������
//����ֵ��1���п��ٿ�������0���޿��ٿ�������
//��ע����ͨ��ģ�������ɹ�������ô˺���
***************************************************************************/
u8 Lock2_GetOpenAccountRequestFlag(void)
{
    return OpenAccountRequestFlag; 
}
/**************************************************************************
//��������Lock2_TrigOpenAccount
//���ܣ� �������ٿ���
//���룺��
//�������
//����ֵ����
//��ע�����յ�ƽ̨���ٿ���Ӧ��ʱ���øú���
***************************************************************************/
void Lock2_TrigOpenAccount(void)
{
	  if(0 == OpenAccountFlag)
		{
        OpenAccountCmdRxFlag = 1; 
		}
		else
		{
			  LcdClearScreen();
        LcdShowStr(10,20,"�ѿ���",0);
		}
}

/**************************************************************************
//��������Lock2_Init
//���ܣ�������ز���
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
static void Lock2_Init(void)
{
    u8 Buffer[5];
    u8 BufferLen;
    
    BufferLen = EepromPram_ReadPram(E2_LOCK2_ENABLE_FLAG_ID,Buffer);
    if(E2_LOCK2_ENABLE_FLAG_ID_LEN == BufferLen)
    {
        Lock2EnableFlag = Buffer[0];
        if(Lock2EnableFlag > 1)
        {
            Lock2EnableFlag = 0;
        }
    }
    else
    {
        Lock2EnableFlag = 0;
			  
    }
		
    BufferLen = FRAM_BufferRead(Buffer, FRAM_QUICK_ACCOUNT_LEN, FRAM_QUICK_ACCOUNT_ADDR);
		if(FRAM_QUICK_ACCOUNT_LEN == BufferLen)
    {
        OpenAccountFlag = Buffer[0];
        if(OpenAccountFlag > 1)
        {
            OpenAccountFlag = 0;
        }
    }
    else
    {
        OpenAccountFlag = 0;
    }
		
		///if((0 == Lock2EnableFlag)&&(1 == OpenAccountFlag))
		if(0 == Lock2EnableFlag)
		{
		    OpenAccountFlag = 0;
        FRAM_BufferWrite(FRAM_QUICK_ACCOUNT_ADDR, &OpenAccountFlag, FRAM_QUICK_ACCOUNT_LEN);
		}
}
/**************************************************************************
//��������Lock2_ParameterIsRight
//���ܣ��������Ƿ���ȷ
//���룺��
//�������
//����ֵ��������ȷ����1�����󷵻�0
//��ע�����������������򱸷ݷ������������ò���ȷ������������
***************************************************************************/
/*
static u8 Lock2_ParameterIsRight(void)
{
    u8 Buffer[25];
    u8 BufferLen;
    
    BufferLen = EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID,Buffer);
    if(0 != BufferLen)
    {
        if(0 == strncmp((const char *)Buffer,"jt1.gghypt.net",14))
        {
            BufferLen = EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID,Buffer);
            if(0 != BufferLen)
            {
                if(0 == strncmp((const char *)Buffer,"jt2.gghypt.net",14))
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
*/































