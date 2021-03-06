
/*******************************************************************************
 * File Name:           Image_Store.c
 * Function Describe:   实现多媒体图片检索，单条图片检索上传功能
 * Relate Module:
 * Writer:              fanqinghai 
 * Date:                2016-03-15
 *******************************************************************************/


#include "include.h"


#define BUFFER_SIZE             550
#define ATTEMPE_STEP_LEN      6*FLASH_ONE_SECTOR_BYTES   //一副图片最多分配24k空间
#define STORE_IMAGE_NMB         3    //可保存图片张数

static u32 SendMediaID;
u8 FlashBuffer[BUFFER_SIZE];    

/**************************************************************************
//函数名：delay
//功能：延时函数
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void delay()
{
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
}

/**************************************************************************
//函数名：Store_Image_Date
//功能：实现图片存储到铁电里
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
u8 Store_Image_Date(u16 pacagenmb,u8 *pBuffer ,u16 length)
{

    static u32 Address;
    static u8 CurrentStep =0;
    static u16 LastPacagenmb ;
    u16 TotalPacage;
    static u16 TotalDataLong;
   // u8 bufer[4] = {0};
    u8 i =0;
    u32 RtcCount ;
    memset(FlashBuffer,0,BUFFER_SIZE);
    memcpy(FlashBuffer,pBuffer,length);
    //获取当前地址
    if(0 == pacagenmb)   //第一包
    {
        
        CurrentStep ++;     //有数据
        if(CurrentStep>2)
        {
            CurrentStep =0;
        }
        Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
        for(i=0;i<6;i++)    //每个图片分配24K空间
        {
            
            sFLASH_EraseSector(Address);
            Address += FLASH_ONE_SECTOR_BYTES;
            IWDG_ReloadCounter();
        }
        
        delay();     //可有可无
        TotalDataLong = 0;
        LastPacagenmb = 0;
        Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN; //地址复位
    }
    //写数据 允许循环覆盖写
    sFLASH_WriteBuffer(FlashBuffer, Address, length);
    Address += length; //
    TotalDataLong += length;
    
     if(0xAA == pacagenmb)
     {
         memset(FlashBuffer,0,2);
         TotalPacage = LastPacagenmb+1;
         TotalPacage += 1;              //包从第0包开始，所以再加1包
         FlashBuffer[0] = (TotalPacage&0xff00)>>8;
         FlashBuffer[1] = TotalPacage&0x00ff;
         Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -2;
         sFLASH_WriteBuffer(FlashBuffer, Address, 2);

         ////保存数据总长度//////////////
        memset(FlashBuffer,0,2);
        FlashBuffer[0] = (TotalDataLong&0xff00)>>8;
        FlashBuffer[1] = TotalDataLong&0x00ff;
        Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -4;
        sFLASH_WriteBuffer(FlashBuffer, Address, 2);

       ////保存拍照时间//////////////
        memset(FlashBuffer,0,4);
        RtcCount = RTC_GetCounter();
        FlashBuffer[0] = (RtcCount&0xff000000)>>24;
        FlashBuffer[1] = (RtcCount&0x00ff0000)>>16;
        FlashBuffer[2] = (RtcCount&0x0000ff00)>>8;
        FlashBuffer[3] = RtcCount&0x000000ff;
        
        Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -8;
        sFLASH_WriteBuffer(FlashBuffer, Address, 4);
     }
    LastPacagenmb =  pacagenmb ; 
    return 0;
}
/**************************************************************************
//函数名：Get_ID
//功能：获得要检索的图片id
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
void Get_ID(u32 MediaID)
{
    SendMediaID = MediaID;

}
/**************************************************************************
//函数名：MediaSearch_GetMediaIndex
//功能：查询指定时间的图片信息
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
u16 MediaSearch_GetMediaIndex(u8 *pBuffer, u8 MediaType, TIME_T *StartTime, TIME_T *EndTime)
{
    u32 Address;
    u32 FindTime;
    u32 Start_Time;
    u32 End_Time;
    u16 Length;
    u8 CurrentStep =0;
    u8 i;
    u8 Time[4];
    static u8 *p=NULL;
        
    p= pBuffer;
    Length = 0;
    if(!(Public_CheckTimeStruct(StartTime))||(!Public_CheckTimeStruct(EndTime))) //时间结构出错
    {
        return 0;
    }
     //只支持图片检索
	if(MediaType  ==1)   //音频
	{
        //多媒体ID
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x01;
        //多媒体类型
        *p++ = 0x01;
        //通道ID
        *p++ = 0x01;
        //事件编码
        *p++ = 0;
        //位置信息
        Report_GetPositionBasicInfo(p);
        Length += 35;
        p = NULL;
        return Length ;
    }
    else if(MediaType  ==2)  //视频
    {
        //多媒体ID
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x01;
        //多媒体类型
        *p++ = 0x02;
        //通道ID
        *p++ = 0x01;
        //事件编码
        *p++ = 0;
        //位置信息
        Report_GetPositionBasicInfo(p);
        Length += 35;
        p = NULL;
        return Length ;
    }
    else if(MediaType  ==0)   //图像
    {
        CurrentStep = 0;
      for(i=0;i<STORE_IMAGE_NMB;i++)

        {

            IWDG_ReloadCounter();
            CurrentStep ++;     //有数据
            if(CurrentStep>2)
            {
                CurrentStep =0;
            }
            
            Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -8;
            sFLASH_ReadBuffer(Time, Address, 4);
            FindTime = 0;
            FindTime |= Time[0]<<24;
            FindTime |= Time[1]<<16;
            FindTime |= Time[2]<<8;
            FindTime |= Time[3];
            
            //Public_ConvertBCDToTime(tt,Time);
            Start_Time = ConverseGmtime(StartTime);
            End_Time = ConverseGmtime(EndTime);
            if((FindTime>Start_Time)&&(FindTime<End_Time))
            {
                memset(FlashBuffer,0,50);
                Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
                sFLASH_ReadBuffer(FlashBuffer, Address, 36);
                //多媒体ID
                *p++ = FlashBuffer[0];
                *p++ = FlashBuffer[1];
                *p++ = FlashBuffer[2];
                *p++ = FlashBuffer[3];
                //多媒体类型
                *p++ = FlashBuffer[4];
                //通道ID
                *p++ = FlashBuffer[7];
                //事件编码
                *p++ = FlashBuffer[6];
                //位置信息
                memcpy(p,FlashBuffer+8,28);
                p += 28;
                //Report_GetPositionBasicInfo(p);
                Length += 35;
        
            }
        }
        p = NULL;
        return Length ;
    }
    return 0;
}
/**************************************************************************
//函数名：SendImage_TimeTask
//功能：实现图片按ID检索发送
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
FunctionalState SendImage_TimeTask(void)
{
    static u32 Address;
    static u8 flag = 0;
    u32 ReadID;
    static u8 CurrentStep = 0;
    static u16 TotalPacage;
    static u16 SendPacage;
    static u16 TotalDataLong;
    
    u16  Length ;  
    u8 channel = CHANNEL_DATA_1;//为编译通过而添加
    u8 i =0;

    if(!flag)
    {
        CurrentStep = 0 ;
        for(i=0;i<STORE_IMAGE_NMB;i++)
        {
            IWDG_ReloadCounter();
            CurrentStep ++;     //有数据
            if(CurrentStep>2)
            {
                CurrentStep =0;
            }
            
            Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
            memset(FlashBuffer,0,4);
            sFLASH_ReadBuffer(FlashBuffer, Address, 100);
            ReadID = 0;
            ReadID |= FlashBuffer[0]<<24;
            ReadID |= FlashBuffer[1]<<16;
            ReadID |= FlashBuffer[2]<<8;
            ReadID |= FlashBuffer[3];
            
            if(SendMediaID == ReadID)
            {
                memset(FlashBuffer,0,2);
                TotalPacage = 0;
                TotalDataLong = 0;

                Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -2;
                sFLASH_ReadBuffer(FlashBuffer, Address, 2);
                TotalPacage |= FlashBuffer[0]<<8;
                TotalPacage |= FlashBuffer[1];

                memset(FlashBuffer,0,2);
                Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -4;
                sFLASH_ReadBuffer(FlashBuffer, Address, 2);
                TotalDataLong |= FlashBuffer[0]<<8;
                TotalDataLong |= FlashBuffer[1];

                flag = 1;
                SendPacage = 1;    // 从第一包开始
                Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
        
                break;
            }
        
        }
        if(3==i)
        {
            flag = 0;
            return DISABLE;
        }
    }
    else
    {
        if(SendPacage<=TotalPacage)
        {

            if(SendPacage==TotalPacage)
            {
                Length = TotalDataLong%512;
            }
            else
            {
                Length = 512;
            }
            memset(FlashBuffer,0,BUFFER_SIZE);
            sFLASH_ReadBuffer(FlashBuffer, Address, Length);
            
            if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
            {
                SendPacage++;
                Address += 512;

              return ENABLE; 
            }
        }
        else               //发送完毕
        {
            flag = 0;                   
            return DISABLE;
        }

    }
    return ENABLE;
}

/*********************************************************************
//函数名称  :Media_UploadTimeTask(void)
//功能      :存储多媒体数据上传定时任务，发送视频文件
//输入      :
//输出      :
//使用资源  :
//全局变量  :
//调用函数  :
//中断资源  :
//返回      :
//备注      :
*********************************************************************/
FunctionalState MediaUploadTimeTask(void)
{
    static u32 Address;
    static u8 flag = 0;
    static u16 TotalPacage;
    static u16 SendPacage;
    static u32 TotalDataLong;
    
    u16  Length ;  
    u8 channel = CHANNEL_DATA_1;//为编译通过而添加
    u8 *p = NULL;
        
        if(! flag)
        {
            
            Address = VIDEO_START_SECTOR*FLASH_ONE_SECTOR_BYTES;
            
            memset(FlashBuffer,0,4);
            sFLASH_ReadBuffer(FlashBuffer, Address, 4);
            
            TotalDataLong |= FlashBuffer[0]<<24;
            TotalDataLong |= FlashBuffer[1]<<16;
            TotalDataLong |= FlashBuffer[2]<<8;
            TotalDataLong |= FlashBuffer[3];
            
            TotalPacage = TotalDataLong/512 + 1;
            memset(FlashBuffer,0,BUFFER_SIZE);
             p=FlashBuffer;
            *p++ = 0;//多媒体ID
            *p++ = 0;
            *p++ = 0;
            *p++ = 0x01;
            
            *p++ = 2;//多媒体类型，2为视频
            *p++ = 0xaa;//多媒体格式� 表示其它类型
            *p++ = 0x00;//多媒体事件  0:平台自动上发
            *p++ = 0x01;//多媒体通道
            Report_GetPositionBasicInfo(p);
            Address = VIDEO_START_SECTOR*FLASH_ONE_SECTOR_BYTES+4;
            sFLASH_ReadBuffer(FlashBuffer+36, Address, 512);
            flag = 1;
            SendPacage = 1;
            Length =  512+36;
            if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
            {
                SendPacage++;
                Address += 512;
            
              return ENABLE; 
            }
            
        }

        else
        {

            if(SendPacage<=TotalPacage)
            {

                if(SendPacage==TotalPacage)
                {
                    Length = TotalDataLong%512;
                }
                else
                {
                    Length = 512;
                }
                memset(FlashBuffer,0,BUFFER_SIZE);
                sFLASH_ReadBuffer(FlashBuffer, Address, Length);
                
                if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
                {
                    SendPacage++;
                    Address += 512;

                  return ENABLE; 
                }
            }
            else               //发送完毕
            {
                flag = 0;                   
                return DISABLE;
            }


        }
     return ENABLE;     
}
/*********************************************************************
//函数名称  :SendAudio_TimeTask(void)
//功能      :发送录音文件
//输入      :
//输出      :
//使用资源  :
//全局变量  :
//调用函数  :
//中断资源  :
//返回      :
//备注      :
*********************************************************************/
FunctionalState SendAudio_TimeTask(void)
{
    static u32 Address;
    static u8 flag = 0;
    static u16 TotalPacage;
    static u16 SendPacage;
    static u32 TotalDataLong;
    
    u16  Length ;  
    u8 channel = CHANNEL_DATA_1;//为编译通过而添加
    u8 *p = NULL;
        
        if(! flag)
        {
            
            Address = AUDIO_START_SECTOR*FLASH_ONE_SECTOR_BYTES;
            
            memset(FlashBuffer,0,4);
            sFLASH_ReadBuffer(FlashBuffer, Address, 4);
            
            TotalDataLong |= FlashBuffer[0]<<24;
            TotalDataLong |= FlashBuffer[1]<<16;
            TotalDataLong |= FlashBuffer[2]<<8;
            TotalDataLong |= FlashBuffer[3];            
            TotalPacage = TotalDataLong/512 + 1;
            memset(FlashBuffer,0,BUFFER_SIZE);
            p=FlashBuffer;
            *p++ = 0;//多媒体ID
            *p++ = 0;
            *p++ = 0;
            *p++ = 0x01;
            *p++ = 1;//多媒体类型，1为音频
            *p++ = 0x02;//多媒体格式� 2为MP3
            *p++ = 0x00;//多媒体事件  0:平台自动上发
            *p++ = 0x01;//多媒体通道
            Report_GetPositionBasicInfo(p);
            Address = AUDIO_START_SECTOR*FLASH_ONE_SECTOR_BYTES+4;
            sFLASH_ReadBuffer(FlashBuffer+36, Address, 512);
            flag = 1;
            SendPacage = 1;
            Length =  512+36;
            if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
            {
                SendPacage++;
                Address += 512;
            
              return ENABLE; 
            }
            
        }

        else
        {

            if(SendPacage<=TotalPacage)
            {

                if(SendPacage==TotalPacage)
                {
                    Length = TotalDataLong%512;
                }
                else
                {
                    Length = 512;
                }
                memset(FlashBuffer,0,BUFFER_SIZE);
                sFLASH_ReadBuffer(FlashBuffer, Address, Length);
                
                if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
                {
                    SendPacage++;
                    Address += 512;

                  return ENABLE; 
                }
            }
            else               //发送完毕
            {
                flag = 0;                   
                return DISABLE;
            }


        }
        
    return ENABLE; 
}









