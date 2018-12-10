/*******************************************************************************
 * File Name:           Gnss_app.c 
 * Function Describe:   
 * Relate Module:       GNSS ģ�����
 * Writer:              Joneming
 * Date:                2013-12-11
 * ReWriter:            
 * Date:                
 *******************************************************************************/
#include "include.h"
#include "VDR.h"



//GNSS�������� ���Ȼ�����
#define GNSS_RECV_BUFFER_SIZE           1000
#define GNSS_UPLOAD_TYPE                0x00
////////////////////////////////
#define GNSS_BLIND_HEAD_LEN             7
////////////////////////
#define FLASH_GNSS_SAVE_STEP_LEN        800

#define GNSS_UPDATE_PARAM_REPORT_NUM_MAX 1000

//����ģʽ
enum 
{
    NONE_UPDATE     = 0,    //���ش洢�����ϴ���Ĭ��ֵ����
    INTERVAL_TIME,          //��ʱ�����ϴ���
    INTERVAL_METER,         //���������ϴ���     
    COUNT_TIME_STOP = 0x0B, //���ۼ�ʱ���ϴ����ﵽ����ʱ����Զ�ֹͣ�ϴ���  
    COUNT_METER_STOP,       //���ۼƾ����ϴ����ﵽ������Զ�ֹͣ�ϴ���
    COUNT_NUMBER_STOP,      //���ۼ������ϴ����ﵽ�ϴ��������Զ�ֹͣ�ϴ�*/
    GNSSUPLOADMODE_MAX
}E_GNSSUPLOADMODE;



//#define   GNSS_GGA_SIZE       100 
//sizeof("$GNGGA,000000.00,2309.920734,N,11325.877530,E,0,00,0.0,50.9,M,0.0,M,,,0.0*74\r\n")

typedef struct
{
    unsigned short recvdatalen;
    unsigned short datalen; 
    unsigned char nullDataCnt;
    unsigned char recvDataflag;
    unsigned char parseBuff[GNSS_RECV_BUFFER_SIZE];
    unsigned char recvBuff[GNSS_RECV_BUFFER_SIZE];
}ST_GNSS_ATTRIB;

static ST_GNSS_ATTRIB s_stGnssAttrib;

typedef struct
{
    unsigned long preTimeVal;
    unsigned long preinterval;  
    unsigned long freq; 
    unsigned long interval;    
    unsigned short reReportCnt;
    signed short CheckBlindSector;//
    unsigned short updateParamReportCnt;
    unsigned char updateParamReportFlag;    
    unsigned char CheckBlindFinish;
    unsigned char uploadmode;
    unsigned char collectFlag;
    unsigned char freqUpFlag;
    unsigned char updataFlag;
    unsigned char resendCnt;
    unsigned char StartBlindReportflag;    
}ST_GNSS_PARAM;
static ST_GNSS_PARAM s_stGnssParam;

    
const char *p_ConstSize[]=
{   
    "$GNGGA,000000.00,2309.920734,N,11325.877530,E,0,00,0.0,50.9,M,0.0,M,,,0.0*74\r\n",\
    "$GNGLL,2309.920734,N,11325.877530,E,000000.00,V,0*18\r\n",\
    "$GPGSA,A,1,,,,,,,,,,,,,0.0,0.0,0.0,0.0*32\r\n",\
    "$GNRMC,000000.00,V,2309.920734,N,11325.877530,E,,,010111,,,N*51\r\n",\
    "$GNTXT,01,01,01,ANTENNA OPEN*3B\r\n"
};
//����0x0090�������£�bit0Ϊ1����GPS��λ;bit1Ϊ1����Compass��λ;bit2Ϊ1����GLONASS��λ;bit3Ϊ1����Galileo��λ
#ifdef GPS_MODE_TYPE_TD3017//dxl,2013.11.28��̩�����о��ͨ�����һ��
const char *p_startupCmd[]=
{
    "$CCSIR,2,1*4B\r\n",//��GPS��λ(������)
    "$CCSIR,1,1*48\r\n",//��Compass��λ(������)
    "$CCSIR,3,1*4A\r\n",//˫ģ��λ(������)
    "$CCSIR,2,0*4A\r\n",//��GPS��λ(������)
    "$CCSIR,1,0*49\r\n",//��Compass��λ(������)
    "$CCSIR,3,0*4B\r\n"//˫ģ��λ(������)
};
#endif

#ifdef GPS_MODE_TYPE_UM330
const char *p_startupCmd[]=
{
    "$CFGSYS,h01\r\n",//��GPS��λ
    "$CFGSYS,h10\r\n",//��Compass��λ
    "$CFGSYS,h11\r\n",//˫ģ��λ
    "$CFGSYS,h01\r\n",//��GPS��λ
    "$CFGSYS,h10\r\n",//��Compass��λ
    "$CFGSYS,h11\r\n"//˫ģ��λ
};
#endif
    
//�������޸�ָ��
//4800--9600--19200--38400--57600 -- 115200 
const char *p_BpCmd[]=
{ \
    "$PCAS01,0*1C\r\n",\
    "$PCAS01,1*1D\r\n",\
    "$PCAS01,2*1E\r\n",\
    "$PCAS01,3*1F\r\n",\
    "$PCAS01,4*18\r\n",\
    "$PCAS01,5*19\r\n"
};
const u32 Baudrate[]={4800,9600,19200,38400,57600,115200};
//ģ����������ݸ���Ƶ��
//500ms --1000ms --2000ms--3000ms--4000ms ����һ��
#ifdef GPS_MODE_TYPE_TD3017
const char *p_DatFreCmd[]=
{\
    "$PCAS02,500*1A\r\n",
    "$PCAS02,1000*2E\r\n",
    "$PCAS02,2000*2D\r\n",
    "$PCAS02,3000*2C\r\n",
    "$PCAS02,4000*2B\r\n"
};
#endif
#ifdef GPS_MODE_TYPE_UM330
const char *p_DatFreCmd[]=
{\
    "$CFGMSG,0,,1\r\n",//��2������Ϊ�ձ�ʾ���ø����(NMEA)���е����,���Ƶ�����ֻ����1��1��,
    "$CFGMSG,0,,1\r\n",//1��1��
    "$CFGMSG,0,,2\r\n",//2��1��
    "$CFGMSG,0,,3\r\n",//3��1��
    "$CFGMSG,0,,4\r\n" //4��1��
};
#endif

enum 
{
    GNSS_TIMER_TASK,                        //    
    GNSS_TIMER_TEST,                        //  
    GNSS_TIMER_RESEND,                      //  
    GNSS_TIMERS_MAX
}E_GNSSTIME;

static LZM_TIMER s_stGnssTimer[GNSS_TIMERS_MAX];
///////////////////////////////

//*************�ⲿ����***************
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ

static HISTORY_AREA_STATUS s_stGnssStatus;
//�ṹ
typedef struct
{
    signed short ReportSector ;//ä������������
    signed short ReportStep;//ä�������Ĳ���    
    unsigned char ReportFlag;//ä���ϱ���־��1ΪҪ���ϱ������ϱ���0Ϊ��Ҫ��
}ST_GNSS_SAVE;

static ST_GNSS_SAVE s_stGnssSave;
//////////////////
void Gnss_StartCheckAndUploadData(void);
/*********************************************************************
//��������  :Gnss_BlindAreaParamInit
//����      :���ä������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :��ȷ����SUCCESS,���󷵻�ERROR
//��ע      :
*********************************************************************/
void Gnss_BlindAreaParamInit(void)
{
    s_stGnssStatus.StartSector  = FLASH_GNSS_SAVE_START_SECTOR;     //��ʼ����
    s_stGnssStatus.EndSector    = FLASH_GNSS_SAVE_END_SECTOR;       //��������
    s_stGnssStatus.CurrentSector = s_stGnssStatus.StartSector;  //��ǰ����
    s_stGnssStatus.OldestSector = s_stGnssStatus.StartSector;   //��������
    s_stGnssStatus.LoopCount    = 0;                    //ѭ������
    s_stGnssStatus.CurrentStep  = 0;                    //��ǰ����
    s_stGnssStatus.SectorStep   = FLASH_ONE_SECTOR_BYTES / FLASH_GNSS_SAVE_STEP_LEN;    //���������ܲ��� 
    ////////////////
    s_stGnssParam.CheckBlindSector = s_stGnssStatus.StartSector;//
}
/*********************************************************************
//��������  :Gnss_EraseBlindArea()
//����      :����ä������
//      :
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gnss_EraseBlindArea(void)
{
    u16 i;
    u32 Address;
    u8  Buffer[3];

    //****************��������*********************
    for(i=FLASH_GNSS_SAVE_START_SECTOR; i<FLASH_GNSS_SAVE_END_SECTOR; i++)
    {
        Address = i*FLASH_ONE_SECTOR_BYTES;
        sFLASH_EraseSector(Address);
        IWDG_ReloadCounter();//ι��
    }
    //****************��ʼ������*********************
    Gnss_BlindAreaParamInit();

    s_stGnssParam.CheckBlindFinish = 1;
    s_stGnssSave.ReportStep   = 0;
    s_stGnssSave.ReportSector = s_stGnssStatus.StartSector;

    //�ӵ�0����ʼ
    Buffer[0] = 0;
    Buffer[1] = 0;
    FRAM_BufferWrite(FRAM_GNSS_SAVE_STEP_ADDR,Buffer,2);//ä������������FRAM
}
/*********************************************************************
//��������  :Gnss_CheckBlindArea
//����      :���ä������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :��ȷ����SUCCESS,���󷵻�ERROR
//��ע      :
*********************************************************************/
ErrorStatus  Gnss_CheckOneBlindArea(void)
{
    static u32 MinTimeCount =0xFFFFFFFF;
    static u32 MaxTimeCount =0;
    static u8 flag =0;
    u32     i;
    u32     Address;
    u32     TimeCount;
    u8      Buffer[FLASH_GNSS_SAVE_STEP_LEN+1];
    u8      VerifySum = 0;
    u8      TempByte;
    s16 step;
    unsigned short datalen;
    TIME_T  tt;
    
    //���ÿһ�������ݰ�����
    for(step=0; step<s_stGnssStatus.SectorStep; step++)
    {
        //���㵱ǰ��ַ
        Address = s_stGnssParam.CheckBlindSector*FLASH_ONE_SECTOR_BYTES + step*FLASH_GNSS_SAVE_STEP_LEN;
        //����ǰ������ʱ���ַ���
        sFLASH_ReadBuffer(Buffer,Address,4);
        //С������
        TimeCount = Public_ConvertBufferToLong(Buffer);
        //����Ƿ��ҵ���Ч��ʱ���ǩ
        if(TimeCount != 0xFFFFFFFF) //�ҵ���Чʱ���ǩ
        {
            Gmtime(&tt, TimeCount);
            if(SUCCESS == CheckTimeStruct(&tt))
            {
                //��־��λ//���ò���ȫ������
                sFLASH_ReadBuffer(Buffer,Address,FLASH_GNSS_SAVE_STEP_LEN);
                //У��λ����Ϣ�����Ƿ���ȷ
                datalen = Public_ConvertBufferToShort(&Buffer[4]);
                VerifySum =0;
                if(datalen+GNSS_BLIND_HEAD_LEN<=FLASH_GNSS_SAVE_STEP_LEN)
                {
                    VerifySum = Public_GetSumVerify(&Buffer[GNSS_BLIND_HEAD_LEN], datalen);
                }                    
                if((VerifySum != Buffer[6])||(datalen== 0)||( datalen+GNSS_BLIND_HEAD_LEN>FLASH_GNSS_SAVE_STEP_LEN))////У�����
                {
                    flag = 0;
                    Gnss_EraseBlindArea();
                    //����ä��λ��
                    return SUCCESS;
                }
                flag = 1;
                //�ж�ʱ����������
                if(TimeCount < MinTimeCount)
                {
                    MinTimeCount = TimeCount;
                    s_stGnssStatus.OldestSector = s_stGnssParam.CheckBlindSector; //��������
                }
                //�ж�ʱ����������
                if(TimeCount > MaxTimeCount)
                {
                    MaxTimeCount = TimeCount;
                    s_stGnssStatus.CurrentStep = step;//���²���
                    s_stGnssStatus.CurrentSector = s_stGnssParam.CheckBlindSector;//����������
                }
            }
            else
            {
                flag = 0;
                Gnss_EraseBlindArea();
                //����ä��λ��
                return SUCCESS;
            }
        }
    }


    s_stGnssParam.CheckBlindSector++;
    if(s_stGnssParam.CheckBlindSector < s_stGnssStatus.EndSector)return ERROR;
    ////////////
    if(flag)
    {
        s_stGnssStatus.CurrentStep++;
    }
    
    //��鵱ǰ�����Ƿ񳬳���ǰ����
    if(s_stGnssStatus.CurrentStep >= s_stGnssStatus.SectorStep)
    {
        //ָ����һ����������ʼλ��
        s_stGnssStatus.CurrentStep = 0;
        s_stGnssStatus.CurrentSector++;

        //��鵱ǰ�����Ƿ񳬳��������
        if(s_stGnssStatus.CurrentSector >= s_stGnssStatus.EndSector)
        {
            //����ָ����ʼ����
            s_stGnssStatus.CurrentSector = s_stGnssStatus.StartSector;  
        }
    }
    //�����������һ���ĵ�ַ
    Address = (s_stGnssStatus.EndSector-1)*FLASH_ONE_SECTOR_BYTES + (s_stGnssStatus.SectorStep-1)*FLASH_GNSS_SAVE_STEP_LEN;
    //��ȡ���һ���ĸ����ֽ�
    if(flag)
    {
        for(i=0; i<20; i++)
        {
            sFLASH_ReadBuffer(&TempByte,Address+i,1);
            //�ж��Ƿ�Ϊ��Ч�ֽ�    
            if(TempByte != 0xFF)
            {
                s_stGnssStatus.LoopCount = 1;
                break;
            }
        }
    }
    ///////////////////////
    return SUCCESS;
}
 
/*********************************************************************
//��������  :Gnss_CheckBlindArea
//����      :���ä������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :��ȷ����SUCCESS,���󷵻�ERROR
//��ע      :
*********************************************************************/
void Gnss_CheckBlindArea(void)
{
    if(SUCCESS==Gnss_CheckOneBlindArea())
    {
        s_stGnssParam.CheckBlindFinish = 1;
        LZM_PublicSetOnceTimer(&s_stGnssTimer[GNSS_TIMER_TASK],LZM_AT_ONCE,Gnss_StartCheckAndUploadData); 
        if(s_stGnssParam.StartBlindReportflag)//
        {
            s_stGnssParam.StartBlindReportflag =0;
            LZM_PublicSetOnceTimer(&s_stGnssTimer[GNSS_TIMER_TEST],PUBLICSECS(0.2),Gnss_StartBlindReport);
        }
    }
}
/*********************************************************************
//��������  :Gnss_CheckBlindArea
//����      :���ä������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :��ȷ����SUCCESS,���󷵻�ERROR
//��ע      :
*********************************************************************/
void Gnss_CheckBlindAreaStart(void)
{
    Gnss_BlindAreaParamInit();
    s_stGnssParam.CheckBlindFinish = 0;//
    LZM_PublicSetCycTimer(&s_stGnssTimer[GNSS_TIMER_TASK],LZM_AT_ONCE,Gnss_CheckBlindArea); 
}
/*********************************************************************
//��������  :Gnss_WriteOneGnssDataToFlash
//����      :��һ����Ϣд��FLASH
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :һ��ä�����ݰ�����ʱ��4�ֽ�+��Ϣ����2�ֽ�+У���1�ֽ�
*********************************************************************/
ErrorStatus Gnss_WriteOneGnssDataToFlash(u8 *pBuffer, unsigned short datalen)
{
    u32 Address;
    u8  Buffer[FLASH_GNSS_SAVE_STEP_LEN];
    u8  i;
    u8  j;
    u32 ttCount;
    u32 TimeCount;
    u8  VerifyCode;
    s16 Sector;
    s16 Step;
    TIME_T  tt;
    if(0==s_stGnssParam.CheckBlindFinish)return ERROR;
    ttCount = RTC_GetCounter();
    //�жϵ�ǰ����ʱ���Ƿ������һ����ʱ��
    if((s_stGnssStatus.CurrentStep > 0)||
    (s_stGnssStatus.CurrentSector > s_stGnssStatus.StartSector)||
    (s_stGnssStatus.LoopCount > 0)) //�洢����������
    {

        Sector = s_stGnssStatus.CurrentSector;
        Step = s_stGnssStatus.CurrentStep;
        Step--;
        if(Step < 0)
        {
            Step = s_stGnssStatus.SectorStep-1;
            Sector--;
            if(Sector < s_stGnssStatus.StartSector)
            {
                Sector = s_stGnssStatus.EndSector-1;
            }
        }
        Address = Sector*FLASH_ONE_SECTOR_BYTES+Step*FLASH_GNSS_SAVE_STEP_LEN;
        sFLASH_ReadBuffer(Buffer,Address,4);//��ʱ���ֽ�
        TimeCount = Public_ConvertBufferToLong(Buffer);
        Gmtime(&tt, TimeCount);
        if(ERROR == CheckTimeStruct(&tt))
        {
            sFLASH_EraseSector(Address);//����������
            s_stGnssStatus.CurrentStep = 0;//��ǰ�����Ƶ�������ͷ
            return ERROR;
        }
        if(ttCount < TimeCount)
        {
            return ERROR;   
        }
    }
    Address = s_stGnssStatus.CurrentSector*FLASH_ONE_SECTOR_BYTES + s_stGnssStatus.CurrentStep*FLASH_GNSS_SAVE_STEP_LEN;//��ȡ��ǰ��ַ  
    sFLASH_ReadBuffer(Buffer,Address,10);//�ж��Ƿ���д������
    for(i=0; i<10; i++)
    {
        if(0xff != Buffer[i])
        {
            sFLASH_EraseSector(Address);//����������
            s_stGnssStatus.CurrentStep = 0;//��ǰ�����Ƶ�������ͷ
            for(j=0; j<200; j++)//�ȴ�һ��ʱ���д��
            {
            }
            Address = s_stGnssStatus.CurrentSector*FLASH_ONE_SECTOR_BYTES;//������ַ
            break;
        }
    }
    ///////////////////////////////
    if(datalen+GNSS_BLIND_HEAD_LEN>FLASH_GNSS_SAVE_STEP_LEN) datalen = FLASH_GNSS_SAVE_STEP_LEN - GNSS_BLIND_HEAD_LEN;
    ////////////////////
    Public_ConvertLongToBuffer(ttCount,&Buffer[0]);
    Public_ConvertShortToBuffer(datalen,&Buffer[4]);
    VerifyCode=Public_GetSumVerify(pBuffer,datalen);
    Buffer[6] = VerifyCode;//У����
    memcpy(&Buffer[GNSS_BLIND_HEAD_LEN],pBuffer,datalen);
    ////////////////////////
    sFLASH_WriteBuffer(Buffer, Address, datalen+GNSS_BLIND_HEAD_LEN);//����д��flash
    s_stGnssStatus.CurrentStep++;//��ǰ������1
    if(s_stGnssStatus.CurrentStep  >= s_stGnssStatus.SectorStep)//�жϵ�ǰ������û�п�Խ����
    {
        s_stGnssStatus.CurrentStep = 0; //��ǰ����Ϊ0
        s_stGnssStatus.CurrentSector++; //��ǰ��������һ��
        Address = s_stGnssStatus.CurrentSector*FLASH_ONE_SECTOR_BYTES;//����������
        sFLASH_EraseSector(Address);
        if(s_stGnssStatus.CurrentSector >= s_stGnssStatus.EndSector)//�жϵ�ǰ�����Ƿ�ﵽĩβ
        {
            s_stGnssStatus.CurrentSector = s_stGnssStatus.StartSector;//ָ����ʼ����
            s_stGnssStatus.LoopCount++; //ѭ���洢������1
        }
        if(s_stGnssStatus.LoopCount > 0)//������������
        {
            s_stGnssStatus.OldestSector++;
            if(s_stGnssStatus.OldestSector >= s_stGnssStatus.EndSector)
            {
                s_stGnssStatus.OldestSector = s_stGnssStatus.StartSector;//ָ����ʼ����
            }            
        }
    }   
    ///////////////////
    s_stGnssParam.reReportCnt = 1;
    //////////////////
    return SUCCESS;
}
/*********************************************************************
//��������  :Gnss_BlindReport
//����      :ä������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gnss_BlindReport(void)
{
    u8  Buffer2[4];
    u16 datalen =0;
    u32 Address;
    u8  count;
    u8  flag;
    u8  WriteFlag = 0;
    u8  VerifyCode;
    s16 BlindSectorBackup;
    s16 BlindStepBackup;
    s16 Sector =0;
      u8 channel = CHANNEL_DATA_1;//Ϊ����ͨ�������
    
    flag = 0;
    if((s_stGnssSave.ReportSector == s_stGnssStatus.CurrentSector)&&(s_stGnssSave.ReportStep >= s_stGnssStatus.CurrentStep))
    {
        flag = 1;
    }
    else if((s_stGnssSave.ReportSector >= FLASH_GNSS_SAVE_END_SECTOR)||(s_stGnssSave.ReportSector < FLASH_GNSS_SAVE_START_SECTOR))
    {
        flag = 1;
    }
    else if((s_stGnssSave.ReportStep >= s_stGnssStatus.SectorStep)||(s_stGnssSave.ReportStep < 0))
    {
        flag = 1;
    }
    if(0 == flag)
    {
        count = 0;
        flag = 0;
        BlindSectorBackup = s_stGnssSave.ReportSector;
        BlindStepBackup = s_stGnssSave.ReportStep;
        Address = s_stGnssSave.ReportSector*FLASH_ONE_SECTOR_BYTES + s_stGnssStatus.SectorStep*FLASH_GNSS_SAVE_STEP_LEN+1;
        sFLASH_ReadBuffer(&flag,Address,1);
        if(0xaa == flag)//�������Ѳ�����
        {
            s_stGnssSave.ReportSector++;//ָ����һ������
            s_stGnssSave.ReportStep = 0;//ָ��������ʼ��
            if(s_stGnssSave.ReportSector >= FLASH_GNSS_SAVE_END_SECTOR)
            {
                s_stGnssSave.ReportSector = FLASH_GNSS_SAVE_START_SECTOR;
            }
        }
        else
        {
            Address = s_stGnssSave.ReportSector*FLASH_ONE_SECTOR_BYTES + s_stGnssSave.ReportStep*FLASH_GNSS_SAVE_STEP_LEN;
            sFLASH_ReadBuffer(RadioShareBuffer,Address,FLASH_GNSS_SAVE_STEP_LEN);//��һ��ä������
            datalen = Public_ConvertBufferToShort(&RadioShareBuffer[4]);
            VerifyCode = Public_GetSumVerify(&RadioShareBuffer[GNSS_BLIND_HEAD_LEN], datalen);            
            if(VerifyCode == RadioShareBuffer[6])//У����ȷ
            {
                count++;
            }
            s_stGnssSave.ReportStep++;
            if(s_stGnssSave.ReportStep >= s_stGnssStatus.SectorStep)
            {
                WriteFlag = 1;
                Sector = s_stGnssSave.ReportSector;
                s_stGnssSave.ReportSector++;//ָ����һ������
                s_stGnssSave.ReportStep = 0;//ָ��������ʼ��
                if(s_stGnssSave.ReportSector >= FLASH_GNSS_SAVE_END_SECTOR)
                {
                    s_stGnssSave.ReportSector = FLASH_GNSS_SAVE_START_SECTOR;
                }
            }
        }
        flag = 0;
        if((s_stGnssSave.ReportSector == s_stGnssStatus.CurrentSector)&&(s_stGnssSave.ReportStep >= s_stGnssStatus.CurrentStep))
        {
            flag = 1;
        }
        else if((s_stGnssSave.ReportSector >= FLASH_GNSS_SAVE_END_SECTOR)||(s_stGnssSave.ReportSector < FLASH_GNSS_SAVE_START_SECTOR))
        {
            flag = 1;
        }
        else if((s_stGnssSave.ReportStep > s_stGnssStatus.SectorStep)||(s_stGnssSave.ReportStep < 0))
        {
            flag = 1;
        }
        ///////////
        if(1 == flag)
        {
            //����Ҫ����
            s_stGnssSave.ReportSector = s_stGnssStatus.CurrentSector;
            s_stGnssSave.ReportStep = s_stGnssStatus.CurrentStep;
        }
        if(count)//1-10���������
        {
            if(ACK_OK == RadioProtocol_OriginalDataUpTrans(channel,GNSS_UPLOAD_TYPE,&RadioShareBuffer[GNSS_BLIND_HEAD_LEN],datalen))//ä��λ����Ϣ�����ϴ�����
            {
                if(1 == WriteFlag)
                {
                    WriteFlag = 0;
                    Address = Sector*FLASH_ONE_SECTOR_BYTES + s_stGnssStatus.SectorStep*FLASH_GNSS_SAVE_STEP_LEN+1;
                    flag = 0xaa;
                    sFLASH_WriteBuffer(&flag, Address, 1);//��Ǹ��������ϱ�
                }
            }
            else
            {
                s_stGnssSave.ReportSector = BlindSectorBackup;
                s_stGnssSave.ReportStep = BlindStepBackup;
            }            
            Buffer2[0] = s_stGnssSave.ReportStep >> 8;
            Buffer2[1] = s_stGnssSave.ReportStep;
            FRAM_BufferWrite(FRAM_GNSS_SAVE_STEP_ADDR,Buffer2,2);//ä������������FRAM
        }
    }
    else
    {
//        ICCard_ReadRepairServiceDataForSendPC(); dxl,2015.9,
        s_stGnssParam.reReportCnt = 0;
        s_stGnssSave.ReportFlag = 0;//����Ҫ����
        s_stGnssSave.ReportSector = s_stGnssStatus.CurrentSector;
        s_stGnssSave.ReportStep = s_stGnssStatus.CurrentStep;        
        Buffer2[0] = s_stGnssSave.ReportStep >> 8;
        Buffer2[1] = s_stGnssSave.ReportStep;
        FRAM_BufferWrite(FRAM_GNSS_SAVE_STEP_ADDR,Buffer2,2);//ä������������FRAM
    }
}
/*********************************************************************
//��������  :Gnss_CheckAndBlindReport
//����      :ä������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gnss_CheckAndBlindReport(void)
{
    Gnss_BlindReport();
    if(s_stGnssSave.ReportFlag&&communicatio_GetMudulState(COMMUNICATE_STATE))
    {
        LZM_PublicSetOnceTimer(&s_stGnssTimer[GNSS_TIMER_TEST],PUBLICSECS(2.6),Gnss_CheckAndBlindReport);
    }
}
/*************************************************************
** ��������: Gnss_StartBlindReport
** ��������: ����Gnssä������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Gnss_StartBlindReport(void)
{
    u8  BufferLen;
    u8  Buffer[5];
    s16 Sector;
    u32 OldestTimeCount;
    u32 TimeCount;
    u32 Address;
    u8  flag;

    if(0==s_stGnssParam.CheckBlindFinish)
    {
        s_stGnssParam.StartBlindReportflag = 1;
        return ;
    }
    //ä��������ʼ�����Ĳ���
    BufferLen = FRAM_BufferRead(Buffer, 2, FRAM_GNSS_SAVE_STEP_ADDR);
    if(BufferLen == 2)
    {
        s_stGnssSave.ReportStep = Public_ConvertBufferToShort(Buffer);
        if(s_stGnssSave.ReportStep >= s_stGnssStatus.SectorStep)
        {
            s_stGnssSave.ReportStep = 0;
        }
    }
    else
    {
        s_stGnssSave.ReportStep = 0;
    }
    //ȷ��ä�������Ŀ�ʼ����
    Sector = FLASH_GNSS_SAVE_START_SECTOR;
    OldestTimeCount = 0xffffffff;
    for(;;)
    {
        Address = Sector*FLASH_ONE_SECTOR_BYTES + s_stGnssStatus.SectorStep*FLASH_GNSS_SAVE_STEP_LEN+1;
        sFLASH_ReadBuffer(&flag,Address,1);
        if(0xaa != flag)//������δ������
        {
            Address = Sector*FLASH_ONE_SECTOR_BYTES;
            sFLASH_ReadBuffer(Buffer,Address,4);//��һ��ä������
            TimeCount=Public_ConvertBufferToLong(Buffer);
            if(TimeCount < OldestTimeCount)
            {
                OldestTimeCount = TimeCount;
                s_stGnssSave.ReportSector = Sector;
            }
        }
        Sector++;
        if(Sector >= FLASH_GNSS_SAVE_END_SECTOR)
        {
            break;
        }
    }

    if((s_stGnssSave.ReportSector == s_stGnssStatus.CurrentSector)
    &&(s_stGnssSave.ReportStep >= s_stGnssStatus.CurrentStep))
    {
        s_stGnssSave.ReportFlag   = 0;
        s_stGnssParam.reReportCnt = 0;
//        ICCard_ReadRepairServiceDataForSendPC(); dxl,2015.9,
    }
    else
    {
        s_stGnssSave.ReportFlag     = 1;
        s_stGnssParam.reReportCnt   = 1;
        LZM_PublicSetOnceTimer(&s_stGnssTimer[GNSS_TIMER_TEST],PUBLICSECS(1),Gnss_CheckAndBlindReport);
    }
}
/*************************************************************
** ��������: Gnss_CommRxIsr
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Gnss_CommRxIsr(unsigned char val)
{
    s_stGnssAttrib.nullDataCnt  = 0;
    s_stGnssAttrib.recvDataflag = 1;
    if(s_stGnssAttrib.recvdatalen < GNSS_RECV_BUFFER_SIZE)s_stGnssAttrib.recvdatalen++;
    s_stGnssAttrib.recvBuff[s_stGnssAttrib.recvdatalen] = val;
}
/*************************************************************
** ��������: TiredDrive_SaveFramTimeToDriverInfo
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Gnss_ShowDebugInfo(unsigned char *str)
{
    #if 0   
    TIME_T stTime;
    RTC_GetCurTime(&stTime);
    LOG_PR("\r\n %s T=%02d-%02d %02d:%02d:%02d \r\n ",str,stTime.month,stTime.day,stTime.hour,stTime.min,stTime.sec);
    #endif
}
/*************************************************************
** ��������: EN_GnssArea
** ��������: �ɼ������ϴ���־
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void EN_GnssAreaEx(void)
{
    s_stGnssAttrib.datalen      = 0;
    s_stGnssParam.freqUpFlag    = 0;
    s_stGnssParam.updataFlag    = 0;
    s_stGnssParam.preTimeVal    = RTC_GetCounter();
    if(s_stGnssParam.uploadmode < 2)s_stGnssParam.preinterval = 0;
    /////////////////
    s_stGnssParam.collectFlag   = 1;
}

/*************************************************************
** ��������: EN_GnssArea
** ��������: �ɼ������ϴ���־
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void EN_GnssArea(void)
{
    EN_GnssAreaEx();
    s_stGnssParam.updateParamReportFlag = 0;
    s_stGnssParam.updateParamReportCnt  = 0;
}
/*************************************************************
** ��������: DIS_GnssArea
** ��������: �ر��ϴ���־
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void DIS_GnssArea(void)
{
    s_stGnssAttrib.datalen      = 0;
    s_stGnssParam.freqUpFlag    = 0;
    s_stGnssParam.updataFlag    = 0;
    s_stGnssParam.collectFlag   = 0;
    s_stGnssParam.updateParamReportFlag = 0;
    s_stGnssParam.updateParamReportCnt  = 0;
}
/*************************************************************
** ��������: Gnss_GetcollectFlag
** ��������: ��ȡ�ϴ������ע
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Gnss_GetcollectFlag(void)
{
    return (1==s_stGnssParam.collectFlag)?1:0;
}

/*********************************************************************
//��������  :Delay_nop
//����      ://�ڲ��ӳٺ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Delay_nop(u32 x)
{ 
    u32 i;
    for(i = 0; i <= x; i ++);
}
/*********************************************************************
//��������  :void GNSS_BaudrateInit(u32 rate)
//����      :GPS��ʼ�� �����ʲ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void GNSS_BaudrateInit(u32 rate)
{
    COMM_SetBaudRateInitialize(COM4,rate);
}
/****************************************************************************/
/*function  name:           uint Gnss_ReadCollectFreq(void) */
/*input parameter:          none  */
/*output parameter:         ���زɼ����� */
/*FUNCTION:                         ʵʱ��ȡ���ݣ������û�����
*/
/*****************************************************************************/ 
u32 Gnss_ReadCollectFreq(void)
{
    uchar buf[5];
    if(EepromPram_ReadPram(E2_GPS_SET_GATHER_RATE_ID,buf))
    {
        return Public_ConvertBufferToLong(buf);
    }
    return 1;
}
/*************************************************************
** ��������: Gnss_ReadUpdateMode
** ��������: ��ȡ��λ�����ϴ���ʽ
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Gnss_ReadUpdateMode(void)
{
    unsigned char tmp;
    if(EepromPram_ReadPram(E2_GPS_SET_UPLOAD_MODE_ID,&tmp))
    {
        switch(tmp)
        {
            case 0:
                if(Area_GetCollectGnssFlag())//modify by joneming
                {
                    EN_GnssArea();//modify by joneming
                }
                #ifdef HUOYUN_DEBUG_OPEN
                else
                {
                    DIS_GnssArea();//dxl,2013.12.6,Ϊ0ʱ��ʾ���ϴ�
                }
                #endif
                return tmp;
                //break;
            case 1:
                if(!Area_GetCollectGnssFlag()&&Area_GetInAreaFlag())//modify by joneming
                {
                    DIS_GnssArea();//modify by joneming
                }
                #ifdef HUOYUN_DEBUG_OPEN
                else
                {
                    EN_GnssArea();//dxl,2013.12.6,Ϊ0ʱ��ʾ���ϴ�
                }
                #else
                else
                if(s_stGnssParam.updateParamReportFlag)
                {
									   if(1 == BBGNTestFlag)//dxl,2016.5.20,�������ܲ���ʱ��û����������������ⲻ����GNSS�����ϴ�
										 {
										 
										 }
										 else
										 {
                          EN_GnssAreaEx();
										 }
                }
                #endif
                return tmp;
            case 2:
            case 0x0b:
            case 0x0c:
            case 0x0d:
                if(s_stGnssParam.updateParamReportFlag)
                {
                     EN_GnssAreaEx();
                }
                return tmp;
            default:
                return 0;
        }
    }
    return 0;
}
/*************************************************************
** ��������: Gnss_GetCurUpdateMode
** ��������: ȡ�õ�ǰ��λ�����ϴ���ʽ
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Gnss_GetCurUpdateMode(void)
{
    return s_stGnssParam.uploadmode;
}
/*************************************************************
** ��������: Gnss_ReadBaseData
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/  //��ȡ�ϴ���׼
u32 Gnss_ReadBaseData(void)
{
    uchar buf[5];
    if(EepromPram_ReadPram(E2_GPS_SET_UPLOAD_VALUE_ID,buf))
    {
        return Public_ConvertBufferToLong(buf);
    }
    return 0;   
}
/*********************************************************************/
/*function  name:           void    GnssCheckChangePar(void)        */
/*input parameter:          none  */
/*output parameter:         none */
/*FUNCTION:  ʵʱ�����ı亯�����������õ��ȴ˺������ı�ɼ�Ƶ�ʣ��ı䲨����
             �ı�GNSS�Ĳɼ�ģʽ ������ģʽ��GPSģʽ��˫ģʽ��
*/
/*********************************************************************/ 
void Gnss_UpdataUploadParam(void)
{
    unsigned char uploadmode;
    //��ȡ�ϴ�ģʽ
    uploadmode = Gnss_ReadUpdateMode();
    if((s_stGnssParam.uploadmode != uploadmode)||(1==s_stGnssParam.updateParamReportFlag))
    {
        s_stGnssParam.uploadmode = uploadmode;
        s_stGnssParam.preinterval = 0;
    }    
    //ʵʱ��ȡ�ϴ���׼
    s_stGnssParam.interval = Gnss_ReadBaseData();
    //��ȡ�ϴ�Ƶ��
    s_stGnssParam.freq = Gnss_ReadCollectFreq();
    /////////////////
    if(INTERVAL_TIME == s_stGnssParam.uploadmode)
    {
        if(0xffffffff == s_stGnssParam.interval||0 == s_stGnssParam.interval)
        {
            s_stGnssParam.interval = 1;//Ĭ��һ��
        }
    }
    //////////////////////////
    s_stGnssAttrib.datalen      = 0;
    s_stGnssParam.reReportCnt   = 0;
}
/****************************************************************************/
/*function  name:           void Check_cmd_String(void);   */
/*input parameter:          none  */
/*output parameter:         none */
/*FUNCTION:      ��������ַ���     
//Aouthor :   fanqinghai  2016.03.24
*/
/*****************************************************************************/ 
void Check_cmd_String(char * buff,u16 modem)
{
    char *pbuf = NULL;
    u8 sum = 0;
    char str[5];
    pbuf = buff ;
    for(;;)
    {
        if(',' == *pbuf++)
        {

            sprintf(str, "%-X", modem); 

            strcpy(pbuf,str);
            break ;    
        }
        
    }

    pbuf = buff ; //��λ
    for(;;)
    {
        if('$' == *pbuf++)
        {
            buff  = pbuf; //$'��У��
            break ;
        }
    }
    sum = *pbuf++;
    for(;;)
    {
        

        if('\0' != *pbuf)
        {
            sum ^=  *pbuf++; 
        }
        else if('\0' == *pbuf)
        {
           *pbuf++ = '*' ; 
           
           sprintf(str, "%-2X", sum); 
           strcpy(pbuf,str);
           pbuf+=2  ;
           *pbuf++ = '\r';
           *pbuf++ = '\n';
           break ;
        }
    }
}


/*********************************************************************/
/*function  name:           void    GnssCheckChangePar(void)        */
/*input parameter:          none  */
/*output parameter:         none */
/*FUNCTION:  ʵʱ�����ı亯�����������õ��ȴ˺������ı�ɼ�Ƶ�ʣ��ı䲨����
             �ı�GNSS�Ĳɼ�ģʽ ������ģʽ��GPSģʽ��˫ģʽ��
*/
/*********************************************************************/ 
void GnssCheckChangePar(void)
{
    uchar tmp;
    char buf[20];
    //ʵʱ�ı�ɼ�Ƶ��
    if(EepromPram_ReadPram(E2_GPS_SET_OUTPUT_RATE_ID,&tmp))
    {
        if(tmp<5)//&&tmp>0)
        {
            #ifdef GPS_MODE_TYPE_TD3017
            strcpy(buf,p_DatFreCmd[tmp]);
            Gps_SendData((u8*)buf,strlen(buf));
            Delay_nop(600*1000);//ytl 2013 06 05 
					  #endif

            
            #ifdef GPS_MODE_TYPE_ATGM332D   //fanqinghai 216.03.23 �п�΢���ӳ���
            memset(buf,0,sizeof(buf));
            strcpy(buf,"$PCAS02,");
            if(!tmp)
            {
                Check_cmd_String(buf,0x500);  //���500
            }
            else
            {
                Check_cmd_String(buf,0x1000*tmp);  
            }
            Gps_SendData((u8*)buf, strlen(buf));
            Delay_nop(600*1000);        //����ʱ�´η�����ȥ
            memset(buf,0,sizeof(buf));
            strcpy(buf,"$PCAS00*01\r\n");
            Gps_SendData((u8*)buf, strlen(buf));//��������ϵͳ����
						Delay_nop(600*1000);
            #endif
        }
    }   

    /*
    //ʵʱ�ı䲨����
    //���ò������ٶ�
    if(EepromPram_ReadPram(E2_GPS_SET_BAUD_ID,&tmp))
    {
        if(tmp<5)
        {
            strcpy(buf,p_BpCmd[tmp]);
            //GNSS_putstr(buf); 
            Gps_SendData(buf,strlen(buf));
            //GNSS_BaudrateInit(Baudrate[tmp]);
            Delay_nop(500*1000);    //ytl 2013 06 05 
        }
    }
    */
    //ʵʱ�ı�GNSSģʽ
    if(EepromPram_ReadPram(E2_GPS_SET_MODE_ID,&tmp))
    {
        if(1<=tmp&&tmp<=3)//��GPSģʽ,dxl
        {
					#ifdef GPS_MODE_TYPE_TD3017
            strcpy(buf,p_startupCmd[tmp+2]);
            Gps_SendData((u8*)buf,strlen(buf));//�б���ʱ���������Σ�3��ģʽ���ǲ��û�϶�λģʽ  
            //��ʱ,dxl,2013.12.5
            Delay_nop(600*1000);					
          #endif

            #ifdef GPS_MODE_TYPE_UM330
            //���ں�о��ͨ��ģ�飬�����궨λģʽ�󣬻���Ҫ���ͱ�����������������ְ�Ĭ�ϵĶ�λģʽ������
            strcpy(buf,"$CFGSAVE,h1f\r\n");
            Gps_SendData((u8*)buf, strlen(buf));//��������ϵͳ����
            //��ʱ
            Delay_nop(600*1000);
            #endif
            #ifdef GPS_MODE_TYPE_ATGM332D     //fanqinghai 216.03.23 �п�΢���ӳ���
            memset(buf,0,sizeof(buf));
            strcpy(buf,"$PCAS04,");
            Check_cmd_String(buf,tmp);
            Gps_SendData((u8*)buf, strlen(buf));
					  Delay_nop(600*1000);
					
            memset(buf,0,sizeof(buf));
            strcpy(buf,"$PCAS00*01\r\n");
            Gps_SendData((u8*)buf, strlen(buf));  //��������ϵͳ����
						Delay_nop(600*1000);
            #endif
            
        }
    }
    //////////////////
    s_stGnssParam.updateParamReportCnt = 0;
    s_stGnssParam.updateParamReportFlag = 1;
    /////////////////
    Gnss_UpdataUploadParam();
}

/****************************************************************************/
/*function  name:           void Gnss_App_TimeTask(void);   */
/*input parameter:          none  */
/*output parameter:         none */
/*FUNCTION:                         
*/
/*****************************************************************************/ 
FunctionalState Gnss_App_TimeTask(void)
{
    return DISABLE;
}
/*************************************************************
** ��������: Gnss_SaveCurCollectDataToFlash
** ��������: 
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Gnss_SaveCurCollectDataToFlash(void)
{
    if(s_stGnssAttrib.datalen)
    Gnss_WriteOneGnssDataToFlash(s_stGnssAttrib.parseBuff,s_stGnssAttrib.datalen);
}
/*************************************************************
** ��������: Gnss_CheckAndUploadData
** ��������: �����жϽ�������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Gnss_OriginalDataUpTrans(void)
{
    u8 channel = CHANNEL_DATA_1;//Ϊ����ͨ�������
    
    if(0==s_stGnssAttrib.datalen)return ;
    #ifdef HUOYUN_DEBUG_OPEN
    if(Area_GetCollectGnssFlag())
    {
        if(Area_GetCloseGPRSFlag())//����ä��
        {
            Gnss_SaveCurCollectDataToFlash();
            return;
        } 
    }
    /////////////////////////
    if(s_stGnssParam.reReportCnt)//ä������ʱ���ϱ�ʵʱGNSS��Ϣ
    {
        if(s_stGnssParam.reReportCnt < 1000)s_stGnssParam.reReportCnt++;
        else s_stGnssParam.reReportCnt = 0;
        return;
    }
    #endif
    
    if(communicatio_GetMudulState(COMMUNICATE_STATE))
    {
			RadioProtocol_OriginalDataUpTrans(channel,GNSS_UPLOAD_TYPE,s_stGnssAttrib.parseBuff,s_stGnssAttrib.datalen);//dxl,2016.5.23
			/*  dxl,2016.5.23
        if(ACK_ERROR==RadioProtocol_OriginalDataUpTrans(channel,GNSS_UPLOAD_TYPE,s_stGnssAttrib.parseBuff,s_stGnssAttrib.datalen))
        {
            if(s_stGnssParam.resendCnt++ < 4)//�ط�3�Σ�һ��4��
            {
                LZM_PublicSetOnceTimer(&s_stGnssTimer[GNSS_TIMER_RESEND],PUBLICSECS(0.2),Gnss_OriginalDataUpTrans);
                return;
            }
        } 
*/			
    }
    ////////////////////////////
    s_stGnssParam.resendCnt = 0;
    ////////////////////////
    if(s_stGnssParam.updateParamReportFlag)//
    {
        s_stGnssParam.updateParamReportCnt++;
        if(s_stGnssParam.updateParamReportCnt>=GNSS_UPDATE_PARAM_REPORT_NUM_MAX)
        {
            DIS_GnssArea();
        }
    }    
}
/*************************************************************
** ��������: Gnss_GetCurTotalMeter
** ��������: ��þ���(��)
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned long Gnss_GetCurTotalMeter(void)
{
    
    unsigned long curtotalmeter;    
    if(VDRSpeed_GetCurSpeedType())
    {
        curtotalmeter = MileMeter_GetTotalMile();        
    }
    else
    {
        curtotalmeter = VDRPulse_GetTotalMile();
    }
    curtotalmeter *=10;
    return curtotalmeter;
}
/*********************************************************************
//��������  :uchar Gnss_CheckCmpConstantParam
//����      :
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/      
unsigned char Gnss_CheckCmpConstantParam(unsigned long curcheckVal,unsigned long value)
{
    if(curcheckVal>s_stGnssParam.preinterval)
    {
        if(curcheckVal>=value+s_stGnssParam.preinterval)
        {
            s_stGnssParam.preinterval +=value;
            if(curcheckVal>s_stGnssParam.preinterval+value)
            {
                s_stGnssParam.preinterval = curcheckVal;
            }            
            return 1;
        }
    }
    else
    if(curcheckVal < s_stGnssParam.preinterval)
    {
        s_stGnssParam.preinterval = curcheckVal;
    }
    //////////////////
    return 0;
}

/*************************************************************
** ��������: Gnss_CheckAndUploadData
** ��������: �����жϽ�������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Gnss_CheckAndUploadData(void)
{
    unsigned long curcheckVal;    
    if(0==Gnss_GetcollectFlag())return;//�ж���Ҫ�ɼ�����
    if(0==Area_GetInAreaFlag())//û���κ�������
    {
        if(Area_GetRectAreaTotalNum()||Area_GetRoundAreaTotalNum()||Polygon_GetAreaTotalNum())
        {
            DIS_GnssArea();
            return;//�ж���Ҫ�ɼ�����
        }
    }
    //��ȡʱ��
    curcheckVal = RTC_GetCounter();
    ///////////////////
    if(curcheckVal>=s_stGnssParam.freq+s_stGnssParam.preTimeVal)
    {
        s_stGnssParam.preTimeVal += s_stGnssParam.freq;
        if(curcheckVal>=s_stGnssParam.freq+s_stGnssParam.preTimeVal)
        {
            s_stGnssParam.preTimeVal = curcheckVal;
        }
        s_stGnssParam.freqUpFlag = 1;
    }
    ///////////////////////
    switch(s_stGnssParam.uploadmode)//�ɼ�ģʽ
    {
        case NONE_UPDATE://���ر���,����Gnss�����Ƿ����
            if(Gnss_CheckCmpConstantParam(curcheckVal,s_stGnssParam.interval))
            {
                #ifdef HUOYUN_DEBUG_OPEN
                if(Area_GetCollectGnssFlag())
                {
                    if(Area_GetCloseGPRSFlag())//����ä��
                    {
                        Gnss_SaveCurCollectDataToFlash();
                    }                        
                    else
                    {
                        Gnss_OriginalDataUpTrans();
                    }                        
                }
                #endif
            }
            break;
        case INTERVAL_TIME://��ʱ�ϴ�,����Gnss�����Ƿ����
            if(Gnss_CheckCmpConstantParam(curcheckVal,s_stGnssParam.interval))
            {
                if(Area_GetCollectGnssFlag()&&Area_GetCloseGPRSFlag())//����ä��
                {
                    #ifdef HUOYUN_DEBUG_OPEN
                    Gnss_SaveCurCollectDataToFlash();
                    #endif
                }
                else
                {
                    Gnss_OriginalDataUpTrans();
                }
            }                       
            break;
        case INTERVAL_METER://��������,����Gnss�����Ƿ����
            curcheckVal = Gnss_GetCurTotalMeter();
            //////////////////////////
            if(Gnss_CheckCmpConstantParam(curcheckVal,s_stGnssParam.interval))
            {
                Gnss_OriginalDataUpTrans();
            }                       
            break;
        case COUNT_TIME_STOP://���ۼ�ʱ���ϴ����ﵽ����ʱ����Զ�ֹͣ�ϴ���  
            if(0==s_stGnssParam.preinterval)s_stGnssParam.preinterval=curcheckVal;
            if(curcheckVal<s_stGnssParam.preinterval+s_stGnssParam.interval)
            {
                if(s_stGnssParam.updataFlag)
                {
                    s_stGnssParam.updataFlag = 0;
                    Gnss_OriginalDataUpTrans();
                }
            }
            else
            {
                DIS_GnssArea();
            }                   
            break;
        case COUNT_METER_STOP://�����ۼƾ����ϴ����ﵽ������Զ�ֹͣ�ϴ�
            curcheckVal =Gnss_GetCurTotalMeter();
            ////////////////
            if(0==s_stGnssParam.preinterval)s_stGnssParam.preinterval = curcheckVal;                   
            if(curcheckVal < s_stGnssParam.preinterval+s_stGnssParam.interval)  
            {
                if(s_stGnssParam.updataFlag)
                {
                    s_stGnssParam.updataFlag = 0;
                    Gnss_OriginalDataUpTrans();
                }
            }
            else
            {
                DIS_GnssArea();
            }
            break;
        case COUNT_NUMBER_STOP:////���ۼ������ϴ����ﵽ�ϴ��������Զ�ֹͣ�ϴ�*/
            if(s_stGnssParam.preinterval < s_stGnssParam.interval)
            {
                if(s_stGnssParam.updataFlag)
                {
                    s_stGnssParam.updataFlag = 0;
                    s_stGnssParam.preinterval++;                    
                    Gnss_OriginalDataUpTrans();
                }
            }
            else
            {
                DIS_GnssArea();
            }
            break;
        default:
            break;

    }
}
/*************************************************************
** ��������: Gnss_CheckAndUploadData
** ��������: �����жϽ�������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Gnss_StartCheckAndUploadData(void)
{
    LZM_PublicSetCycTimer(&s_stGnssTimer[GNSS_TIMER_TASK],PUBLICSECS(0.1),Gnss_CheckAndUploadData);
}
/****************************************************************************/
/*function  name:           void InitConfigGNSS(void)       */
/*input parameter:          none  */
/*output parameter:         none */
/*FUNCTION:                         ����Ҫ���ʼ��GNSSģ��
*/
/*****************************************************************************/     
void Gnss_ReadParam(void)
{
    uchar tmp;//length;
    char buf[20];
    //Ĭ���ϵ����ó�9600
    //���ø���Ƶ��

    if(EepromPram_ReadPram(E2_GPS_SET_OUTPUT_RATE_ID,&tmp))
    {
        if(tmp<5)//&&tmp>0)
        {
					#ifdef GPS_MODE_TYPE_TD3017
            strcpy(buf,p_DatFreCmd[tmp]);
            Gps_SendData((u8*)buf,strlen(buf));
            Delay_nop(600*1000);//ytl 2013 06 05 
					#endif
            
#ifdef GPS_MODE_TYPE_ATGM332D   //fanqinghai 216.03.23 �п�΢���ӳ���
            Delay_nop(600*1000);
            memset(buf,0,sizeof(buf));
            strcpy(buf,"$PCAS02,");
            if(!tmp)
            {
                Check_cmd_String(buf,0x500);  //���500
            }
            else
            {
                Check_cmd_String(buf,0x1000*tmp);  
            }
            Gps_SendData((u8*)buf, strlen(buf));//��������ϵͳ����
            //��ʱ
            Delay_nop(600*1000);
#endif
        }
    }
    //����������ʽ---�˲��ֲ�ȷ������ʱ����
    if(EepromPram_ReadPram(E2_GPS_SET_MODE_ID,&tmp))
    {
        if(1<=tmp&&tmp<=3)//��GPSģʽ
        {
					#ifdef GPS_MODE_TYPE_TD3017
            strcpy(buf,p_startupCmd[tmp-1]);
            Gps_SendData((u8*)buf,strlen(buf));
					#endif
            
#ifdef GPS_MODE_TYPE_ATGM332D   //fanqinghai 216.03.23 �п�΢���ӳ���
            Delay_nop(600*1000);
            memset(buf,0,sizeof(buf));
                        strcpy(buf,"$PCAS04,");
            Check_cmd_String(buf,tmp);
            Gps_SendData((u8*)buf, strlen(buf));//��������ϵͳ����
            //��ʱ
            Delay_nop(600*1000);
#endif
        }
    }
    /////////////////
    Gnss_UpdataUploadParam(); 
    ///////////////////
    //Gnss_CheckBlindArea();
    /////////////////////////
    LZM_PublicSetOnceTimer(&s_stGnssTimer[GNSS_TIMER_TASK],PUBLICSECS(0.1),Gnss_CheckBlindAreaStart);
    ///////////////////////////////////
}
/****************************************************************************/
/*function  name:           void InitConfigGNSS(void)       */
/*input parameter:          none  */
/*output parameter:         none */
/*FUNCTION:                         ����Ҫ���ʼ��GNSSģ��
*/
/*****************************************************************************/     
void InitConfigGNSS(void)
{
    LZM_PublicKillTimerAll(s_stGnssTimer,GNSS_TIMERS_MAX); 
    //////////////////////////////
    s_stGnssAttrib.datalen      = 0;
    s_stGnssAttrib.recvdatalen  = 0;    
    s_stGnssAttrib.recvDataflag = 0;
    s_stGnssParam.CheckBlindSector = 0;
    s_stGnssParam.CheckBlindFinish = 0;
    ////////////////////////////
    memset(&s_stGnssParam,0,sizeof(s_stGnssParam));
    //////////////////////////////
    SetTimerTask(TIME_GNSS_REAL,LZM_TIME_BASE);
    //////////////////////////////////////////////////////////
    LZM_PublicSetOnceTimer(&s_stGnssTimer[GNSS_TIMER_TASK],PUBLICSECS(2),Gnss_ReadParam);
}
/*************************************************************
** ��������: Gnss_CheckReckData
** ��������: �����жϽ�������
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Gnss_CheckReckData(void)
{
    s_stGnssAttrib.nullDataCnt++;    
    if(s_stGnssAttrib.nullDataCnt>=3)//>=150msû���յ�����
    {
        if(s_stGnssAttrib.nullDataCnt>SYSTICK_10SECOND)//10s��û���յ�����
        {
            s_stGnssAttrib.datalen = 0;
        }
        if(s_stGnssAttrib.recvDataflag)
        {
            s_stGnssAttrib.recvDataflag = 0;
            if(s_stGnssParam.freqUpFlag)
            {
                s_stGnssParam.freqUpFlag = 0;
                s_stGnssParam.updataFlag = 1;
                s_stGnssAttrib.datalen   = s_stGnssAttrib.recvdatalen;                         
                memcpy(s_stGnssAttrib.parseBuff,s_stGnssAttrib.recvBuff,s_stGnssAttrib.datalen);
                //Gnss_ShowDebugInfo("updata");
            }
            s_stGnssAttrib.recvdatalen  = 0;
        }
    }
}
/*************************************************************
** ��������: GnssRealData_TimeTask
** ��������: ʵʱ�������ú����� 50ms����һ��
** ��ڲ���: 
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
FunctionalState GnssRealData_TimeTask(void)
{
    Gnss_CheckReckData();
    //////////////////////
    LZM_PublicTimerHandler(s_stGnssTimer,GNSS_TIMERS_MAX);
    return ENABLE;       
}
/*********************************************************************
//��������  :uchar CmpConstantPar(u32 source,u32 obj,u32 const_num)
//����      ://�ڲ��л�����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/      
uchar CmpConstantPar(u32 source,u32 obj,u32 const_num)
{
    if(source>=obj)
    {
        if(source-obj>=const_num)
            return true;
        else
            return false;
    }
    else
    {
        if(obj-source>=const_num)
            return true;
        else
            return false;
    }
}
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

