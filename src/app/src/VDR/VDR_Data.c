/************************************************************************
//程序名称：VDRData.c
//功能：该模块提供行驶记录仪读写数据接口。
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2015.7
//版本记录：

//备注：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：提供读写行驶速度、位置信息、事故疑点、超时驾驶、驾驶人身份、外部供电、参数修改、速度状态日志等的接口
*************************************************************************/

/********************文件包含*************************/
#include <stdio.h>
#include <string.h>

#include "VDR.h"
#include "Public.h"
#include "taskschedule.h"


/********************本地变量*************************/


/********************全局变量*************************/


/********************外部变量*************************/


/********************本地函数声明*********************/
static ErrorStatus VDRData_CheckWriteParameter(u8 Type,u16 length, u32 Time);
static ErrorStatus VDRData_CheckReadParameter(u8 Type,TIME_T *StartTime,TIME_T *EndTime,u16 block);
static void VDRData_Check(u8 Type);



/********************本地变量*************************/
static VDR_DATA_STRUCT Data[VDR_DATA_TYPE_MAX];
static u8   DataBuffer[VDR_DATA_BUFFER_SIZE] = {0};//读或写行驶记录仪数据时的缓存
static s16  CheckSector[VDR_DATA_TYPE_MAX] = {0,0,0,0,0,0,0,0};
static u32  CheckTimeCountMin[VDR_DATA_TYPE_MAX] = {0,0,0,0,0,0,0,0};
static u32  CheckTimeCountMax[VDR_DATA_TYPE_MAX] = {0,0,0,0,0,0,0,0};
static u16  CheckErrorCount[VDR_DATA_TYPE_MAX] = {0,0,0,0,0,0,0,0};
static u16  CheckSuccessCount[VDR_DATA_TYPE_MAX] = {0,0,0,0,0,0,0,0};
//static s16    PacketSector[VDR_DATA_TYPE_MAX] = {0,0,0,0,0,0,0,0};
//static s16    PacketStep[VDR_DATA_TYPE_MAX] = {0,0,0,0,0,0,0,0};

const u16 DataStepLen[VDR_DATA_TYPE_MAX] = {
    VDR_DATA_SPEED_STEP_LEN,
    VDR_DATA_POSITION_STEP_LEN,
    VDR_DATA_DOUBT_STEP_LEN,
    VDR_DATA_OVER_TIME_STEP_LEN,
    VDR_DATA_DRIVER_STEP_LEN,
    VDR_DATA_POWER_STEP_LEN,
    VDR_DATA_PARAMETER_STEP_LEN,
    VDR_DATA_SPEED_STATUS_STEP_LEN,
};

const u16 DataCollectLen[VDR_DATA_TYPE_MAX] = {
    VDR_DATA_SPEED_COLLECT_LEN,
    VDR_DATA_POSITION_COLLECT_LEN,
    VDR_DATA_DOUBT_COLLECT_LEN,
    VDR_DATA_OVER_TIME_COLLECT_LEN,
    VDR_DATA_DRIVER_COLLECT_LEN,
    VDR_DATA_POWER_COLLECT_LEN,
    VDR_DATA_PARAMETER_COLLECT_LEN,
    VDR_DATA_SPEED_STATUS_COLLECT_LEN,
};

const u16 DataPacketList[VDR_DATA_TYPE_MAX] = {
    VDR_DATA_SPEED_PACKET_LIST,
    VDR_DATA_POSITION_PACKET_LIST,
    VDR_DATA_DOUBT_PACKET_LIST,
    VDR_DATA_OVER_TIME_PACKET_LIST,
    VDR_DATA_DRIVER_PACKET_LIST,
    VDR_DATA_POWER_PACKET_LIST,
    VDR_DATA_PARAMETER_PACKET_LIST,
    VDR_DATA_SPEED_STATUS_PACKET_LIST,
};

const u16 DataStartSector[VDR_DATA_TYPE_MAX] = {
    FLASH_VDR_DATA_SPEED_START_SECTOR,
    FLASH_VDR_DATA_POSITION_START_SECTOR,
    FLASH_VDR_DATA_DOUBT_START_SECTOR,
    FLASH_VDR_DATA_OVER_TIME_START_SECTOR,
    FLASH_VDR_DATA_DRIVER_START_SECTOR,
    FLASH_VDR_DATA_POWER_START_SECTOR,
    FLASH_VDR_DATA_PARAMETER_START_SECTOR,
    FLASH_VDR_DATA_SPEED_STATUS_START_SECTOR,
};

const u16 DataEndSector[VDR_DATA_TYPE_MAX] = {
    FLASH_VDR_DATA_SPEED_END_SECTOR,
    FLASH_VDR_DATA_POSITION_END_SECTOR,
    FLASH_VDR_DATA_DOUBT_END_SECTOR,
    FLASH_VDR_DATA_OVER_TIME_END_SECTOR,
    FLASH_VDR_DATA_DRIVER_END_SECTOR,
    FLASH_VDR_DATA_POWER_END_SECTOR,
    FLASH_VDR_DATA_PARAMETER_END_SECTOR,
    FLASH_VDR_DATA_SPEED_STATUS_END_SECTOR,
};

/********************全局变量*************************/


/********************外部变量*************************/


/********************函数定义*************************/
/**************************************************************************
//函数名：VDRData_TimeTask(void)
//功能：记录仪数据定时任务
//输入：无
//输出：无
//返回值：始终为ENABLE
//备注：50ms调度1次，本任务只是对存储的变量做初始化，实时监测，发现问题进行纠错处理
***************************************************************************/
FunctionalState VDRData_TimeTask(void)
{
    static  u16 TimeCount[VDR_DATA_TYPE_MAX] = {0}; 
  static  u16 RtcWriteCount = 0;
    
    TIME_T Rtc;
    u32 RtcCount;
    u8  i;
    u8  flag;
    u8  Buffer[5];

    

    //检查变量是否超出范围
    for(i=0; i<VDR_DATA_TYPE_MAX; i++)
    {
        flag = 0;
        if((Data[i].CurrentSector > Data[i].EndSector)||(Data[i].CurrentSector < Data[i].StartSector))
        {
            flag = 1;
        }

        if((Data[i].OldestSector > Data[i].EndSector)||(Data[i].OldestSector < Data[i].StartSector))
        {
            flag = 1;
        }

        if((Data[i].CurrentStep > Data[i].SectorStep)||(Data[i].CurrentSector < 0))
        {
            flag = 1;
        }

        if(Data[i].StepLen != DataStepLen[i])
        {
            flag = 1;
        }

        if(Data[i].StartSector != DataStartSector[i])
        {
            flag = 1;
        }

        if(Data[i].EndSector != DataEndSector[i])
        {
            flag = 1;
        }

        if(Data[i].SectorStep != (FLASH_ONE_SECTOR_BYTES/DataStepLen[i]))
        {
            flag = 1;
        }
        
        if(1 == flag)
        {
            Data[i].InitFlag = 0;
            CheckSector[i] = 0;
            while(0 == Data[i].InitFlag)
            {
                VDRData_Check(i);
            }
        }

        if(0 == Data[i].WriteEnableFlag)
        {
            TimeCount[i]++;
            if(TimeCount[i] >= 20*60*30)//禁止写持续半小时，则认为出错了，回复写使能
            {
                TimeCount[i] = 0;
                Data[i].WriteEnableFlag = 1;
            }
        }
        else
        {
            TimeCount[i] = 0;
        }

    
    }

    RtcWriteCount++;
    if(RtcWriteCount >= 20)//1秒记录1次
    {
        RtcWriteCount = 0;
      
        RTC_GetCurTime(&Rtc);
        if(1 == Public_CheckTimeStruct(&Rtc))
        {
          RtcCount = RTC_GetCounter();
            Public_ConvertLongToBuffer(RtcCount,Buffer);
          FRAM_BufferWrite(FRAM_RTC_TIME_ADDR, Buffer, FRAM_RTC_TIME_LEN);  
            FRAM_BufferWrite(FRAM_RTC_TIME_BACKUP_ADDR, Buffer, FRAM_RTC_TIME_BACKUP_LEN);
        }
        
    }

    return ENABLE;
}
/**************************************************************************
//函数名：VDRData_Init
//功能：数据模块初始化
//输入：无
//输出：无
//返回值：始终为ENABLE
//备注：50ms调度1次，本任务只是对存储的变量做初始化，实时监测，发现问题进行纠错处理
***************************************************************************/
void VDRData_Init(void)
{
    u8  i;
    
    for(i=0; i<VDR_DATA_TYPE_MAX; i++)
    {
        Data[i].InitFlag = 0;
        CheckSector[i] = 0;
    }
    for(i=0; i<VDR_DATA_TYPE_MAX;)
    {
        if(0 == Data[i].InitFlag)//未自检完成
        {
            VDRData_Check(i);
        }
        else
        {
            i++;
        }
    }
}
/**************************************************************************
//函数名：VDRData_Write
//功能：写一条行驶记录仪数据
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//pBuffer:指向数据首地址；length:数据长度；Time:32位hex表示的RTC时间
//输出：无
//返回值：成功SUCCESS,失败ERROR
//备注：行驶记录仪数据记录到flash的格式为：时间（4字节）+数据内容（固定长度）+校验和（1字节，是前面所有字节内容的校验和）
//VDR_DATA_TYPE_SPEED数据内容格式为：行驶记录仪标准表A.17规定的格式（126字节）+13字节预留空间
//VDR_DATA_TYPE_POSITION数据内容格式为：行驶记录仪标准表A.19规定的格式（666字节）+10字节预留空间
//VDR_DATA_TYPE_DOUBT数据内容格式为：行驶记录仪标准表A.21规定的格式（234字节）+12字节预留空间
//VDR_DATA_TYPE_OVERTIME数据内容格式为：行驶记录仪标准表A.24规定的格式（50字节）+12字节预留空间
//VDR_DATA_TYPE_DRIVER数据内容格式为：行驶记录仪标准表A.26规定的格式（25字节）+6字节预留空间
//VDR_DATA_TYPE_POWER数据内容格式为：行驶记录仪标准表A.28规定的格式（7字节）+6字节预留空间
//VDR_DATA_TYPE_PRAMATER数据内容格式为：行驶记录仪标准表A.30规定的格式（7字节）+6字节预留空间
//VDR_DATA_TYPE_SPEED_STATUS数据内容格式为：行驶记录仪标准表A.32规定的格式（133字节）+6字节预留空间
***************************************************************************/
ErrorStatus VDRData_Write(u8 Type, u8 *pBuffer, u16 length, u32 Time)
{
    u16     i,j;
    u32     Address;
    u32     TimeCount;
    u8  VerifySum;
    u8  flag;

    //参数正确性检查
    if(ERROR == VDRData_CheckWriteParameter(Type,length,Time))
    {
        return ERROR;
    }
    if((0 == Data[Type].InitFlag)||(0 == Data[Type].WriteEnableFlag))
    {
        return ERROR;
    }

    //写之前判断flash区域是否被写过，写过的话则需擦除
    Address = Data[Type].CurrentSector*FLASH_ONE_SECTOR_BYTES+Data[Type].CurrentStep*Data[Type].StepLen;
    sFLASH_ReadBuffer(DataBuffer,Address,Data[Type].StepLen);
    for(i=0; i<Data[Type].StepLen; i++)
    {
        if(0xff != DataBuffer[i])
        {
            sFLASH_EraseSector(Address);
            Data[Type].CurrentStep = 0;
            for(j=0; j<200; j++)
            {

            }
            Address = Data[Type].CurrentSector*FLASH_ONE_SECTOR_BYTES;
            break;
        }
    }

    //写数据
        Public_ConvertLongToBuffer(Time,DataBuffer);
    memcpy(DataBuffer+4,pBuffer,length);
    VerifySum = Public_GetSumVerify(DataBuffer,length+4);
    DataBuffer[4+length] = VerifySum;
    sFLASH_WriteBuffer(DataBuffer,Address,length+5);

    //读数据
    for(i=0; i<VDR_DATA_BUFFER_SIZE; i++)
    {
        DataBuffer[i] = 0;
    }
    sFLASH_ReadBuffer(DataBuffer,Address,length+5);
    TimeCount = Public_ConvertBufferToLong(DataBuffer);
    flag = 0;
    if(TimeCount != Time)
    {
        flag = 1;
    }
    if(DataBuffer[length+4] != VerifySum)
    {
        flag = 1;
    }
    for(i=0; i<length; i++)
    {
        if(DataBuffer[4+i] != *(pBuffer+i))
        {
            flag = 1;
            break;
        }
    }
    if(1 == flag)
    {
        sFLASH_EraseSector(Address);
        Data[Type].CurrentStep = 0;
        return ERROR;
    }

    //步数加1
    Data[Type].CurrentStep++;
    if(Data[Type].CurrentStep >= Data[Type].SectorStep)
    {
        Data[Type].CurrentStep = 0;
        Data[Type].CurrentSector++;
        if(Data[Type].CurrentSector >= Data[Type].EndSector)
        {
            Data[Type].CurrentSector = Data[Type].StartSector;
            Data[Type].LoopFlag = 1;
        }
        Address = Data[Type].CurrentSector*FLASH_ONE_SECTOR_BYTES;
        sFLASH_EraseSector(Address);
        if(1 == Data[Type].LoopFlag)
        {
            Data[Type].OldestSector++;
            if(Data[Type].OldestSector >= Data[Type].EndSector)
            {
                Data[Type].OldestSector = Data[Type].StartSector;
            }
        }
    }

    return SUCCESS; 
    
}
/**************************************************************************
//函数名：VDRData_Read
//功能：读取指定结束时间前的N块行驶记录仪数据
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//pBuffer:指向数据首地址；EndTime:结束时间；block:块数
//Sector,Step:表明数据在存储区中的位置
//输出：无
//返回值：读取的数据长度
//备注：采用开区间的方式，即如果有一条记录的时间刚好等于结束时间，该记录将不被检出
***************************************************************************/
u16 VDRData_Read(u8 *pBuffer,s16 *pSector,s16 *pStep,u8 Type,TIME_T StartTime,TIME_T EndTime, u8 block)
{
    u16     length;
    u8  BlockCount; 
    u8  flag;
    u8  FindFlag;
    s16     Sector;
    s16     Step;
    u32     Address;
    u32     TimeCount;
    u32     EndTimeCount;
    u32 StartTimeCount;
    u8  *p = NULL;
    u8  SectorStepFlag = 0;

    length = 0;
    *pSector = 0;//这两个值为0表示没有读取到数据
    *pStep = 0;
    //参数正确性检查
    if(ERROR == VDRData_CheckReadParameter(Type,&StartTime,&EndTime,block))
    {
        return length;
    }
    if(0 == Data[Type].InitFlag)
    {
        return length;
    }
    if(NULL != p)
    {
        return length;
    }

  if(block > DataPacketList[Type])
    {
        block = DataPacketList[Type];
    }
    

    
    //从最新的那个扇区倒推查找，先确定扇区
    Sector = Data[Type].CurrentSector;
    Step = 0;
    EndTimeCount = ConverseGmtime(&EndTime);
    StartTimeCount = ConverseGmtime(&StartTime);
    FindFlag = 0;
    for(;;)
    {
        flag = 0;
        for(Step=0; Step<Data[Type].SectorStep; Step++)//在该扇区找到一条有效数据，一般为第1条 dxl,2015.10.24
        {
            Address = Sector*FLASH_ONE_SECTOR_BYTES+Step*Data[Type].StepLen;
            sFLASH_ReadBuffer(DataBuffer,Address,Data[Type].StepLen);
            TimeCount = Public_ConvertBufferToLong(DataBuffer);
            if((DataBuffer[Data[Type].StepLen-1] == Public_GetSumVerify(DataBuffer,Data[Type].StepLen-1))&&(0xffffffff != TimeCount))
            {
                flag = 1;
                break;
            }
        }
        if(1 == flag)
        {
            if(TimeCount < EndTimeCount)//查找小于结束时间的数据
            {
                FindFlag = 1;
                break;
            }
        }
        if(Sector == Data[Type].OldestSector)
        {
            break;
        }
        Sector--;
        if(Sector < Data[Type].StartSector)
        {
            Sector = Data[Type].EndSector - 1;
        }
        
    }

    //读取数据
    BlockCount = 0;
    if(1 == FindFlag)
    {
        if(Sector == Data[Type].CurrentSector)
        {
            Step = Data[Type].CurrentStep;
                        Step--;
                        if(Step < 0)
            {
                Step = Data[Type].SectorStep-1;
                Sector--;
                if(Sector < Data[Type].StartSector)
                {
                    Sector = Data[Type].EndSector - 1;
                }
            }
        }
        else
        {
            Step = Data[Type].SectorStep-1;
        }
        p = pBuffer;
        for(;;)
        {
            Address = Sector*FLASH_ONE_SECTOR_BYTES+Step*Data[Type].StepLen;
            sFLASH_ReadBuffer(DataBuffer,Address,Data[Type].StepLen);
            TimeCount = Public_ConvertBufferToLong(DataBuffer);
            if((DataBuffer[Data[Type].StepLen-1] == Public_GetSumVerify(DataBuffer,Data[Type].StepLen-1))&&(TimeCount < EndTimeCount))
            {
                if(TimeCount < StartTimeCount)
                {
                    break;
                }
                if(0 == SectorStepFlag)
                {
                  SectorStepFlag = 1;
                    *pSector = Sector;
                    *pStep = Step;
                }
                length += DataCollectLen[Type];
                if((length+DataCollectLen[Type]) <= 1000)
                {
                    memcpy(p,DataBuffer+4,DataCollectLen[Type]);
                    p += DataCollectLen[Type];
                    BlockCount++;
                }
                else
                {
          memcpy(p,DataBuffer+4,DataCollectLen[Type]);
                    p += DataCollectLen[Type];
                    BlockCount++;
                    break;
                }
                
            }

            if((Sector == Data[Type].OldestSector)&&(0 == Step))
            {
                break;
            }

            if((BlockCount >= block)||(BlockCount >= DataPacketList[Type]))
            {
                break;
            }

            Step--;
            if(Step < 0)
            {
                Step = Data[Type].SectorStep-1;
                Sector--;
                if(Sector < Data[Type].StartSector)
                {
                    Sector = Data[Type].EndSector - 1;
                }
            }
            
            
        }
    }
    
    return length;
    
}
/**************************************************************************
//函数名：VDRData_SetWriteEnableFlag
//功能：设置某个数据类型的写使能标志(禁止或使能)
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//Value:0为禁止,1为使能
//输出：无
//返回值：成功返回SUCCESS,失败返回ERROR
//备注:!!!!!特别注意谁禁止的谁必须负责开启,否则影响后面数据的记录!!!!
//在远程读取行驶记录仪分包数据时建议先禁止写,然后传输完成后再开启写,
//这样做的原因是:远程读取数据的过程中若没有禁止写,则很可能会有新数据记录,
//从而导致分包信息动态变化,读取的分包数据就不准确了
***************************************************************************/
ErrorStatus VDRData_SetWriteEnableFlag(u8 Type, u8 Value)
{
    if((Type >= VDR_DATA_TYPE_MAX)||(Value > 1))
    {
        return ERROR;
    }
    else
    {
        Data[Type].WriteEnableFlag = Value;
        return SUCCESS;
    }
}
/**************************************************************************
//函数名：VDRData_ReadWriteEnableFlag
//功能：读某个数据类型的写使能标志
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：//Value:0为禁止,1为使能,若返回0xFF表示参数错误
//备注:!!!!!特别注意谁禁止的谁必须负责开启,否则影响后面数据的记录!!!!
//在远程读取行驶记录仪分包数据时建议先禁止写,然后传输完成后再开启写,
//这样做的原因是:远程读取数据的过程中若没有禁止写,则很可能会有新数据记录,
//从而导致分包信息动态变化,读取的分包数据就不准确了
***************************************************************************/
u8 VDRData_ReadWriteEnableFlag(u8 Type)
{
    if(Type >= VDR_DATA_TYPE_MAX)
    {
        return 0xff;
    }
    
    return Data[Type].WriteEnableFlag;
}
/**************************************************************************
//函数名：VDRData_ReadInitFlag
//功能：获取初始化标志
//输入：无
//输出：无
//返回值：初始化标志
//备注：1表示已完成初始化，0表示未完成初始化，0xFF表示参数错误，只有已完成初始化写才有效。
***************************************************************************/
u8 VDRData_ReadInitFlag(u8 Type)
{
    if(Type >= VDR_DATA_TYPE_MAX)
    {
        return 0xff;
    }
    
    return Data[Type].InitFlag;
}
/**************************************************************************
//函数名：VDRData_ReadPacket
//功能：读取指定序号的那包数据
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//pBuffer:指向数据首地址；PacketNum:条序号,从1开始,时间最老的为第1包
//Direction:0:顺序读出(老时间排在前面,新时间排在后面);1:逆序读出(新时间排在前面,老时间排在后面)
//输出：无
//返回值：读取的数据长度
//备注：读取的数据内容为各命令数据块的组合，组合条数如下：
//VDR_DATA_SPEED_PACKET_LIST        7//打包传输时，每包打包的条数
//VDR_DATA_POSITION_PACKET_LIST     1
//VDR_DATA_DOUBT_PACKET_LIST        4
//VDR_DATA_OVER_TIME_PACKET_LIST    20
//VDR_DATA_DRIVER_PACKET_LIST       40
//VDR_DATA_POWER_PACKET_LIST        142
//VDR_DATA_PARAMETER_PACKET_LIST        142
//VDR_DATA_SPEED_STATUS_PACKET_LIST 7
***************************************************************************/
u16 VDRData_ReadPacket(u8 *pBuffer,u8 Type,u16 PacketNum,u8 Direction)
{
    s16 ListNum;
    u16 TotalPacket;
    u16 i;
    u16 length;
    u16 BufferLen;
    u8 *p = NULL;

    TotalPacket = VDRData_ReadTotalPacket(Type);

    if(0 == TotalPacket)
    {
        return 0;
    }
    else if(PacketNum > TotalPacket)
    {
        return 0;
    }
    else if(PacketNum < 1)
    {
        return 0;
    }

    p = pBuffer;
    length = 0;
    if(0 == Direction)
    {
        ListNum = (PacketNum-1)*DataPacketList[Type];
        for(i=0; i<DataPacketList[Type]; i++)
        {
            BufferLen = VDRData_ReadList(p,Type,ListNum);
            p += BufferLen;
            length += BufferLen;
            ListNum++;
        }
    }
    else
    {
        ListNum = PacketNum*DataPacketList[Type];
        if(ListNum > 0)
        {
          ListNum--;//条序号是从0开始
        }
        for(i=0; i<DataPacketList[Type]; i++)
        {
            BufferLen = VDRData_ReadList(p,Type,ListNum);
            p += BufferLen;
            length += BufferLen;
            ListNum--;
        }
    }

    return length;
}
/**************************************************************************
//函数名：VDRData_ReadList
//功能：读取指定序号的那条数据
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//pBuffer:指向数据首地址；ListNum:条序号,从0开始,时间最老的那条为第0条
//输出：无
//返回值：读取的数据长度
//备注：读取的数据内容格式为各命令的数据块格式,例如行驶速度数据块格式为表A.17
***************************************************************************/
u16 VDRData_ReadList(u8 *pBuffer,u8 Type,s16 ListNum)
{
    u32 Address;
    s16 Sector;
        s16 Step;
    
    s16 OffList;//偏移条数
    u16 TotalList;
    u16 i;

    TotalList = VDRData_ReadTotalList(Type);

    if(0 == TotalList)
    {
        return 0;
    }
    else if(ListNum >= TotalList)
    {
        return 0;
    }
    else if(ListNum < 0)
    {
      return 0;
    }
    
    OffList = ListNum;
        Sector = Data[Type].OldestSector;
        Step = 0;
        for(i=0; i<OffList; i++)
        {
                Step++;
                if(Step >= Data[Type].SectorStep)
                {
                        Step = 0;
                        Sector++;
                        if(Sector >= Data[Type].EndSector)
                        {
                                Sector = Data[Type].StartSector;
                        }
                }
        }
    Address = Sector*FLASH_ONE_SECTOR_BYTES+Step*Data[Type].StepLen;
    sFLASH_ReadBuffer(DataBuffer,Address,Data[Type].StepLen);
    if(DataBuffer[Data[Type].StepLen-1] == Public_GetSumVerify(DataBuffer,Data[Type].StepLen-1))
    {
        memcpy(pBuffer,DataBuffer+4,DataCollectLen[Type]);
    }
    else
    {
        for(i=0; i<DataCollectLen[Type]; i++)
        {
            *(pBuffer+i) = 0x00;//填充0
        }
    }   
    return DataCollectLen[Type];
}
/**************************************************************************
//函数名：VDRData_ReadListTime
//功能：读取指定序号的那条数据的时间
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//pBuffer:指向数据首地址；ListNum:条序号,从0开始,时间最老的那条为第0条
//输出：无
//返回值：读取的时间
//备注：读取的数据内容格式为各命令的数据块格式,例如行驶速度数据块格式为表A.17
***************************************************************************/
u32 VDRData_ReadListTime(u8 Type,s16 ListNum)
{
    u32 TimeCount;
    u32 Address;
    s16 Sector;
  s16 Step;
    
    s16 OffList;//偏移条数
    u16 TotalList;
    u16 i;
    
    u8 Buffer[5];

    TotalList = VDRData_ReadTotalList(Type);

    if(0 == TotalList)
    {
        return 0;
    }
    else if(ListNum >= TotalList)
    {
        return 0;
    }
    else if(ListNum < 0)
    {
      return 0;
    }
    
    OffList = ListNum;
        Sector = Data[Type].OldestSector;
        Step = 0;
        for(i=0; i<OffList; i++)
        {
                Step++;
                if(Step >= Data[Type].SectorStep)
                {
                        Step = 0;
                        Sector++;
                        if(Sector >= Data[Type].EndSector)
                        {
                                Sector = Data[Type].StartSector;
                        }
                }
        }
                
    Address = Sector*FLASH_ONE_SECTOR_BYTES+Step*Data[Type].StepLen;
    sFLASH_ReadBuffer(Buffer,Address,4);
                
    TimeCount = Public_ConvertBufferToLong(Buffer); 
                
    return TimeCount;
}
/**************************************************************************
//函数名：VDRData_ReadTotalPacket(u8 Type)
//功能：读某个记录类型存储的总包数 
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：总包数 
//备注：
//VDR_DATA_SPEED_PACKET_LIST        7//打包传输时，每包最大的打包条数
//VDR_DATA_POSITION_PACKET_LIST     1
//VDR_DATA_DOUBT_PACKET_LIST        4
//VDR_DATA_OVER_TIME_PACKET_LIST    20
//VDR_DATA_DRIVER_PACKET_LIST       40
//VDR_DATA_POWER_PACKET_LIST        142
//VDR_DATA_PARAMETER_PACKET_LIST        142
//VDR_DATA_SPEED_STATUS_PACKET_LIST 7
***************************************************************************/
u16 VDRData_ReadTotalPacket(u8 Type)
{
    u16 TotalList;
    u16 TotalPacket;

    TotalList = VDRData_ReadTotalList(Type);

    if(0 == (TotalList%DataPacketList[Type]))
    {
        TotalPacket = TotalList/DataPacketList[Type];
    }
    else
    {
        TotalPacket = TotalList/DataPacketList[Type]+1;
    }

    return TotalPacket;
}
/**************************************************************************
//函数名：VDRData_ReadListNum
//功能：获取给定位置（Sector,Step）的条序号
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：总条数
//备注：从序号0开始
***************************************************************************/
u16 VDRData_ReadListNum(u8 Type,s16 Sector,s16 Step)
{
    u16 ListNum;

    if(Type >= VDR_DATA_TYPE_MAX)
    {
        return 0;
    }
    if((Sector > Data[Type].EndSector)||(Sector < Data[Type].StartSector))
    {
      return 0;
    }
    if(Step > Data[Type].SectorStep)
    {
      return 0;
    }

    if(0 == Data[Type].LoopFlag)
    {
        ListNum = (Sector-Data[Type].OldestSector)*Data[Type].SectorStep+Step;
    }
    else
    {
        ListNum = (Sector-Data[Type].StartSector)*Data[Type].SectorStep+Step
                +(Data[Type].EndSector-Data[Type].OldestSector)*Data[Type].SectorStep;
    }
    
    //ListNum++;
    if(ListNum > VDRData_ReadTotalList(Type))
    {
      ListNum = VDRData_ReadTotalList(Type);
    }
    

    return ListNum;
}
/**************************************************************************
//函数名：VDRData_ReadTotalList(u8 Type)
//功能：读某个记录类型存储的总条数
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：总条数，条数序号从0开始 
//备注：
***************************************************************************/
u16 VDRData_ReadTotalList(u8 Type)
{
    u16 TotalList;

    if(Type >= VDR_DATA_TYPE_MAX)
    {
        return 0;
    }

    if(0 == Data[Type].LoopFlag)
    {
        TotalList = (Data[Type].CurrentSector-Data[Type].OldestSector)*Data[Type].SectorStep
                               +Data[Type].CurrentStep;
    }
    else
    {
        TotalList = (Data[Type].CurrentSector-Data[Type].StartSector)*Data[Type].SectorStep
                +Data[Type].CurrentStep
                +(Data[Type].EndSector-Data[Type].OldestSector)*Data[Type].SectorStep;
    }

    return TotalList;
}

/**************************************************************************
//函数名：VDRData_ReadNewestTime(u8 Type)
//功能：获取存储的最新记录时间
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：32位hex值表示的时间
//备注：返回值为0表示还没有记录数据，返回0xFFFFFFFF表示参数错误或内部禁止读
***************************************************************************/
u32 VDRData_ReadNewestTime(u8 Type)
{
    s16     Sector;
    s16     Step;
    u32     Address;
    u32     TimeCount = 0;


    if(Type >= VDR_DATA_TYPE_MAX)
    {
        return 0xFFFFFFFF;
    }
    else if(0 == Data[Type].InitFlag)
    {
        return 0xFFFFFFFF;
    }
    else if((Data[Type].CurrentSector == Data[Type].OldestSector)&&(0 == Data[Type].CurrentStep))
    {
        return 0;
    }

    Sector = Data[Type].CurrentSector;
    Step = Data[Type].CurrentStep;
    
    Step--;
    if(Step < 0)
    {
        Step = Data[Type].SectorStep - 1;
        Sector--;
        if(Sector < Data[Type].StartSector)
        {
            Sector = Data[Type].EndSector - 1;
        }
    }

    for(;;)
    {
        Address = Sector*FLASH_ONE_SECTOR_BYTES+Step*Data[Type].StepLen;
        sFLASH_ReadBuffer(DataBuffer,Address,Data[Type].StepLen);
        TimeCount = Public_ConvertBufferToLong(DataBuffer);
        if(DataBuffer[Data[Type].StepLen-1] == Public_GetSumVerify(DataBuffer,Data[Type].StepLen-1))
        {
            break;
        }

        Step--;
        if(Step < 0)
        {
            Step = Data[Type].SectorStep-1;
            Sector--;
            if(Sector < Data[Type].StartSector)
            {
                Sector = Data[Type].EndSector - 1;
            }
        }
            
        if((Sector == Data[Type].OldestSector)&&(0 == Step))
        {
            break;
        }
    }
    return TimeCount;
}
/**************************************************************************
//函数名：VDRData_ReadOldestTime(u8 Type)
//功能：获取存储的最老记录时间
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：32位hex值表示的时间
//备注：返回值为0表示还没有记录数据，返回0xFFFFFFFF表示参数错误或内部禁止读
***************************************************************************/
u32 VDRData_ReadOldestTime(u8 Type)
{
    s16     Sector;
    s16     Step;
    u32     Address;
    u32     TimeCount = 0;

    if(Type >= VDR_DATA_TYPE_MAX)
    {
        return 0xffffffff;
    }
    if(0 == Data[Type].InitFlag)
    {
        return 0xffffffff;
    }

    if((Data[Type].CurrentSector == Data[Type].OldestSector)&&(0 == Data[Type].CurrentStep))
    {
        return 0;
    }

    Sector = Data[Type].OldestSector;
    Step = 0;
    

    for(;;)
    {
        Address = Sector*FLASH_ONE_SECTOR_BYTES+Step*Data[Type].StepLen;
        sFLASH_ReadBuffer(DataBuffer,Address,Data[Type].StepLen);
        TimeCount = Public_ConvertBufferToLong(DataBuffer);
        if(DataBuffer[Data[Type].StepLen-1] == Public_GetSumVerify(DataBuffer,Data[Type].StepLen-1))
        {
            break;
        }

        Step++;
        if(Step >= Data[Type].SectorStep)
        {
            Step = 0;
            Sector++;
            if(Sector >= Data[Type].EndSector)
            {
                Sector = Data[Type].StartSector;
            }
        }
            
        if((Sector == Data[Type].CurrentSector)&&(Step >= Data[Type].CurrentStep))
        {
            break;
        }
    }
    return TimeCount;
}
/**************************************************************************
//函数名：VDRData_EraseAllFlashArea(void)
//功能：擦除记录仪所有存储的数据
//输入：无
//输出：无
//返回值：无
//备注：擦除记录仪flash存储的数据，对于铁电或其他地方存储的记录仪数据没有去擦除
***************************************************************************/
void VDRData_EraseAllFlashArea(void)
{
    u32 Addr;
    Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;//擦除位置信息暂存扇区（1小时存1次）
    sFLASH_EraseSector(Addr);
    Addr = FLASH_OVER_TIME_DRIVE_INFO_START_SECTOR*FLASH_ONE_SECTOR_BYTES;//擦除超时驾驶的驾驶员信息
    sFLASH_EraseSector(Addr);
    VDRData_EraseFlashArea(VDR_DATA_TYPE_SPEED);//擦除行驶速度记录
    VDRData_EraseFlashArea(VDR_DATA_TYPE_POSITION);//擦除位置信息记录
    VDRData_EraseFlashArea(VDR_DATA_TYPE_DOUBT);//擦除事故疑点记录
    VDRData_EraseFlashArea(VDR_DATA_TYPE_OVERTIME);//擦除超时驾驶记录
    VDRData_EraseFlashArea(VDR_DATA_TYPE_DRIVER);//擦除驾驶人身份记录
    VDRData_EraseFlashArea(VDR_DATA_TYPE_POWER);//擦除外部供电记录
    VDRData_EraseFlashArea(VDR_DATA_TYPE_PARAMETER);//擦除参数修改记录
    VDRData_EraseFlashArea(VDR_DATA_TYPE_SPEED_STATUS);//擦除速度状态日志记录
    VDRData_Init();//dxl,2015.10.27,之前放在所有扇区擦除的前面
}
/**************************************************************************
//函数名：VDRData_EraseFlashArea(u8 Type)
//功能：擦除某个存储区的数据
//输入：Type:类型，可选值为VDR_DATA_TYPE_SPEED、VDR_DATA_TYPE_POSITION、
//VDR_DATA_TYPE_DOUBT、VDR_DATA_TYPE_OVERTIME、VDR_DATA_TYPE_DRIVER、
//VDR_DATA_TYPE_POWER、VDR_DATA_TYPE_PRAMATER、VDR_DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：无
//备注：
***************************************************************************/
void VDRData_EraseFlashArea(u8 Type)
{
    s16     i;
    u32     Address;
    
    if(Type >= VDR_DATA_TYPE_MAX)
    {
        return;
    }

    //for(i=Data[Type].StartSector; i<Data[Type].EndSector; i++)dxl,2015.10.27,当未初始化Data[Type]就调用擦除的话会有问题
    for(i=DataStartSector[Type]; i<DataEndSector[Type]; i++)
    {
        Address = i*FLASH_ONE_SECTOR_BYTES;
        sFLASH_EraseSector(Address);
        IWDG_ReloadCounter();
    }
    Data[Type].StartSector = DataStartSector[Type];
    Data[Type].EndSector = DataEndSector[Type];
    Data[Type].StepLen = DataStepLen[Type];
    Data[Type].SectorStep = FLASH_ONE_SECTOR_BYTES/DataStepLen[Type];
    Data[Type].OldestSector = DataStartSector[Type];
    Data[Type].CurrentSector = DataStartSector[Type];
    Data[Type].CurrentStep = 0;
    Data[Type].LoopFlag = 0;
    CheckSector[Type] = DataStartSector[Type];
    CheckTimeCountMin[Type] = 0xffffffff;
    CheckTimeCountMax[Type] = 0;
    CheckErrorCount[Type] = 0;
    CheckSuccessCount[Type] = 0;
        Data[Type].InitFlag = 1;
    Data[Type].WriteEnableFlag = 1;
}

//////////////////////////////以下是内部函数////////////////////////////

/**************************************************************************
//函数名：VDRData_CheckWriteParameter
//功能：检查写参数
//输入：写参数：类型、长度、时间
//输出：无
//返回值：成功：SUCCESS；失败：ERROR
//备注：
***************************************************************************/
static ErrorStatus VDRData_CheckWriteParameter(u8 Type,u16 length, u32 Time)
{
    if(Type >= VDR_DATA_TYPE_MAX)
    {
        return ERROR;
    }

    if(length != (DataStepLen[Type]-5))
    {
        return ERROR;
    }

    if(Time <= VDRData_ReadNewestTime(Type))
    {
        return ERROR;
    }

    return SUCCESS;
}
/**************************************************************************
//函数名：VDRData_CheckReadParameter
//功能：检查写参数
//输入：写参数：类型、长度、时间
//输出：无
//返回值：成功：SUCCESS；失败：ERROR
//备注：
***************************************************************************/
static ErrorStatus VDRData_CheckReadParameter(u8 Type,TIME_T *StartTime,TIME_T *EndTime,u16 block)
{
    u32     TimeCount;
    

    if(Type >= VDR_DATA_TYPE_MAX)
    {
        return ERROR;
    }

    if(0 == Public_CheckTimeStruct(EndTime))
    {
        return ERROR;
    }

    if(0 == Public_CheckTimeStruct(StartTime))
    {
        return ERROR;
    }

    TimeCount = ConverseGmtime(EndTime);
    if(TimeCount < VDRData_ReadOldestTime(Type))
    {
        return ERROR;
    }

    TimeCount = ConverseGmtime(StartTime);
    if(TimeCount > VDRData_ReadNewestTime(Type))
    {
        return ERROR;
    }

    return SUCCESS;
}
/**************************************************************************
//函数名：VDRData_Check(u8 Type)
//功能：自检某个存储区
//输入：Type:类型，可选值为DATA_TYPE_SPEED、DATA_TYPE_POSITION、
//DATA_TYPE_DOUBT、DATA_TYPE_OVER_TIME、DATA_TYPE_DRIVER、
//DATA_TYPE_POWER、DATA_TYPE_PRAMATER、DATA_TYPE_SPEED_STATUS、
//输出：无
//返回值：无
//备注：
***************************************************************************/
static void VDRData_Check(u8 Type)
{
    s16     i;
    s16     Step;
    u32     Address;
    u32     TimeCount;
    TIME_T  tt;
    

    if(Type >= VDR_DATA_TYPE_MAX)
    {
        return;
    }

    if(0 == CheckSector[Type])
    {
        Data[Type].StartSector = DataStartSector[Type];
        Data[Type].EndSector = DataEndSector[Type];
        Data[Type].StepLen = DataStepLen[Type];
        Data[Type].SectorStep = FLASH_ONE_SECTOR_BYTES/DataStepLen[Type];
        Data[Type].OldestSector = DataStartSector[Type];
        Data[Type].CurrentSector = DataStartSector[Type];
        Data[Type].CurrentStep = 0;
        Data[Type].LoopFlag = 0;
        CheckSector[Type] = DataStartSector[Type];
        CheckTimeCountMin[Type] = 0xffffffff;
        CheckTimeCountMax[Type] = 0;
        CheckErrorCount[Type] = 0;
        CheckSuccessCount[Type] = 0;
    }
		else if(CheckSector[Type] >= Data[Type].EndSector)
    {
				Address = (Data[Type].EndSector-1)*FLASH_ONE_SECTOR_BYTES+(Data[Type].SectorStep-1)*Data[Type].StepLen;
        sFLASH_ReadBuffer(DataBuffer,Address,Data[Type].StepLen);
        for(i=0; i<Data[Type].StepLen; i++)
        {
            if(0xff != DataBuffer[i])
            {
                Data[Type].LoopFlag = 1;
							  break;
            }
        }
				if(1 == Data[Type].LoopFlag)
				{
				    if(Data[Type].CurrentSector == (Data[Type].EndSector-1))//正好满了，不多一条，也不少一条
            {
                Data[Type].CurrentSector = Data[Type].StartSector;
                Data[Type].CurrentStep = 0;
							  Address = Data[Type].CurrentSector*FLASH_ONE_SECTOR_BYTES;
                sFLASH_EraseSector(Address);
							  Data[Type].OldestSector++;//最老扇区下移
                if(Data[Type].OldestSector >= Data[Type].EndSector)
                {
                    Data[Type].OldestSector = Data[Type].StartSector;
                }
            }
				}
				
				if(Data[Type].OldestSector > Data[Type].CurrentSector)
				{
				    Data[Type].LoopFlag = 1;
				}
				

        Data[Type].InitFlag = 1;
        Data[Type].WriteEnableFlag = 1;

        if((0 == CheckSuccessCount[Type])&&(CheckErrorCount[Type] >= (2*Data[Type].SectorStep)))
        {
            VDRData_EraseFlashArea(Type);
                        return ;
        }
    }
    else
    {
        IWDG_ReloadCounter();
        for(Step=0; Step<Data[Type].SectorStep; Step++)
        {
            Address = CheckSector[Type]*FLASH_ONE_SECTOR_BYTES+Step*Data[Type].StepLen;
            sFLASH_ReadBuffer(DataBuffer,Address,Data[Type].StepLen);
            TimeCount = Public_ConvertBufferToLong(DataBuffer);
            if(0xffffffff == TimeCount)
            {
                break;
            }
            else
            {
                if(DataBuffer[Data[Type].StepLen-1] == Public_GetSumVerify(DataBuffer,Data[Type].StepLen-1))
                {
                    Gmtime(&tt,TimeCount);
                    if(1 == Public_CheckTimeStruct(&tt))
                    {
                        CheckSuccessCount[Type]++;

                        if(TimeCount < CheckTimeCountMin[Type])
                        {
                            CheckTimeCountMin[Type] = TimeCount;
                            Data[Type].OldestSector = CheckSector[Type];
                        }
                        if(TimeCount > CheckTimeCountMax[Type])
                        {
                            CheckTimeCountMax[Type] = TimeCount;
                            Data[Type].CurrentSector = CheckSector[Type];
                            Data[Type].CurrentStep = Step+1;
                        }
                    }
                    else
                    {
                        CheckErrorCount[Type]++;
                        if(CheckErrorCount[Type] >= (2*Data[Type].SectorStep))
                        {
                            VDRData_EraseFlashArea(Type);
                                                        return ;
                        }
                    }
                }
                else
                {
                    CheckErrorCount[Type]++;
                    if(CheckErrorCount[Type] >= (2*Data[Type].SectorStep))
                    {
                        VDRData_EraseFlashArea(Type);
                                                return ;
                    }
                }
            }
        }

        CheckSector[Type]++;    
    }   
    
}







