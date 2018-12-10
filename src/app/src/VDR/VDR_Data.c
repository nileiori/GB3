/************************************************************************
//�������ƣ�VDRData.c
//���ܣ���ģ���ṩ��ʻ��¼�Ƕ�д���ݽӿڡ�
//�汾�ţ�V0.1
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2015.7
//�汾��¼��

//��ע���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1���ṩ��д��ʻ�ٶȡ�λ����Ϣ���¹��ɵ㡢��ʱ��ʻ����ʻ����ݡ��ⲿ���硢�����޸ġ��ٶ�״̬��־�ȵĽӿ�
*************************************************************************/

/********************�ļ�����*************************/
#include <stdio.h>
#include <string.h>

#include "VDR.h"
#include "Public.h"
#include "taskschedule.h"


/********************���ر���*************************/


/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


/********************���غ�������*********************/
static ErrorStatus VDRData_CheckWriteParameter(u8 Type,u16 length, u32 Time);
static ErrorStatus VDRData_CheckReadParameter(u8 Type,TIME_T *StartTime,TIME_T *EndTime,u16 block);
static void VDRData_Check(u8 Type);



/********************���ر���*************************/
static VDR_DATA_STRUCT Data[VDR_DATA_TYPE_MAX];
static u8   DataBuffer[VDR_DATA_BUFFER_SIZE] = {0};//����д��ʻ��¼������ʱ�Ļ���
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

/********************ȫ�ֱ���*************************/


/********************�ⲿ����*************************/


/********************��������*************************/
/**************************************************************************
//��������VDRData_TimeTask(void)
//���ܣ���¼�����ݶ�ʱ����
//���룺��
//�������
//����ֵ��ʼ��ΪENABLE
//��ע��50ms����1�Σ�������ֻ�ǶԴ洢�ı�������ʼ����ʵʱ��⣬����������о�����
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

    

    //�������Ƿ񳬳���Χ
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
            if(TimeCount[i] >= 20*60*30)//��ֹд������Сʱ������Ϊ�����ˣ��ظ�дʹ��
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
    if(RtcWriteCount >= 20)//1���¼1��
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
//��������VDRData_Init
//���ܣ�����ģ���ʼ��
//���룺��
//�������
//����ֵ��ʼ��ΪENABLE
//��ע��50ms����1�Σ�������ֻ�ǶԴ洢�ı�������ʼ����ʵʱ��⣬����������о�����
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
        if(0 == Data[i].InitFlag)//δ�Լ����
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
//��������VDRData_Write
//���ܣ�дһ����ʻ��¼������
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//pBuffer:ָ�������׵�ַ��length:���ݳ��ȣ�Time:32λhex��ʾ��RTCʱ��
//�������
//����ֵ���ɹ�SUCCESS,ʧ��ERROR
//��ע����ʻ��¼�����ݼ�¼��flash�ĸ�ʽΪ��ʱ�䣨4�ֽڣ�+�������ݣ��̶����ȣ�+У��ͣ�1�ֽڣ���ǰ�������ֽ����ݵ�У��ͣ�
//VDR_DATA_TYPE_SPEED�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.17�涨�ĸ�ʽ��126�ֽڣ�+13�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_POSITION�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.19�涨�ĸ�ʽ��666�ֽڣ�+10�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_DOUBT�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.21�涨�ĸ�ʽ��234�ֽڣ�+12�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_OVERTIME�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.24�涨�ĸ�ʽ��50�ֽڣ�+12�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_DRIVER�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.26�涨�ĸ�ʽ��25�ֽڣ�+6�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_POWER�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.28�涨�ĸ�ʽ��7�ֽڣ�+6�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_PRAMATER�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.30�涨�ĸ�ʽ��7�ֽڣ�+6�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_SPEED_STATUS�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.32�涨�ĸ�ʽ��133�ֽڣ�+6�ֽ�Ԥ���ռ�
***************************************************************************/
ErrorStatus VDRData_Write(u8 Type, u8 *pBuffer, u16 length, u32 Time)
{
    u16     i,j;
    u32     Address;
    u32     TimeCount;
    u8  VerifySum;
    u8  flag;

    //������ȷ�Լ��
    if(ERROR == VDRData_CheckWriteParameter(Type,length,Time))
    {
        return ERROR;
    }
    if((0 == Data[Type].InitFlag)||(0 == Data[Type].WriteEnableFlag))
    {
        return ERROR;
    }

    //д֮ǰ�ж�flash�����Ƿ�д����д���Ļ��������
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

    //д����
        Public_ConvertLongToBuffer(Time,DataBuffer);
    memcpy(DataBuffer+4,pBuffer,length);
    VerifySum = Public_GetSumVerify(DataBuffer,length+4);
    DataBuffer[4+length] = VerifySum;
    sFLASH_WriteBuffer(DataBuffer,Address,length+5);

    //������
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

    //������1
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
//��������VDRData_Read
//���ܣ���ȡָ������ʱ��ǰ��N����ʻ��¼������
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//pBuffer:ָ�������׵�ַ��EndTime:����ʱ�䣻block:����
//Sector,Step:���������ڴ洢���е�λ��
//�������
//����ֵ����ȡ�����ݳ���
//��ע�����ÿ�����ķ�ʽ���������һ����¼��ʱ��պõ��ڽ���ʱ�䣬�ü�¼���������
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
    *pSector = 0;//������ֵΪ0��ʾû�ж�ȡ������
    *pStep = 0;
    //������ȷ�Լ��
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
    

    
    //�����µ��Ǹ��������Ʋ��ң���ȷ������
    Sector = Data[Type].CurrentSector;
    Step = 0;
    EndTimeCount = ConverseGmtime(&EndTime);
    StartTimeCount = ConverseGmtime(&StartTime);
    FindFlag = 0;
    for(;;)
    {
        flag = 0;
        for(Step=0; Step<Data[Type].SectorStep; Step++)//�ڸ������ҵ�һ����Ч���ݣ�һ��Ϊ��1�� dxl,2015.10.24
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
            if(TimeCount < EndTimeCount)//����С�ڽ���ʱ�������
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

    //��ȡ����
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
//��������VDRData_SetWriteEnableFlag
//���ܣ�����ĳ���������͵�дʹ�ܱ�־(��ֹ��ʹ��)
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//Value:0Ϊ��ֹ,1Ϊʹ��
//�������
//����ֵ���ɹ�����SUCCESS,ʧ�ܷ���ERROR
//��ע:!!!!!�ر�ע��˭��ֹ��˭���븺����,����Ӱ��������ݵļ�¼!!!!
//��Զ�̶�ȡ��ʻ��¼�Ƿְ�����ʱ�����Ƚ�ֹд,Ȼ������ɺ��ٿ���д,
//��������ԭ����:Զ�̶�ȡ���ݵĹ�������û�н�ֹд,��ܿ��ܻ��������ݼ�¼,
//�Ӷ����·ְ���Ϣ��̬�仯,��ȡ�ķְ����ݾͲ�׼ȷ��
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
//��������VDRData_ReadWriteEnableFlag
//���ܣ���ĳ���������͵�дʹ�ܱ�־
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ��//Value:0Ϊ��ֹ,1Ϊʹ��,������0xFF��ʾ��������
//��ע:!!!!!�ر�ע��˭��ֹ��˭���븺����,����Ӱ��������ݵļ�¼!!!!
//��Զ�̶�ȡ��ʻ��¼�Ƿְ�����ʱ�����Ƚ�ֹд,Ȼ������ɺ��ٿ���д,
//��������ԭ����:Զ�̶�ȡ���ݵĹ�������û�н�ֹд,��ܿ��ܻ��������ݼ�¼,
//�Ӷ����·ְ���Ϣ��̬�仯,��ȡ�ķְ����ݾͲ�׼ȷ��
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
//��������VDRData_ReadInitFlag
//���ܣ���ȡ��ʼ����־
//���룺��
//�������
//����ֵ����ʼ����־
//��ע��1��ʾ����ɳ�ʼ����0��ʾδ��ɳ�ʼ����0xFF��ʾ��������ֻ������ɳ�ʼ��д����Ч��
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
//��������VDRData_ReadPacket
//���ܣ���ȡָ����ŵ��ǰ�����
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//pBuffer:ָ�������׵�ַ��PacketNum:�����,��1��ʼ,ʱ�����ϵ�Ϊ��1��
//Direction:0:˳�����(��ʱ������ǰ��,��ʱ�����ں���);1:�������(��ʱ������ǰ��,��ʱ�����ں���)
//�������
//����ֵ����ȡ�����ݳ���
//��ע����ȡ����������Ϊ���������ݿ����ϣ�����������£�
//VDR_DATA_SPEED_PACKET_LIST        7//�������ʱ��ÿ�����������
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
          ListNum--;//������Ǵ�0��ʼ
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
//��������VDRData_ReadList
//���ܣ���ȡָ����ŵ���������
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//pBuffer:ָ�������׵�ַ��ListNum:�����,��0��ʼ,ʱ�����ϵ�����Ϊ��0��
//�������
//����ֵ����ȡ�����ݳ���
//��ע����ȡ���������ݸ�ʽΪ����������ݿ��ʽ,������ʻ�ٶ����ݿ��ʽΪ��A.17
***************************************************************************/
u16 VDRData_ReadList(u8 *pBuffer,u8 Type,s16 ListNum)
{
    u32 Address;
    s16 Sector;
        s16 Step;
    
    s16 OffList;//ƫ������
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
            *(pBuffer+i) = 0x00;//���0
        }
    }   
    return DataCollectLen[Type];
}
/**************************************************************************
//��������VDRData_ReadListTime
//���ܣ���ȡָ����ŵ��������ݵ�ʱ��
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//pBuffer:ָ�������׵�ַ��ListNum:�����,��0��ʼ,ʱ�����ϵ�����Ϊ��0��
//�������
//����ֵ����ȡ��ʱ��
//��ע����ȡ���������ݸ�ʽΪ����������ݿ��ʽ,������ʻ�ٶ����ݿ��ʽΪ��A.17
***************************************************************************/
u32 VDRData_ReadListTime(u8 Type,s16 ListNum)
{
    u32 TimeCount;
    u32 Address;
    s16 Sector;
  s16 Step;
    
    s16 OffList;//ƫ������
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
//��������VDRData_ReadTotalPacket(u8 Type)
//���ܣ���ĳ����¼���ʹ洢���ܰ��� 
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ���ܰ��� 
//��ע��
//VDR_DATA_SPEED_PACKET_LIST        7//�������ʱ��ÿ�����Ĵ������
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
//��������VDRData_ReadListNum
//���ܣ���ȡ����λ�ã�Sector,Step���������
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ��������
//��ע�������0��ʼ
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
//��������VDRData_ReadTotalList(u8 Type)
//���ܣ���ĳ����¼���ʹ洢��������
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ����������������Ŵ�0��ʼ 
//��ע��
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
//��������VDRData_ReadNewestTime(u8 Type)
//���ܣ���ȡ�洢�����¼�¼ʱ��
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ��32λhexֵ��ʾ��ʱ��
//��ע������ֵΪ0��ʾ��û�м�¼���ݣ�����0xFFFFFFFF��ʾ����������ڲ���ֹ��
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
//��������VDRData_ReadOldestTime(u8 Type)
//���ܣ���ȡ�洢�����ϼ�¼ʱ��
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ��32λhexֵ��ʾ��ʱ��
//��ע������ֵΪ0��ʾ��û�м�¼���ݣ�����0xFFFFFFFF��ʾ����������ڲ���ֹ��
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
//��������VDRData_EraseAllFlashArea(void)
//���ܣ�������¼�����д洢������
//���룺��
//�������
//����ֵ����
//��ע��������¼��flash�洢�����ݣ���������������ط��洢�ļ�¼������û��ȥ����
***************************************************************************/
void VDRData_EraseAllFlashArea(void)
{
    u32 Addr;
    Addr = FLASH_VDR_POSITION_START_SECOTR*FLASH_ONE_SECTOR_BYTES;//����λ����Ϣ�ݴ�������1Сʱ��1�Σ�
    sFLASH_EraseSector(Addr);
    Addr = FLASH_OVER_TIME_DRIVE_INFO_START_SECTOR*FLASH_ONE_SECTOR_BYTES;//������ʱ��ʻ�ļ�ʻԱ��Ϣ
    sFLASH_EraseSector(Addr);
    VDRData_EraseFlashArea(VDR_DATA_TYPE_SPEED);//������ʻ�ٶȼ�¼
    VDRData_EraseFlashArea(VDR_DATA_TYPE_POSITION);//����λ����Ϣ��¼
    VDRData_EraseFlashArea(VDR_DATA_TYPE_DOUBT);//�����¹��ɵ��¼
    VDRData_EraseFlashArea(VDR_DATA_TYPE_OVERTIME);//������ʱ��ʻ��¼
    VDRData_EraseFlashArea(VDR_DATA_TYPE_DRIVER);//������ʻ����ݼ�¼
    VDRData_EraseFlashArea(VDR_DATA_TYPE_POWER);//�����ⲿ�����¼
    VDRData_EraseFlashArea(VDR_DATA_TYPE_PARAMETER);//���������޸ļ�¼
    VDRData_EraseFlashArea(VDR_DATA_TYPE_SPEED_STATUS);//�����ٶ�״̬��־��¼
    VDRData_Init();//dxl,2015.10.27,֮ǰ������������������ǰ��
}
/**************************************************************************
//��������VDRData_EraseFlashArea(u8 Type)
//���ܣ�����ĳ���洢��������
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVERTIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void VDRData_EraseFlashArea(u8 Type)
{
    s16     i;
    u32     Address;
    
    if(Type >= VDR_DATA_TYPE_MAX)
    {
        return;
    }

    //for(i=Data[Type].StartSector; i<Data[Type].EndSector; i++)dxl,2015.10.27,��δ��ʼ��Data[Type]�͵��ò����Ļ���������
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

//////////////////////////////�������ڲ�����////////////////////////////

/**************************************************************************
//��������VDRData_CheckWriteParameter
//���ܣ����д����
//���룺д���������͡����ȡ�ʱ��
//�������
//����ֵ���ɹ���SUCCESS��ʧ�ܣ�ERROR
//��ע��
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
//��������VDRData_CheckReadParameter
//���ܣ����д����
//���룺д���������͡����ȡ�ʱ��
//�������
//����ֵ���ɹ���SUCCESS��ʧ�ܣ�ERROR
//��ע��
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
//��������VDRData_Check(u8 Type)
//���ܣ��Լ�ĳ���洢��
//���룺Type:���ͣ���ѡֵΪDATA_TYPE_SPEED��DATA_TYPE_POSITION��
//DATA_TYPE_DOUBT��DATA_TYPE_OVER_TIME��DATA_TYPE_DRIVER��
//DATA_TYPE_POWER��DATA_TYPE_PRAMATER��DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ����
//��ע��
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
				    if(Data[Type].CurrentSector == (Data[Type].EndSector-1))//�������ˣ�����һ����Ҳ����һ��
            {
                Data[Type].CurrentSector = Data[Type].StartSector;
                Data[Type].CurrentStep = 0;
							  Address = Data[Type].CurrentSector*FLASH_ONE_SECTOR_BYTES;
                sFLASH_EraseSector(Address);
							  Data[Type].OldestSector++;//������������
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







