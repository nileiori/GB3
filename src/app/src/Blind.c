/************************************************************************
//程序名称：Blind.c
//功能：该模块实现盲区补报功能。
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2014.10
//版本记录：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：需要实现双连接的盲区补报。连接1和连接2的盲区存储区域是独立的、分开的。
//但如果只使用了连接1（连接2未开启，连接2 TCP端口参数设置为0时表示连接2不开启），
//两个存储区域合二为一。每次终端上线后会重新检测一次看是否需要补报，若有未补报的
//数据，则开始补报，每次把5条位置信息打包补报，只有收到平台应答后才会补报下一包，
//否则一直补报当前包，最多发送10次，每次20秒。若10次后仍然没有应答
//则会等待30分钟后再重发当前包。
*************************************************************************/

/********************文件包含*************************/
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

/********************本地变量*************************/
static BLIND_STRUCT Link1Blind;
static s16 Link1SearchSector;
static u16 Link1TimeCount = 0;
static u32 Link1WaitTimeCount = 0;

static BLIND_STRUCT Link2Blind;
static s16 Link2SearchSector;
static u16 Link2TimeCount = 0;
static u32 Link2WaitTimeCount = 0;
static u16 Link2OpenFlag = 0;//连接2是否开启标志，1为开启，0为未开启
static s16  BlindStepLen = 0;//盲区每步的长度
static s16  BlindPacketNum = 0;//盲区补报时打包的条数，实际出货时为5条，检测时为15-20条
//static FIL BlindFile;

//static u8 PrintBuffer[2200] = {0};
//static u16 PrintPacketNum = 0;
//static u8 ReadBuffer[1000] = {0};
//static u8 ParseBuffer[1000] = {0};
//static u8 LastTailTwoByte[2] = {0,0};//上一个数据块最后（尾部）2字节
//static u8 CurHeadTwoByte[2] = {0,0};//当前数据块开头（头部）2字节
//static u8 FourByte[4] = {0};
//static u8 FrameSearchType = 0;
//static u32 ParseBufferCount = 0;
//static u16 FrameStart = 0xffff;
//static u16 FrameEnd = 0xffff;
//static u16 SearchLen = 0;

static u8 Link1BlindSaveEnableFlag = 0;
//static u8 Link2BlindSaveEnableFlag = 0;

/********************全局变量*************************/


/********************外部变量*************************/
extern u8 RadioShareBuffer[];
extern TIME_T CurTime;
extern u8   GBTestFlag;//0:正常出货运行模式;1:国标检测模式
extern u8  BBGNTestFlag;//0为正常模式，1为部标功能检测模式
extern u32 Link1ReportTimeSpace;//同Link1ReportTime一样，用作外部访问
extern Queue  VdrTxQ;//VDR发送队列
extern u8  BBXYTestFlag;//0为正常模式，1为部标协议检测模式
/********************本地函数声明*********************/
static void Blind_Link1Init(void);//连接1变量初始化
static void Blind_Link1Search(void);//连接1查找确定盲区记录、上报的初始位置
static void Blind_Link1SearchSector(s16 SearchSector);//检查某个扇区中的盲区信息
static u8 Blind_Link1Report(void);//连接1上报一包盲区数据
static void Blind_Link1ReportAck(void);//连接1盲区补报应答处理
static u8 Blind_GetLink1ReportFlag(void);//获取连接1盲区上报标志
static void Blind_StartLink1Report(void);//开启连接1盲区补报
static void Blind_Link1Erase(void);//擦除连接1盲区存储区域
static u8 Blind_Link1Save(u8 *pBuffer, u8 length, u8 attribute);//存储一条连接1的盲区数据
static void Blind_StartLink1Report(void);

static void Blind_Link2Init(void);//连接2变量初始化
static void Blind_Link2Search(void);//连接2查找确定盲区记录、上报的初始位置
static void Blind_Link2SearchSector(s16 SearchSector);//检查某个扇区中的盲区信息
static u8 Blind_Link2Report(void);//连接2上报一包盲区数据
static void Blind_Link2ReportAck(void);//连接2盲区补报应答处理
static u8 Blind_GetLink2ReportFlag(void);//获取连接2盲区上报标志
static void Blind_StartLink2Report(void);//开启连接2盲区补报
static void Blind_Link2Erase(void);//擦除连接2盲区存储区域
static u8 Blind_Link2Save(u8 *pBuffer, u8 length, u8 attribute);//存储一条连接2的盲区数据
static void Blind_StartLink2Report(void);

static void Blind_UpdateLink2OpenFlag(void);//更新变量Link2OpenFlag

//static u8 Blind_SearchFrame(u16 *SearchLen, u16 *FrameStart, u16 *FrameEnd, u8 *pBuffer, u16 BufferLen, u8 Type);
//static u32 Blind_PrintFrame(u8 *pBuffer, u16 BufferLen, u32 TimeCount);
//static u16 Blind_CheckDataIsOK(void);
/********************函数定义*************************/
/**************************************************************************
//函数名：Blind_TimeTask
//功能：实现盲区补报功能
//输入：无
//输出：无
//返回值：无
//备注：盲区补报定时任务，50ms调度1次，任务调度器需要调用此函数。
//当在进行FTP升级时需要关闭该任务，因为盲区记录与固件远程升级共用一块存储区
//注意如果谁关闭了该任务，谁就要负责打开该任务，上电启动默认是打开的
***************************************************************************/
FunctionalState Blind_TimeTask(void)
{
	  //static u8 flag1  = 0;
	  static u8 flag2  = 0;
	
    if(1 == GBTestFlag)//国标检测时该区域用于记录日志
    {
        return DISABLE;
    }
		
		if((1 == BBGNTestFlag)&&(1 == Link1BlindSaveEnableFlag))//dxl,2016.5.19盲区记录期间
		{
		    return ENABLE;
		}
    
    /**************连接1盲区补报****************/
    if(BLIND_STATE_INIT == Link1Blind.State)//初始化所有相关变量
    {
        Blind_Link1Init();//完成后自动转入BLIND_STATE_SEARCH 
    }
    else if(BLIND_STATE_SEARCH == Link1Blind.State)//逐步查找确定当前的盲区补报位置
    {
        Blind_Link1Search();//完成后自动转入BLIND_STATE_REPORT
    }
    else if(BLIND_STATE_REPORT == Link1Blind.State)//盲区上报
    {
			  //if((0 == flag1)&&(1 == BBGNTestFlag))
				//{
					  //Blind_Erase(0xff);
					  //LcdClearScreen(); 
            //LcdShowCaptionEx((char *)"盲区1自检完成",2); 
			     // Blind_CheckDataIsOK();//dxl,2016.5.19
					  //flag1 = 1;
				//}
        if(Link1ReportTimeSpace < 5)//dxl,2016.5.10部标盲区检测时是1秒一条，为了使盲区部标更可靠，做此限定
				{
				
				}
				else
				{
        Link1WaitTimeCount = 0; 
        if(CHANNEL_DATA_1 == (CHANNEL_DATA_1&GetTerminalAuthorizationFlag()))//只有连接1在线时才补报
        {
            Link1TimeCount++;
            if(Link1TimeCount >= BLIND_REPORT_DELAY_TIME)//20秒
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
                    if(0 == Blind_Link1Report())//盲区上报
                    {
						            Gnss_StartBlindReport();
                        Link1Blind.State = BLIND_STATE_WAIT;//转入等待状态
                        
                    }
                }
                else if(Link1Blind.ReportSendNum > 10) 
                {
                    Link1Blind.State = BLIND_STATE_WAIT;//转入等待状态
                }
            }
					}
				}
    }
    else if(BLIND_STATE_WAIT == Link1Blind.State)//等待
    {
        Link1WaitTimeCount++;
        if(Link1WaitTimeCount >= (18000*SECOND))//300分钟dxl,2016.5.19避开盲区检测时段（3小时）
        {
            Link1WaitTimeCount = 0;
            Link1Blind.State = BLIND_STATE_INIT;
        }
    }
    else//异常
    {
        Link1Blind.State = BLIND_STATE_INIT;
    }


    /**************连接2盲区补报****************/
    if(0 == Link2OpenFlag)
    {
        return ENABLE;
    }

    if(BLIND_STATE_INIT == Link2Blind.State)//初始化所有相关变量
    {
        Blind_Link2Init();//完成后自动转入BLIND_STATE_SEARCH
        
    }
    else if(BLIND_STATE_SEARCH == Link2Blind.State)//逐步查找确定当前的盲区补报位置
    {
        Blind_Link2Search();//完成后自动转入BLIND_STATE_REPORT
    }
    else if(BLIND_STATE_REPORT == Link2Blind.State)//盲区上报
    {
			  if((0 == flag2)&&(1 == BBGNTestFlag))
				{
					  LcdClearScreen(); 
            LcdShowCaptionEx((char *)"盲区2自检完成",2); 
					  flag2 = 1;
				}
        Link2WaitTimeCount = 0; 
        //if(0x01 == (0x01&GetTerminalAuthorizationFlag()))//只有连接1在线时才补报
        //{
            Link2TimeCount++;
            if(Link2TimeCount >= BLIND_REPORT_DELAY_TIME)//20秒
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
                    if(0 == Blind_Link2Report())//盲区上报
                    {
                        Link2Blind.State = BLIND_STATE_WAIT;//转入等待状态
                    }
                }
                else if(Link2Blind.ReportSendNum > 10)
                {
                    Link2Blind.State = BLIND_STATE_WAIT;//转入等待状态
                }
            }
        //}
    }
    else if(BLIND_STATE_WAIT == Link2Blind.State)//等待
    {
        Link2WaitTimeCount++;
        if(Link2WaitTimeCount >= (18000*SECOND))//300分钟dxl,2016.5.19避开盲区检测时段（3小时）
        {
            Link2WaitTimeCount = 0;
            Link2Blind.State = BLIND_STATE_INIT;
        }
    }
    else//异常
    {
        Link2Blind.State = BLIND_STATE_INIT;
    }

    

    return ENABLE;
}
/**************************************************************************
//函数名：Blind_Save
//功能：保存某通道的一条盲区数据（位置汇报数据）
//输入：通道号，盲区数据，数据长度
//输出：无
//返回值：0表示成功，1表示失败，参数不正确会返回失败
//备注：通道号只能为CHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2其中之一
***************************************************************************/
u8 Blind_Save(u8 channel, u8 *pBuffer, u8 length)
{
    u8 flag = 0;
    u8 attribute;
	
	  if(1 == BBXYTestFlag)//部标协议检测时不记录盲区
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
//函数名：Blind_Erase
//功能：擦除某通道的所有存储扇区
//输入：通道号，只能为CHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2其中之一
//输出：无
//返回值：0表示成功，1表示失败，参数不正确会返回失败
//备注：要擦除所有的盲区数据，参数需为0xff，在下载远程升级固件之前需先擦除
//所有盲区区域，因为盲区存储区域与固件存储区域共用同一块flash空间
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
//函数名：Blind_StartReport
//功能：开始某个通道的盲区补报
//输入：通道号
//输出：无
//返回值：无
//备注：通道号只能为CHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2其中之一
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
//函数名：Blind_ReportAck
//功能：盲区补报应答处理
//输入：通道号，只能为CHANNEL_SMS,CHANNEL_DATA_1,CHANNEL_DATA_2其中之一
//输出：无
//返回值：0表示成功，1表示失败，参数不正确会返回失败
//备注：收到盲区补报应答后需调用此函数
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
//函数名：Blind_GetLink2OpenFlag
//功能：获取第2个连接开启标志
//输入：无
//输出：无
//返回值：第2个连接开启标志
//备注：Link2OpenFlag为1表示开启了第2个连接，为0表示没有开启
***************************************************************************/
u8 Blind_GetLink2OpenFlag(void)
{
    return Link2OpenFlag;
}
/**************************************************************************
//函数名：Blind_EnableLink1Save
//功能：使能连接1记录盲区
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void Blind_EnableLink1Save(void)
{
    Link1BlindSaveEnableFlag = 1;
}
/**************************************************************************
//函数名：Blind_DisableLink1Save
//功能：禁止连接1记录盲区
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void Blind_DisableLink1Save(void)
{
    Link1BlindSaveEnableFlag = 0;
}
/**************************************************************************
//函数名：Blind_EnableLink2Save
//功能：使能连接2记录盲区
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void Blind_EnableLink2Save(void)
{
//    Link2BlindSaveEnableFlag = 1;
}
/**************************************************************************
//函数名：Blind_DisableLink2Save
//功能：禁止连接2记录盲区
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void Blind_DisableLink2Save(void)
{
//    Link2BlindSaveEnableFlag = 0;
}
/**************************************************************************
//函数名：Blind_Link1Init
//功能：连接1变量初始化
//输入：无
//输出：无
//返回值：无
//备注：盲区补报定时任务需要调用此函数
***************************************************************************/
static void Blind_Link1Init(void)
{
    u8 i;

    //读第2个连接的TCP端口号，判断是否开启第2个连接，开启了则需要拆分存储区域，每次该参数状态变化时需要擦除flash
    //为了简单点，先不做切换，整个存储区域先分成两块，前一半属于连接1，后一半属于连接2
    Blind_UpdateLink2OpenFlag();
    if(1 == Link2OpenFlag)//开启了连接2
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
		
		if(1 == BBGNTestFlag)//dxl,2016.4.7兼容部标检测和实际应用
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
    Link1Blind.OldestTimeCount = 0;//赋给最小值
    Link1Blind.SaveSector = Link1Blind.StartSector;
    Link1Blind.SaveStep = 0;
    Link1Blind.SaveTimeCount = 0;//赋给最小值
    Link1Blind.SaveEnableFlag = 0;
    Link1Blind.ReportSendNum = 0;
		
		
		BlindPacketNum = 800/BlindStepLen;//dxl,2016.4.12,盲区补报打包条数改成可变
		
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
//函数名：Blind_Link1Search
//功能：连接1查找确定盲区记录、上报的初始位置
//输入：无
//输出：无
//返回值：无
//备注：盲区补报定时任务需要调用此函数
***************************************************************************/
static void Blind_Link1Search(void)
{
    if((Link1SearchSector >= Link1Blind.StartSector)&&(Link1SearchSector < Link1Blind.EndSector))
    {
        Blind_Link1SearchSector(Link1SearchSector);//查找连接1某个扇区中的每条盲区信息
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
            //盲区区域为空
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
//函数名：Blind_Link1SearchSector
//功能：检查某个扇区中的盲区信息
//输入：扇区号
//输出：当前存储和补报盲区的位置
//返回值：无
//备注：
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
    sFLASH_ReadBuffer(&flag,Addr,1);//读整个扇区是否已补报标志
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

    Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + (Link1Blind.SectorStep-1)*BlindStepLen;//读扇区的最后1步
    sFLASH_ReadBuffer(Buffer,Addr,4);
    ttCount = Public_ConvertBufferToLong(Buffer);
    if(0xffffffff != ttCount)
    {
        Gmtime(&tt,ttCount);
        if(SUCCESS == CheckTimeStruct(&tt))
        {
            sFLASH_ReadBuffer(Buffer,Addr,BlindStepLen);
            if(CHANNEL_DATA_1 != Buffer[4])//通道不正确
            {
                Link1Blind.ErrorCount++;
                if(Link1Blind.ErrorCount > 3)
                {
                    Link1Blind.ErrorCount = 0;
                    Blind_Erase(0xff);//擦除所有盲区存储区域
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
                    Blind_Erase(0xff);//擦除所有盲区存储区域
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
                Blind_Erase(0xff);//擦除所有盲区存储区域
                return ;
            }
        }
    }
    else
    {
        Addr = SearchSector*FLASH_ONE_SECTOR_BYTES;//读扇区的第1步
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
                    if(CHANNEL_DATA_1 != Buffer[4])//通道不正确
                    {
                        Link1Blind.ErrorCount++;
                        if(Link1Blind.ErrorCount > 3)
                        {
                            Link1Blind.ErrorCount = 0;
                            Blind_Erase(0xff);//擦除所有盲区存储区域
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
                            Blind_Erase(0xff);//擦除所有盲区存储区域
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
                        Blind_Erase(0xff);//擦除所有盲区存储区域
                        return ;
                    }
                }
            }
        }
    }

/*每个扇区每一步都检查，太浪费时间，优化后的只检查第1步和最后1步
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
                if(CHANNEL_DATA_1 != Buffer[4])//通道不正确
                {
                    Link1Blind.ErrorCount++;
                    if(Link1Blind.ErrorCount > 3)
                    {
                        Link1Blind.ErrorCount = 0;
                        Blind_Erase(0xff);//擦除所有盲区存储区域
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
//函数名：Blind_CheckDataIsOK
//功能：检查盲区1数据是否正确
//输入：无
//输出：无
//返回值：不正确的条数
//备注：纯粹为了盲区补报时间连续性判断，不用于实际应用
***************************************************************************/
/*
static u16 Blind_CheckDataIsOK(void)
{
	  static u16 TotalErrorCount;
	  static u16 GpsTimeErrorCount;
	  static u16 RtcTimeErrorCount;
    static u16 ErrorCount;//错误条数
	  static u16 FFErrorCount;//未存储条数计数
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
    ErrorCount = 0;//错误条数
	  FFErrorCount = 0;//未存储条数计数
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
                if(CHANNEL_DATA_1 != Buffer[4])//通道不正确
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
//函数名：Blind_Link1Report
//功能：连接1上报一包盲区数据
//输入：无
//输出：无
//返回值：无
//备注：盲区补报定时任务需要调用此函数
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
    u16 length = 3;//预留3个字节



    for(count=0; count<BlindPacketNum; )//每包包含5条位置信息
    {
        if(0 == Blind_GetLink1ReportFlag())
        {
            break;
        }
        Addr = Link1Blind.ReportSector*FLASH_ONE_SECTOR_BYTES + Link1Blind.SectorStep*BlindStepLen + 1;
        sFLASH_ReadBuffer(&flag,Addr,1);//读整个扇区是否已补报标志
        if(0xaa == flag)//已补报过
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
                sFLASH_ReadBuffer(&flag,Addr,1);//读该条是否已补报标志
                if(0xaa == flag)//已补报过
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
                            RadioShareBuffer[length] = 0;//长度高字节填补0
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
        RadioShareBuffer[0] = 0;//两个字节表示项数
        RadioShareBuffer[1] = count;
        RadioShareBuffer[2] = 1;//0:正常的位置批量汇报;1:盲区补报
        RadioProtocol_PostionInformation_BulkUpTrans(CHANNEL_DATA_1,RadioShareBuffer,length);
    }

    return 1;
}
/**************************************************************************
//函数名：Blind_Link1ReportAck
//功能：连接1盲区补报应答处理
//输入：无
//输出：无
//返回值：无
//备注：收到连接1的盲区补报应答时需调用此函数
***************************************************************************/
static void Blind_Link1ReportAck(void)
{
    u8 i;
    u8 flag;
    u32 Addr;
    s16 Sector;

    Link1Blind.ReportSendNum = 0;
    Link1TimeCount = BLIND_REPORT_DELAY_TIME-9;//0.4秒后上报下一包

    Sector = Link1Blind.LastReportSector[0];

    for(i=0; i<BlindPacketNum; i++)
    {
        if((Link1Blind.LastReportSector[i] >= Link1Blind.StartSector)
            &&(Link1Blind.LastReportSector[i] < Link1Blind.EndSector)
                &&(Link1Blind.LastReportStep[i] < Link1Blind.SectorStep))
        {
            Addr = Link1Blind.LastReportSector[i]*FLASH_ONE_SECTOR_BYTES+(Link1Blind.LastReportStep[i]+1)*BlindStepLen - 1;
            flag = 0xaa;
            sFLASH_WriteBuffer(&flag, Addr, 1);//标记该条盲区已上报

            if(Link1Blind.LastReportStep[i] >= (Link1Blind.SectorStep-1))
            {
                Addr = Sector*FLASH_ONE_SECTOR_BYTES+Link1Blind.SectorStep*BlindStepLen + 1;
                flag = 0xaa;
                sFLASH_WriteBuffer(&flag, Addr, 1);//标记该扇区的盲区已上报
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
//函数名：Blind_GetLink1ReportFlag
//功能：获取连接1盲区上报标志
//输入：无
//输出：无
//返回值：盲区上报标志
//备注：1表示需要上报，0表示不需要上报，终端每隔10分钟会自动检测一次看是否需要上报
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
//函数名：Blind_StartLink1Report
//功能：开启连接1盲区补报
//输入：无
//输出：无
//返回值：无
//备注：开启盲区补报，每次连接1鉴权成功需调用此函数
***************************************************************************/
static void Blind_StartLink1Report(void)
{
    //if(BLIND_STATE_WAIT == Link1Blind.State)
    //{
        Link1Blind.State = BLIND_STATE_INIT;
    //}
}
/**************************************************************************
//函数名：Blind_Link1Erase
//功能：擦除连接1盲区存储区域
//输入：无
//输出：无
//返回值：无
//备注：检测到盲区记录的位置信息有误时（有误的条数大于50）会调用此函数，
//或者是远程FTP升级擦除区域时会调用此函数，因为盲区与远程固件升级共用一块区域
***************************************************************************/
static void Blind_Link1Erase(void)
{
    u16 i;
    u32 Addr;
	
	  if(1 == BBGNTestFlag)
		{
		    LcdClearScreen(); 
        LcdShowCaptionEx((char *)"擦除盲区1",2); 
		}
    
    Link1Blind.SaveEnableFlag = 0;//禁止存储
    Link1Blind.State = BLIND_STATE_INIT;

    for(i=Link1Blind.StartSector; i<Link1Blind.EndSector; i++)
    {
        Addr = i*FLASH_ONE_SECTOR_BYTES;
        sFLASH_EraseSector(Addr);
        IWDG_ReloadCounter();
    }   
}
/**************************************************************************
//函数名：Blind_Link1Save
//功能：存储一条连接1的盲区数据
//输入：一条位置信息及长度
//输出：无
//返回值：0为成功，非0为失败，长度超出时会返回失败
//备注：一条盲区数据包括：时间4字节+属性1字节（预留）+校验和1字节+位置信息长度1字节+位置信息若干字节
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

    if((length > (BlindStepLen-9))||(length < 28)||(0 == Link1Blind.SaveEnableFlag))//每步最后两个字节用作错误标记，已上传标记
    {
        return 1;
    }
    
		
    if((Link1Blind.StartSector == Link1Blind.SaveSector)&&(0 == Link1Blind.SaveStep)&&(0 == Link1Blind.SaveTimeCount))//为空
    {
            
    }
    else//不为空，已存有数据
    {
        Sector = Link1Blind.SaveSector;
        Step = Link1Blind.SaveStep;
        Step--;//获取已存储的最新的那一步时间
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

        if(ERROR == CheckTimeStruct(&tt))//检查已存储的最新的那一步时间是否正确
        {
            sFLASH_EraseSector(Addr);
            Link1Blind.SaveStep = 0;
            return 1;
        }

        if(ERROR == CheckTimeStruct(&CurTime))//检查当前时间是否正确
        {
            return 1;
        }

        //TimeCount = ConverseGmtime(&CurTime);//比较当前时间与最新记录的那一步时间
				TimeCount = RTC_GetCounter();
        if(TimeCount < ttCount)
        {
            return 1;
        }
        
    }
    
    Addr = Link1Blind.SaveSector*FLASH_ONE_SECTOR_BYTES+Link1Blind.SaveStep*BlindStepLen;//检查当前存储的那一步的地址是否为空
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
                //等待一会儿
            }
            break;
        }
    }

    sum = 0;//计算校验和
    for(i=0; i<length; i++)
    {
        sum += *(pBuffer+i);
    }

    
    memcpy(Buffer+7,pBuffer,length);//拷贝位置信息至缓冲

    p = Buffer;//补填7字节的数据
    *p++ = (TimeCount&0xff000000) >> 24;
    *p++ = (TimeCount&0xff0000) >> 16;
    *p++ = (TimeCount&0xff00) >> 8;
    *p++ = TimeCount&0xff;
    *p++ = attribute;
    *p++ = sum;
    *p++ = length;

    sFLASH_WriteBuffer(Buffer,Addr,length+7);//写入flash

    Link1Blind.SaveStep++;//下移一步
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
//函数名：Blind_Link2Init
//功能：连接2变量初始化
//输入：无
//输出：无
//返回值：无
//备注：盲区补报定时任务需要调用此函数
***************************************************************************/
static void Blind_Link2Init(void)
{
    u8 i;

    //读第2个连接的TCP端口号，判断是否开启第2个连接，开启了则需要拆分存储区域，每次该参数状态变化时需要擦除flash
    //整个存储区域先分成两块，前一半属于连接1，后一半属于连接2
    Blind_UpdateLink2OpenFlag();
    Link2Blind.StartSector = FLASH_BLIND_MIDDLE_SECTOR;
    Link2Blind.EndSector = FLASH_BLIND_END_SECTOR;
    Link2Blind.SectorStep = FLASH_ONE_SECTOR_BYTES/BlindStepLen;
    Link2Blind.OldestSector = Link2Blind.StartSector;
    Link2Blind.OldestStep = 0;
    Link2Blind.OldestTimeCount = 0;//赋给最小值
    Link2Blind.SaveSector = Link2Blind.StartSector;
    Link2Blind.SaveStep = 0;
    Link2Blind.SaveTimeCount = 0;//赋给最小给
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
//函数名：Blind_Link2Search
//功能：连接2查找确定盲区记录、上报的初始位置
//输入：无
//输出：无
//返回值：无
//备注：盲区补报定时任务需要调用此函数
***************************************************************************/
static void Blind_Link2Search(void)
{
    if((Link2SearchSector >= Link2Blind.StartSector)&&(Link2SearchSector < Link2Blind.EndSector))
    {
        Blind_Link2SearchSector(Link2SearchSector);//查找连接1某个扇区中的每条盲区信息
    }
    else
    {
        Link2Blind.State = BLIND_STATE_INIT;
        return ;
    }
    
    if(Link2Blind.ErrorCount > 3)
    {
        Link2Blind.ErrorCount = 0;
        Blind_Link2Erase();//擦除连接1整个盲区flash
        Link2Blind.State = BLIND_STATE_INIT;
        return ;
    }

    Link2SearchSector++;
    if(Link2SearchSector >= Link2Blind.EndSector)
    {
        if((Link2Blind.StartSector == Link2Blind.SaveSector)&&(0 == Link2Blind.SaveStep)&&(0 == Link2Blind.SaveTimeCount))
        {
            //盲区区域为空
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
//函数名：Blind_Link2SearchSector
//功能：检查某个扇区中的盲区信息
//输入：扇区号
//输出：当前存储和补报盲区的位置
//返回值：无
//备注：
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
    sFLASH_ReadBuffer(&flag,Addr,1);//读整个扇区是否已补报标志
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

    Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + (Link2Blind.SectorStep-1)*BlindStepLen;//读扇区最后1步
    sFLASH_ReadBuffer(Buffer,Addr,4);
    ttCount = Public_ConvertBufferToLong(Buffer);
    if(0xffffffff != ttCount)
    {
        Gmtime(&tt,ttCount);
        if(SUCCESS == CheckTimeStruct(&tt))
        {
            Addr = SearchSector*FLASH_ONE_SECTOR_BYTES + (Link2Blind.SectorStep-1)*BlindStepLen;
            sFLASH_ReadBuffer(Buffer,Addr,BlindStepLen);
            if(CHANNEL_DATA_2 != Buffer[4])//通道不正确
            {
                Link2Blind.ErrorCount++;
                if(Link2Blind.ErrorCount > 3)
                {
                    Blind_Erase(0xff);//擦除所有盲区存储区域
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
                    Blind_Erase(0xff);//擦除所有盲区存储区域
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
                Blind_Erase(0xff);//擦除所有盲区存储区域
                return;
            }
        }
    }
    else
    {
        Addr = SearchSector*FLASH_ONE_SECTOR_BYTES;//读扇区第1步
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
                        if(CHANNEL_DATA_2 != Buffer[4])//通道不正确
                        {
                            Link2Blind.ErrorCount++;
                            if(Link2Blind.ErrorCount > 3)
                            {
                                Blind_Erase(0xff);//擦除所有盲区存储区域
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
                            Blind_Erase(0xff);//擦除所有盲区存储区域
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
                if(CHANNEL_DATA_2 != Buffer[4])//通道不正确
                {
                    Link2Blind.ErrorCount++;
                    if(Link2Blind.ErrorCount > 3)
                    {
                        Blind_Erase(0xff);//擦除所有盲区存储区域
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
//函数名：Blind_Link2Report
//功能：连接2上报一包盲区数据
//输入：无
//输出：无
//返回值：无
//备注：盲区补报定时任务需要调用此函数
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
    u16 length = 3;//预留3个字节


    for(count=0; count<BlindPacketNum; )//每包包含5条位置信息
    {
        if(0 == Blind_GetLink2ReportFlag())
        {
            break;
        }
        Addr = Link2Blind.ReportSector*FLASH_ONE_SECTOR_BYTES + Link2Blind.SectorStep*BlindStepLen + 1;
        sFLASH_ReadBuffer(&flag,Addr,1);//读整个扇区是否已补报标志
        if(0xaa == flag)//已补报过
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
                sFLASH_ReadBuffer(&flag,Addr,1);//读该条是否已补报标志
                if(0xaa == flag)//已补报过
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
                            RadioShareBuffer[length] = 0;//长度高字节填补0
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
        RadioShareBuffer[0] = 0;//两个字节表示项数
        RadioShareBuffer[1] = count;
        RadioShareBuffer[2] = 1;//0:正常的位置批量汇报;1:盲区补报
        RadioProtocol_PostionInformation_BulkUpTrans(CHANNEL_DATA_2,RadioShareBuffer,length);
    }

    return 1;
}
/**************************************************************************
//函数名：Blind_Link2ReportAck
//功能：连接2盲区补报应答处理
//输入：无
//输出：无
//返回值：无
//备注：收到连接2的盲区补报应答时需调用此函数
***************************************************************************/
static void Blind_Link2ReportAck(void)
{
    u8 i;
    u8 flag;
    u32 Addr;
    s16 Sector;

    Link2Blind.ReportSendNum = 0;
    Link2TimeCount = BLIND_REPORT_DELAY_TIME-20;//1秒后上报下一包

    Sector = Link2Blind.LastReportSector[0];

    for(i=0; i<BlindPacketNum; i++)
    {
        if((Link2Blind.LastReportSector[i] >= Link2Blind.StartSector)
            &&(Link2Blind.LastReportSector[i] < Link2Blind.EndSector)
                &&(Link2Blind.LastReportStep[i] < Link2Blind.SectorStep))
        {
            Addr = Link2Blind.LastReportSector[i]*FLASH_ONE_SECTOR_BYTES+(Link2Blind.LastReportStep[i]+1)*BlindStepLen - 1;
            flag = 0xaa;
            sFLASH_WriteBuffer(&flag, Addr, 1);//标记该条盲区已上报

            if(Link2Blind.LastReportStep[i] >= (Link2Blind.SectorStep-1))
            {
                Addr = Sector*FLASH_ONE_SECTOR_BYTES+Link2Blind.SectorStep*BlindStepLen + 1;
                flag = 0xaa;
                sFLASH_WriteBuffer(&flag, Addr, 1);//标记该扇区的盲区已上报
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
//函数名：Blind_GetLink2ReportFlag
//功能：获取连接2盲区上报标志
//输入：无
//输出：无
//返回值：盲区上报标志
//备注：1表示需要上报，0表示不需要上报，终端每隔10分钟会自动检测一次看是否需要上报
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
//函数名：Blind_StartLink2Report
//功能：开启连接2盲区补报
//输入：无
//输出：无
//返回值：无
//备注：开启盲区补报，每次连接1鉴权成功需调用此函数
***************************************************************************/
static void Blind_StartLink2Report(void)
{
    //if(BLIND_STATE_WAIT == Link1Blind.State)
    //{
        Link2Blind.State = BLIND_STATE_INIT;
    //}
}
/**************************************************************************
//函数名：Blind_Link2Erase
//功能：擦除连接2盲区存储区域
//输入：无
//输出：无
//返回值：无
//备注：检测到盲区记录的位置信息有误时（有误的条数大于50）会调用此函数，
//或者是远程FTP升级擦除区域时会调用此函数，因为盲区与远程固件升级共用一块区域
***************************************************************************/
static void Blind_Link2Erase(void)
{
    u16 i;
    u32 Addr;
	
	  if(1 == BBGNTestFlag)
		{
		    LcdClearScreen(); 
        LcdShowCaptionEx((char *)"擦除盲区2",2); 
		}
    
    if(1 == Link2OpenFlag)
    {
        Link2Blind.SaveEnableFlag = 0;//禁止存储
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
//函数名：Blind_Link2Save
//功能：存储一条连接2的盲区数据
//输入：一条位置信息及长度
//输出：无
//返回值：0为成功，非0为失败，长度超出时会返回失败
//备注：一条盲区数据包括：时间4字节+属性1字节（预留）+校验和1字节+位置信息长度1字节+位置信息若干字节
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

    if((length > (BlindStepLen-9))||(length < 28)||(0 == Link2Blind.SaveEnableFlag))//每步最后两个字节用作错误标记，已上传标记
    {
        return 1;
    }
    
    if((Link2Blind.StartSector == Link2Blind.SaveSector)&&(0 == Link2Blind.SaveStep)&&(0 == Link2Blind.SaveTimeCount))//为空
    {
            
    }
    else//不为空，已存有数据
    {
        Sector = Link2Blind.SaveSector;
        Step = Link2Blind.SaveStep;
        Step--;//获取已存储的最新的那一步时间
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

        if(ERROR == CheckTimeStruct(&tt))//检查已存储的最新的那一步时间是否正确
        {
            sFLASH_EraseSector(Addr);
            Link2Blind.SaveStep = 0;
            return 1;
        }

        if(ERROR == CheckTimeStruct(&CurTime))//检查当前时间是否正确
        {
            return 1;
        }

        TimeCount = ConverseGmtime(&CurTime);//比较当前时间与最新记录的那一步时间
        if(TimeCount < ttCount)
        {
            return 1;
        }
        
    }
    
    Addr = Link2Blind.SaveSector*FLASH_ONE_SECTOR_BYTES+Link2Blind.SaveStep*BlindStepLen;//检查当前存储的那一步的地址是否为空
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
                //等待一会儿
            }
            break;
        }
    }

    sum = 0;//计算校验和
    for(i=0; i<length; i++)
    {
        sum += *(pBuffer+i);
    }

    
    memcpy(Buffer+7,pBuffer,length);//拷贝位置信息至缓冲

    p = Buffer;//补填7字节的数据
    *p++ = (TimeCount&0xff000000) >> 24;
    *p++ = (TimeCount&0xff0000) >> 16;
    *p++ = (TimeCount&0xff00) >> 8;
    *p++ = TimeCount&0xff;
    *p++ = attribute;
    *p++ = sum;
    *p++ = length;

    sFLASH_WriteBuffer(Buffer,Addr,length+7);//写入flash

    Link2Blind.SaveStep++;//下移一步
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
//函数名：Blind_UpdateLink2OpenFlag
//功能：更新变量Link2OpenFlag
//输入：无
//输出：无
//返回值：无
//备注：Link2OpenFlag为1表示开启了第2个连接
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
/////////////////////////以下仅为调试盲区补报而开发，不用于检测，也不用于实际应用/////////////////
/**************************************************************************
//函数名：Blind_CheckLog
//功能：检查盲区补报日志，并把日志打印到DB9串口，提醒数据是否有问题
//输入：无
//输出：无
//返回值：无
//备注：盲区补报日志文件名称固定为blind.dat
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
					
					  if((0x7e == FourByte[0])&&(0x07 == FourByte[1])&&(0x04 == FourByte[2]))//7e 07 04刚好分属在不同的数据块中
						{
						    FrameSearchType = 1;
							  
						}
						else if((0x7e == FourByte[1])&&(0x07 == FourByte[2])&&(0x04 == FourByte[3]))//7e 07 04刚好分属在不同的数据块中
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
//函数名：Blind_PrintFrame
//功能：打印一帧数据到DB9串口
//输入：无
//输出：无
//返回值：无
//备注：返回最新的那条位置信息时间
***************************************************************************/
/*
static u32 Blind_PrintFrame(u8 *pBuffer, u16 BufferLen, u32 TimeCount)
{
	
	  u8 *p = NULL;
	  u8 flag;
	  u16 ListNum;//盲区打包条数
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
	
		p += 13;//跳过7E(1)+命令字(2)+长度(2)+手机号(6)+流水号(2)
		ListNum = 0;//数据项个数
		ListNum |= *p++ << 8;
		ListNum |= *p++;
		p++;//跳过位置数据类型
	  flag = 0;
		for(i=0; i<ListNum; i++)
		{
				 ListLen = 0;//位置汇报数据体长度
				 ListLen |= *p++ << 8;
				 ListLen |= *p++;
				 if(ListLen < 28)
				 {
							return ReturnTimeCount;
			   }
				 p += 22;//跳过报警字(4)+状态字(4)+经度(4)+纬度(4)+速度(2)+高程(2)+方向(2)
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
				 p += 6;//时间
				 p += ListLen-28;
				 LastTimeCount = CurTimeCount;
				 ReturnTimeCount = CurTimeCount;
		}
				
    PrintPacketNum++;		
		PrintLen = VDRPub_HEX2ASC(PrintBuffer, pBuffer,BufferLen);
										
		if(1 == flag)//有异常
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
		
		for(Delay=20000000; Delay>0; Delay--)//纯延时，为的是数据能在该延时阶段发送出去
		{
										
		}
		
		return ReturnTimeCount;
}
*/
/**************************************************************************
//函数名：Blind_SearchFrame
//功能：查找一帧盲区补报数据
//输入：Type ：搜索类型，1：表示拼接；0：表示全新搜索
//输出：无
//返回值：搜索类型，1：表示拼接；0：表示全新搜索
//备注：当已找到一帧数据时会立即结束搜索，并返回0；当没有找到一帧数据时返回值等于输入值Type
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
				if(1 == flag)//找到了头
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
						if(1 == flag)//找到完整一帧数据
						{
						    return 0;
						}
						else//找到前半部分
						{
							  *FrameEnd = BufferLen-1;
								*SearchLen = BufferLen;
						    return 1;
						}
				}
				else//没找到
				{
				    *SearchLen = BufferLen;
					  return Type;
				}
		}
		else//类型错误
		{
			  *SearchLen = BufferLen;
		    return Type;
		}
		
//		return Type;
}
*/

























