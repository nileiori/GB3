/************************************************************************
//�������ƣ�Blind.c
//���ܣ���ģ��ʵ��ä���������ܡ�
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2014.10
//�汾��¼���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1����Ҫʵ��˫���ӵ�ä������������1������2��ä���洢�����Ƕ����ġ��ֿ��ġ�
//�����ֻʹ��������1������2δ����������2 TCP�˿ڲ�������Ϊ0ʱ��ʾ����2����������
//�����洢����϶�Ϊһ��ÿ���ն����ߺ�����¼��һ�ο��Ƿ���Ҫ����������δ������
//���ݣ���ʼ������ÿ�ΰ�5��λ����Ϣ���������ֻ���յ�ƽ̨Ӧ���ŻᲹ����һ����
//����һֱ������ǰ������෢��10�Σ�ÿ��20�롣��10�κ���Ȼû��Ӧ��
//���ȴ�30���Ӻ����ط���ǰ����
*************************************************************************/

/********************�ļ�����*************************/
#include <stdio.h>
#include <string.h>

#include "Blind.h"
#include "modem_lib.h"
#include "GPIOControl.h"
#include "other.h"
#include "modem_app_com.h"
#include "spi_flashapi.h"
#include "rtc.h"
#include "Public.h"
#include "spi_flash.h"
#include "RadioProtocol.h"
#include "EepromPram.h"
#include "Gnss_app.h"
#include "Gdi.h"
#include "ff.h"
#include "usbh_usr.h"
#include "VDR_Pub.h"
#include "VDR_Usart.h"
#include "queue.h"

/********************���ر���*************************/
static BLIND_STRUCT Link1Blind;
static s16 Link1SearchSector;
static u16 Link1TimeCount = 0;
static u32 Link1WaitTimeCount = 0;

static BLIND_STRUCT Link2Blind;
static s16 Link2SearchSector;
static u16 Link2TimeCount = 0;
static u32 Link2WaitTimeCount = 0;
static u16 Link2OpenFlag = 0;//����2�Ƿ�����־��1Ϊ������0Ϊδ����
static s16  BlindStepLen = 0;//ä��ÿ���ĳ���
static s16  BlindPacketNum = 0;//ä������ʱ�����������ʵ�ʳ���ʱΪ5�������ʱΪ15-20��
//static FIL BlindFile;

//static u8 PrintBuffer[2200] = {0};
//static u16 PrintPacketNum = 0;
//static u8 ReadBuffer[1000] = {0};
//static u8 ParseBuffer[1000] = {0};
//static u8 LastTailTwoByte[2] = {0,0};//��һ�����ݿ����β����2�ֽ�
//static u8 CurHeadTwoByte[2] = {0,0};//��ǰ���ݿ鿪ͷ��ͷ����2�ֽ�
//static u8 FourByte[4] = {0};
//static u8 FrameSearchType = 0;
//static u32 ParseBufferCount = 0;
//static u16 FrameStart = 0xffff;
//static u16 FrameEnd = 0xffff;
//static u16 SearchLen = 0;

static u8 Link1BlindSaveEnableFlag = 0;
//static u8 Link2BlindSaveEnableFlag = 0;

/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/
extern u8 RadioShareBuffer[];
extern TIME_T CurTime;
extern u8   GBTestFlag;//0:������������ģʽ;1:������ģʽ
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
extern u32 Link1ReportTimeSpace;//ͬLink1ReportTimeһ���������ⲿ����
extern Queue  VdrTxQ;//VDR���Ͷ���
extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ����Э����ģʽ
/********************���غ�������*********************/
static void Blind_Link1Init(void);//����1������ʼ��
static void Blind_Link1Search(void);//����1����ȷ��ä����¼���ϱ��ĳ�ʼλ��
static void Blind_Link1SearchSector(s16 SearchSector);//���ĳ�������е�ä����Ϣ
static u8 Blind_Link1Report(void);//����1�ϱ�һ��ä������
static void Blind_Link1ReportAck(void);//����1ä������Ӧ����
static u8 Blind_GetLink1ReportFlag(void);//��ȡ����1ä���ϱ���־
static void Blind_StartLink1Report(void);//��������1ä������
static void Blind_Link1Erase(void);//��������1ä���洢����
static u8 Blind_Link1Save(u8 *pBuffer, u8 length, u8 attribute);//�洢һ������1��ä������
static void Blind_StartLink1Report(void);

static void Blind_Link2Init(void);//����2������ʼ��
static void Blind_Link2Search(void);//����2����ȷ��ä����¼���ϱ��ĳ�ʼλ��
static void Blind_Link2SearchSector(s16 SearchSector);//���ĳ�������е�ä����Ϣ
static u8 Blind_Link2Report(void);//����2�ϱ�һ��ä������
static void Blind_Link2ReportAck(void);//����2ä������Ӧ����
static u8 Blind_GetLink2ReportFlag(void);//��ȡ����2ä���ϱ���־
static void Blind_StartLink2Report(void);//��������2ä������
static void Blind_Link2Erase(void);//��������2ä���洢����
static u8 Blind_Link2Save(u8 *pBuffer, u8 length, u8 attribute);//�洢һ������2��ä������
static void Blind_StartLink2Report(void);

static void Blind_UpdateLink2OpenFlag(void);//���±���Link2OpenFlag

//static u8 Blind_SearchFrame(u16 *SearchLen, u16 *FrameStart, u16 *FrameEnd, u8 *pBuffer, u16 BufferLen, u8 Type);
//static u32 Blind_PrintFrame(u8 *pBuffer, u16 BufferLen, u32 TimeCount);
//static u16 Blind_CheckDataIsOK(void);
/********************��������*************************/
/**************************************************************************
//��������Blind_TimeTask
//���ܣ�ʵ��ä����������
//���룺��
//�������
//����ֵ����
//��ע��ä��������ʱ����50ms����1�Σ������������Ҫ���ô˺�����
//���ڽ���FTP����ʱ��Ҫ�رո�������Ϊä����¼��̼�Զ����������һ��洢��
//ע�����˭�ر��˸�����˭��Ҫ����򿪸������ϵ�����Ĭ���Ǵ򿪵�
***************************************************************************/
FunctionalState Blind_TimeTask(void)
{
	  //static u8 flag1  = 0;
	  static u8 flag2  = 0;
	
    if(1 == GBTestFlag)//������ʱ���������ڼ�¼��־
    {
        return DISABLE;
    }
		
		if((1 == BBGNTestFlag)&&(1 == Link1BlindSaveEnableFlag))//dxl,2016.5.19ä����¼�ڼ�
		{
		    return ENABLE;
		}
    
    /**************����1ä������****************/
    if(BLIND_STATE_INIT == Link1Blind.State)//��ʼ��������ر���
    {
        Blind_Link1Init();//��ɺ��Զ�ת��BLIND_STATE_SEARCH 
    }
    else if(BLIND_STATE_SEARCH == Link1Blind.State)//�𲽲���ȷ����ǰ��ä������λ��
    {
        Blind_Link1Search();//��ɺ��Զ�ת��BLIND_STATE_REPORT
    }
    else if(BLIND_STATE_REPORT == Link1Blind.State)//ä���ϱ�
    {
			  //if((0 == flag1)&&(1 == BBGNTestFlag))
				//{
					  //Blind_Erase(0xff);
					  //LcdClearScreen(); 
            //LcdShowCaptionEx((char *)"ä��1�Լ����",2); 
			     // Blind_CheckDataIsOK();//dxl,2016.5.19
					  //flag1 = 1;
				//}
        if(Link1ReportTimeSpace < 5)//dxl,2016.5.10����ä�����ʱ��1��һ����Ϊ��ʹä��������ɿ��������޶�
				{
				
				}
				else
				{
        Link1WaitTimeCount = 0; 
        if(CHANNEL_DATA_1 == (CHANNEL_DATA_1&GetTerminalAuthorizationFlag()))//ֻ������1����ʱ�Ų���
        {
            Link1TimeCount++;
            if(Link1TimeCount >= BLIND_REPORT_DELAY_TIME)//20��
            {
                Link1TimeCount = 0; 
                Link1Blind.ReportSendNum++;
                if((Link1Blind.ReportSendNum > 0)&&(Link1Blind.ReportSendNum <= 10))
                {
                    if(0 != Link1Blind.LastReportSector[0])
                    {
                        Link1Blind.ReportSector = Link1Blind.LastReportSector[0];
                        Link1Blind.ReportStep = Link1Blind.LastReportStep[0];
                    }
                    if(0 == Blind_Link1Report())//ä���ϱ�
                    {
						            Gnss_StartBlindReport();
                        Link1Blind.State = BLIND_STATE_WAIT;//ת��ȴ�״̬
                        
                    }
                }
                else if(Link1Blind.ReportSendNum > 10) 
                {
                    Link1Blind.State = BLIND_STATE_WAIT;//ת��ȴ�״̬
                }
            }
					}
				}
    }
    else if(BLIND_STATE_WAIT == Link1Blind.State)//�ȴ�
    {
        Link1WaitTimeCount++;
        if(Link1WaitTimeCount >= (18000*SECOND))//300���ӣddxl,2016.5.19�ܿ�ä�����ʱ�Σ�3Сʱ��
        {
            Link1WaitTimeCount = 0;
            Link1Blind.State = BLIND_STATE_INIT;
        }
    }
    else//�쳣
    {
        Link1Blind.State = BLIND_STATE_INIT;
    }


    /**************����2ä������****************/
    if(0 == Link2OpenFlag)
    {
        return ENABLE;
    }

    if(BLIND_STATE_INIT == Link2Blind.State)//��ʼ��������ر���
    {
        Blind_Link2Init();//��ɺ��Զ�ת��BLIND_STATE_SEARCH
        
    }
    else if(BLIND_STATE_SEARCH == Link2Blind.State)//�𲽲���ȷ����ǰ��ä������λ��
    {
        Blind_Link2Search();//��ɺ��Զ�ת��BLIND_STATE_REPORT
    }
    else if(BLIND_STATE_REPORT == Link2Blind.State)//ä���ϱ�
    {
			  if((0 == flag2)&&(1 == BBGNTestFlag))
				{
					  LcdClearScreen(); 
            LcdShowCaptionEx((char *)"ä��2�Լ����",2); 
					  flag2 = 1;
				}
        Link2WaitTimeCount = 0; 
        //if(0x01 == (0x01&GetTerminalAuthorizationFlag()))//ֻ������1����ʱ�Ų���
        //{
            Link2TimeCount++;
            if(Link2TimeCount >= BLIND_REPORT_DELAY_TIME)//20��
            {
                Link2TimeCount = 0; 
                Link2Blind.ReportSendNum++;
                if((Link2Blind.ReportSendNum > 0)&&(Link2Blind.ReportSendNum <= 10))
                {
                    if(0 != Link2Blind.LastReportSector[0])
                    {
                        Link2Blind.ReportSector = Link2Blind.LastReportSector[0];
                        Link2Blind.ReportStep = Link2Blind.LastReportStep[0];
                    }
                    if(0 == Blind_Link2Report())//ä���ϱ�
                    {
                        Link2Blind.State = BLIND_STATE_WAIT;//ת��ȴ�״̬
                    }
                }
                else if(Link2Blind.ReportSendNum > 10)
                {
                    Link2Blind.State = BLIND_STATE_WAIT;//ת��ȴ�״̬
                }
            }
        //}
    }
    else if(BLIND_STATE_WAIT == Link2Blind.State)//�ȴ�
    {
        Link2WaitTimeCount++;
        if(Link2WaitTimeCount >= (18000*SECOND))//300����dxl,2016.5.19�ܿ�ä�����ʱ�Σ�3Сʱ��
        {
            Link2WaitTimeCount = 0;
            Link2Blind.State = BLIND_STATE_INIT;
        }
    }
    else//�쳣
    {
        Link2Blind.State = BLIND_STATE_INIT;
    }

    

    return ENABLE;
}
/**************************************************************************
//��������Blind_Save
//���ܣ�����ĳͨ����һ��ä�����ݣ�λ�û㱨���ݣ�
//���룺ͨ���ţ�ä�����ݣ����ݳ���
//�������
//����ֵ��0��ʾ�ɹ���1��ʾʧ�ܣ���������ȷ�᷵��ʧ��
//��ע��ͨ����ֻ��ΪCHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2����֮һ
***************************************************************************/
u8 Blind_Save(u8 channel, u8 *pBuffer, u8 length)
{
    u8 flag = 0;
    u8 attribute;
	
	  if(1 == BBXYTestFlag)//����Э����ʱ����¼ä��
		{
		    return flag;
		}

    attribute = channel;

    if(CHANNEL_DATA_1 == channel)
    {
        flag = Blind_Link1Save(pBuffer, length, attribute);
        return flag;
    }
    else if(CHANNEL_DATA_2 == channel)
    {
        flag = Blind_Link2Save(pBuffer, length, attribute);
        return flag;
    }
    else
    {
        return 1;
    }
}
/**************************************************************************
//��������Blind_Erase
//���ܣ�����ĳͨ�������д洢����
//���룺ͨ���ţ�ֻ��ΪCHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2����֮һ
//�������
//����ֵ��0��ʾ�ɹ���1��ʾʧ�ܣ���������ȷ�᷵��ʧ��
//��ע��Ҫ�������е�ä�����ݣ�������Ϊ0xff��������Զ�������̼�֮ǰ���Ȳ���
//����ä��������Ϊä���洢������̼��洢������ͬһ��flash�ռ�
***************************************************************************/
u8 Blind_Erase(u8 channel)
{

    Link1Blind.State = BLIND_STATE_INIT;
    Link2Blind.State = BLIND_STATE_INIT;

    if(CHANNEL_DATA_1 == channel)
    {
        Blind_Link1Erase();
        return 0;
    }
    else if(CHANNEL_DATA_2 == channel)
    {
        Blind_Link2Erase();
        return 0;
    }
    else if(0xff == channel)
    {
        Blind_Link1Erase();
        Blind_Link2Erase();
        return 0;
    }
    else
    {
        return 1;
    }

    
    
}
/**************************************************************************
//��������Blind_StartReport
//���ܣ���ʼĳ��ͨ����ä������
//���룺ͨ����
//�������
//����ֵ����
//��ע��ͨ����ֻ��ΪCHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2����֮һ
***************************************************************************/
void Blind_StartReport(u8 channel)
{
    if(CHANNEL_DATA_1 == channel)
    {
        Blind_StartLink1Report();
    }
    else if(CHANNEL_DATA_2 == channel)
    {
        Blind_StartLink2Report();
    }
}
/**************************************************************************
//��������Blind_ReportAck
//���ܣ�ä������Ӧ����
//���룺ͨ���ţ�ֻ��ΪCHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2����֮һ
//�������
//����ֵ��0��ʾ�ɹ���1��ʾʧ�ܣ���������ȷ�᷵��ʧ��
//��ע���յ�ä������Ӧ�������ô˺���
***************************************************************************/
u8 Blind_ReportAck(u8 channel)
{
    if(CHANNEL_DATA_1 == channel)
    {
        Blind_Link1ReportAck();
        return 0;
    }
    else if(CHANNEL_DATA_2 == channel)
    {
        Blind_Link2ReportAck();
        return 0;
    }
    else
    {
        return 1;
    }   
}
/**************************************************************************
//��������Blind_GetLink2OpenFlag
//���ܣ���ȡ��2�����ӿ�����־
//���룺��
//�������
//����ֵ����2�����ӿ�����־
//��ע��Link2OpenFlagΪ1��ʾ�����˵�2�����ӣ�Ϊ0��ʾû�п���
***************************************************************************/
u8 Blind_GetLink2OpenFlag(void)
{
    return Link2OpenFlag;
}
/**************************************************************************
//��������Blind_EnableLink1Save
//���ܣ�ʹ������1��¼ä��
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void Blind_EnableLink1Save(void)
{
    Link1BlindSaveEnableFlag = 1;
}
/**************************************************************************
//��������Blind_DisableLink1Save
//���ܣ���ֹ����1��¼ä��
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void Blind_DisableLink1Save(void)
{
    Link1BlindSaveEnableFlag = 0;
}
/**************************************************************************
//��������Blind_EnableLink2Save
//���ܣ�ʹ������2��¼ä��
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void Blind_EnableLink2Save(void)
{
//    Link2BlindSaveEnableFlag = 1;
}
/**************************************************************************
//��������Blind_DisableLink2Save
//���ܣ���ֹ����2��¼ä��
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void Blind_DisableLink2Save(void)
{
//    Link2BlindSaveEnableFlag = 0;
}
/**************************************************************************
//��������Blind_Link1Init
//���ܣ�����1������ʼ��
//���룺��
//�������
//����ֵ����
//��ע��ä��������ʱ������Ҫ���ô˺���
***************************************************************************/
static void Blind_Link1Init(void)
{
    u8 i;

    //����2�����ӵ�TCP�˿ںţ��ж��Ƿ�����2�����ӣ�����������Ҫ��ִ洢����ÿ�θò���״̬�仯ʱ��Ҫ����flash
    //Ϊ�˼򵥵㣬�Ȳ����л��������洢�����ȷֳ����飬ǰһ����������1����һ����������2
    Blind_UpdateLink2OpenFlag();
    if(1 == Link2OpenFlag)//����������2
    {
        Link1Blind.StartSector = FLASH_BLIND_START_SECTOR;
        Link1Blind.EndSector = FLASH_BLIND_MIDDLE_SECTOR;
    }
    else
    {
        Link1Blind.StartSector = FLASH_BLIND_START_SECTOR;
        Link1Blind.EndSector = FLASH_BLIND_END_SECTOR;
    }
		
		UpdateRunMode();
		
		if(1 == BBGNTestFlag)//dxl,2016.4.7���ݲ������ʵ��Ӧ��
		{
		    BlindStepLen = FLASH_BLIND_STEP_LEN_BBGN_TEST;
			  Link1Blind.StartSector = FLASH_BLIND_START_SECTOR;//dxl,2016.5.19
        Link1Blind.EndSector = FLASH_BLIND_END_SECTOR;
		}
		else
		{
		    BlindStepLen = FLASH_BLIND_STEP_LEN;
		}

    Link1Blind.SectorStep = FLASH_ONE_SECTOR_BYTES/BlindStepLen;

    Link1Blind.OldestSector = Link1Blind.StartSector;
    Link1Blind.OldestStep = 0;
    Link1Blind.OldestTimeCount = 0;//������Сֵ
    Link1Blind.SaveSector = Link1Blind.StartSector;
    Link1Blind.SaveStep = 0;
    Link1Blind.SaveTimeCount = 0;//������Сֵ
    Link1Blind.SaveEnableFlag = 0;
    Link1Blind.ReportSendNum = 0;
		
		
		BlindPacketNum = 800/BlindStepLen;//dxl,2016.4.12,ä��������������ĳɿɱ�
		
    for(i=0; i<BlindPacketNum; i++)
    {
        Link1Blind.LastReportSector[i] = 0;
        Link1Blind.LastReportStep[i] = 0;
    }
    Link1Blind.ErrorCount = 0;
    Link1Blind.State = BLIND_STATE_SEARCH;

    Link1TimeCount = 0;
    Link1WaitTimeCount = 0; 
    Link1SearchSector = Link1Blind.StartSector;
		
	

    
}
/**************************************************************************
//��������Blind_Link1Search
//���ܣ�����1����ȷ��ä����¼���ϱ��ĳ�ʼλ��
//���룺��
//�������
//����ֵ����
//��ע��ä��������ʱ������Ҫ���ô˺���
***************************************************************************/
static void Blind_Link1Search(void)
{
    if((Link1SearchSector >= Link1Blind.StartSector)&&(Link1SearchSector < Link1Blind.EndSector))
    {
        Blind_Link1SearchSector(Link1SearchSector);//��������1ĳ�������е�ÿ��ä����Ϣ
    }
    else
    {
        Link1Blind.State = BLIND_STATE_INIT;
        return ;
    }
    
    if(Link1Blind.ErrorCount > 3)
    {
        Link1Blind.ErrorCount = 0;
        Blind_Link1Erase();
        return ;
    }

    Link1SearchSector++;
    if(Link1SearchSector >= Link1Blind.EndSector)
    {
        if((Link1Blind.StartSector == Link1Blind.SaveSector)&&(0 == Link1Blind.SaveStep)&&(0 == Link1Blind.SaveTimeCount))
        {
            //ä������Ϊ��
        }
        else
        {
            Link1Blind.SaveStep++;
            if(Link1Blind.SaveStep >= Link1Blind.SectorStep)
            {
                Link1Blind.SaveStep = 0;
                Link1Blind.SaveSector++;
                if(Link1Blind.SaveSector >= Link1Blind.EndSector)
                {
                    Link1Blind.SaveSector = Link1Blind.StartSector;
                }
            }
        }
        Link1Blind.ReportSector = Link1Blind.OldestSector;
        Link1Blind.ReportStep = Link1Blind.OldestStep;
        Link1Blind.SaveEnableFlag = 1;
        Link1Blind.State = BLIND_STATE_REPORT;
    }
}
/**************************************************************************
//��������Blind_Link1SearchSector
//���ܣ����ĳ�������е�ä����Ϣ
//���룺������
//�������ǰ�洢�Ͳ���ä����λ��
//����ֵ����
//��ע��
***************************************************************************/
static void Blind_Link1SearchSector(s16 SearchSector)
{
    s16 i,j;
    u32 Addr;
    u8 flag;
    u8 Buffer[FLASH_BLIND_STEP_LEN+1];
    u8 sum;
    TIME_T tt;
    u32 ttCount;

    Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + Link1Blind.SectorStep*BlindStepLen + 1;
    sFLASH_ReadBuffer(&flag,Addr,1);//�����������Ƿ��Ѳ�����־
    if(0xaa == flag)
    {
        Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + (Link1Blind.SectorStep-1)*BlindStepLen;
        sFLASH_ReadBuffer(Buffer,Addr,4);
        ttCount = Public_ConvertBufferToLong(Buffer);
        Gmtime(&tt,ttCount);
        if(SUCCESS == CheckTimeStruct(&tt))
        {
            if(ttCount > Link1Blind.OldestTimeCount)
            {
                Link1Blind.OldestSector = SearchSector;
                Link1Blind.OldestStep = Link1Blind.SectorStep-1;
                Link1Blind.OldestTimeCount = ttCount;
            }
        }
        return ;
    }

    Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + (Link1Blind.SectorStep-1)*BlindStepLen;//�����������1��
    sFLASH_ReadBuffer(Buffer,Addr,4);
    ttCount = Public_ConvertBufferToLong(Buffer);
    if(0xffffffff != ttCount)
    {
        Gmtime(&tt,ttCount);
        if(SUCCESS == CheckTimeStruct(&tt))
        {
            sFLASH_ReadBuffer(Buffer,Addr,BlindStepLen);
            if(CHANNEL_DATA_1 != Buffer[4])//ͨ������ȷ
            {
                Link1Blind.ErrorCount++;
                if(Link1Blind.ErrorCount > 3)
                {
                    Link1Blind.ErrorCount = 0;
                    Blind_Erase(0xff);//��������ä���洢����
                    return ;
                }   
            }
            sum = 0;
            for(j=0; j<Buffer[6]; j++)
            {
                sum += Buffer[j+7];
            }
            if((sum != Buffer[5])||(0 == Buffer[6]))
            {
                Link1Blind.ErrorCount++;
                if(Link1Blind.ErrorCount > 3)
                {
                    Link1Blind.ErrorCount = 0;
                    Blind_Erase(0xff);//��������ä���洢����
                    return ;
                }
            }
            else
            {
                if((ttCount > Link1Blind.OldestTimeCount)&&(0xaa == Buffer[BlindStepLen-1]))
                {
                    Link1Blind.OldestSector = SearchSector;
                    Link1Blind.OldestStep = 0;
                    Link1Blind.OldestTimeCount = ttCount;
                }

                if(ttCount > Link1Blind.SaveTimeCount)
                {
                    Link1Blind.SaveSector = SearchSector;
                    Link1Blind.SaveStep = Link1Blind.SectorStep-1;
                    Link1Blind.SaveTimeCount = ttCount;
                }
            }
        }
        else
        {
            Link1Blind.ErrorCount++;
            if(Link1Blind.ErrorCount > 3)
            {
                Link1Blind.ErrorCount = 0;
                Blind_Erase(0xff);//��������ä���洢����
                return ;
            }
        }
    }
    else
    {
        Addr = SearchSector*FLASH_ONE_SECTOR_BYTES;//�������ĵ�1��
        sFLASH_ReadBuffer(Buffer,Addr,4);
        ttCount = Public_ConvertBufferToLong(Buffer);
        if(0xffffffff != ttCount)
        {
            for(i=0; i<Link1Blind.SectorStep; i++)
            {
                Addr = SearchSector*FLASH_ONE_SECTOR_BYTES+i*BlindStepLen;
                sFLASH_ReadBuffer(Buffer,Addr,BlindStepLen);
                ttCount = Public_ConvertBufferToLong(Buffer);
                if(0xffffffff == ttCount)
                {
                    break;
                }
                Gmtime(&tt,ttCount);
                if(SUCCESS == CheckTimeStruct(&tt))
                {
                    if(CHANNEL_DATA_1 != Buffer[4])//ͨ������ȷ
                    {
                        Link1Blind.ErrorCount++;
                        if(Link1Blind.ErrorCount > 3)
                        {
                            Link1Blind.ErrorCount = 0;
                            Blind_Erase(0xff);//��������ä���洢����
                            return ;
                        }   
                    }
                    sum = 0;
                    for(j=0; j<Buffer[6]; j++)
                    {
                        sum += Buffer[j+7];
                    }
                    if((sum != Buffer[5])||(0 == Buffer[6]))
                    {
                        Link1Blind.ErrorCount++;
                        if(Link1Blind.ErrorCount > 3)
                        {
                            Link1Blind.ErrorCount = 0;
                            Blind_Erase(0xff);//��������ä���洢����
                            return ;
                        }
                    }
                    else
                    {
                        if((ttCount > Link1Blind.OldestTimeCount)&&(0xaa == Buffer[BlindStepLen-1]))
                        {
                            Link1Blind.OldestSector = SearchSector;
                            Link1Blind.OldestStep = i;
                            Link1Blind.OldestTimeCount = ttCount;
                        }

                        if(ttCount > Link1Blind.SaveTimeCount)
                        {
                            Link1Blind.SaveSector = SearchSector;
                            Link1Blind.SaveStep = i;
                            Link1Blind.SaveTimeCount = ttCount;
                        }
                    }
                }
                else
                {
                    Link1Blind.ErrorCount++;
                    if(Link1Blind.ErrorCount > 3)
                    {
                        Link1Blind.ErrorCount = 0;
                        Blind_Erase(0xff);//��������ä���洢����
                        return ;
                    }
                }
            }
        }
    }

/*ÿ������ÿһ������飬̫�˷�ʱ�䣬�Ż����ֻ����1�������1��
    Link1Blind.ErrorCount = 0;
    for(i=0; i<Link1Blind.SectorStep; i++)
    {
        Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + i*BlindStepLen;
        sFLASH_ReadBuffer(Buffer,Addr,4);
        ttCount = Public_ConvertBufferToLong(Buffer);
        if(0xffffffff != ttCount)
        {
            Gmtime(&tt,ttCount);
            if(SUCCESS == CheckTimeStruct(&tt))
            {
                sFLASH_ReadBuffer(Buffer,Addr,BlindStepLen);
                if(CHANNEL_DATA_1 != Buffer[4])//ͨ������ȷ
                {
                    Link1Blind.ErrorCount++;
                    if(Link1Blind.ErrorCount > 3)
                    {
                        Link1Blind.ErrorCount = 0;
                        Blind_Erase(0xff);//��������ä���洢����
                        break;
                    }   
                }
                sum = 0;
                for(j=0; j<Buffer[6]; j++)
                {
                    sum += Buffer[j+7];
                }
                if((sum != Buffer[5])||(0 == Buffer[6]))
                {
                    Link1Blind.ErrorCount++;
                }
                else
                {
                    if((ttCount > Link1Blind.OldestTimeCount)&&(0xaa == Buffer[BlindStepLen-1]))
                    {
                        Link1Blind.OldestSector = SearchSector;
                        Link1Blind.OldestStep = i;
                        Link1Blind.OldestTimeCount = ttCount;
                    }

                    if(ttCount > Link1Blind.SaveTimeCount)
                    {
                        Link1Blind.SaveSector = SearchSector;
                        Link1Blind.SaveStep = i;
                        Link1Blind.SaveTimeCount = ttCount;
                    }
                }
            }
            else
            {
                Link1Blind.ErrorCount++;
            }
        }
    }
*/
    
}
/**************************************************************************
//��������Blind_CheckDataIsOK
//���ܣ����ä��1�����Ƿ���ȷ
//���룺��
//�������
//����ֵ������ȷ������
//��ע������Ϊ��ä������ʱ���������жϣ�������ʵ��Ӧ��
***************************************************************************/
/*
static u16 Blind_CheckDataIsOK(void)
{
	  static u16 TotalErrorCount;
	  static u16 GpsTimeErrorCount;
	  static u16 RtcTimeErrorCount;
    static u16 ErrorCount;//��������
	  static u16 FFErrorCount;//δ�洢��������
	  static u32 LastRtcTimeCount;
	  static u32 LastGpsTimeCount;
	  
	  static u16 StepCount;
	
	  u32 CurRtcTimeCount;
	  u32 CurGpsTimeCount;
	  u32 Addr;
	  s16 CheckSector;
	  s16 CheckStep;
	  u8  j;
	  u8  sum;
	  u8 Buffer[100];
//	  u8 BufferLen;
	  TIME_T CurTime;
	  TIME_T CurTime2;
	
	
	  TotalErrorCount = 0;
	  GpsTimeErrorCount = 0;
	  RtcTimeErrorCount = 0;
    ErrorCount = 0;//��������
	  FFErrorCount = 0;//δ�洢��������
	  LastRtcTimeCount = 0;
	  LastGpsTimeCount = 0;
		//BufferLen = 0;
	  StepCount = 0;
		
	  for(CheckSector=Link1Blind.StartSector; CheckSector<Link1Blind.EndSector; CheckSector++)
	  {
	  for(CheckStep=0; CheckStep<Link1Blind.SectorStep; CheckStep++)
    {
        Addr = CheckSector*FLASH_ONE_SECTOR_BYTES + CheckStep*BlindStepLen;
        sFLASH_ReadBuffer(Buffer,Addr,4);
        CurRtcTimeCount = Public_ConvertBufferToLong(Buffer);
        if(0xffffffff != CurRtcTimeCount)
        {
					  StepCount++; 
            Gmtime(&CurTime,CurRtcTimeCount); 
            if(SUCCESS == CheckTimeStruct(&CurTime))
            {
                sFLASH_ReadBuffer(Buffer,Addr,BlindStepLen);
                if(CHANNEL_DATA_1 != Buffer[4])//ͨ������ȷ
                {
                    ErrorCount++;
                }
                sum = 0;
                for(j=0; j<Buffer[6]; j++)
                {
                    sum += Buffer[j+7];
                }
                if((sum != Buffer[5])||(0 == Buffer[6]))
                {
                    ErrorCount++;
                }
								VDRPub_ConvertBCDToTime(&CurTime2,Buffer+29);
								CurGpsTimeCount = ConverseGmtime(&CurTime2);
								
								if((CurRtcTimeCount - LastRtcTimeCount) != 1)
								{
								    RtcTimeErrorCount++;
									  //QueueInBuffer(Buffer,BlindStepLen,VdrTxQ);
									  
								}
								LastRtcTimeCount = CurRtcTimeCount;
								
								if((CurGpsTimeCount - LastGpsTimeCount) != 1)
								{
								    GpsTimeErrorCount++;
									  QueueInBuffer(Buffer,BlindStepLen,VdrTxQ);
								}
								LastGpsTimeCount = CurGpsTimeCount;
            }
            else
            {
                ErrorCount++;
            }
        }
				else
				{
				    FFErrorCount++;
				}
    }
	}
		
	TotalErrorCount = ErrorCount+FFErrorCount+GpsTimeErrorCount+RtcTimeErrorCount;
	
	return TotalErrorCount;
	  
}
*/
/**************************************************************************
//��������Blind_Link1Report
//���ܣ�����1�ϱ�һ��ä������
//���룺��
//�������
//����ֵ����
//��ע��ä��������ʱ������Ҫ���ô˺���
***************************************************************************/
static u8 Blind_Link1Report(void)
{
    u32 Addr;
    u8 i;
    u8 count = 0;
    u8 flag;
    u8 Buffer[FLASH_BLIND_STEP_LEN+1];
    u8 BufferLen;
    u8 sum;
    u16 length = 3;//Ԥ��3���ֽ�



    for(count=0; count<BlindPacketNum; )//ÿ������5��λ����Ϣ
    {
        if(0 == Blind_GetLink1ReportFlag())
        {
            break;
        }
        Addr = Link1Blind.ReportSector*FLASH_ONE_SECTOR_BYTES + Link1Blind.SectorStep*BlindStepLen + 1;
        sFLASH_ReadBuffer(&flag,Addr,1);//�����������Ƿ��Ѳ�����־
        if(0xaa == flag)//�Ѳ�����
        {
            Link1Blind.ReportSector++;
            Link1Blind.ReportStep = 0;
            if(Link1Blind.ReportSector >= Link1Blind.EndSector)
            {
                Link1Blind.ReportSector = Link1Blind.StartSector;
            }
        }
        else
        {
            for(;count<BlindPacketNum;)
            {
                if(0 == Blind_GetLink1ReportFlag())
                {
                    break;
                }
                Addr = Link1Blind.ReportSector*FLASH_ONE_SECTOR_BYTES + (Link1Blind.ReportStep+1)*BlindStepLen - 1;
                sFLASH_ReadBuffer(&flag,Addr,1);//�������Ƿ��Ѳ�����־
                if(0xaa == flag)//�Ѳ�����
                {
                    Link1Blind.ReportStep++;
                }
                else
                {
                    Addr = Link1Blind.ReportSector*FLASH_ONE_SECTOR_BYTES + Link1Blind.ReportStep*BlindStepLen;
                    sFLASH_ReadBuffer(Buffer,Addr,BlindStepLen);
                    BufferLen = Buffer[6];
                    sum = 0;
                    for(i=0; i<BufferLen; i++)
                    {
                        sum += Buffer[7+i];
                    }
                    if(Buffer[5] == sum)
                    {
                        if(((length+BufferLen+2) < RADIO_PROTOCOL_BUFFER_SIZE)&&(BufferLen < (BlindStepLen-7)))
                        {
                            RadioShareBuffer[length] = 0;//���ȸ��ֽ��0
                            memcpy(RadioShareBuffer+length+1,Buffer+6,BufferLen+1);
                            length += (BufferLen+2);
                            Link1Blind.LastReportSector[count] = Link1Blind.ReportSector;
                            Link1Blind.LastReportStep[count] = Link1Blind.ReportStep;
                            count++;
                        }
                        
                    }
                    Link1Blind.ReportStep++;
                }
                if(Link1Blind.ReportStep >= Link1Blind.SectorStep)
                {
                    Link1Blind.ReportSector++;
                    Link1Blind.ReportStep = 0;
                    if(Link1Blind.ReportSector >= Link1Blind.EndSector)
                    {
                        Link1Blind.ReportSector = Link1Blind.StartSector;
                    }
                    break;
                }
            }
        }
    }
    
    if(0 == count)
    {
        return 0;
    }
    else
    {
        RadioShareBuffer[0] = 0;//�����ֽڱ�ʾ����
        RadioShareBuffer[1] = count;
        RadioShareBuffer[2] = 1;//0:������λ�������㱨;1:ä������
        RadioProtocol_PostionInformation_BulkUpTrans(CHANNEL_DATA_1,RadioShareBuffer,length);
    }

    return 1;
}
/**************************************************************************
//��������Blind_Link1ReportAck
//���ܣ�����1ä������Ӧ����
//���룺��
//�������
//����ֵ����
//��ע���յ�����1��ä������Ӧ��ʱ����ô˺���
***************************************************************************/
static void Blind_Link1ReportAck(void)
{
    u8 i;
    u8 flag;
    u32 Addr;
    s16 Sector;

    Link1Blind.ReportSendNum = 0;
    Link1TimeCount = BLIND_REPORT_DELAY_TIME-9;//0.4����ϱ���һ��

    Sector = Link1Blind.LastReportSector[0];

    for(i=0; i<BlindPacketNum; i++)
    {
        if((Link1Blind.LastReportSector[i] >= Link1Blind.StartSector)
            &&(Link1Blind.LastReportSector[i] < Link1Blind.EndSector)
                &&(Link1Blind.LastReportStep[i] < Link1Blind.SectorStep))
        {
            Addr = Link1Blind.LastReportSector[i]*FLASH_ONE_SECTOR_BYTES+(Link1Blind.LastReportStep[i]+1)*BlindStepLen - 1;
            flag = 0xaa;
            sFLASH_WriteBuffer(&flag, Addr, 1);//��Ǹ���ä�����ϱ�

            if(Link1Blind.LastReportStep[i] >= (Link1Blind.SectorStep-1))
            {
                Addr = Sector*FLASH_ONE_SECTOR_BYTES+Link1Blind.SectorStep*BlindStepLen + 1;
                flag = 0xaa;
                sFLASH_WriteBuffer(&flag, Addr, 1);//��Ǹ�������ä�����ϱ�
            }
        }
        
    }

    for(i=0; i<BlindPacketNum; i++)
    {
        Link1Blind.LastReportSector[i] = 0;
        Link1Blind.LastReportStep[i] = 0;
    }
    
        
}
/**************************************************************************
//��������Blind_GetLink1ReportFlag
//���ܣ���ȡ����1ä���ϱ���־
//���룺��
//�������
//����ֵ��ä���ϱ���־
//��ע��1��ʾ��Ҫ�ϱ���0��ʾ����Ҫ�ϱ����ն�ÿ��10���ӻ��Զ����һ�ο��Ƿ���Ҫ�ϱ�
***************************************************************************/
static u8 Blind_GetLink1ReportFlag(void)
{
    s16 Sector;
    s16 Step;

    Sector = Link1Blind.SaveSector;
    Step =  Link1Blind.SaveStep;
/*
    if(0 != Link1Blind.SaveTimeCount)
    {
        Step--;
        if(Step < 0)
        {
            Step = Link1Blind.SectorStep-1;
            Sector--;
            if(Sector < Link1Blind.StartSector)
            {
                Sector = Link1Blind.EndSector - 1;
            }
        }
    }
    */
    if((Link1Blind.ReportSector == Sector)&&(Link1Blind.ReportStep >= Step))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
/**************************************************************************
//��������Blind_StartLink1Report
//���ܣ���������1ä������
//���룺��
//�������
//����ֵ����
//��ע������ä��������ÿ������1��Ȩ�ɹ�����ô˺���
***************************************************************************/
static void Blind_StartLink1Report(void)
{
    //if(BLIND_STATE_WAIT == Link1Blind.State)
    //{
        Link1Blind.State = BLIND_STATE_INIT;
    //}
}
/**************************************************************************
//��������Blind_Link1Erase
//���ܣ���������1ä���洢����
//���룺��
//�������
//����ֵ����
//��ע����⵽ä����¼��λ����Ϣ����ʱ���������������50������ô˺�����
//������Զ��FTP������������ʱ����ô˺�������Ϊä����Զ�̹̼���������һ������
***************************************************************************/
static void Blind_Link1Erase(void)
{
    u16 i;
    u32 Addr;
	
	  if(1 == BBGNTestFlag)
		{
		    LcdClearScreen(); 
        LcdShowCaptionEx((char *)"����ä��1",2); 
		}
    
    Link1Blind.SaveEnableFlag = 0;//��ֹ�洢
    Link1Blind.State = BLIND_STATE_INIT;

    for(i=Link1Blind.StartSector; i<Link1Blind.EndSector; i++)
    {
        Addr = i*FLASH_ONE_SECTOR_BYTES;
        sFLASH_EraseSector(Addr);
        IWDG_ReloadCounter();
    }   
}
/**************************************************************************
//��������Blind_Link1Save
//���ܣ��洢һ������1��ä������
//���룺һ��λ����Ϣ������
//�������
//����ֵ��0Ϊ�ɹ�����0Ϊʧ�ܣ����ȳ���ʱ�᷵��ʧ��
//��ע��һ��ä�����ݰ�����ʱ��4�ֽ�+����1�ֽڣ�Ԥ����+У���1�ֽ�+λ����Ϣ����1�ֽ�+λ����Ϣ�����ֽ�
***************************************************************************/
static u8 Blind_Link1Save(u8 *pBuffer, u8 length, u8 attribute)
{
    u8 Buffer[FLASH_BLIND_STEP_LEN+1] = {0};
    u8 i,j;
    u32 Addr;
    u32 TimeCount =0;
    u8 *p = NULL;
    u8 sum;
    s16 Sector;
    s16 Step;
    TIME_T tt;
    u32 ttCount;
		
		if((1 == BBGNTestFlag)&&(0 == Link1BlindSaveEnableFlag))//dxl,2016.5.15
		{
				    return 1;
		}

    if((length > (BlindStepLen-9))||(length < 28)||(0 == Link1Blind.SaveEnableFlag))//ÿ����������ֽ����������ǣ����ϴ����
    {
        return 1;
    }
    
		
    if((Link1Blind.StartSector == Link1Blind.SaveSector)&&(0 == Link1Blind.SaveStep)&&(0 == Link1Blind.SaveTimeCount))//Ϊ��
    {
            
    }
    else//��Ϊ�գ��Ѵ�������
    {
        Sector = Link1Blind.SaveSector;
        Step = Link1Blind.SaveStep;
        Step--;//��ȡ�Ѵ洢�����µ���һ��ʱ��
        if(Step < 0)
        {
            Step = Link1Blind.SectorStep - 1;
            Sector--;
            if(Sector < Link1Blind.StartSector)
            {
                Sector = Link1Blind.EndSector - 1;
            }
        }
        Addr = Sector*FLASH_ONE_SECTOR_BYTES+Step*BlindStepLen;
        sFLASH_ReadBuffer(Buffer,Addr,4);
        ttCount = Public_ConvertBufferToLong(Buffer);
        Gmtime(&tt,ttCount);

        if(ERROR == CheckTimeStruct(&tt))//����Ѵ洢�����µ���һ��ʱ���Ƿ���ȷ
        {
            sFLASH_EraseSector(Addr);
            Link1Blind.SaveStep = 0;
            return 1;
        }

        if(ERROR == CheckTimeStruct(&CurTime))//��鵱ǰʱ���Ƿ���ȷ
        {
            return 1;
        }

        //TimeCount = ConverseGmtime(&CurTime);//�Ƚϵ�ǰʱ�������¼�¼����һ��ʱ��
				TimeCount = RTC_GetCounter();
        if(TimeCount < ttCount)
        {
            return 1;
        }
        
    }
    
    Addr = Link1Blind.SaveSector*FLASH_ONE_SECTOR_BYTES+Link1Blind.SaveStep*BlindStepLen;//��鵱ǰ�洢����һ���ĵ�ַ�Ƿ�Ϊ��
    sFLASH_ReadBuffer(Buffer,Addr,5);
    for(i=0; i<5; i++)
    {
        if(0xff != Buffer[i])
        {
            sFLASH_EraseSector(Addr);
            Link1Blind.SaveStep = 0;
            Addr = Link1Blind.SaveSector*FLASH_ONE_SECTOR_BYTES;
            for(j=0; j<200; j++)
            {
                //�ȴ�һ���
            }
            break;
        }
    }

    sum = 0;//����У���
    for(i=0; i<length; i++)
    {
        sum += *(pBuffer+i);
    }

    
    memcpy(Buffer+7,pBuffer,length);//����λ����Ϣ������

    p = Buffer;//����7�ֽڵ�����
    *p++ = (TimeCount&0xff000000) >> 24;
    *p++ = (TimeCount&0xff0000) >> 16;
    *p++ = (TimeCount&0xff00) >> 8;
    *p++ = TimeCount&0xff;
    *p++ = attribute;
    *p++ = sum;
    *p++ = length;

    sFLASH_WriteBuffer(Buffer,Addr,length+7);//д��flash

    Link1Blind.SaveStep++;//����һ��
    if(Link1Blind.SaveStep >= Link1Blind.SectorStep)
    {
        Link1Blind.SaveSector++;
        Link1Blind.SaveStep = 0;
        if(Link1Blind.SaveSector >= Link1Blind.EndSector)
        {
            Link1Blind.SaveSector = Link1Blind.StartSector;
        }
        Addr = Link1Blind.SaveSector*FLASH_ONE_SECTOR_BYTES;
        sFLASH_EraseSector(Addr);
    }

    return 0;
    
}

//////////////////////////////////////////////////////////////////
/**************************************************************************
//��������Blind_Link2Init
//���ܣ�����2������ʼ��
//���룺��
//�������
//����ֵ����
//��ע��ä��������ʱ������Ҫ���ô˺���
***************************************************************************/
static void Blind_Link2Init(void)
{
    u8 i;

    //����2�����ӵ�TCP�˿ںţ��ж��Ƿ�����2�����ӣ�����������Ҫ��ִ洢����ÿ�θò���״̬�仯ʱ��Ҫ����flash
    //�����洢�����ȷֳ����飬ǰһ����������1����һ����������2
    Blind_UpdateLink2OpenFlag();
    Link2Blind.StartSector = FLASH_BLIND_MIDDLE_SECTOR;
    Link2Blind.EndSector = FLASH_BLIND_END_SECTOR;
    Link2Blind.SectorStep = FLASH_ONE_SECTOR_BYTES/BlindStepLen;
    Link2Blind.OldestSector = Link2Blind.StartSector;
    Link2Blind.OldestStep = 0;
    Link2Blind.OldestTimeCount = 0;//������Сֵ
    Link2Blind.SaveSector = Link2Blind.StartSector;
    Link2Blind.SaveStep = 0;
    Link2Blind.SaveTimeCount = 0;//������С��
    Link2Blind.SaveEnableFlag = 0;
    Link2Blind.ReportSendNum = 0;
	
    for(i=0; i<BlindPacketNum; i++)
    {
        Link2Blind.LastReportSector[i] = 0;
        Link2Blind.LastReportStep[i] = 0;
    }
    Link2Blind.ErrorCount = 0;
    Link2Blind.State = BLIND_STATE_SEARCH;

    Link2TimeCount = 0;
    Link2WaitTimeCount = 0; 
    Link2SearchSector = Link2Blind.StartSector;

    
}
/**************************************************************************
//��������Blind_Link2Search
//���ܣ�����2����ȷ��ä����¼���ϱ��ĳ�ʼλ��
//���룺��
//�������
//����ֵ����
//��ע��ä��������ʱ������Ҫ���ô˺���
***************************************************************************/
static void Blind_Link2Search(void)
{
    if((Link2SearchSector >= Link2Blind.StartSector)&&(Link2SearchSector < Link2Blind.EndSector))
    {
        Blind_Link2SearchSector(Link2SearchSector);//��������1ĳ�������е�ÿ��ä����Ϣ
    }
    else
    {
        Link2Blind.State = BLIND_STATE_INIT;
        return ;
    }
    
    if(Link2Blind.ErrorCount > 3)
    {
        Link2Blind.ErrorCount = 0;
        Blind_Link2Erase();//��������1����ä��flash
        Link2Blind.State = BLIND_STATE_INIT;
        return ;
    }

    Link2SearchSector++;
    if(Link2SearchSector >= Link2Blind.EndSector)
    {
        if((Link2Blind.StartSector == Link2Blind.SaveSector)&&(0 == Link2Blind.SaveStep)&&(0 == Link2Blind.SaveTimeCount))
        {
            //ä������Ϊ��
        }
        else
        {
            Link2Blind.SaveStep++;
            if(Link2Blind.SaveStep >= Link2Blind.SectorStep)
            {
                Link2Blind.SaveStep = 0;
                Link2Blind.SaveSector++;
                if(Link2Blind.SaveSector >= Link2Blind.EndSector)
                {
                    Link2Blind.SaveSector = Link2Blind.StartSector;
                }
            }
        }
        Link2Blind.ReportSector = Link2Blind.OldestSector;
        Link2Blind.ReportStep = Link2Blind.OldestStep;
        Link2Blind.SaveEnableFlag = 1;
        Link2Blind.State = BLIND_STATE_REPORT;
    }
}
/**************************************************************************
//��������Blind_Link2SearchSector
//���ܣ����ĳ�������е�ä����Ϣ
//���룺������
//�������ǰ�洢�Ͳ���ä����λ��
//����ֵ����
//��ע��
***************************************************************************/
static void Blind_Link2SearchSector(s16 SearchSector)
{
    s16 i,j;
    u32 Addr;
    u8 flag;
    u8 Buffer[FLASH_BLIND_STEP_LEN+1];
    u8 sum;
    TIME_T tt;
    u32 ttCount;

    Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + Link2Blind.SectorStep*BlindStepLen + 1;
    sFLASH_ReadBuffer(&flag,Addr,1);//�����������Ƿ��Ѳ�����־
    if(0xaa == flag)
    {
        Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + (Link2Blind.SectorStep-1)*BlindStepLen;
        sFLASH_ReadBuffer(Buffer,Addr,4);
        ttCount = Public_ConvertBufferToLong(Buffer);
        Gmtime(&tt,ttCount);
        if(SUCCESS == CheckTimeStruct(&tt))
        {
            if(ttCount > Link2Blind.OldestTimeCount)
            {
                Link2Blind.OldestSector = SearchSector;
                Link2Blind.OldestStep = Link2Blind.SectorStep-1;
                Link2Blind.OldestTimeCount = ttCount;
            }
        }
        return ;
    }

    Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + (Link2Blind.SectorStep-1)*BlindStepLen;//���������1��
    sFLASH_ReadBuffer(Buffer,Addr,4);
    ttCount = Public_ConvertBufferToLong(Buffer);
    if(0xffffffff != ttCount)
    {
        Gmtime(&tt,ttCount);
        if(SUCCESS == CheckTimeStruct(&tt))
        {
            Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + (Link2Blind.SectorStep-1)*BlindStepLen;
            sFLASH_ReadBuffer(Buffer,Addr,BlindStepLen);
            if(CHANNEL_DATA_2 != Buffer[4])//ͨ������ȷ
            {
                Link2Blind.ErrorCount++;
                if(Link2Blind.ErrorCount > 3)
                {
                    Blind_Erase(0xff);//��������ä���洢����
                    return;
                }
            }
            sum = 0;
            for(j=0; j<Buffer[6]; j++)
            {
                sum += Buffer[j+7];
            }
            if((sum != Buffer[5])||(0 == Buffer[6]))
            {
                Link2Blind.ErrorCount++;
                if(Link2Blind.ErrorCount > 3)
                {
                    Blind_Erase(0xff);//��������ä���洢����
                    return;
                }
            }
            else
            {
                if((ttCount > Link2Blind.OldestTimeCount)&&(0xaa == Buffer[BlindStepLen-1]))
                {
                    Link2Blind.OldestSector = SearchSector;
                    Link2Blind.OldestStep = 0;
                    Link2Blind.OldestTimeCount = ttCount;
                }

                if(ttCount > Link2Blind.SaveTimeCount)
                {
                    Link2Blind.SaveSector = SearchSector;
                    Link2Blind.SaveStep = Link2Blind.SectorStep-1;
                    Link2Blind.SaveTimeCount = ttCount;
                }
            }
        }
        else
        {
            Link2Blind.ErrorCount++;
            if(Link2Blind.ErrorCount > 3)
            {
                Blind_Erase(0xff);//��������ä���洢����
                return;
            }
        }
    }
    else
    {
        Addr = SearchSector*FLASH_ONE_SECTOR_BYTES;//��������1��
        sFLASH_ReadBuffer(Buffer,Addr,4);
        ttCount = Public_ConvertBufferToLong(Buffer);
        if(0xffffffff != ttCount)
        {
            for(i=0; i<Link2Blind.SectorStep; i++)
            {
                Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + i*BlindStepLen;
                sFLASH_ReadBuffer(Buffer,Addr,BlindStepLen);
                ttCount = Public_ConvertBufferToLong(Buffer);
                if(0xffffffff != ttCount)
                {
                    Gmtime(&tt,ttCount);
                    if(SUCCESS == CheckTimeStruct(&tt))
                    {
                        if(CHANNEL_DATA_2 != Buffer[4])//ͨ������ȷ
                        {
                            Link2Blind.ErrorCount++;
                            if(Link2Blind.ErrorCount > 3)
                            {
                                Blind_Erase(0xff);//��������ä���洢����
                                return;
                            }
                        }
                        sum = 0;
                        for(j=0; j<Buffer[6]; j++)
                        {
                            sum += Buffer[j+7];
                        }
                        if((sum != Buffer[5])||(0 == Buffer[6]))
                        {
                            Link2Blind.ErrorCount++;
                        }
                        else
                        {
                            if((ttCount > Link2Blind.OldestTimeCount)&&(0xaa == Buffer[BlindStepLen-1]))
                            {
                                Link2Blind.OldestSector = SearchSector;
                                Link2Blind.OldestStep = i;
                                Link2Blind.OldestTimeCount = ttCount;
                            }

                            if(ttCount > Link2Blind.SaveTimeCount)
                            {
                                Link2Blind.SaveSector = SearchSector;
                                Link2Blind.SaveStep = i;
                                Link2Blind.SaveTimeCount = ttCount;
                            }
                        }
                    }
                    else
                    {
                        Link2Blind.ErrorCount++;
                        if(Link2Blind.ErrorCount > 3)
                        {
                            Blind_Erase(0xff);//��������ä���洢����
                            return;
                        }
                    }
                }
            }
        }       
        else
        {

        }
    }

/*
    Link2Blind.ErrorCount = 0;
    for(i=0; i<Link2Blind.SectorStep; i++)
    {
        Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + i*BlindStepLen;
        sFLASH_ReadBuffer(Buffer,Addr,4);
        ttCount = Public_ConvertBufferToLong(Buffer);
        if(0xffffffff != ttCount)
        {
            Gmtime(&tt,ttCount);
            if(SUCCESS == CheckTimeStruct(&tt))
            {
                sFLASH_ReadBuffer(Buffer,Addr,BlindStepLen);
                if(CHANNEL_DATA_2 != Buffer[4])//ͨ������ȷ
                {
                    Link2Blind.ErrorCount++;
                    if(Link2Blind.ErrorCount > 3)
                    {
                        Blind_Erase(0xff);//��������ä���洢����
                        break;
                    }
                }
                sum = 0;
                for(j=0; j<Buffer[6]; j++)
                {
                    sum += Buffer[j+7];
                }
                if((sum != Buffer[5])||(0 == Buffer[6]))
                {
                    Link2Blind.ErrorCount++;
                }
                else
                {
                    if((ttCount > Link2Blind.OldestTimeCount)&&(0xaa == Buffer[BlindStepLen-1]))
                    {
                        Link2Blind.OldestSector = SearchSector;
                        Link2Blind.OldestStep = i;
                        Link2Blind.OldestTimeCount = ttCount;
                    }

                    if(ttCount > Link2Blind.SaveTimeCount)
                    {
                        Link2Blind.SaveSector = SearchSector;
                        Link2Blind.SaveStep = i;
                        Link2Blind.SaveTimeCount = ttCount;
                    }
                }
            }
            else
            {
                Link2Blind.ErrorCount++;
            }
        }
    }
*/
    
}
/**************************************************************************
//��������Blind_Link2Report
//���ܣ�����2�ϱ�һ��ä������
//���룺��
//�������
//����ֵ����
//��ע��ä��������ʱ������Ҫ���ô˺���
***************************************************************************/
static u8 Blind_Link2Report(void)
{
    u32 Addr;
    u8 i;
    u8 count = 0;
    u8 flag;
    u8 Buffer[FLASH_BLIND_STEP_LEN+1];
    u8 BufferLen;
    u8 sum;
    u16 length = 3;//Ԥ��3���ֽ�


    for(count=0; count<BlindPacketNum; )//ÿ������5��λ����Ϣ
    {
        if(0 == Blind_GetLink2ReportFlag())
        {
            break;
        }
        Addr = Link2Blind.ReportSector*FLASH_ONE_SECTOR_BYTES + Link2Blind.SectorStep*BlindStepLen + 1;
        sFLASH_ReadBuffer(&flag,Addr,1);//�����������Ƿ��Ѳ�����־
        if(0xaa == flag)//�Ѳ�����
        {
            Link2Blind.ReportSector++;
            Link2Blind.ReportStep = 0;
            if(Link2Blind.ReportSector >= Link2Blind.EndSector)
            {
                Link2Blind.ReportSector = Link2Blind.StartSector;
            }
        }
        else
        {
            for(;count<BlindPacketNum;)
            {
                if(0 == Blind_GetLink2ReportFlag())
                {
                    break;
                }
                Addr = Link2Blind.ReportSector*FLASH_ONE_SECTOR_BYTES + (Link2Blind.ReportStep+1)*BlindStepLen - 1;
                sFLASH_ReadBuffer(&flag,Addr,1);//�������Ƿ��Ѳ�����־
                if(0xaa == flag)//�Ѳ�����
                {
                    Link2Blind.ReportStep++;
                }
                else
                {
                    Addr = Link2Blind.ReportSector*FLASH_ONE_SECTOR_BYTES + Link2Blind.ReportStep*BlindStepLen;
                    sFLASH_ReadBuffer(Buffer,Addr,BlindStepLen);
                    BufferLen = Buffer[6];
                    sum = 0;
                    for(i=0; i<BufferLen; i++)
                    {
                        sum += Buffer[7+i];
                    }
                    if(Buffer[5] == sum)
                    {
                        if(((length+BufferLen+2) < RADIO_PROTOCOL_BUFFER_SIZE)&&(BufferLen < (BlindStepLen-7)))
                        {
                            RadioShareBuffer[length] = 0;//���ȸ��ֽ��0
                            memcpy(RadioShareBuffer+length+1,Buffer+6,BufferLen+1);
                            length += (BufferLen+2);
                            Link2Blind.LastReportSector[count] = Link2Blind.ReportSector;
                            Link2Blind.LastReportStep[count] = Link2Blind.ReportStep;
                            count++;
                        }
                    }
                    Link2Blind.ReportStep++;
                }
                if(Link2Blind.ReportStep >= Link2Blind.SectorStep)
                {
                    Link2Blind.ReportSector++;
                    Link2Blind.ReportStep = 0;
                    if(Link2Blind.ReportSector >= Link2Blind.EndSector)
                    {
                        Link2Blind.ReportSector = Link2Blind.StartSector;
                    }
                    break;
                }
            }
        }
    }
    
    if(0 == count)
    {
        return 0;
    }
    else
    {
        for(i=count; i<BlindPacketNum; i++)
        {
            Link2Blind.LastReportSector[i] = 0;
            Link2Blind.LastReportStep[i] = 0;
        }
        RadioShareBuffer[0] = 0;//�����ֽڱ�ʾ����
        RadioShareBuffer[1] = count;
        RadioShareBuffer[2] = 1;//0:������λ�������㱨;1:ä������
        RadioProtocol_PostionInformation_BulkUpTrans(CHANNEL_DATA_2,RadioShareBuffer,length);
    }

    return 1;
}
/**************************************************************************
//��������Blind_Link2ReportAck
//���ܣ�����2ä������Ӧ����
//���룺��
//�������
//����ֵ����
//��ע���յ�����2��ä������Ӧ��ʱ����ô˺���
***************************************************************************/
static void Blind_Link2ReportAck(void)
{
    u8 i;
    u8 flag;
    u32 Addr;
    s16 Sector;

    Link2Blind.ReportSendNum = 0;
    Link2TimeCount = BLIND_REPORT_DELAY_TIME-20;//1����ϱ���һ��

    Sector = Link2Blind.LastReportSector[0];

    for(i=0; i<BlindPacketNum; i++)
    {
        if((Link2Blind.LastReportSector[i] >= Link2Blind.StartSector)
            &&(Link2Blind.LastReportSector[i] < Link2Blind.EndSector)
                &&(Link2Blind.LastReportStep[i] < Link2Blind.SectorStep))
        {
            Addr = Link2Blind.LastReportSector[i]*FLASH_ONE_SECTOR_BYTES+(Link2Blind.LastReportStep[i]+1)*BlindStepLen - 1;
            flag = 0xaa;
            sFLASH_WriteBuffer(&flag, Addr, 1);//��Ǹ���ä�����ϱ�

            if(Link2Blind.LastReportStep[i] >= (Link2Blind.SectorStep-1))
            {
                Addr = Sector*FLASH_ONE_SECTOR_BYTES+Link2Blind.SectorStep*BlindStepLen + 1;
                flag = 0xaa;
                sFLASH_WriteBuffer(&flag, Addr, 1);//��Ǹ�������ä�����ϱ�
            }
        }
        
    }

    for(i=0; i<BlindPacketNum; i++)
    {
        Link2Blind.LastReportSector[i] = 0;
        Link2Blind.LastReportStep[i] = 0;
    }
    
        
}
/**************************************************************************
//��������Blind_GetLink2ReportFlag
//���ܣ���ȡ����2ä���ϱ���־
//���룺��
//�������
//����ֵ��ä���ϱ���־
//��ע��1��ʾ��Ҫ�ϱ���0��ʾ����Ҫ�ϱ����ն�ÿ��10���ӻ��Զ����һ�ο��Ƿ���Ҫ�ϱ�
***************************************************************************/
static u8 Blind_GetLink2ReportFlag(void)
{
    s16 Sector;
    s16 Step;

    Sector = Link2Blind.SaveSector;
    Step =  Link2Blind.SaveStep;
/*
    if(0 != Link2Blind.SaveTimeCount)
    {
        Step--;
        if(Step < 0)
        {
            Step = Link2Blind.SectorStep-1;
            Sector--;
            if(Sector < Link2Blind.StartSector)
            {
                Sector = Link2Blind.EndSector - 1;
            }
        }
    }
*/
    if((Link2Blind.ReportSector == Sector)&&(Link2Blind.ReportStep >= Step))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
/**************************************************************************
//��������Blind_StartLink2Report
//���ܣ���������2ä������
//���룺��
//�������
//����ֵ����
//��ע������ä��������ÿ������1��Ȩ�ɹ�����ô˺���
***************************************************************************/
static void Blind_StartLink2Report(void)
{
    //if(BLIND_STATE_WAIT == Link1Blind.State)
    //{
        Link2Blind.State = BLIND_STATE_INIT;
    //}
}
/**************************************************************************
//��������Blind_Link2Erase
//���ܣ���������2ä���洢����
//���룺��
//�������
//����ֵ����
//��ע����⵽ä����¼��λ����Ϣ����ʱ���������������50������ô˺�����
//������Զ��FTP������������ʱ����ô˺�������Ϊä����Զ�̹̼���������һ������
***************************************************************************/
static void Blind_Link2Erase(void)
{
    u16 i;
    u32 Addr;
	
	  if(1 == BBGNTestFlag)
		{
		    LcdClearScreen(); 
        LcdShowCaptionEx((char *)"����ä��2",2); 
		}
    
    if(1 == Link2OpenFlag)
    {
        Link2Blind.SaveEnableFlag = 0;//��ֹ�洢
        Link2Blind.State = BLIND_STATE_INIT;

        for(i=Link2Blind.StartSector; i<Link2Blind.EndSector; i++)
        {
            Addr = i*FLASH_ONE_SECTOR_BYTES;
            sFLASH_EraseSector(Addr);
            IWDG_ReloadCounter();
        }
    }   
}
/**************************************************************************
//��������Blind_Link2Save
//���ܣ��洢һ������2��ä������
//���룺һ��λ����Ϣ������
//�������
//����ֵ��0Ϊ�ɹ�����0Ϊʧ�ܣ����ȳ���ʱ�᷵��ʧ��
//��ע��һ��ä�����ݰ�����ʱ��4�ֽ�+����1�ֽڣ�Ԥ����+У���1�ֽ�+λ����Ϣ����1�ֽ�+λ����Ϣ�����ֽ�
***************************************************************************/
static u8 Blind_Link2Save(u8 *pBuffer, u8 length, u8 attribute)
{
    u8 Buffer[FLASH_BLIND_STEP_LEN+1] = {0};
    u8 i,j;
    u32 Addr;
    u32 TimeCount =0;
    u8 *p = NULL;
    u8 sum;
    s16 Sector;
    s16 Step;
    TIME_T tt;
    u32 ttCount;

    if((length > (BlindStepLen-9))||(length < 28)||(0 == Link2Blind.SaveEnableFlag))//ÿ����������ֽ����������ǣ����ϴ����
    {
        return 1;
    }
    
    if((Link2Blind.StartSector == Link2Blind.SaveSector)&&(0 == Link2Blind.SaveStep)&&(0 == Link2Blind.SaveTimeCount))//Ϊ��
    {
            
    }
    else//��Ϊ�գ��Ѵ�������
    {
        Sector = Link2Blind.SaveSector;
        Step = Link2Blind.SaveStep;
        Step--;//��ȡ�Ѵ洢�����µ���һ��ʱ��
        if(Step < 0)
        {
            Step = Link2Blind.SectorStep - 1;
            Sector--;
            if(Sector < Link2Blind.StartSector)
            {
                Sector = Link2Blind.EndSector - 1;
            }
        }
        Addr = Sector*FLASH_ONE_SECTOR_BYTES+Step*BlindStepLen;
        sFLASH_ReadBuffer(Buffer,Addr,4);
        ttCount = Public_ConvertBufferToLong(Buffer);
        Gmtime(&tt,ttCount);

        if(ERROR == CheckTimeStruct(&tt))//����Ѵ洢�����µ���һ��ʱ���Ƿ���ȷ
        {
            sFLASH_EraseSector(Addr);
            Link2Blind.SaveStep = 0;
            return 1;
        }

        if(ERROR == CheckTimeStruct(&CurTime))//��鵱ǰʱ���Ƿ���ȷ
        {
            return 1;
        }

        TimeCount = ConverseGmtime(&CurTime);//�Ƚϵ�ǰʱ�������¼�¼����һ��ʱ��
        if(TimeCount < ttCount)
        {
            return 1;
        }
        
    }
    
    Addr = Link2Blind.SaveSector*FLASH_ONE_SECTOR_BYTES+Link2Blind.SaveStep*BlindStepLen;//��鵱ǰ�洢����һ���ĵ�ַ�Ƿ�Ϊ��
    sFLASH_ReadBuffer(Buffer,Addr,5);
    for(i=0; i<5; i++)
    {
        if(0xff != Buffer[i])
        {
            sFLASH_EraseSector(Addr);
            Link2Blind.SaveStep = 0;
            Addr = Link2Blind.SaveSector*FLASH_ONE_SECTOR_BYTES;
            for(j=0; j<200; j++)
            {
                //�ȴ�һ���
            }
            break;
        }
    }

    sum = 0;//����У���
    for(i=0; i<length; i++)
    {
        sum += *(pBuffer+i);
    }

    
    memcpy(Buffer+7,pBuffer,length);//����λ����Ϣ������

    p = Buffer;//����7�ֽڵ�����
    *p++ = (TimeCount&0xff000000) >> 24;
    *p++ = (TimeCount&0xff0000) >> 16;
    *p++ = (TimeCount&0xff00) >> 8;
    *p++ = TimeCount&0xff;
    *p++ = attribute;
    *p++ = sum;
    *p++ = length;

    sFLASH_WriteBuffer(Buffer,Addr,length+7);//д��flash

    Link2Blind.SaveStep++;//����һ��
    if(Link2Blind.SaveStep >= Link2Blind.SectorStep)
    {
        Link2Blind.SaveSector++;
        Link2Blind.SaveStep = 0;
        if(Link2Blind.SaveSector >= Link2Blind.EndSector)
        {
            Link2Blind.SaveSector = Link2Blind.StartSector;
        }
        Addr = Link2Blind.SaveSector*FLASH_ONE_SECTOR_BYTES;
        sFLASH_EraseSector(Addr);
    }

    return 0;
    
}
/**************************************************************************
//��������Blind_UpdateLink2OpenFlag
//���ܣ����±���Link2OpenFlag
//���룺��
//�������
//����ֵ����
//��ע��Link2OpenFlagΪ1��ʾ�����˵�2������
***************************************************************************/
static void Blind_UpdateLink2OpenFlag(void) 
{
    u8 Buffer[30];
    u8 BufferLen;
    u32 TcpPort;

    BufferLen = EepromPram_ReadPram(E2_SECOND_MAIN_SERVER_IP_ID, Buffer);
    if(0 == BufferLen)
    {
        Link2OpenFlag = 0;
    }
    else
    {
        BufferLen = EepromPram_ReadPram(E2_SECOND_MAIN_SERVER_TCP_PORT_ID, Buffer);
        
        if(E2_SECOND_MAIN_SERVER_TCP_PORT_ID_LEN == BufferLen)
        {
            TcpPort = Public_ConvertBufferToLong(Buffer);
            if(0 == TcpPort)
            {
                Link2OpenFlag = 0;
            }
            else
            {
                Link2OpenFlag = 1;
            }
        }
        else
        {
            Link2OpenFlag = 0;
        }
        
    }
}
/////////////////////////���½�Ϊ����ä�������������������ڼ�⣬Ҳ������ʵ��Ӧ��/////////////////
/**************************************************************************
//��������Blind_CheckLog
//���ܣ����ä��������־��������־��ӡ��DB9���ڣ����������Ƿ�������
//���룺��
//�������
//����ֵ����
//��ע��ä��������־�ļ����ƹ̶�Ϊblind.dat
***************************************************************************/
/*
void Blind_CheckLog(void)
{
	  
		
	  u32 RestLen;
//		u32 CopyLen;
		u32 i;
		u32 LastTimeCount = 0;
		u32 length = 0;
//		u8 *p=NULL;
//		u8 temp;
//		u8 flag;
	  FRESULT res;
		UINT ReadLen;
		//TIME_T Time;
	
    if(FR_OK != f_open(&BlindFile,ff_NameConver((u8 *)"blind.DAT"),FA_READ))
    {
        return;
    }
		
		RestLen=BlindFile.fsize;
		
		for(; RestLen>0; )
		{
			  length = 0;
			  ReadLen = 0;
		    res = f_read(&BlindFile,ReadBuffer,640,&ReadLen); 
			  RestLen -= ReadLen;
        if((FR_OK ==res)&&(ReadLen > 2))
        {
					  memcpy(CurHeadTwoByte,ReadBuffer,2);
					  memcpy(FourByte,LastTailTwoByte,2);
					  memcpy(FourByte+2,CurHeadTwoByte,2);
					
					  if((0x7e == FourByte[0])&&(0x07 == FourByte[1])&&(0x04 == FourByte[2]))//7e 07 04�պ÷����ڲ�ͬ�����ݿ���
						{
						    FrameSearchType = 1;
							  
						}
						else if((0x7e == FourByte[1])&&(0x07 == FourByte[2])&&(0x04 == FourByte[3]))//7e 07 04�պ÷����ڲ�ͬ�����ݿ���
						{
						    FrameSearchType = 1;
						}
					  else
						{
							  
					  }
						
						    for(;length < ReadLen;)
							  {
				            FrameSearchType = Blind_SearchFrame(&SearchLen, &FrameStart, &FrameEnd,ReadBuffer+length,ReadLen, FrameSearchType);
							      if((0xffff != FrameStart)||(0xffff != FrameEnd))
								    {
								        memcpy(ParseBuffer+ParseBufferCount,ReadBuffer+length+FrameStart,FrameEnd-FrameStart+1);
											  ParseBufferCount += FrameEnd-FrameStart+1;
											  if(0 == FrameSearchType)
												{
											      LastTimeCount = Blind_PrintFrame(ParseBuffer,ParseBufferCount,LastTimeCount);
											      ParseBufferCount = 0;
											      for(i=0; i<1000; i++)
											      {
												        ParseBuffer[i] = 0;
												    } 
												}														
								    }
										length += SearchLen;
							  }
				}
				else
				{
					  
				    break;
				}
				memcpy(LastTailTwoByte,ReadBuffer+ReadLen-2,2);
		}
}
*/
/**************************************************************************
//��������Blind_PrintFrame
//���ܣ���ӡһ֡���ݵ�DB9����
//���룺��
//�������
//����ֵ����
//��ע���������µ�����λ����Ϣʱ��
***************************************************************************/
/*
static u32 Blind_PrintFrame(u8 *pBuffer, u16 BufferLen, u32 TimeCount)
{
	
	  u8 *p = NULL;
	  u8 flag;
	  u16 ListNum;//ä���������
	  u16 ListLen;
	  u16 i;
		u16 PrintLen;
	  u32 ReturnTimeCount;
	  u32 CurTimeCount;
	  u32 LastTimeCount;
		u32 Delay;
	  TIME_T Time;
	
    p = pBuffer;
	  ReturnTimeCount = TimeCount;
	
		p += 13;//����7E(1)+������(2)+����(2)+�ֻ���(6)+��ˮ��(2)
		ListNum = 0;//���������
		ListNum |= *p++ << 8;
		ListNum |= *p++;
		p++;//����λ����������
	  flag = 0;
		for(i=0; i<ListNum; i++)
		{
				 ListLen = 0;//λ�û㱨�����峤��
				 ListLen |= *p++ << 8;
				 ListLen |= *p++;
				 if(ListLen < 28)
				 {
							return ReturnTimeCount;
			   }
				 p += 22;//����������(4)+״̬��(4)+����(4)+γ��(4)+�ٶ�(2)+�߳�(2)+����(2)
				 VDRPub_ConvertBCDToTime(&Time,p);
				 if(SUCCESS != CheckTimeStruct(&Time))
         {
							return ReturnTimeCount;
				 }
				 CurTimeCount = ConverseGmtime(&Time);
				 if(0 == i)
				 {
				     if(1 != (CurTimeCount - ReturnTimeCount))
						 {
						     flag = 1;
						 }
				 }
				 else
				 {
				     if(1 != (CurTimeCount - LastTimeCount))
						 {
						     flag = 1;
						 }
				 }
				 p += 6;//ʱ��
				 p += ListLen-28;
				 LastTimeCount = CurTimeCount;
				 ReturnTimeCount = CurTimeCount;
		}
				
    PrintPacketNum++;		
		PrintLen = VDRPub_HEX2ASC(PrintBuffer, pBuffer,BufferLen);
										
		if(1 == flag)//���쳣
		{
				memcpy(PrintBuffer+PrintLen,"------------------------------",30);
				PrintLen += 30;
				sprintf((char *)PrintBuffer+PrintLen,"%8d",PrintPacketNum);
				PrintLen += 8;
				memcpy(PrintBuffer+PrintLen,"----------ERROR---------------",30);
			  PrintLen += 30;
			  PrintBuffer[PrintLen++] = 0x0d;
			  PrintBuffer[PrintLen++] = 0x0a;
		}
		else
		{
				memcpy(PrintBuffer+PrintLen,"------------------------------",30);
				PrintLen += 30;
				sprintf((char *)PrintBuffer+PrintLen,"%8d",PrintPacketNum);
				PrintLen += 8;
				memcpy(PrintBuffer+PrintLen,"-----------OK------------------",30);
				PrintLen += 30;
			  PrintBuffer[PrintLen++] = 0x0d;
			  PrintBuffer[PrintLen++] = 0x0a;
		}
										
		VDRUsart_SendData(PrintBuffer,PrintLen);
		
		for(Delay=20000000; Delay>0; Delay--)//����ʱ��Ϊ�����������ڸ���ʱ�׶η��ͳ�ȥ
		{
										
		}
		
		return ReturnTimeCount;
}
*/
/**************************************************************************
//��������Blind_SearchFrame
//���ܣ�����һ֡ä����������
//���룺Type ���������ͣ�1����ʾƴ�ӣ�0����ʾȫ������
//�������
//����ֵ���������ͣ�1����ʾƴ�ӣ�0����ʾȫ������
//��ע�������ҵ�һ֡����ʱ����������������������0����û���ҵ�һ֡����ʱ����ֵ��������ֵType
***************************************************************************/
/*
static u8 Blind_SearchFrame(u16 *SearchLen, u16 *FrameStart, u16 *FrameEnd, u8 *pBuffer, u16 BufferLen, u8 Type)
{
    u8 *p = NULL;
	  u8  flag;
	  u16 i,j;
//	  u16 length;
	
	  p = pBuffer;
	
	  *FrameStart = 0xffff;
	  *FrameEnd = 0xffff;
	
	  if(1 == Type)
		{
			  flag = 0;
		    for(i=0; i<BufferLen; i++)
			  {
				    if(0x7e == *(p+i))
						{
							  *FrameStart = 0;
							  *FrameEnd = i;
							  *SearchLen = i+1;
							  flag = 1;
						    break;
						}
				}
				if(1 == flag)
				{
				    return 0;
				}
				else
				{
				    return Type;
				}
		}
		else if(0 == Type)
		{
		    flag = 0;
		    for(i=0; i<BufferLen-2; i++)
			  {
				    if((0x7e == *(p+i))&&(0x07 == *(p+i+1))&&(0x04 == *(p+i+2)))
						{
							  *FrameStart = i;
							  j = i+2;
							  flag = 1;
						    break;
						}
				}
				if(1 == flag)//�ҵ���ͷ
				{
					  flag = 0;
				    for(i=j; i<BufferLen; i++)
					  {
						    if(0x7e == *(p+i))
						    {
							      *FrameEnd = i;
									  *SearchLen = i+1;
							      flag = 1;
						        break;
						    }
						
						}
						if(1 == flag)//�ҵ�����һ֡����
						{
						    return 0;
						}
						else//�ҵ�ǰ�벿��
						{
							  *FrameEnd = BufferLen-1;
								*SearchLen = BufferLen;
						    return 1;
						}
				}
				else//û�ҵ�
				{
				    *SearchLen = BufferLen;
					  return Type;
				}
		}
		else//���ʹ���
		{
			  *SearchLen = BufferLen;
		    return Type;
		}
		
//		return Type;
}
*/

























