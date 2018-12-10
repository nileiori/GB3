/************************************************************************
//程序名称：VDR_Doubt.c
//功能：疑点数据功能
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明

*************************************************************************/

/********************文件包含*************************/
//#include "stm32f2xx_rtc.h"
#include "stm32f10x_rtc.h"
#include "VDR.h"
#include "fm25c160.h"
#include "EepromPram.h"
#include "Rtc.h"
#include "Public.h"
#include "Gps_App.h"
#include "Io.h"
#include "Adc.h"
#include "other.h"
/********************本地变量*************************/
static  u8  CarRunFlag = 0;//车辆停驶、行驶标志，1为行驶，0为停驶。
static  u8  VdrDoubtInitFlag = 0;//初始化标志，1为已初始化，0为未初始化。
static  u8  VdrDoubtBuffer[VDR_DATA_DOUBT_STEP_LEN];//疑点数据缓冲
static  u16     VdrDoubtCount = 0;//疑点数据点数记录
static  u8  VdrDoubtTestBuffer[300];//疑点数据缓冲
static  u8  VdrMyTestBuffer[300] = {0};
static u8 VdrDoubtLastPowerFlag = 0;//0为上电，1为断电
static u8   VdrDoubtPowerFlag = 0;
static u8 TestFlag1 = 0;
static TIME_T Flag1Time;
static u8 TestFlag2 = 0;
static TIME_T Flag2Time;
static u8 TestFlag3 = 0;
static TIME_T Flag3Time;
static u8 TestFlag4 = 0;
static TIME_T Flag4Time;

u32 TestTimeCount1 = 0;

/********************全局变量*************************/
u16 PowerShutValue = 50;//??5.0V
u8  VdrDoubtSecondSpeed = 0;
u8  VdrDoubtSecondSpeedReadyFlag1 = 0;//VDR_SpeedStatus.c使用
u8  VdrDoubtSecondSpeedReadyFlag2 = 0;//VDR_Speed.c使用
u8  VdrDoubtSecondSpeedReadyFlag3 = 0;


/********************外部变量*************************/
extern u8   GBTestFlag;//0:正常出货运行模式;1:国标检测模式，该标志通过菜单可选择
extern u8  SpeedFlag;//速度类型,0为脉冲,1为GPS,2自动脉冲,3自动GPS
/********************本地函数声明*********************/
static u16 VDRDoubt_Read1(u8 *pBuffer,u8 Type,u8 MoveStep,u8 NavigationFlag);
static u16 VDRDoubt_Read2(u8 *pBuffer,u8 Type,u8 NavigationFlag);
static void VDRDoubt_WritePositionNoChange(void);
static void VDRDoubt_WritePowerOff(void);
static void VDRDoubt_WritePowerOn(void);
static void VDRDoubt_WriteStop(void);
static void VDRDoubt_SaveCurrentSpeedStatus(void);
static void VDRDoubt_RunOrStop(void);
static void VDRDoubt_CalSecondSpeed(void);

/********************函数定义*************************/

/**************************************************************************
//函数名：VDRDoubt_TimeTask
//功能：实现疑点数据的记录
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：疑点数据定时任务，50ms进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState VDRDoubt_TimeTask(void)
{
    if(0 == VdrDoubtInitFlag)
    {
        if(1 == VDRData_ReadInitFlag(VDR_DATA_TYPE_DOUBT))
        {
            if(0 == GBTestFlag)//国标检测时使用备电
            {
              VDRDoubt_WritePowerOn();
            }
            VdrDoubtInitFlag = 1;
        }
    }
    else
    {
        VDRDoubt_RunOrStop();//行驶与停驶的判定
        
        VDRDoubt_CalSecondSpeed();//计算秒平均速度

        VDRDoubt_SaveCurrentSpeedStatus();//记录当前的速度和状态，0.2秒一个

        VDRDoubt_WriteStop();//保存正常停车的疑点数据

        VDRDoubt_WritePowerOff();//保存断电的疑点数据

        VDRDoubt_WritePositionNoChange();//保存行驶但位置无变化的疑点数据
    }

    return ENABLE;      
}
/**************************************************************************
//函数名：VDRDoubt_GetCarRunFlag
//功能：获取车辆行驶标志
//输入：无
//输出：无
//返回值：1为行驶，0为停驶
//备注：行驶速度大于0持续10秒为行驶，速度等于0持续10秒为停驶
***************************************************************************/
u8 VDRDoubt_GetCarRunFlag(void)
{
    return CarRunFlag;
}
/**************************************************************************
//函数名：VDRDoubt_GetSecondSpeed
//功能：获取每秒钟速度
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
u8 VDRDoubt_GetSecondSpeed(void)
{
	#if(TACHOGRAPHS_19056_TEST)
		return Tachographs_Test_Get_Speed(); 
	#else
		return VdrDoubtSecondSpeed;
	#endif
}
/**************************************************************************
//函数名：VDRDoubt_RunOrStop
//功能：行驶与停驶的判定
//输入：无
//输出：无
//返回值：无
//备注：对本地变量CarRunFlag(值为1为行驶，为0为停驶)有影响
***************************************************************************/
static void VDRDoubt_RunOrStop(void)
{
    static u16 RunCount = 0;
    static u16 StopCount = 0;
	
	  

    u8  Speed;

	  if(0x01 == (0x01&SpeedFlag))
		{
		    Speed = Gps_ReadSpeed();
		}
		else
		{
        Speed = VDRPulse_GetInstantSpeed();
		}

    if(Speed > 0)
    {
        StopCount = 0;
        RunCount++;
        if(RunCount >= 10*SECOND)
        {
            RunCount--;
            CarRunFlag = 1;
        }
    }
    else
    {
        RunCount = 0;
        StopCount++;
        if(StopCount >= 10*SECOND)
        {
            StopCount--;
            CarRunFlag = 0;
        }
    }
}
/**************************************************************************
//函数名：VDRDoubt_CalSecondSpeed
//功能：计算每秒钟平均速度
//输入：无
//输出：无
//返回值：无
//备注：只有事故疑点的速度是瞬时速度，其它秒（平均）速度，每分钟平均速度
//都必须由推导得来，它们之间的关系是：由瞬时速度--->事故疑点速度--->秒速度--->
//每分钟平均速度，另外值得注意的是：只有秒速度或分钟速度计算出来后才能使用，
//而且当前时刻使用的速度值是前一秒或前一分钟的值
***************************************************************************/
static void VDRDoubt_CalSecondSpeed(void)
{
        static u8 Count1 = 0;
      static u8 Count2 = 0;
      static u8 SecondSpeed[10] = {0};
      static u32 LastTimeCount = 0;
        
        u32 TimeCount;
        u16 sum;
        u8  i;
        
        Count1++;
      if(Count1 >= (2*SYSTICK_0p1SECOND))
      {
          Count1 = 0;
				  if(0x01 == (SpeedFlag&0x01))
				  {
				      SecondSpeed[Count2++] = Gps_ReadSpeed();
				  }
				  else
				  {
              SecondSpeed[Count2++] = VDRPulse_GetInstantSpeed();
				  }
          if(Count2 > 5)
          {
            Count2 = 5;
          }
      }
        
        TimeCount = RTC_GetCounter();
    if(TimeCount != LastTimeCount)//秒变化时刻计算秒速度
    { 
          sum = 0;
          for(i=0;i<Count2;i++)
          {
            sum += SecondSpeed[i];
          }
          VdrDoubtSecondSpeed = sum/Count2;
            VdrDoubtSecondSpeedReadyFlag1 = 1;
            VdrDoubtSecondSpeedReadyFlag2 = 1;
            VdrDoubtSecondSpeedReadyFlag3 = 1;
          for(i=0;i<5;i++)
          {
            SecondSpeed[i] = 0;
          }
          Count2 = 0;
          LastTimeCount = TimeCount;
     }
}
/**************************************************************************
//函数名：VDRDoubt_SaveCurrentSpeedStatus
//功能：保存当前的速度和状态
//输入：无
//输出：无
//返回值：无
//备注：每隔0.2秒取一个瞬时速度（事故疑点数据），取5个速度求和取平均得秒（平均）速度
//这样做的目的：通过事故疑点的速度记录可推导出秒平均速度，使秒平均速度有依据，
//而不是取个瞬时速度，只有事故疑点的速度是瞬时速度，其它秒平均速度，每分钟平均速度
//都必须由推导得来，它们之间的关系是：由瞬时速度--->事故疑点速度--->秒速度--->
//每分钟平均速度
***************************************************************************/
static void VDRDoubt_SaveCurrentSpeedStatus(void)
{
    static u8 Count1 = 0;

    

    u8  Status;
    u8  Buffer[10];
    
        u16     WriteAddr;
        u32 TimeCount;
        
    Count1++;
    if(Count1 >= (2*SYSTICK_0p1SECOND))
    {
        Count1 = 0;

        Status = 0;
        Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_BRAKE) << 7;//bit7:刹车
        Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_LEFT_LIGHT) << 6;//bit6:左转
        Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_RIGHT_LIGHT) << 5;//bit5:右转
        Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_FAR_LIGHT) << 4;//bit4:远光
        Status |= Io_ReadExtCarStatusBit(CAR_STATUS_BIT_NEAR_LIGHT) << 3;//bit3:近光
        //bit2-bit1未定义
        Status |= Io_ReadStatusBit(STATUS_BIT_ACC);//bit0:ACC

			  if(0x01 == (SpeedFlag&0x01))
				{
				    Buffer[0] = Gps_ReadSpeed();
				}
				else
				{
            Buffer[0] = VDRPulse_GetInstantSpeed();
				}

        Buffer[1] = Status;

        WriteAddr = FRAM_VDR_DOUBT_DATA_ADDR+3*VdrDoubtCount;
        FRAM_BufferWrite(WriteAddr,Buffer,2);//记录速度与状态
        
        
        
        VdrDoubtTestBuffer[2*VdrDoubtCount] = Buffer[0];
        VdrDoubtTestBuffer[2*VdrDoubtCount+1] = Buffer[1];
        
        VdrDoubtCount++;
        if(VdrDoubtCount >= 150)
        {
            VdrDoubtCount = 0;
        }

        Public_ConvertShortToBuffer(VdrDoubtCount,Buffer);
        WriteAddr = FRAM_VDR_DOUBT_COUNT_ADDR;
        FRAM_BufferWrite(WriteAddr,Buffer,2);//记录疑点数据点数

        
        //Count2++;
        //if(Count2 >= 5)//每秒记录一次时间
        //{
            //Count2 = 0;
            //VDRPub_ConvertNowTimeToBCD(Buffer);
            //WriteAddr = FRAM_VDR_DOUBT_TIME_ADDR;
            //FRAM_BufferWrite(WriteAddr,Buffer,6);//记录当前时间
                        TimeCount = RTC_GetCounter();
                        Public_ConvertLongToBuffer(TimeCount,Buffer);
                FRAM_BufferWrite(FRAM_VDR_DOUBT_TIME_ADDR, Buffer, FRAM_VDR_DOUBT_TIME_LEN);
        //}
        
    }
    
    
}
/**************************************************************************
//函数名：VDRDoubt_WriteStop
//功能：写停车事故疑点
//输入：无
//输出：无
//返回值：无
//备注：将疑点数据从FRAM读出保存到FLASH中，保存到FLASH的数据内容符合表A.22要求
***************************************************************************/
static void VDRDoubt_WriteStop(void)
{
    static u8 LastRunFlag = 0;//1为行驶，0为停驶
    static u8 NavigationFlag = 0;//1为导航，0为不导航
    
    u8  RunFlag;
    u16 length;
    u32 TimeCount;

    
    RunFlag = VDRDoubt_GetCarRunFlag();

    if((1 == LastRunFlag)&&(0 == RunFlag)&&(0 == VdrDoubtPowerFlag))//由行驶变为停驶
    {
        //length = VDRDoubt_Read(VdrDoubtBuffer,50);    
    length = VDRDoubt_Read2(VdrDoubtBuffer,0x03,NavigationFlag);
    NavigationFlag = 0;     
        if(0 != length)
        {
            TimeCount = RTC_GetCounter();
            TimeCount -= 10;
            VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, TimeCount);
            TestFlag1++;
            RTC_GetCurTime(&Flag1Time);
        }

    }
    else if((0 == LastRunFlag)&&(1 == RunFlag))//由停驶转为行驶
    {
      NavigationFlag = 0;
    }
    else if(1 == RunFlag)
    {
      if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
        {
          NavigationFlag = 1;
        }
    }
    LastRunFlag = RunFlag;
    
}
/**************************************************************************
//函数名：VDRDoubt_WritePowerOn
//功能：写上电事故疑点
//输出：无
//返回值：无
//备注：将疑点数据从FRAM读出保存到FLASH中，保存到FLASH的数据内容符合表A.22要求
***************************************************************************/
static void VDRDoubt_WritePowerOn(void)
{
    u8  Buffer[10];
    u8  BufferLen;
    u16 i;
    u16 Addr;
    u16 length;
    TIME_T  Time;
    u32 TimeCount;

    BufferLen = FRAM_BufferRead(Buffer,2,FRAM_VDR_DOUBT_COUNT_ADDR);//读上一次记录的点位置
    if(0 != BufferLen)
    {
        VdrDoubtCount = Public_ConvertBufferToShort(Buffer);
        length = VDRDoubt_Read2(VdrDoubtBuffer,0x00,0); 
        if(0 != length)
        {
            Public_ConvertBcdToValue(Buffer, VdrDoubtBuffer, 6);
            Time.year = Buffer[0];
            Time.month = Buffer[1];
            Time.day = Buffer[2];
            Time.hour = Buffer[3];
            Time.min = Buffer[4];
            Time.sec = Buffer[5];
            if(1 == Public_CheckTimeStruct(&Time))
            {
                TimeCount = ConverseGmtime(&Time);
                VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, TimeCount);
                TestFlag2++;
              RTC_GetCurTime(&Flag2Time);
            }
        }
        
    }
    else
    {
        
    }
    VdrDoubtCount = 0;
    Buffer[0] = 0;
    Buffer[1] = 0;
    for(i=0; i<150; i++)
    {
        Addr = FRAM_VDR_DOUBT_DATA_ADDR+3*i;
        FRAM_BufferWrite(Addr,Buffer,2);
        VdrDoubtTestBuffer[2*i] = Buffer[0];
        VdrDoubtTestBuffer[2*i+1] = Buffer[1];
        if((1 == VdrDoubtTestBuffer[2*i+1])||(1 == VdrDoubtTestBuffer[2*i]))
        {
            VdrDoubtTestBuffer[2*i+1] = 0;
            VdrDoubtTestBuffer[2*i] = 0;
        }
    }
}
/**************************************************************************
//函数名：VDRDoubt_WritePowerOff
//功能：写掉电事故疑点
//输入：无
//输出：无
//返回值：无
//备注：将疑点数据从FRAM读出保存到FLASH中，保存到FLASH的数据内容符合表A.22要求
***************************************************************************/
static void VDRDoubt_WritePowerOff(void)
{

    static u8 PowerOffCount = 0;
    static u8 PowerOnCount = 0;
    static u8 NavigationFlag = 0;
    
    u16 length;
    u16 Ad = 0;
    u32 Time;
    s16 ListNum;
    s16 i;
    u8  flag = 0;

    if(0 == VDRDoubt_GetCarRunFlag())//停驶
    {
        return ;
    }

    Ad = Ad_GetValue(ADC_MAIN_POWER);//读取主电电压
    Ad = Ad*33*9/0xfff;//转成以0.1V为单位的值

    if(Ad <= PowerShutValue)
    {
        PowerOnCount = 0;
        PowerOffCount++;
        if(PowerOffCount >= 4)
        {
            PowerOffCount = 0;
          VdrDoubtPowerFlag = 1;
        }
    }
    else
    {
        PowerOffCount = 0;
        PowerOnCount++;
        if(PowerOnCount >= 4)
        {
            PowerOnCount = 0;
          VdrDoubtPowerFlag = 0;
        }
    }

    if((0 == VdrDoubtLastPowerFlag)&&(1 == VdrDoubtPowerFlag))//由上电变为掉电
    {
        length = VDRDoubt_Read1(VdrDoubtBuffer,0x00,6,NavigationFlag);//往前挪动6步，因为断电识别需要约1秒的时间
        NavigationFlag = 0;
        if(0 != length)
        {
            Time = RTC_GetCounter();
            VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, Time);
            TestFlag3++;
            RTC_GetCurTime(&Flag3Time);
            //读刚刚写入的那条数据
            ListNum = VDRData_ReadTotalList(VDR_DATA_TYPE_DOUBT);
            ListNum--;//从0开始，所以要--
            length = VDRData_ReadList(VdrMyTestBuffer,VDR_DATA_TYPE_DOUBT,ListNum);
            flag = 0;
            for(i=0; i<length; i++)
            {
              if((VdrMyTestBuffer[i] != VdrDoubtBuffer[i])||(0 == VdrMyTestBuffer[24]))
                {
                  flag = 1;
                    break;
                }
            }
            if(1 == flag)
            {
              flag++;
            }
        }
    }
    else if((1 == VdrDoubtLastPowerFlag)&&(0 == VdrDoubtPowerFlag))//由掉电变上电
    {
      NavigationFlag = 0;
    }
    else
    {
      if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
      {
        NavigationFlag = 1;
      }
  }
    
    VdrDoubtLastPowerFlag = VdrDoubtPowerFlag;
}
/**************************************************************************
//函数名：VDRDoubt_WritePositionNoChange
//功能：写位置信息无变化的事故疑点数据
//输入：无
//输出：无
//返回值：无
//备注：将疑点数据从FRAM读出保存到FLASH中，保存到FLASH的数据内容符合表A.22要求
***************************************************************************/
static void VDRDoubt_WritePositionNoChange(void)
{
    static  u8      LastNoChangeFlag = 0;//0为有变化，1为没有变化
    static  u16     Count = 0;
    static  GPS_STRUCT  LastPosition;//上一个点的位置信息

    u8  NoChangeFlag;
    u16 length;
    u32 TimeCount;
    GPS_STRUCT  Position;//当前点的位置信息

    if(0 == VDRDoubt_GetCarRunFlag())//停驶
    {
        LastNoChangeFlag = 0;
        Count = 0;
        return ;
    }

    if(0 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))//不定位
  {
        Count = 0;
        LastNoChangeFlag = 0;
    return;
  }

    NoChangeFlag = 0;
    Gps_CopygPosition(&Position);//获取有效位置信息
    if(((Position.Latitue_D   == LastPosition.Latitue_D)
            &&(Position.Latitue_F  == LastPosition.Latitue_F))
            &&(Position.Latitue_FX == LastPosition.Latitue_FX))  
    {
        Count++;
        if(Count >= 10*SECOND)
        {
            Count = 0;
            NoChangeFlag = 1;
        }
    }
    else
    {
        Count = 0;
    }
  Gps_CopygPosition(&LastPosition);
    
    if((0 == LastNoChangeFlag)&&(1 == NoChangeFlag))//由位置信息有变化变为位置信息无变化
    {
        length = VDRDoubt_Read2(VdrDoubtBuffer,0x00,1); 
        if(0 != length)
        {
            TimeCount = RTC_GetCounter();
            VDRData_Write(VDR_DATA_TYPE_DOUBT, VdrDoubtBuffer, VDR_DATA_DOUBT_STEP_LEN-5, TimeCount);
            TestFlag4++;
            RTC_GetCurTime(&Flag4Time);
        }
    LastNoChangeFlag = NoChangeFlag;
    }
    
    
}
/**************************************************************************
//函数名：VDRDoubt_Read2
//功能：读事故疑点数据
//输入：Type:bit0为1表示当前时间需要减10秒，为0表示不需要减10秒;bit1为1表示事故疑点速度需要一个为0的数据，为0表示不需要，即全为非0
//输入：MoveStep:往前移动的步数,0.2秒1步
//输出：pBuffer:指向数据缓冲；
//返回值：获取的数据长度
//备注：数据内容符合表A.22要求
***************************************************************************/
static u16 VDRDoubt_Read1(u8 *pBuffer,u8 Type,u8 MoveStep, u8 NavigationFlag)
{
    u8  *p = NULL;
    u8  Buffer[10];
    u8  BufferLen;
    u8  count;
    u8  flag;
    u16 Addr;
    u16 length;
    s16 i;
    s16 j;
        u32     TimeCount;
        TIME_T  Time;
    
    length = FRAM_BufferRead(Buffer,FRAM_VDR_DOUBT_TIME_LEN,FRAM_VDR_DOUBT_TIME_ADDR);
    if((0 == length)||(Type > 3))
    {
        return 0;
    }
        TimeCount = Public_ConvertBufferToLong(Buffer);
          if(TimeCount > 10)
                {
                    if(0x01 == (Type&0x01))
                    {
            TimeCount -= 10;
                    }
                }
        Gmtime(&Time, TimeCount);
        if(0 == Public_CheckTimeStruct(&Time))
        {
                return 0;
        }
        p = pBuffer;//结束时间，6字节，BCD码,需要在这个时间上减去10秒
        VDRPub_ConvertTimeToBCD(p, &Time);
    p += 6;

    length = EepromPram_ReadPram(E2_CURRENT_LICENSE_ID,p);//驾驶证号码，18字节，ASCII码
    if(0 == length)
    {
        memset(p,'0',18);
    }
    p += 18;

        
    
    BufferLen = FRAM_BufferRead(Buffer,2,FRAM_VDR_DOUBT_COUNT_ADDR);//读上一次记录的点位置
    if(0 != BufferLen)
    {
        VdrDoubtCount = Public_ConvertBufferToShort(Buffer);
    }
    else
    {
        return 0;
    }
    i = VdrDoubtCount;
  i--;
    if(i < 0)
    {
        i = 150+i;
    }
    for(j=0; j<100; j++)//疑点数据，200字节，hex值
    {
        Addr = FRAM_VDR_DOUBT_DATA_ADDR+i*3;
        length = FRAM_BufferRead(Buffer,2,Addr);
        if(Buffer[0] > 0)
        {
                    break;//找到了速度第1个不为0的点
        }
        else
        {
                
        }
        i--;
        if(i < 0)
        {
            i = 150+i;
        }
    }
    if(0x02 == (Type&0x02))
    {
      i++;
    }
    else
    {
    
    }
    if(i >= 150)
    {
        i = 0;
    }
    
    if(i < MoveStep)
    {
      i = 150+i-MoveStep;
    }
    else
    {
      i = i-MoveStep;
    }
                
    count = 0;
    flag = 0;
    for(j=0; j<100; j++)//疑点数据，200字节，hex值
    {
        Addr = FRAM_VDR_DOUBT_DATA_ADDR+i*3;
        length = FRAM_BufferRead(Buffer,2,Addr);
        if(0 == length)
        {
            return 0;
        }
        if(Buffer[0] > 0)
        {
                count++;
                if(count >= 1)  //表明行驶过
                {
                    flag = 1;
                }
        }
        else
        {
                count = 0;
        }
        memcpy(p,Buffer,2);
        p += 2;

        i--;
        if(i < 0)
        {
            i = 150+i;
        }
    }

    if(0 == flag)
    {
        return 0;
    }
    
    //有效位置点
    //if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
    if(1 == NavigationFlag)
    {
      Public_GetCurPositionInfoDataBlock(p);
    }
    else
    {
      VDRPub_GetNoNavigationPostion(p);
    }

    return 234;
    
}
/**************************************************************************
//函数名：VDRDoubt_Read2
//功能：读事故疑点数据
//输入：Type:bit0为1表示当前时间需要减10秒，为0表示不需要减10秒;bit1为1表示事故疑点速度需要一个为0的数据，为0表示不需要，即全为非0
//输出：pBuffer:指向数据缓冲；
//返回值：获取的数据长度
//备注：数据内容符合表A.22要求
***************************************************************************/
static u16 VDRDoubt_Read2(u8 *pBuffer,u8 Type,u8 NavigationFlag)
{
    u8  *p = NULL;
    u8  Buffer[10];
    u8  BufferLen;
    u8  count;
    u8  flag;
    u16 Addr;
    u16 length;
    s16 i;
    s16 j;
        u32     TimeCount;
        TIME_T  Time;
    
    length = FRAM_BufferRead(Buffer,FRAM_VDR_DOUBT_TIME_LEN,FRAM_VDR_DOUBT_TIME_ADDR);
    if((0 == length)||(Type > 3))
    {
        return 0;
    }
        TimeCount = Public_ConvertBufferToLong(Buffer);
          if(TimeCount > 10)
                {
                    if(0x01 == (Type&0x01))
                    {
            TimeCount -= 10;
                    }
                }
        Gmtime(&Time, TimeCount);
        if(0 == Public_CheckTimeStruct(&Time))
        {
                return 0;
        }
        p = pBuffer;//结束时间，6字节，BCD码,需要在这个时间上减去10秒
        VDRPub_ConvertTimeToBCD(p, &Time);
    p += 6;

    length = EepromPram_ReadPram(E2_CURRENT_LICENSE_ID,p);//驾驶证号码，18字节，ASCII码
    if(0 == length)
    {
        memset(p,'0',18);
    }
    p += 18;

        
    
    BufferLen = FRAM_BufferRead(Buffer,2,FRAM_VDR_DOUBT_COUNT_ADDR);//读上一次记录的点位置
    if(0 != BufferLen)
    {
        VdrDoubtCount = Public_ConvertBufferToShort(Buffer);
    }
    else
    {
        return 0;
    }
    i = VdrDoubtCount;
        i--;
    if(i < 0)
    {
        i = 149;
    }
    for(j=0; j<100; j++)//疑点数据，200字节，hex值
    {
        Addr = FRAM_VDR_DOUBT_DATA_ADDR+i*3;
        length = FRAM_BufferRead(Buffer,2,Addr);
        if(Buffer[0] > 0)
        {
                    break;//找到了速度第1个不为0的点
        }
        else
        {
                
        }
        i--;
        if(i < 0)
        {
            i = 149;
        }
    }
    if(0x02 == (Type&0x02))
    {
      i++;
    }
    else
    {
    
    }
  
    if(i >= 150)
    {
        i = 0;
    }
                
    count = 0;
    flag = 0;
    for(j=0; j<100; j++)//疑点数据，200字节，hex值
    {
        Addr = FRAM_VDR_DOUBT_DATA_ADDR+i*3;
        length = FRAM_BufferRead(Buffer,2,Addr);
        if(0 == length)
        {
            return 0;
        }
        if(Buffer[0] > 0)
        {
                count++;
                if(count >= 1)//表明行驶过
                {
                    flag = 1;
                }
        }
        else
        {
                count = 0;
        }
        memcpy(p,Buffer,2);
        p += 2;

        i--;
        if(i < 0)
        {
            i = 149;
        }
    }

    if(0 == flag)
    {
        return 0;
    }
    
    //有效位置点
    //if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
    if(1 == NavigationFlag)
    {
      Public_GetCurPositionInfoDataBlock(p);
    }
    else
    {
      VDRPub_GetNoNavigationPostion(p);
    }

    return 234;
    
}






