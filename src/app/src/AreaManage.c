/*******************************************************************************
 * File Name:           AreaManage.c 
 * Function Describe:   
 * Relate Module:       ������
 * Writer:              Joneming
 * Date:                2012-09-17
 * ReWriter:            Joneming
 * Date:                2013-05-21
 ******* ��ӿ��ű������ܡ��ر�ͨѶģ�鹦�ܡ��ɼ�GNSS��ϸ��λ���ݹ��ܣ�
 ******* �޸Ľ��������������ʾ����λ��:��ԭ����λ14��15�޸�Ϊ:12��13
 *******************************************************************************/
#include "include.h"
#include "VDR_Speed.h"
#include "VDR_Overspeed.h"
////////////////////////////
///////////////////////////////////
#define IN_OUT_AREA_OVERHEAD_INFO_ID              0x12 //�������򱨾�
#define IN_OUT_AREA_OVERHEAD_INFO_LEN             0x06;//�������򱨾�������Ϣ����
///////////////////////////////////
///////////////////////////
//#define TEST_USE_BIG_AREA
#define TEST_USE_SMALL_AREA
//�ṹ
typedef struct
{
    unsigned long LeftUpLat;        //���ϵ�γ��
    unsigned long LeftUpLong;       //���ϵ㾭��
    unsigned long RightDownLat;     //���µ�γ��
    unsigned long RightDownLong;    //���µ㾭��
    unsigned char AreaFlag;         //�����־
    unsigned char CheckFlag;        //����־
}ST_TEST_AREA;

#ifdef TEST_USE_BIG_AREA
static ST_TEST_AREA s_stTestBigArea;
#endif
#ifdef TEST_USE_SMALL_AREA
static ST_TEST_AREA s_stTestSmallArea;
#endif

#define TEST_AREA_EXTRA_ADD_LATITUDE  600//��λΪ0.0001��:600*0.185m =111γ��
#define TEST_AREA_EXTRA_ADD_LONGITUDE 700//��λΪ0.0001��:700*0.15m ����


#define MAX_AREA_ADDR           0xFFFFFFFF

#define MAX_RECT_AREA_SUM       100  //������������
#define MAX_ROUND_AREA_SUM      100  //���Բ��������
/*
#ifndef FLASH_ONE_SECTOR_BYTES
#define FLASH_ONE_SECTOR_BYTES  ONE_SECTOR_BYTES
#endif

#ifndef FLASH_RECTANGLE_AREA_START_SECTOR
#define FLASH_RECTANGLE_AREA_START_SECTOR   (AREA_START_SECTOR)
#endif
#ifndef FLASH_RECTANGLE_AREA_END_SECTOR
#define FLASH_RECTANGLE_AREA_END_SECTOR     (AREA_START_SECTOR+4)
#endif

#ifndef FLASH_ROUND_AREA_START_SECTOR
#define FLASH_ROUND_AREA_START_SECTOR         (AREA_START_SECTOR+5)
#endif
#ifndef FLASH_ROUND_AREA_END_SECTOR
#define FLASH_ROUND_AREA_END_SECTOR     AREA_END_SECTOR
#endif
*/
//////////////////////////////////////
#define RECT_AREA_ADDR_START  (unsigned long)(FLASH_RECTANGLE_AREA_START_SECTOR*FLASH_ONE_SECTOR_BYTES) //���������ŵ�,������Ҫ��0��ʼ....
#define RECT_AREA_ADDR_END  (unsigned long)((FLASH_RECTANGLE_AREA_END_SECTOR)*FLASH_ONE_SECTOR_BYTES)

#define ROUND_AREA_ADDR_START   (unsigned long)(FLASH_ROUND_AREA_START_SECTOR*FLASH_ONE_SECTOR_BYTES) //Բ�������ŵ�,������Ҫ��0��ʼ....
#define ROUND_AREA_ADDR_END  (unsigned long)(FLASH_ROUND_AREA_END_SECTOR*FLASH_ONE_SECTOR_BYTES) 


// 0,����  1:��Բ���� 2,���������� 3:��Բ���� 4:��������
typedef enum{ALARM_ITEM_SPEED_AREA=0,ALARM_ITEM_IN_ROUND_AREA,ALARM_ITEM_IN_RECT_AREA,ALARM_ITEM_OUT_ROUND_AREA,ALARM_ITEM_OUT_RECT_AREA} E_AREA_ALARM_ITEM;


#define VOICE_BYTE 50


//�����������Խṹ  156�ֽ�
typedef struct
{
    unsigned long Head;             //ͷ��־
    unsigned long AreaID;           //����ID
    unsigned long LeftUpLat;        //���ϵ�γ��
    unsigned long LeftUpLong;       //���ϵ㾭��
    unsigned long RightDownLat;     //���µ�γ��
    unsigned long RightDownLong;    //���µ㾭��
    unsigned short AreaAttrib;      //��������
    unsigned short LimitSpeed;      //����ֵ    
    unsigned char  StartMoment[6];  //��ʼʱ��
    unsigned char  EndMoment[6];    //����ʱ��    
    unsigned char  speedDuration;   //����ʱ��
    unsigned char  Reserve[11];      //�����ֽ�
    unsigned short UserAreaAttrib;   //�Զ�����������
    unsigned char  InVoiceSize;
    unsigned char  OutVoiceSize;
    unsigned char  InVoiceStr[VOICE_BYTE];
    unsigned char  OutVoiceStr[VOICE_BYTE];                   
}STRECT_AREA_ATTRIB;

#define STRECT_AREA_ATTRIB_SIZE sizeof(STRECT_AREA_ATTRIB)

//Բ���������Խṹ  152�ֽ�
typedef struct
{
    unsigned long Head;             //ͷ��־
    unsigned long AreaID;           //����ID   
    unsigned short AreaAttrib;      //��������
    unsigned short LimitSpeed;      //����ֵ
    unsigned long CenterLat;        //����γ��
    unsigned long CenterLong;       //���ľ���
    unsigned long Radius;           //�뾶
    unsigned char  StartMoment[6];  //��ʼʱ��
    unsigned char  EndMoment[6];    //����ʱ��    
    unsigned char  speedDuration;   //����ʱ��
    unsigned char  Reserve[11];      //�����ֽ�
    ////////////////////////////////////
    unsigned short UserAreaAttrib;   //�Զ�����������    
    unsigned char  InVoiceSize;
    unsigned char  OutVoiceSize;    
    unsigned char  InVoiceStr[VOICE_BYTE];
    unsigned char  OutVoiceStr[VOICE_BYTE];
}STROUND_AREA_ATTRIB;

#define STROUND_AREA_ATTRIB_SIZE sizeof(STROUND_AREA_ATTRIB)

#define HEAD_FLAG               0xABCDDCBA
#define AREA_HEAD_SIZE          8//��ȡͷ��־������ID
#define AREA_ATTRIB_SIZE        56//��ȡ���������ֵ����в���

//�ṹ
typedef struct
{
    unsigned long Latitude;                     //γ����������
    unsigned long Longitude;                    //������������
    unsigned long InOutAreaID;                  //��������ID
    unsigned char CheckRectNum;                  //������������
    unsigned char CheckRoundNum;                 //Բ����������
    unsigned char ValidRectNum;                 //����������Ч����
    unsigned char ValidRoundNum;                //Բ��������Ч����    
    unsigned char INOUTPostType;                //������������    
    unsigned char Orientation;                  //������
    unsigned char InOutAreaAlarm;               //�������򱨾�
    unsigned char preSecond;                    //��ǰ����
    unsigned char preSpeed;                     //ǰһ����ٶ�
    unsigned char GpsSpeed;                     //
    unsigned char nonavigationCnt;              //������������
    unsigned char OpenDoorAlarmFlag;            //�����ڷǷ������ű�����־
    unsigned char CollectGnssFlag;              //����ɼ�GNSS��־
    unsigned char CloseGPRSFlag;                //����ر�ͨѶģ���־
    unsigned char InAreaFlag;                   //�������ڱ�־
}ST_AREA_ATTRIB;

static ST_AREA_ATTRIB s_stAreaAttrib;

//������ƿ�ṹ
typedef struct
{
    unsigned long addr;             //��ַ//��ΪMAX_AREA_NUMʱ,��ʾû�н����κ����� 
    unsigned long startTime;        //��ʼʱ��
    unsigned long endTime;          //����ʱ��
    unsigned long AreaID;           //����ID 
    unsigned short AreaAttrib;      //��������
    unsigned short UserAreaAttrib;  //�Զ�����������
    unsigned short LimitSpeed;      //����ֵ
    unsigned char speedDuration;    //����ʱ��
    unsigned char AreaCnt;          //���������
    unsigned char preAreaCnt;       //���������
    unsigned char AreaFlag;         //�����־
    unsigned char timeType;         //ʱ������
    unsigned char protoclType;      //Э������,��������ʱʹ��
    unsigned char VoiceStr[VOICE_BYTE];//�����������
}STAREA_PCB;
///////////////////////
enum 
{
    AREA_TIMER_TASK,                        //
    AREA_TIMER_SCAN_ROUND,                  //    
    AREA_TIMER_SCAN_RECT,                   //     
    AREA_TIMER_TEST,                        //    
    AREA_TIMERS_MAX
}STAREATIME;

static LZM_TIMER s_stAREATimer[AREA_TIMERS_MAX];

static STAREA_PCB s_stRectArea;  //�������Ա���
static STAREA_PCB s_stRoundArea;  //Բ�����Ա���

extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ������ģʽ
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ

extern unsigned char s_ucMsgFlashBufferer[];
///////////////////////////////////////////
unsigned char Area_IsInRectArea(STRECT_AREA_ATTRIB *Ptr) ; //�жϵ�ǰ������Ƿ���Ptr��ָ�ľ���������
unsigned char Area_IsInRoundArea(STROUND_AREA_ATTRIB *Ptr) ; //�жϵ�ǰ������Ƿ���Ptr��ָ��Բ��������
unsigned char Area_RoundComparePos(T_GPS_POS *CenterPos,T_GPS_POS *CurPos,unsigned long Radius); //�жϵ��Ƿ���Բ��������
void Area_ScanRoundArea(void) ;//ɨ��Բ������
void Area_ScanRectArea(void) ;//ɨ���������
void Area_ChoiceAreaArmDest(unsigned char AlarmItem); //����Χ�ڵı�������
void AreaManage_ScanTask(void);
void Area_CloseCollectGNSSDetailLocatorData(void) ;
//////////////////////////////

#if 0//���㷨ʹ��
//-------------------------------------------------
//���빫ʽ,����������
//-------------------------------------------------
const unsigned int COS_PARA[]={
1000,998,996,990,980,
970,956,941,924,904,
885,864,835,815,786,
748,720,693,657,623,
589,556,525,486,449,
414,380,347,316,281,
253,221,192,169,143,
120,98,  85, 62,43,
30, 19,  11, 5, 1,0
};
#define AREA_LENGTH (unsigned long)(111)  //111319m  //ÿһ�ȶ�Ӧ�ĳ��ȵ�λ:Km   111000��GoogleEarch����ֵ
#define AREA_MAX(X,Y)  ((X)>=(Y)? (X):(Y)) //�ҳ����ֵ
#define AREA_MIN(X,Y)  ((X)<(Y)? (X):(Y))  //�ҳ���Сֵ

//-----------------------------------------------
//����:����㵽�����
//����:ָ��������λ��ṹ��ָ��
//���:��
//��ע:
//-----------------------------------------------
unsigned long CalDotDotMile(T_GPS_POS *gpsdataSrc, T_GPS_POS *gpsdataDest)
{
    unsigned long long lon,lat,result,lontemp,lattemp; //ʹ��64bit������
    unsigned char LatIndex;

    lat = gpsdataSrc->Lat;
    lattemp = gpsdataDest->Lat;

    lon = gpsdataSrc->Long;
    lontemp = gpsdataDest->Long;

    lat= AREA_MAX(lat,lattemp)-AREA_MIN(lat,lattemp);
    lon = AREA_MAX(lon,lontemp)-AREA_MIN(lon,lontemp);
    LatIndex=(lattemp/1000000UL)>>1;

    lon=lon*lon;    
    lon=lon*COS_PARA[LatIndex]/1000; 

    lat=lat*lat;

    result = (long long)sqrt(lon + lat);
    result = result*AREA_LENGTH/1000; //��þ��뵥λ(��)    
    return  result;
}
/********************************************************************
* ���� : ����㵽�߾���
* ����: ָ��������λ��ṹ��ָ��
* ���: ��
* ȫ�ֱ���: 
*
* ��ע: (�޸ļ�¼���ݡ�ʱ��)
********************************************************************/
unsigned long CalDotLineMile(T_GPS_POS *gpsdataSrc, T_GPS_POS *gpsdataDest1,T_GPS_POS *gpsdataDest2)
{
    unsigned long long lon,lat,result,lontemp1,lattemp1,lontemp2,lattemp2;
    unsigned char LatIndex;
    long long lonx,laty,lontempx1,lattempy1,lontempx2,lattempy2;

    lat = gpsdataSrc->Lat;
    lattemp1 = gpsdataDest1->Lat;
    lattemp2 = gpsdataDest2->Lat;   
    lon = gpsdataSrc->Long;
    lontemp1 = gpsdataDest1->Long;
    lontemp2 = gpsdataDest2->Long;
    laty=lat;
    lattempy1=lattemp1;
    lattempy2=lattemp2;
    lonx=lon;
    lontempx1=lontemp1;
    lontempx2=lontemp2;

    //���㹫ʽ��ĸ����
    LatIndex=(lat/1000000UL)>>1;
    lat = AREA_MAX(lattemp1,lattemp2)-AREA_MIN(lattemp1,lattemp2);
    lon = AREA_MAX(lontemp1,lontemp2)-AREA_MIN(lontemp1,lontemp2);

    lon=lon*lon;    
    lon=lon*COS_PARA[LatIndex]/1000UL; 

    lat=lat*lat;

    result = (long long)sqrt(lon + lat);
    //������Ӳ���
    lonx=(lattempy2-lattempy1)*(lonx-lontempx1)-(lontempx2-lontempx1)*(laty-lattempy1);
    if(lonx<0)
        lonx = -lonx;
    lon=lonx;
    result=lon/result*AREA_LENGTH/1000*COS_PARA[LatIndex]/1000;  //��þ��뵥λ(��) 
    return  result;
}
#else
//-----------------------------------------------
//����:����㵽�����
//����:ָ��������λ��ṹ��ָ��
//���:��
//��ע:
//-----------------------------------------------
unsigned long CalDotDotMile(T_GPS_POS *gpsdataSrc, T_GPS_POS *gpsdataDest)
{
    return (unsigned long)Route_GetPointDistance(gpsdataSrc,gpsdataDest);
}
#endif
/*************************************************************
** ��������: Area_GetCurSpeed
** ��������: ȡ�õ�ǰ�ٶ�
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: SpeedMonitor_GetCurSpeed
*************************************************************/
unsigned char Area_GetCurSpeed(void)
{
    unsigned char speed = 0,GpsSpeed = 0;
	  speed = VDRSpeed_GetCurSpeed();
    GpsSpeed = Gps_ReadSpeed();
    if(0==speed&&GpsSpeed>5)speed = GpsSpeed;
    return speed;
}
/*************************************************************
** ��������: Area_TestRectAreaParameterInitialize
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
void Area_TestRectAreaParameterInitialize(void)
{
    #ifdef TEST_USE_BIG_AREA
    s_stTestBigArea.AreaFlag = AREA_STATUS_MAX;
    s_stTestBigArea.CheckFlag = 0;
    #endif
    #ifdef TEST_USE_SMALL_AREA
    s_stTestSmallArea.AreaFlag = AREA_STATUS_MAX;
    s_stTestSmallArea.CheckFlag = 0;
    #endif
}
/*************************************************************
** ��������: Area_SetTestRectAreaParameter
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
void Area_SetTestRectAreaParameter(T_GPS_POS *LeftUpPos,T_GPS_POS *RightDownPos)
{
    #ifdef TEST_USE_BIG_AREA
    if(0==s_stTestBigArea.CheckFlag)
    {
        //////������///////////////////
        s_stTestBigArea.AreaFlag = AREA_STATUS_MAX;
        s_stTestBigArea.CheckFlag = 1;
        ///////���ϵ�////////////////////
        s_stTestBigArea.LeftUpLat =LeftUpPos->Lat+TEST_AREA_EXTRA_ADD_LATITUDE;//γ����������
        s_stTestBigArea.LeftUpLong =LeftUpPos->Long-TEST_AREA_EXTRA_ADD_LONGITUDE;//������������
        ////////���µ�///////////
        s_stTestBigArea.RightDownLat =RightDownPos->Lat-TEST_AREA_EXTRA_ADD_LATITUDE;//γ����������
        s_stTestBigArea.RightDownLong =RightDownPos->Long+TEST_AREA_EXTRA_ADD_LONGITUDE;//������������
        //////////////////////////////////
    }
    #endif
    /////////////////////////
    #ifdef TEST_USE_SMALL_AREA
    if(0 == s_stTestSmallArea.CheckFlag)
    {
        //////С����///////////////////
        s_stTestSmallArea.AreaFlag = AREA_STATUS_MAX;
        s_stTestSmallArea.CheckFlag = 1;
         ///////���ϵ�////////////////////
        s_stTestSmallArea.LeftUpLat =LeftUpPos->Lat-TEST_AREA_EXTRA_ADD_LATITUDE;//γ����������
        s_stTestSmallArea.LeftUpLong =LeftUpPos->Long+TEST_AREA_EXTRA_ADD_LONGITUDE;//������������
        ////////���µ�///////////
        s_stTestSmallArea.RightDownLat =RightDownPos->Lat+TEST_AREA_EXTRA_ADD_LATITUDE;//γ����������
        s_stTestSmallArea.RightDownLong =RightDownPos->Long-TEST_AREA_EXTRA_ADD_LONGITUDE;//������������
    }
    #endif
}
#ifdef TEST_USE_BIG_AREA
/*************************************************************
** ��������: Area_ScanTestRectArea
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
void Area_ScanTestBigRectArea(void)
{
    unsigned char Status;
    if(0==s_stTestBigArea.CheckFlag)return;    
    if(AREA_STATUS_IN != s_stTestBigArea.AreaFlag)//����������
    {
        Status = ComparePos( (T_GPS_POS *)&(s_stTestBigArea.LeftUpLat), (T_GPS_POS *)&(s_stTestBigArea.RightDownLat),(T_GPS_POS *)&s_stAreaAttrib.Latitude);    
        if(0 == Status)//0:��������
        {
            s_stTestBigArea.AreaFlag = AREA_STATUS_IN;
//            Report_ClearReportTimeCount(); dxl,2016.5.23
//            ClrTimerTask(TIME_POSITION);//���������λ�û㱨
            ///////////////////////
					  Area_OpenCollectGNSS();//dxl,2016.5.23����
        }   
    }
    else
    if(AREA_STATUS_IN == s_stTestBigArea.AreaFlag)//����������
    {
        Status = ComparePos( (T_GPS_POS *)&(s_stTestBigArea.LeftUpLat), (T_GPS_POS *)&(s_stTestBigArea.RightDownLat),(T_GPS_POS *)&s_stAreaAttrib.Latitude);    
        if(1 == Status)//1:����������
        {
            s_stTestBigArea.AreaFlag = AREA_STATUS_OUT;
            /////////////////////////
            //Report_ClearReportTimeCount();dxl,2016.5.23
            //SetTimerTask(TIME_POSITION,1);//��������λ�û㱨
            ///////////////////////
					  //Area_CloseCollectGNSS();//dxl,2016.5.23����,������رգ�����ʱ0.9�룬Ȼ���ٹرգ��Ա������1��GNSS�����ϴ������
					  //��������
            DelayAckCommand = 15;
            //������ʱ����
            SetTimerTask(TIME_DELAY_TRIG, 9*SYSTICK_0p1SECOND);
        }
    }
}
#endif
/*************************************************************
** ��������: Area_ScanTestRectArea
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
#ifdef TEST_USE_SMALL_AREA
void Area_ScanTestSmallRectArea(void)
{
    unsigned char Status;
    if(0==s_stTestSmallArea.CheckFlag)return;    
    if(AREA_STATUS_IN != s_stTestSmallArea.AreaFlag)//����������
    {
        Status = ComparePos( (T_GPS_POS *)&(s_stTestSmallArea.LeftUpLat), (T_GPS_POS *)&(s_stTestSmallArea.RightDownLat),(T_GPS_POS *)&s_stAreaAttrib.Latitude);    
        if(0 == Status)//0:��������
        {
            s_stTestSmallArea.AreaFlag = AREA_STATUS_IN;
            //��С������GNSS�ɼ�
        }   
    }
    else
    if(AREA_STATUS_IN == s_stTestSmallArea.AreaFlag)//����������
    {
        Status = ComparePos( (T_GPS_POS *)&(s_stTestSmallArea.LeftUpLat), (T_GPS_POS *)&(s_stTestSmallArea.RightDownLat),(T_GPS_POS *)&s_stAreaAttrib.Latitude);    
        if(1 == Status)//1:����������
        {
            s_stTestSmallArea.AreaFlag = AREA_STATUS_OUT;
            //��С�����GNSS�ɼ�
            ////////////////////////////
            //Report_ClearReportTimeCount(); dxl,2016.5.23����
            //Area_CloseCollectGNSSDetailLocatorData();
            /////////////////////
        }
    }
}
#endif
/*************************************************************
** ��������: Area_ScanTestRectArea
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
void Area_ScanTestRectArea(void)
{
    #ifdef TEST_USE_BIG_AREA
    Area_ScanTestBigRectArea();
    #endif
    #ifdef TEST_USE_SMALL_AREA
    Area_ScanTestSmallRectArea();
    #endif
}
/*************************************************************
** ��������: Area_GetConverseBCDTimeType
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Area_GetConverseBCDTimeType(unsigned char *buffer)
{
    if(0==buffer[0]&& 0==buffer[1]&& 0==buffer[2])
    {
        return 1;
    }

    return 0;
}
//-----------------------------------------------
//����:BCD��ʽʱ��,����HEX
//����:1,ָ��BCD��ʽʱ�̵�ָ�룬2������(0:ȫ�� 1:��ת��ʱ����)
//���:
//��ע:
//-----------------------------------------------
unsigned long Area_ConverseBcdGmtime(unsigned char *Ptr, unsigned char Type)
{
    unsigned long Temp;
    if(Type==0)
    {
        TIME_T stTime;
        if(Public_ConvertBCDToTime(&stTime,Ptr))
        {
            Temp = ConverseGmtime(&stTime);
        }
        else
        {
            RTC_GetCurTime(&stTime);
            Temp = Public_BCD2HEX(*(Ptr+1)); //�����
            if(Temp)stTime.month = Temp;
            Temp = Public_BCD2HEX(*(Ptr+2)); //�����
            if(Temp)stTime.day=Temp;
            stTime.hour = Public_BCD2HEX(*(Ptr+3)); //���ʱ
            stTime.min = Public_BCD2HEX(*(Ptr+4)); //��÷�
            stTime.sec = Public_BCD2HEX(*(Ptr+5)); //�����
            Temp = ConverseGmtime(&stTime);
        }
    }
    else
    {
        Temp = 0; 
        Temp = Temp*24 + Public_BCD2HEX(*(Ptr+3)); //���ʱ
        Temp = Temp*60 + Public_BCD2HEX(*(Ptr+4)); //��÷�
        Temp = Temp*60 + Public_BCD2HEX(*(Ptr+5)); //�����
    }
    return Temp;
}

//-----------------------------------------------
//����:BCD��ʽʱ��,����HEX
//����:1,ָ��BCD��ʽʱ�̵�ָ�룬2������(0:ȫ�� 1:��ת��ʱ����)
//���:
//��ע:
//-----------------------------------------------
unsigned long Area_GetConvertCurTime(unsigned char Type)
{
    unsigned long Temp;    
    if(Type==0)
    {
        Temp =RTC_GetCounter();
    }
    else
    {
        TIME_T stTime;        
        Temp = 0;
        RTC_GetCurTime(&stTime);
        Temp = Temp*24 + stTime.hour; //���ʱ
        Temp = Temp*60 + stTime.min; //��÷�
        Temp = Temp*60 + stTime.sec; //�����
    }
    return Temp;
}
/*************************************************************OK
** ��������: Area_MdyDataInFlash
** ��������: д�����ַ��Flash�������
** ��ڲ���: DataPtr�����׵�ַ,addrΪFlash��ַ,lenΪ���ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_MdyDataInFlash(unsigned char *DataPtr,unsigned long addr,unsigned short len)
{
    unsigned char buffer[100];
    unsigned long blockAddr,i,tempLen,tempAddr,datalen; //��4Kȡ����ַ    
    tempLen = 0;
    datalen = len;
    blockAddr = addr & 0xFFFFF000; //ȡ���ݿ�ʼ��������ַ
    sFLASH_ReadBuffer(s_ucMsgFlashBuffer,blockAddr,FLASH_ONE_SECTOR_BYTES); //������ 
    tempAddr = addr%FLASH_ONE_SECTOR_BYTES;     //����ƫ�Ƶ�ַ
    tempLen = FLASH_ONE_SECTOR_BYTES-tempAddr;  //�ڱ����л��ܱ�������ݳ���
    if(tempLen>datalen)tempLen = datalen;//��ͬһ������
    for(i=0; i<tempLen; i++)//
    {
        s_ucMsgFlashBuffer[tempAddr+i] = *DataPtr++;    
    }
    IWDG_ReloadCounter();
    sFLASH_EraseSector(blockAddr);          //��������
    sFLASH_WriteBuffer(s_ucMsgFlashBuffer,blockAddr,FLASH_ONE_SECTOR_BYTES);//д����
    //////У��/////////////////
    spi_Delay_uS(100);
    sFLASH_ReadBuffer(buffer,blockAddr,100); //������ 
    if(Public_CheckArrayValIsEqual(buffer,s_ucMsgFlashBuffer,100))
    {
        IWDG_ReloadCounter();
        sFLASH_EraseSector(blockAddr);          //��������
        sFLASH_WriteBuffer(s_ucMsgFlashBuffer,blockAddr,FLASH_ONE_SECTOR_BYTES);//д����
    }
    datalen -=tempLen;        
    while(datalen>0)//��������ûд��////////////////
    {
        blockAddr += FLASH_ONE_SECTOR_BYTES;//ȡ��һ������ַ
        sFLASH_ReadBuffer(s_ucMsgFlashBuffer,blockAddr,FLASH_ONE_SECTOR_BYTES); //����������
        tempLen = datalen;
        if(tempLen>FLASH_ONE_SECTOR_BYTES)tempLen = FLASH_ONE_SECTOR_BYTES;
        for(i=0; i<tempLen; i++)
        {
            s_ucMsgFlashBuffer[i] = *DataPtr++;    
        }
        IWDG_ReloadCounter();       
        sFLASH_EraseSector(blockAddr);          //��������
        sFLASH_WriteBuffer(s_ucMsgFlashBuffer,blockAddr,FLASH_ONE_SECTOR_BYTES);//д����
        //////У��/////////////////////
        spi_Delay_uS(100);
        sFLASH_ReadBuffer(buffer,blockAddr,100); //������ 
        if(Public_CheckArrayValIsEqual(buffer,s_ucMsgFlashBuffer,100))
        {
            IWDG_ReloadCounter();
            sFLASH_EraseSector(blockAddr);          //��������
            sFLASH_WriteBuffer(s_ucMsgFlashBuffer,blockAddr,FLASH_ONE_SECTOR_BYTES);//д����
        }
        datalen -= tempLen;
    }
}
/*************************************************************OK
** ��������: Area_ClearSpeedAreaBit
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_ClearSpeedAreaBit(unsigned char status)
{

    if(E_RECT_AREA == status)
    {
        //SpeedMonitor_ClearSpeedParamValid(E_SPEED_TYPE_RECT_AREA);
			  VDROverspeed_ResetAreaParameter(E_RECT_AREA);
			  
    }
    else
    if( E_ROUND_AREA == status)
    {
        //SpeedMonitor_ClearSpeedParamValid(E_SPEED_TYPE_ROUND_AREA);
			  VDROverspeed_ResetAreaParameter(E_ROUND_AREA);
    }
    else
    if( E_POLYGON_AREA == status)
    {
        //SpeedMonitor_ClearSpeedParamValid(E_SPEED_TYPE_POLYGON_AREA);
			  VDROverspeed_ResetAreaParameter(E_POLYGON_AREA);
    }
	
} 
/************************************************************
** ��������: Area_CloseGSMModule
** ��������: �ر�GSMģ��
** ��ڲ���: 
** ���ڲ���:
************************************************************/ 
void Area_CloseGSMModule(void) 
{
    #ifdef __GNSS_APP__
    Communication_Close();
	  if(1 == BBGNTestFlag)//dxl,2016.5.13
		{
	      Blind_EnableLink1Save();
		}
    #endif
}
/************************************************************
** ��������: Area_OpenGSMModule
** ��������: ����GSMģ��
** ��ڲ���: 
** ���ڲ���:
************************************************************/ 
void Area_OpenGSMModule(void) 
{
    #ifdef __GNSS_APP__
    Communication_Open();
	  if(1 == BBGNTestFlag)//dxl,2016.5.13
		{
	      Blind_DisableLink1Save();
		}
    #endif
}
/************************************************************
** ��������: Area_CloseCollectGNSSDetailLocatorData
** ��������: �رղɼ�GNSS��ϸ��λ����
** ��ڲ���: 
** ���ڲ���:
************************************************************/ 
void Area_CloseCollectGNSSDetailLocatorData(void) 
{
    #ifdef __GNSS_APP__
    DIS_GnssArea();
    #endif
}
/************************************************************
** ��������: Area_OpenCollectGNSSDetailLocatorData
** ��������: �����ɼ�GNSS��ϸ��λ����
** ��ڲ���: 
** ���ڲ���:
************************************************************/ 
void Area_OpenCollectGNSSDetailLocatorData(void) 
{
    #ifdef __GNSS_APP__
    EN_GnssArea();
    #endif
}
/************************************************************
** ��������: Area_OutGNSSAreaOpenPosition
** ��������: 
** ��ڲ���: 
** ���ڲ���:
************************************************************/
void Area_OutGNSSAreaOpenPosition(void) 
{
    //�б�����
    //Report_ClearReportTimeCount();
    //SetTimerTask(TIME_POSITION,1);
}
/************************************************************
** ��������: Area_OutGNSSAreaOpenPosition
** ��������: 
** ��ڲ���: 
** ���ڲ���:
************************************************************/
void Area_CloseCollectGNSS(void) 
{
    Area_CloseCollectGNSSDetailLocatorData();
//    LZM_PublicSetOnceTimer(&s_stAREATimer[AREA_TIMER_TEST],PUBLICSECS(4),Area_OutGNSSAreaOpenPosition); dxl,2016.5.23����
}
/************************************************************
** ��������: Area_OpenCollectGNSS
** ��������: 
** ��ڲ���: 
** ���ڲ���:
************************************************************/
void Area_OpenCollectGNSS(void) 
{
	Area_OpenCollectGNSSDetailLocatorData();//dxl,2016.5.23����
//    LZM_PublicSetOnceTimer(&s_stAREATimer[AREA_TIMER_TEST],PUBLICSECS(4),Area_OpenCollectGNSSDetailLocatorData);  dxl,2016.5.23����  
}
/*************************************************************OK
** ��������: Area_GetCollectGnssFlag
** ��������: ��ȡ��ǰGnss�Ƿ���Ҫ�ɼ���־
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
unsigned char Area_GetCollectGnssFlag(void)
{
    return s_stAreaAttrib.CollectGnssFlag;
}
/*************************************************************OK
** ��������: Area_SetCollectGnssFlag
** ��������: ���õ�ǰGnss��Ҫ�ɼ���־
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_SetCollectGnssFlag(unsigned char type)
{
    if(type>E_ROUTE)return;
    if(PUBLIC_CHECKBIT(s_stAreaAttrib.CollectGnssFlag,type))return;
    if(0==s_stAreaAttrib.CollectGnssFlag)
    {
        Area_OpenCollectGNSS();
    }
    PUBLIC_SETBIT(s_stAreaAttrib.CollectGnssFlag,type);    
}
/*************************************************************OK
** ��������: Area_ClearCollectGnssFlag
** ��������: �����ǰGnss��Ҫ�ɼ���־
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_ClearCollectGnssFlag(unsigned char type)
{
    if(type>E_ROUTE)return;
    if(0==PUBLIC_CHECKBIT(s_stAreaAttrib.CollectGnssFlag,type))return;
    PUBLIC_CLRBIT(s_stAreaAttrib.CollectGnssFlag,type);
    if(0==s_stAreaAttrib.CollectGnssFlag)
    {
        Area_CloseCollectGNSS();
    }
}
/*************************************************************OK
** ��������: Area_GetCloseGPRSFlag
** ��������: ��ȡ��ǰ�ر�ͨѶģ���־
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
unsigned char Area_GetCloseGPRSFlag(void)
{
    return s_stAreaAttrib.CloseGPRSFlag;
}
/*************************************************************OK
** ��������: Area_SetCloseGPRSFlag
** ��������: ���õ�ǰ�ر�ͨѶģ���־
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_SetCloseGPRSFlag(unsigned char type)
{
    if(type>E_ROUTE)return;
    if(PUBLIC_CHECKBIT(s_stAreaAttrib.CloseGPRSFlag,type))return;
    if(0==s_stAreaAttrib.CloseGPRSFlag)
    {
        Area_CloseGSMModule();
    }
    PUBLIC_SETBIT(s_stAreaAttrib.CloseGPRSFlag,type);
}
/*************************************************************OK
** ��������: Area_ClearCloseGPRSFlag
** ��������: �����ǰ�ر�ͨѶģ���־
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_ClearCloseGPRSFlag(unsigned char type)
{
    if(type>E_ROUTE)return;
    if(0==PUBLIC_CHECKBIT(s_stAreaAttrib.CloseGPRSFlag,type))return;
    PUBLIC_CLRBIT(s_stAreaAttrib.CloseGPRSFlag,type);
    if(0==s_stAreaAttrib.CloseGPRSFlag)
    {
        Area_OpenGSMModule();
    }
}
/*************************************************************OK
** ��������: Area_GetCollectGnssFlag
** ��������: ��ȡ��ǰ�Ƿ��������ڱ�־
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
unsigned char Area_GetInAreaFlag(void)
{
    return s_stAreaAttrib.InAreaFlag;
}
/*************************************************************OK
** ��������: Area_SetCollectGnssFlag
** ��������: ���õ�ǰ�������ڱ�־
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_SetInAreaFlag(unsigned char type)
{
    if(type>E_ROUTE)return;
    if(PUBLIC_CHECKBIT(s_stAreaAttrib.InAreaFlag,type))return;
    PUBLIC_SETBIT(s_stAreaAttrib.InAreaFlag,type);
    if(!Area_GetCollectGnssFlag())
    {
        Area_CloseCollectGNSSDetailLocatorData();
    }
}
/*************************************************************OK
** ��������: Area_ClearCollectGnssFlag
** ��������: �����ǰ�������ڱ�־
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_ClearInAreaFlag(unsigned char type)
{
    if(type>E_ROUTE)return;
    if(0==PUBLIC_CHECKBIT(s_stAreaAttrib.InAreaFlag,type))return;
    PUBLIC_CLRBIT(s_stAreaAttrib.InAreaFlag,type);
}
/*************************************************************OK
** ��������: Area_SetOpenDoorAlarmBit
** ��������: ���ÿ��ű���λ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_SetOpenDoorAlarmBit(unsigned char type)
{
    if(type>E_ROUTE)return;
    if(PUBLIC_CHECKBIT(s_stAreaAttrib.OpenDoorAlarmFlag,type))return;
    PUBLIC_SETBIT(s_stAreaAttrib.OpenDoorAlarmFlag,type);
    #ifdef __GNSS_APP__
    Io_WriteAlarmBit(ALARM_BIT_LAWLESS_OPEN_DOOR,SET);
    #endif
}
/*************************************************************OK
** ��������: Area_ClearOpenDoorAlarmBit
** ��������: ������ű���λ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_ClearOpenDoorAlarmBit(unsigned char type)
{
    if(type>E_ROUTE)return;
    if(0==PUBLIC_CHECKBIT(s_stAreaAttrib.OpenDoorAlarmFlag,type))return;
    PUBLIC_CLRBIT(s_stAreaAttrib.OpenDoorAlarmFlag,type);
    ////////////////////////////
    if(0 == s_stAreaAttrib.OpenDoorAlarmFlag)
    {
        #ifdef __GNSS_APP__
        Io_WriteAlarmBit(ALARM_BIT_LAWLESS_OPEN_DOOR,RESET); 
        #endif
    }   
}
/*************************************************************OK
** ��������: Area_CheckOpenDoorAlarm
** ��������: ��鿪�ű���
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_CheckOpenDoorAlarm(unsigned char type)
{
    if(type>E_ROUTE)return;
    #ifdef __GNSS_APP__
    if(Io_ReadStatusBit(STATUS_BIT_DOOR1) || Io_ReadStatusBit(STATUS_BIT_DOOR2) || Io_ReadStatusBit(STATUS_BIT_DOOR3)
    || Io_ReadStatusBit(STATUS_BIT_DOOR4) || Io_ReadStatusBit(STATUS_BIT_DOOR5))
    {
        Area_SetOpenDoorAlarmBit(type);
    }
    else  //���Źرգ����������������־
    {
        Area_ClearOpenDoorAlarmBit(type);
    }
    #endif
}
/*************************************************************OK
** ��������: Area_GetInOutAreaAlarmBit
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
unsigned char Area_GetInOutAreaAlarmBit(unsigned char status)
{
    return PUBLIC_CHECKBIT(s_stAreaAttrib.InOutAreaAlarm,status);
}
/*************************************************************OK
** ��������: Area_SetSpeedAreaBit
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_SetInOutAreaAlarmBit(unsigned char status)
{

    PUBLIC_SETBIT(s_stAreaAttrib.InOutAreaAlarm,status);
    Io_WriteAlarmBit(ALARM_BIT_IN_OUT_AREA,SET);
    
}
/*************************************************************
** ��������: Area_SetInOutAreaAlarmInfo
** ��������: �������򱨾���Ϣ
** ��ڲ���: type:������������,AlarmID:��������ID,Orientation����
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_SetInOutAreaAlarmInfo(unsigned char type,unsigned long AlarmID,unsigned char Orientation)
{
    s_stAreaAttrib.InOutAreaAlarm = 0;//������������򱨾�,
    s_stAreaAttrib.INOUTPostType = type;
    s_stAreaAttrib.InOutAreaID = AlarmID;    
    s_stAreaAttrib.Orientation = Orientation;
    Area_SetInOutAreaAlarmBit(type);
}
/*************************************************************OK
** ��������: Area_ClearInOutAreaAlarmBit
** ��������: ������򱨾���Ϣ
** ��ڲ���: type��������
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_ClearInOutAreaAlarmBit(unsigned char type)
{
    PUBLIC_CLRBIT(s_stAreaAttrib.InOutAreaAlarm,type);
    if(0 == s_stAreaAttrib.InOutAreaAlarm)
    {
        Io_WriteAlarmBit(ALARM_BIT_IN_OUT_AREA,RESET);
    }   
}
/*************************************************************OK
** ��������: Area_ClearAllAlarm
** ��������: ������򱨾���Ϣ
** ��ڲ���: type��������
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Area_ClearAllAlarm(unsigned char type)
{
    if(type>E_ROUTE)return;
     Area_ClearInOutAreaAlarmBit(type);
    if(0==PUBLIC_CHECKBIT(s_stAreaAttrib.InAreaFlag,type))return;
    Area_ClearSpeedAreaBit(type);
    Area_ClearOpenDoorAlarmBit(type);
    Area_ClearCloseGPRSFlag(type);
    Area_ClearCollectGnssFlag(type);
    Area_ClearInAreaFlag(type);
}
/*************************************************************
** ��������: Area_RectAreaResetParam
** ��������: �����������ò���
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_RectAreaResetParam(void) 
{
    s_stAreaAttrib.preSpeed = 0xff;
    s_stRectArea.AreaFlag   = AREA_STATUS_MAX;
    s_stRectArea.AreaCnt    = 0;
    s_stRectArea.addr       = MAX_AREA_ADDR;  
    ///////////////////////////////
    Area_ClearAllAlarm(E_RECT_AREA);
    ///////////////////
    Area_TestRectAreaParameterInitialize();
}
/*************************************************************
** ��������: Area_RectAreaCheckForResetParam
** ��������: �����������Ƿ����ò���
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_RectAreaCheckForResetParam(unsigned long AreaID) 
{
    if(s_stRectArea.AreaFlag==AREA_STATUS_IN&&s_stRectArea.AreaID==AreaID)
    {
        Area_RectAreaResetParam();
    }
}
/*************************************************************
** ��������: Area_RectAreaParameterInitialize
** ��������: �������������ʼ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_RectAreaParameterInitialize(void) 
{
    Area_RectAreaResetParam();
    s_stAreaAttrib.CheckRectNum  = 0;
    s_stAreaAttrib.ValidRectNum = 0;
}
/*************************************************************
** ��������: Area_RoundAreaResetParam
** ��������: Բ���������ò���
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_RoundAreaResetParam(void) 
{
    s_stAreaAttrib.preSpeed = 0xff;
    s_stRoundArea.AreaFlag = AREA_STATUS_MAX;
    s_stRoundArea.AreaCnt = 0;
    s_stRoundArea.addr = MAX_AREA_ADDR;
    ////////////////////////////
    Area_ClearAllAlarm(E_ROUND_AREA);
}
/*************************************************************
** ��������: Area_RoundAreaCheckForResetParam
** ��������: Բ���������Ƿ����ò���
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_RoundAreaCheckForResetParam(unsigned long AreaID) 
{
    if(s_stRoundArea.AreaFlag==AREA_STATUS_IN&&s_stRoundArea.AreaID==AreaID)
    {
        Area_RoundAreaResetParam();
    }
}
/*************************************************************
** ��������: Area_RoundAreaParameterInitialize
** ��������: Բ�����������ʼ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_RoundAreaParameterInitialize(void) 
{
    Area_RoundAreaResetParam();
    //���Բ��������
    s_stAreaAttrib.CheckRoundNum  = 0;
    s_stAreaAttrib.ValidRoundNum = 0;
}

/*************************************************************
** ��������: Area_EraseFlash
** ��������: �������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_EraseFlash(unsigned short Start,unsigned short End)  //
{
    unsigned long addr;
    unsigned short i;
    addr =Start*FLASH_ONE_SECTOR_BYTES;
    for(i = Start; i< End; i++)
    {
        sFLASH_EraseSector(addr);
        addr += FLASH_ONE_SECTOR_BYTES;
        IWDG_ReloadCounter();//ι��
    }
}
/*************************************************************
** ��������: Area_ClearRectArea
** ��������: �����������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_ClearRectArea(void)
{
    Area_RectAreaParameterInitialize();
    Area_EraseFlash(FLASH_RECTANGLE_AREA_START_SECTOR,FLASH_RECTANGLE_AREA_END_SECTOR); //��������
}
/*************************************************************
** ��������: Area_ClearRoundArea
** ��������: ���Բ������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_ClearRoundArea(void)
{
    Area_RoundAreaParameterInitialize();
    Area_EraseFlash(FLASH_ROUND_AREA_START_SECTOR,FLASH_ROUND_AREA_END_SECTOR); //��������
}
/*************************************************************
** ��������: Area_EraseAllArea
** ��������: �������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_EraseAllArea(void)
{   
    Area_ClearRectArea();
    ///////////////
    Area_ClearRoundArea();
}

/*************************************************************
** ��������: Area_GetCurGpsCoordinate
** ��������: ��õ�ǰ����
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_GetCurGpsCoordinate(void)
{  
    unsigned char buffer[28];
    Public_GetCurBasicPositionInfo(buffer);
    Public_Mymemcpy((unsigned char *)&s_stAreaAttrib.Latitude,(unsigned char *)&buffer[8],4,1); 
    Public_Mymemcpy((unsigned char *)&s_stAreaAttrib.Longitude,(unsigned char *)&buffer[12],4,1); 
    /////////////////////  
}
//-----------------------------------------------
//����:�жϵ�ǰ������Ƿ���Ptr��ָ�ľ���������
//����:ָ�������ָ��
//���:0:��  1:����  0XFF:����
//��ע:
//-----------------------------------------------
unsigned char Area_IsInRectArea(STRECT_AREA_ATTRIB *Ptr)
{
    ///////////////////////////
    return( ComparePos( (T_GPS_POS *)&(Ptr->LeftUpLat), (T_GPS_POS *)&(Ptr->RightDownLat),(T_GPS_POS *)&s_stAreaAttrib.Latitude));
}

//-----------------------------------------------
//����:�жϵ�ǰ������Ƿ���Ptr��ָ��Բ��������
//����:ָ�������ָ��
//���:0:��  1:����  0XFF:����
//��ע:
//-----------------------------------------------
unsigned char Area_IsInRoundArea(STROUND_AREA_ATTRIB *Ptr)
{
    return( Area_RoundComparePos( (T_GPS_POS *)&(Ptr->CenterLat),(T_GPS_POS *)&s_stAreaAttrib.Latitude, Ptr->Radius) );
}
//-----------------------------------------------
//����:�жϵ��Ƿ��ھ���������
//����:3������
//���:0:��������  1:���� 0XFF:����
//��ע:
//-----------------------------------------------   
unsigned char ComparePos(T_GPS_POS *LeftUpPos,T_GPS_POS *RightDownPos, T_GPS_POS *CurPos) 
{
    T_GPS_POS Dot1,Dot2;
    unsigned long Temp;
    if(LeftUpPos->Lat == RightDownPos->Lat || LeftUpPos->Long == RightDownPos->Long)
        return 0XFF;
    Dot1 = *LeftUpPos;
    Dot2 = *RightDownPos;
    //Dot1�ľ�γ��<Dot2�ľ�γ��
    if(Dot1.Lat >Dot2.Lat)
    {
        Temp = Dot1.Lat;
        Dot1.Lat = Dot2.Lat;
        Dot2.Lat = Temp;
    }
    else if(Dot1.Long >Dot2.Long)
    {
        Temp = Dot1.Long;
        Dot1.Long = Dot2.Long;
        Dot2.Long = Temp;
    }
    if(CurPos->Lat < Dot2.Lat && CurPos->Lat > Dot1.Lat && CurPos->Long < Dot2.Long && CurPos->Long > Dot1.Long)
        return 0; 
    else
        return 1;
}   
//-----------------------------------------------
//����:�жϵ��Ƿ���Բ��������
//����:3������
//���:0:��������  1:���� 0XFF:����
//��ע:
//-----------------------------------------------   
unsigned char Area_RoundComparePos(T_GPS_POS *CenterPos,T_GPS_POS *CurPos,unsigned long Radius) 
{
    if(Route_GetPointDistance(CenterPos, CurPos)<=Radius)
    {
        return 0;   
    }
    else
    {
        return 1;   
    }
}
/*************************************************************
** ��������: Area_CheckRectTime
** ��������: �������Ƿ����ʱ�䷶Χ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 0:������ʱ�䷶Χ,1:����ʱ�䷶Χ
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Area_CheckRectTime(void)
{
    u32 curTime = 0;
    //�Ƿ���Ҫ���ʱ��
    if(!PUBLIC_CHECKBIT(s_stRectArea.AreaAttrib,E_AREA_MOMENT))return 1; //����Ҫ���ʱ��                  
    curTime=Area_GetConvertCurTime(s_stRectArea.timeType); //��ǰʱ��  
    return Public_CheckTimeIsInRange(curTime,s_stRectArea.startTime,s_stRectArea.endTime);    
}
/*************************************************************
** ��������: Area_CheckRoundTime
** ��������: ���Բ���Ƿ����ʱ�䷶Χ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 0:������ʱ�䷶Χ,1:����ʱ�䷶Χ
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Area_CheckRoundTime(void)
{
    u32 curTime = 0;
    //�Ƿ���Ҫ���ʱ��
    if(!PUBLIC_CHECKBIT(s_stRoundArea.AreaAttrib,E_AREA_MOMENT))return 1; //����Ҫ���ʱ��                  
    curTime=Area_GetConvertCurTime(s_stRoundArea.timeType); //��ǰʱ��  
    return Public_CheckTimeIsInRange(curTime,s_stRoundArea.startTime,s_stRoundArea.endTime);    
}
//-----------------------------------------------
//����:ɨ��Բ������
//����:
//���:
//��ע:ÿ��ִ��һ��
//-----------------------------------------------   
void Area_ScanRoundArea(void) 
{
    unsigned char i,j;
    unsigned char Status,len;
    unsigned long OffSetAddr;
    STROUND_AREA_ATTRIB stRoundArea;  //��ȡԲ����������
    //Բ��������
    if(AREA_STATUS_IN != s_stRoundArea.AreaFlag)//����������
    {
        //�ж��Ƿ���������
        if(MAX_AREA_ADDR == s_stRoundArea.addr)
        {
            for(i=0; s_stRoundArea.AreaCnt < s_stAreaAttrib.CheckRoundNum && i<10; s_stRoundArea.AreaCnt++,i++)
            {
                OffSetAddr=ROUND_AREA_ADDR_START + s_stRoundArea.AreaCnt*STROUND_AREA_ATTRIB_SIZE ;  
                sFLASH_ReadBuffer((unsigned char *)&stRoundArea,OffSetAddr,AREA_ATTRIB_SIZE);//������������ 
                if(stRoundArea.AreaID==0) //����ID=0
                    continue;
                s_stRoundArea.AreaAttrib=stRoundArea.AreaAttrib;
                s_stRoundArea.timeType=Area_GetConverseBCDTimeType(stRoundArea.StartMoment);
                s_stRoundArea.startTime=Area_ConverseBcdGmtime(&stRoundArea.StartMoment[0],s_stRoundArea.timeType);
                s_stRoundArea.endTime=Area_ConverseBcdGmtime(&stRoundArea.EndMoment[0],s_stRoundArea.timeType);
                ///////////////////////////////////////
                if(0==Area_CheckRoundTime())//ʱ�䲻����
                    continue;
                Status = Area_IsInRoundArea(&stRoundArea);
                if(Status==0)
                {//��������
                    s_stRoundArea.addr = OffSetAddr;
                    break;
                } 
            }
            //////////           
            if(s_stRoundArea.AreaCnt>=s_stAreaAttrib.CheckRoundNum)s_stRoundArea.AreaCnt = 0;  //���¼���
            //////////////////////////
            if((MAX_AREA_ADDR == s_stRoundArea.addr) && (s_stRoundArea.preAreaCnt != s_stRoundArea.AreaCnt))LZM_PublicSetOnceTimer(&s_stAREATimer[AREA_TIMER_SCAN_ROUND],PUBLICSECS(0.2),Area_ScanRoundArea);
            /////////////////////////// 
        }
        //�ж����������Ƿ����һ��ʱ��
        if(s_stRoundArea.addr != MAX_AREA_ADDR)
        {//����
            s_stRoundArea.AreaFlag = AREA_STATUS_IN;
            sFLASH_ReadBuffer((unsigned char *)&stRoundArea,s_stRoundArea.addr,STROUND_AREA_ATTRIB_SIZE);//������������                
            if(PUBLIC_CHECKBIT(stRoundArea.AreaAttrib,E_IN_SENDTO_DRIVER)) 
            {
                if(PUBLIC_CHECKBIT(stRoundArea.UserAreaAttrib,E_USER_IN_PLAY)&&stRoundArea.InVoiceSize)
                {
                    len=stRoundArea.InVoiceSize;
                    if(len>=VOICE_BYTE)len=VOICE_BYTE-1;
                    for(j=0; j<len;j++)
                    {
                        s_stRoundArea.VoiceStr[j]=stRoundArea.InVoiceStr[j];                    
                    }
                    s_stRoundArea.VoiceStr[j]='\0';
                }
                else
                {
                    strcpy((char *)s_stRoundArea.VoiceStr,"����ʻ��ָ������");
                }
            }
            //////////////////////////
            s_stRoundArea.AreaID =stRoundArea.AreaID; 
            s_stRoundArea.AreaAttrib = stRoundArea.AreaAttrib;
            s_stRoundArea.UserAreaAttrib = stRoundArea.UserAreaAttrib;            
            s_stRoundArea.LimitSpeed=stRoundArea.LimitSpeed; 
            s_stRoundArea.speedDuration=stRoundArea.speedDuration;            
//            if(s_stRoundArea.speedDuration<10)s_stRoundArea.speedDuration = 10;  dxl,2017.4.24,���Բ�Ҫ��ȥ������
            Area_ChoiceAreaArmDest(ALARM_ITEM_IN_ROUND_AREA);
        }
    }
    else
    if(AREA_STATUS_IN == s_stRoundArea.AreaFlag)//��������
    {
        if(0==Area_CheckRoundTime())//ʱ�䲻����
        {
            Area_RoundAreaResetParam();
            return;
        }
        //////////////////////////
        sFLASH_ReadBuffer((unsigned char *)&stRoundArea,s_stRoundArea.addr,STROUND_AREA_ATTRIB_SIZE);//������������
        Status = Area_IsInRoundArea(&stRoundArea);  
        if(Status)
        {//����
            s_stRoundArea.AreaFlag = AREA_STATUS_OUT;//������
            /////////////////////////////////
            sFLASH_ReadBuffer((unsigned char *)&stRoundArea,s_stRoundArea.addr,STROUND_AREA_ATTRIB_SIZE);//������������ 
            if(PUBLIC_CHECKBIT(stRoundArea.AreaAttrib,E_OUT_SENDTO_DRIVER)) 
            {
                if(PUBLIC_CHECKBIT(stRoundArea.UserAreaAttrib,E_USER_OUT_PLAY)&&stRoundArea.OutVoiceSize)
                {
                    len=stRoundArea.OutVoiceSize;
                    if(len>=VOICE_BYTE)len=VOICE_BYTE-1;
                    for(j=0; j<len;j++)
                    {
                        s_stRoundArea.VoiceStr[j]=stRoundArea.OutVoiceStr[j];                    
                    }
                    s_stRoundArea.VoiceStr[j]='\0';
                }
                else
                {
                    strcpy((char *)s_stRoundArea.VoiceStr,"�����뿪ָ������");
                }
            }
            ///////////////////////////
            s_stRoundArea.AreaID = stRoundArea.AreaID;
            s_stRoundArea.AreaAttrib = stRoundArea.AreaAttrib;
            s_stRoundArea.UserAreaAttrib = stRoundArea.UserAreaAttrib;
            Area_ChoiceAreaArmDest(ALARM_ITEM_OUT_ROUND_AREA); 
            ///////////////////////////
            s_stRoundArea.addr = MAX_AREA_ADDR;  
        }
    }
    //��������////////////////////////////////
    if((AREA_STATUS_IN==s_stRoundArea.AreaFlag)&&PUBLIC_CHECKBIT(s_stRoundArea.AreaAttrib,E_AREA_PROHIBIT_OPENDOOR))
    {
        Area_CheckOpenDoorAlarm(E_ROUND_AREA);
    }
}
//-----------------------------------------------
//����:ɨ���������
//����:
//���:
//��ע:ÿ��ִ��һ��
//-----------------------------------------------
void Area_ScanRectArea(void)
{
    unsigned char i,j;
    unsigned char Status,len;
    unsigned long OffSetAddr;
    STRECT_AREA_ATTRIB  stRectArea;   //��ȡ������������
    if(s_stRectArea.AreaFlag != AREA_STATUS_IN)
    {
        if(s_stRectArea.addr == MAX_AREA_ADDR)
        {
            for(i=0; s_stRectArea.AreaCnt<s_stAreaAttrib.CheckRectNum && i<10; s_stRectArea.AreaCnt++,i++)
            {
                OffSetAddr=RECT_AREA_ADDR_START + s_stRectArea.AreaCnt*STRECT_AREA_ATTRIB_SIZE ;  
                sFLASH_ReadBuffer((unsigned char *)&stRectArea,OffSetAddr,AREA_ATTRIB_SIZE);//������������  
                if(stRectArea.AreaID ==0) //����ID=0
                    continue;
                s_stRectArea.AreaAttrib=stRectArea.AreaAttrib;
                /////////////////////////////////
                #ifdef TEST_USE_BIG_AREA
                if(PUBLIC_CHECKBIT(stRectArea.AreaAttrib,E_AREA_ENTER_COLLECT_GNSS))//
                {
                    Area_SetTestRectAreaParameter((T_GPS_POS *)&(stRectArea.LeftUpLat),(T_GPS_POS*)&(stRectArea.RightDownLat));                    
                }
                #endif
                ////////////////////
                s_stRectArea.timeType=Area_GetConverseBCDTimeType(stRectArea.StartMoment);
                s_stRectArea.startTime=Area_ConverseBcdGmtime(&stRectArea.StartMoment[0],s_stRectArea.timeType);
                s_stRectArea.endTime=Area_ConverseBcdGmtime(&stRectArea.EndMoment[0],s_stRectArea.timeType);
                if(0==Area_CheckRectTime())//ʱ�䲻����
                    continue;
                Status = Area_IsInRectArea(&stRectArea);    
                if(Status==0)
                {//��������
                    s_stRectArea.addr = OffSetAddr;
                    break;
                }           
            }            
            if(s_stRectArea.AreaCnt>=s_stAreaAttrib.CheckRectNum)s_stRectArea.AreaCnt = 0;  //���¼���
            ////////////////////////
            if((MAX_AREA_ADDR == s_stRectArea.addr) && (s_stRectArea.preAreaCnt != s_stRectArea.AreaCnt))LZM_PublicSetOnceTimer(&s_stAREATimer[AREA_TIMER_SCAN_RECT],PUBLICSECS(0.2),Area_ScanRectArea);
            //////////////////////////
        }
        ////////////////////////////////////
        if(s_stRectArea.addr != MAX_AREA_ADDR)//����
        {
            s_stRectArea.AreaFlag = AREA_STATUS_IN;                
            sFLASH_ReadBuffer((unsigned char *)&stRectArea,s_stRectArea.addr,STRECT_AREA_ATTRIB_SIZE);//������������    
            if(PUBLIC_CHECKBIT(stRectArea.AreaAttrib,E_IN_SENDTO_DRIVER)) 
            {
                if(PUBLIC_CHECKBIT(stRectArea.UserAreaAttrib,E_USER_IN_PLAY)&&stRectArea.InVoiceSize)
                {
                    len=stRectArea.InVoiceSize;
                    if(len>=VOICE_BYTE)len=VOICE_BYTE-1;
                    for(j=0; j<len;j++)
                    {
                        s_stRectArea.VoiceStr[j]=stRectArea.InVoiceStr[j];                    
                    }
                    s_stRectArea.VoiceStr[j]='\0';
                }
                else
                {
                    strcpy((char *)s_stRectArea.VoiceStr,"����ʻ��ָ������");
                    //LcdShowCaptionEx((char *)"����ʻ��ָ������",17);   //fanqinghai

                }
            }
            ///////////////////////////
            s_stRectArea.AreaID = stRectArea.AreaID;
            s_stRectArea.AreaAttrib = stRectArea.AreaAttrib;
            s_stRectArea.UserAreaAttrib = stRectArea.UserAreaAttrib;            
            s_stRectArea.LimitSpeed = stRectArea.LimitSpeed;
            s_stRectArea.speedDuration =stRectArea.speedDuration;
//            if(s_stRectArea.speedDuration < 10) s_stRectArea.speedDuration = 10; dxl,2017.4.24,���Բ�Ҫ��ȥ������
            ///////////////////////
            if(PUBLIC_CHECKBIT(stRectArea.AreaAttrib,E_AREA_ENTER_COLLECT_GNSS))//
            {
                Area_SetTestRectAreaParameter((T_GPS_POS *)&(stRectArea.LeftUpLat),(T_GPS_POS*)&(stRectArea.RightDownLat));                    
            }
            Area_ChoiceAreaArmDest(ALARM_ITEM_IN_RECT_AREA);
        }
    }
    else
    if(AREA_STATUS_IN == s_stRectArea.AreaFlag)
    {
        if(0==Area_CheckRectTime())//ʱ�䲻����
        {
            Area_RectAreaResetParam();
            return;
        }
        sFLASH_ReadBuffer((unsigned char *)&stRectArea,s_stRectArea.addr,AREA_ATTRIB_SIZE);//������������           
        Status = Area_IsInRectArea(&stRectArea);    
        if(Status)
        {
            s_stRectArea.AreaFlag = AREA_STATUS_OUT;                
            sFLASH_ReadBuffer((unsigned char *)&stRectArea,s_stRectArea.addr,STRECT_AREA_ATTRIB_SIZE);//������������
            if(PUBLIC_CHECKBIT(stRectArea.AreaAttrib,E_OUT_SENDTO_DRIVER)) 
            {
                if(PUBLIC_CHECKBIT(stRectArea.UserAreaAttrib,E_USER_OUT_PLAY)&&stRectArea.OutVoiceSize)
                {
                    len=stRectArea.OutVoiceSize;
                    if(len>=VOICE_BYTE)len=VOICE_BYTE-1;
                    for(j=0; j<len;j++)
                    {
                        s_stRectArea.VoiceStr[j]=stRectArea.OutVoiceStr[j];
                    }
                    s_stRectArea.VoiceStr[j]='\0'; 
                }
                else
                {
                    strcpy((char *)s_stRectArea.VoiceStr,"�����뿪ָ������");
                   //LcdShowCaptionEx((char *)"�����뿪ָ������",17);    //fanqinghai
                }
            }
            ////////////////////////////
            s_stRectArea.AreaID = stRectArea.AreaID;
            s_stRectArea.AreaAttrib = stRectArea.AreaAttrib;
            s_stRectArea.UserAreaAttrib = stRectArea.UserAreaAttrib;              
            Area_ChoiceAreaArmDest(ALARM_ITEM_OUT_RECT_AREA);  
            ///////////////////
            s_stRectArea.addr = MAX_AREA_ADDR;                
        }
    }
    //////////////////////////
    //��������
    if((AREA_STATUS_IN==s_stRectArea.AreaFlag)&&PUBLIC_CHECKBIT(s_stRectArea.AreaAttrib,E_AREA_PROHIBIT_OPENDOOR))
    {
        Area_CheckOpenDoorAlarm(E_RECT_AREA);
    }
    //////////////////////////////////  
    Area_ScanTestRectArea();
}
/************************************************************
** ��������: Area_ClearInOutRectAlarm
** ��������: 
** ��ڲ���: 
** ���ڲ���:
************************************************************/ 
void Area_ClearInOutRectAlarm(void) 
{
    Area_ClearInOutAreaAlarmBit(E_RECT_AREA);
    ///////////////////////////////////////
}
/************************************************************
** ��������: Area_DisposeDriveOutRectArea
** ��������: ����ʻ����������
** ��ڲ���: 
** ���ڲ���:
************************************************************/
void Area_DisposeDriveOutRectArea(void) 
{
    Area_ClearAllAlarm(E_RECT_AREA);
    if(PUBLIC_CHECKBIT(s_stRectArea.AreaAttrib,E_OUT_SENDTO_DRIVER)) 
    {
			  if(1 == BBGNTestFlag)
				{
				
				}
				else
				{
            Public_PlayTTSVoiceStr(s_stRectArea.VoiceStr);
				}
    }
    /////////////////////
    if(PUBLIC_CHECKBIT(s_stRectArea.AreaAttrib,E_AREA_ENTER_COLLECT_GNSS))//
    {
        //���������¹رղɼ�GNSS��ϸ��λ���� ///////////////////////
        Area_TestRectAreaParameterInitialize();
    }
    ///////////////
    if(PUBLIC_CHECKBIT(s_stRectArea.AreaAttrib,E_OUT_SENDTO_SERVER)) 
    {
        //��Ϊ��Ч����,һ���ͱ�
        Area_SetInOutAreaAlarmInfo(E_RECT_AREA,s_stRectArea.AreaID,AREA_STATUS_OUT);
    }
    //////////////////////
    if(PUBLIC_CHECKBIT(s_stRectArea.UserAreaAttrib,E_USER_NO_CHANGE_LIMIT_SPEED))//�����򲻸�������ֵ
    { 
        if(PUBLIC_CHECKBIT(s_stRectArea.AreaAttrib,E_AREA_LIMIT_SPEED))//
        {
            //SpeedMonitor_SetSpeedParamValid(E_SPEED_TYPE_GENERAL,0,s_stRectArea.LimitSpeed,s_stRectArea.speedDuration); dxl,2015.9,
					  
        }
        ////////////////////////////////
    }
}
/************************************************************
** ��������: Area_DisposeDriveInRectArea
** ��������: ����ʻ���������
** ��ڲ���: 
** ���ڲ���:
************************************************************/
extern u8	TerminalAuthorizationFlag;

void Area_DisposeDriveInRectArea(void) 
{
    Area_ClearAllAlarm(E_RECT_AREA);
    //////////////////////
    if(PUBLIC_CHECKBIT(s_stRectArea.AreaAttrib,E_AREA_LIMIT_SPEED))   //
    {
        //SpeedMonitor_SetSpeedParamValid(E_SPEED_TYPE_RECT_AREA,s_stRectArea.AreaID,s_stRectArea.LimitSpeed,s_stRectArea.speedDuration); dxl,2015.9,
			  VDROverspeed_SetAreaParameter(E_RECT_AREA,s_stRectArea.AreaID,s_stRectArea.LimitSpeed&0xff,s_stRectArea.speedDuration);
    }    
    /////////////////Ԥ������////////////////
    ////////////////////////  
    if(PUBLIC_CHECKBIT(s_stRectArea.AreaAttrib,E_IN_SENDTO_DRIVER)) 
    {
			  if(1 == BBGNTestFlag)
				{
				
				}
				else
				{
            Public_PlayTTSVoiceStr(s_stRectArea.VoiceStr);
				}
    }
    //////////////////////////////
    /////������ر�ͨ��ģ��///////////////////////
    if(PUBLIC_CHECKBIT(s_stRectArea.AreaAttrib,E_AREA_ENTER_TURNOFF_GPRS))//
    {
		TerminalAuthorizationFlag &= 0xfd;

		Area_SetCloseGPRSFlag(E_RECT_AREA);
    }
    ////////////////////////////
    if(PUBLIC_CHECKBIT(s_stRectArea.AreaAttrib,E_AREA_ENTER_COLLECT_GNSS))//
    {
        //������ɼ�GNSS��ϸ��λ����/////////////////////
        Area_SetCollectGnssFlag(E_RECT_AREA);
    }
    ///////////////////
    if(PUBLIC_CHECKBIT(s_stRectArea.AreaAttrib,E_IN_SENDTO_SERVER)) 
    {//��Ϊ����,һ����ͱ�
        Area_SetInOutAreaAlarmInfo(E_RECT_AREA,s_stRectArea.AreaID,AREA_STATUS_IN);
    }
    Area_SetInAreaFlag(E_RECT_AREA);
}
/************************************************************
** ��������: Area_DisposeDriveOutRoundArea
** ��������: ����ʻ��Բ������
** ��ڲ���: 
** ���ڲ���:
************************************************************/
void Area_DisposeDriveOutRoundArea(void) 
{
    Area_ClearAllAlarm(E_ROUND_AREA);
    if(PUBLIC_CHECKBIT(s_stRoundArea.AreaAttrib,E_OUT_SENDTO_DRIVER)) 
    {
			  if(1 == BBGNTestFlag)
				{
				
				}
				else
				{
            Public_PlayTTSVoiceStr(s_stRoundArea.VoiceStr); //dxl,2016.5.23��ͣ��������
				}
    }
    ////////////////////////////
    if(PUBLIC_CHECKBIT(s_stRoundArea.AreaAttrib,E_OUT_SENDTO_SERVER)) 
    {//��Ϊ��Ч����,һ���ͱ�
        Area_SetInOutAreaAlarmInfo(E_ROUND_AREA,s_stRoundArea.AreaID,AREA_STATUS_OUT);
    }
    ///////////////////
    if(PUBLIC_CHECKBIT(s_stRoundArea.UserAreaAttrib,E_USER_NO_CHANGE_LIMIT_SPEED))//�����򲻸�������ֵ
    { 
        if(PUBLIC_CHECKBIT(s_stRoundArea.AreaAttrib,E_AREA_LIMIT_SPEED))//
        {
//            SpeedMonitor_SetSpeedParamValid(E_SPEED_TYPE_GENERAL,0,s_stRoundArea.LimitSpeed,s_stRoundArea.speedDuration); dxl,2015.9,
        }
    }
}
/************************************************************
** ��������: Area_DisposeDriveInRectArea
** ��������: ����ʻ��Բ������
** ��ڲ���: 
** ���ڲ���:
************************************************************/
void Area_DisposeDriveInRoundArea(void) 
{ 
    Area_ClearAllAlarm(E_ROUND_AREA);    
    //////////////////////////////////    
    if(PUBLIC_CHECKBIT(s_stRoundArea.AreaAttrib,E_AREA_LIMIT_SPEED))//�Ƿ�����������
    {
//        SpeedMonitor_SetSpeedParamValid(E_SPEED_TYPE_ROUND_AREA,s_stRoundArea.AreaID,s_stRoundArea.LimitSpeed,s_stRoundArea.speedDuration); dxl,2015.9,
			 VDROverspeed_SetAreaParameter(E_ROUND_AREA,s_stRoundArea.AreaID,s_stRoundArea.LimitSpeed&0xff,s_stRoundArea.speedDuration);
    }
    //////////////////////  
    if(PUBLIC_CHECKBIT(s_stRoundArea.AreaAttrib,E_IN_SENDTO_DRIVER)) 
    {
			  if(1 == BBGNTestFlag)//dxl,2016.5.23��ͣ��������
				{
				
				}
				else
				{
            Public_PlayTTSVoiceStr(s_stRoundArea.VoiceStr);
				}
    }
    /////////////////////////
    /////������ر�ͨ��ģ��///////////////////////
    if(PUBLIC_CHECKBIT(s_stRoundArea.AreaAttrib,E_AREA_ENTER_TURNOFF_GPRS))//
    {
        //Area_CloseGSMModule();
        Area_SetCloseGPRSFlag(E_ROUND_AREA);
    }
    ////////////////////////////
    if(PUBLIC_CHECKBIT(s_stRoundArea.AreaAttrib,E_AREA_ENTER_COLLECT_GNSS))//
    {
        //������ɼ�GNSS��ϸ��λ����/////////////////////
        Area_SetCollectGnssFlag(E_ROUND_AREA);
    } 
    ///////////////////
    if(PUBLIC_CHECKBIT(s_stRoundArea.AreaAttrib,E_IN_SENDTO_SERVER)) 
    {//��Ϊ����,һ����ͱ�
        Area_SetInOutAreaAlarmInfo(E_ROUND_AREA,s_stRoundArea.AreaID,AREA_STATUS_IN);
    }
    ///////////////
    Area_SetInAreaFlag(E_ROUND_AREA);
}

/********************************************************************
* ���� : ѡ�񱨾����Ͷ���
* ����: 
* ���: 
* ȫ�ֱ���: 
*
* ��ע: 
// 0,����  1:��Բ���� 2,���������� 3:��Բ���� 4:�������� 5:���� 6:���� 7:ƫ����·
typedef enum{ALARM_ITEM_SPEED_AREA=0,ALARM_ITEM_IN_ROUND_AREA,ALARM_ITEM_IN_RECT_AREA,ALARM_ITEM_OUT_ROUND_AREA,ALARM_ITEM_OUT_RECT_AREA} E_AREA_ALARM_ITEM;
********************************************************************/
void Area_ChoiceAreaArmDest(unsigned char AlarmItem) 
{
    switch(AlarmItem)
    {
        case ALARM_ITEM_SPEED_AREA:
            //Io_WriteOverSpeedConrtolBit(OVER_SPEED_BIT_AREA,SET);
            break;
        case ALARM_ITEM_OUT_RECT_AREA:
            Area_DisposeDriveOutRectArea();                    
            break;
        case ALARM_ITEM_OUT_ROUND_AREA:
            Area_DisposeDriveOutRoundArea();            
            break;           
        case  ALARM_ITEM_IN_RECT_AREA:
            Area_DisposeDriveInRectArea();            
            break;  
        case ALARM_ITEM_IN_ROUND_AREA:            
            Area_DisposeDriveInRoundArea();
            break;                                                                
        default:
            break;    
    }
}
/*********************************************************************
//��������  :Area_GetRectAreaUserDatalen
//����      :���þ�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned short Area_GetRectAreaUserDatalen(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen;
    STRECT_AREA_ATTRIB stRectAreaAttrib;  //��ȡ������������
    datalen =0;
    //�������ID
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.AreaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); 
    datalen+=sizeof(unsigned long);
    //�����������
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.UserAreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); 
    datalen+=sizeof(unsigned short );    
    ///////�Զ���λ///////////////////////    
    if(PUBLIC_CHECKBIT(stRectAreaAttrib.UserAreaAttrib,E_USER_IN_PLAY))
    {     
        //������ʾ������
        Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); 
        datalen+=sizeof(unsigned char);
        //��ʾ�����
        datalen+=stRectAreaAttrib.InVoiceSize;    
    }
    ///////////////////////
    if(PUBLIC_CHECKBIT(stRectAreaAttrib.UserAreaAttrib,E_USER_OUT_PLAY))
    {
         //������ʾ������
        Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0);
        datalen+=sizeof(unsigned char);
        //��ʾ�����
        datalen+=stRectAreaAttrib.OutVoiceSize;
    }
    /////////////////////////
    if(datalen>BufferLen)return 0;
    *AreaID =stRectAreaAttrib.AreaID;
    return datalen;
}
/*************************************************************
** ��������: Area_GetRectAreaDatalenStandard
** ��������: :��������ʱ,��׼Э�鳤��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short Area_GetRectAreaDatalenStandard(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen;
    STRECT_AREA_ATTRIB stRectAreaAttrib;  //��ȡ������������
    datalen =0;
    //�������ID
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.AreaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); 
    datalen+=sizeof(unsigned long);
    //�����������
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.AreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); 
    datalen+=sizeof(unsigned short ); 
     //������ϵ�γ��
    datalen+=sizeof(unsigned long);  
    //������ϵ㾭��
    datalen+=sizeof(unsigned long);  
    
    //������µ�γ��
    datalen+=sizeof(unsigned long);  
    //������µ㾭��
    datalen+=sizeof(unsigned long);
    //////����ʱ��////////////////////////
    if(PUBLIC_CHECKBIT(stRectAreaAttrib.AreaAttrib,E_AREA_MOMENT))
    {
        //��ʼʱ��
        datalen += 6;  
        //����ʱ��
        datalen += 6;  
    }
    /////�������/////////////////////
    if(PUBLIC_CHECKBIT(stRectAreaAttrib.AreaAttrib,E_AREA_LIMIT_SPEED))
    {
        //�������
        datalen +=sizeof(unsigned short);    
        //���ٳ���ʱ��
        datalen +=sizeof(unsigned char); 
    }   
    ///////////////////
    if(datalen>BufferLen)return 0;
    ///////////////////////
    *AreaID =stRectAreaAttrib.AreaID;
    return datalen;
}
/*************************************************************
** ��������: Area_GetRectAreaDatalenJiuTong
** ��������: :��������ʱ,��ͨЭ�鳤��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short Area_GetRectAreaDatalenJiuTong(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen;
    STRECT_AREA_ATTRIB stRectAreaAttrib;  //��ȡ������������
    datalen =0;
    //�������ID
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.AreaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); 
    datalen+=sizeof(unsigned long);
    //�����������
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.AreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); 
    datalen+=sizeof(unsigned short ); 
     //������ϵ�γ��
    datalen+=sizeof(unsigned long);  
    //������ϵ㾭��
    datalen+=sizeof(unsigned long);  
    
    //������µ�γ��
    datalen+=sizeof(unsigned long);  
    //������µ㾭��
    datalen+=sizeof(unsigned long);
    //////����ʱ��////////////////////////
    if(PUBLIC_CHECKBIT(stRectAreaAttrib.AreaAttrib,E_AREA_MOMENT))
    {
        //��ʼʱ��
        datalen += 6;  
        //����ʱ��
        datalen += 6;  
    }
    /////�������/////////////////////
    if(PUBLIC_CHECKBIT(stRectAreaAttrib.AreaAttrib,E_AREA_LIMIT_SPEED))
    {
        //�������
        datalen +=sizeof(unsigned short);    
        //���ٳ���ʱ��
        datalen +=sizeof(unsigned char); 
    }    
    ///////�Զ���λ///////////////////////
    /////////////////////////////////
    if(PUBLIC_CHECKBIT(stRectAreaAttrib.AreaAttrib,14))
    {     
        //������ʾ������
        Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); 
        datalen+=sizeof(unsigned char);
        //��ʾ�����
        datalen+=stRectAreaAttrib.InVoiceSize;    
    }
    ///////////////////////
    if(PUBLIC_CHECKBIT(stRectAreaAttrib.AreaAttrib,15))
    {
        //������ʾ������
        Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0);
        datalen+=sizeof(unsigned char);
        //��ʾ�����
        datalen+=stRectAreaAttrib.OutVoiceSize;
    }    
    ///////////////////
    if(datalen>BufferLen)return 0;
    ///////////////////////
    *AreaID =stRectAreaAttrib.AreaID;
    return datalen;
}
/*************************************************************
** ��������: Area_GetRectAreaDatalenSuoMei
** ��������: :��������ʱ,����Э�鳤��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short Area_GetRectAreaDatalenSuoMei(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen=0;
    return datalen;
}
/*********************************************************************
//��������  :Area_SaveOneRectArea
//����      :���þ�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned short Area_SaveOneRectModifyUserData(unsigned long addr,unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long AreaID;
    unsigned short datalen,UserAreaAttrib;
    STRECT_AREA_ATTRIB stRectAreaAttrib;  //��ȡ������������    
    if(addr+STRECT_AREA_ATTRIB_SIZE>RECT_AREA_ADDR_END)return 0;
    sFLASH_ReadBuffer((unsigned char*)&stRectAreaAttrib,addr,STRECT_AREA_ATTRIB_SIZE); //������
    datalen = 0;
    Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //�������ID
    if(AreaID!=stRectAreaAttrib.AreaID)return 0;
    datalen+=sizeof(unsigned long);
    Public_Mymemcpy((unsigned char *)&UserAreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //�����������
    datalen+=sizeof(unsigned short ); 
    //////////�Զ���/////////////////    
    if(PUBLIC_CHECKBIT(UserAreaAttrib,E_USER_IN_PLAY))
    { 
        PUBLIC_SETBIT(stRectAreaAttrib.UserAreaAttrib,E_USER_IN_PLAY);
        Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
        datalen+=sizeof(unsigned char);  
        Public_Mymemcpy(stRectAreaAttrib.InVoiceStr,(unsigned char *)&pBuffer[datalen],stRectAreaAttrib.InVoiceSize,0); //��ʾ�����
        datalen+=stRectAreaAttrib.InVoiceSize;    
    }
    if(PUBLIC_CHECKBIT(UserAreaAttrib,E_USER_OUT_PLAY))
    {
        PUBLIC_SETBIT(stRectAreaAttrib.UserAreaAttrib,E_USER_OUT_PLAY);
        Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
        datalen+=sizeof(unsigned char);  
        Public_Mymemcpy(stRectAreaAttrib.OutVoiceStr,(unsigned char *)&pBuffer[datalen],stRectAreaAttrib.OutVoiceSize,0); //��ʾ�����
        datalen+=stRectAreaAttrib.OutVoiceSize;
    }
    ////////////////////////////
    if(datalen>BufferLen)return 0;
    //////////////////////////
    Area_MdyDataInFlash((unsigned char*)&stRectAreaAttrib,addr,STRECT_AREA_ATTRIB_SIZE);
    return datalen;
}
/*********************************************************************
//��������  :Area_SaveOneRectArea
//����      :���þ�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned short Area_SaveOneRectArea(unsigned long addr,unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned short datalen;
    STRECT_AREA_ATTRIB stRectAreaAttrib;  //��ȡ������������    
    if(addr+STRECT_AREA_ATTRIB_SIZE>RECT_AREA_ADDR_END)return 0;
    memset(&stRectAreaAttrib,0,sizeof(STRECT_AREA_ATTRIB));
    datalen =0;
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.AreaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //�������ID
    datalen+=sizeof(unsigned long);
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.AreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //�����������
    datalen+=sizeof(unsigned short ); 
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.LeftUpLat,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //������ϵ�γ��
    datalen+=sizeof(unsigned long);  
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.LeftUpLong,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //������ϵ㾭��
    datalen+=sizeof(unsigned long);  
    
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.RightDownLat,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //������µ�γ��
    datalen+=sizeof(unsigned long);  
    Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.RightDownLong,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //������µ㾭��
    datalen+=sizeof(unsigned long); 

    //////////����ʱ��///////////////////
    if(PUBLIC_CHECKBIT(stRectAreaAttrib.AreaAttrib,E_AREA_MOMENT))
    {
        Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.StartMoment[0],(unsigned char *)&pBuffer[datalen],6,0); //��ʼʱ��
        datalen+=6;  
        Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.EndMoment[0],(unsigned char *)&pBuffer[datalen],6,0); //����ʱ��
        datalen+=6;  
    }
    //////////�������////////////
    if(PUBLIC_CHECKBIT(stRectAreaAttrib.AreaAttrib,E_AREA_LIMIT_SPEED))
    {
        Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.LimitSpeed,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //���
        datalen+=sizeof(unsigned short );    
        Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.speedDuration,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //���ٳ���ʱ��
        datalen+=sizeof(unsigned char);  
    }
    //////////////////////////
    if(AREA_PROTOCOL_TYPE_JIUTONG == s_stRectArea.protoclType)
    {
        if(PUBLIC_CHECKBIT(stRectAreaAttrib.AreaAttrib,14))//
        {
            PUBLIC_CLRBIT(stRectAreaAttrib.AreaAttrib,14);
            PUBLIC_SETBIT(stRectAreaAttrib.UserAreaAttrib,E_USER_IN_PLAY);               
            Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
            datalen+=sizeof(unsigned char);  
            Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.InVoiceStr[0],(unsigned char *)&pBuffer[datalen],stRectAreaAttrib.InVoiceSize,0); //��ʾ�����
            datalen+=stRectAreaAttrib.InVoiceSize;    
        }
        ///////////////////////
        if(PUBLIC_CHECKBIT(stRectAreaAttrib.AreaAttrib,15))
        {
            PUBLIC_CLRBIT(stRectAreaAttrib.AreaAttrib,15);
            PUBLIC_SETBIT(stRectAreaAttrib.UserAreaAttrib,E_USER_OUT_PLAY);
            Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
            datalen+=sizeof(unsigned char);  
            Public_Mymemcpy((unsigned char *)&stRectAreaAttrib.OutVoiceStr[0],(unsigned char *)&pBuffer[datalen],stRectAreaAttrib.OutVoiceSize,0); //��ʾ�����
            datalen+=stRectAreaAttrib.OutVoiceSize;
        }
    }
    //////////////////////////
    if(datalen>BufferLen)return 0;
    //////////////////////
    stRectAreaAttrib.Head = HEAD_FLAG;
    ///////////////////////////
    Area_MdyDataInFlash((unsigned char*)&stRectAreaAttrib,addr,STRECT_AREA_ATTRIB_SIZE);
    return datalen;
}

/*********************************************************************
//��������  :Area_RectAreaUpdate(unsigned char *pBuffer, unsigned short BufferLen)
//����      :���þ�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned char Area_RectAreaUpdate(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long OffSetAddr;
    unsigned char i,Sum;
    unsigned short datalen,templen;
    unsigned char *pTmp;
    //////////////
    if(BufferLen==0)return 2;  
    ////////////////////////
    Sum=*pBuffer++;
    if(Sum>MAX_RECT_AREA_SUM)return 1;
    ////////////////////    
    /////////////////
    Area_ClearRectArea();
    ////////////
    datalen=BufferLen-1;
    pTmp=pBuffer;
    //����
    Area_RectAreaParameterInitialize(); 
    for(i=0; i<Sum; i++)
    {
        OffSetAddr=RECT_AREA_ADDR_START + i*STRECT_AREA_ATTRIB_SIZE;
        templen=Area_SaveOneRectArea(OffSetAddr,pTmp,datalen);
        if(0 == templen) break;
        s_stAreaAttrib.CheckRectNum++;
        s_stAreaAttrib.ValidRectNum++;
        if(s_stAreaAttrib.CheckRectNum>=MAX_RECT_AREA_SUM)break;
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }
    return 0;    
}

/*************************************************************
** ��������: Area_GetCurRectAreaDatalen
** ��������: ��ȡ��ǰ���νṹ�峤��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short Area_GetCurRectAreaDatalen(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    if(AREA_PROTOCOL_TYPE_JIUTONG == s_stRectArea.protoclType)
    {
        return Area_GetRectAreaDatalenJiuTong(pBuffer,BufferLen,AreaID);
    }
    else
    {
       return Area_GetRectAreaDatalenStandard(pBuffer,BufferLen,AreaID);
    }
}
/*********************************************************************
//��������  :Area_RectAreaAdd(unsigned char *pBuffer, unsigned short BufferLen)
//����      :���þ�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned char Area_RectAreaAdd(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long OffSetAddr,AreaID;
    unsigned char i,j,Sum;
    STRECT_AREA_ATTRIB stRectAreaAttrib;  //��ȡ������������
    unsigned short datalen,templen;
    unsigned char *pTmp;
    //////////////
    if(BufferLen==0)return 2;
    ////////////
    Sum=*pBuffer++;
    /////////////////////////
    if(Sum+s_stAreaAttrib.CheckRectNum>MAX_RECT_AREA_SUM)return 1;//ʧ��
    //////////////////////////
    pTmp =pBuffer;
    datalen=BufferLen-1;
    //����
    for(i=0; i<Sum; i++)//���ID
    {    
        templen=Area_GetCurRectAreaDatalen(pTmp,datalen,&AreaID);
        if(0 == templen)
					return 2;//��Ϣ����;
        //////////////////////////
        for(j=0; j<s_stAreaAttrib.CheckRectNum; j++)
        {
            OffSetAddr=RECT_AREA_ADDR_START + j*STRECT_AREA_ATTRIB_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stRectAreaAttrib,OffSetAddr,AREA_HEAD_SIZE);
            if(AreaID==stRectAreaAttrib.AreaID)
            {
                return 1;////ʧ��
            }
        }
        ///////////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }
    //////////////////////////////////////
    pTmp =pBuffer;
    datalen=BufferLen-1;    
    for(i=0; i<Sum; i++)
    {
        OffSetAddr=RECT_AREA_ADDR_START + s_stAreaAttrib.CheckRectNum*STRECT_AREA_ATTRIB_SIZE;
        templen=Area_SaveOneRectArea(OffSetAddr,pTmp,datalen);
        if(0 == templen)break;
        ////////////////////
        s_stAreaAttrib.CheckRectNum++;//
        s_stAreaAttrib.ValidRectNum++;
        if(s_stAreaAttrib.CheckRectNum>=MAX_RECT_AREA_SUM)break;
        ////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }    
    return 0;    
}
/*********************************************************************
//��������  :Area_RectAreaModify(unsigned char *pBuffer, unsigned short BufferLen)
//����      :�޸ľ�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned char Area_RectAreaModify(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long OffSetAddr,AreaID;
    unsigned char i,j,Sum,flag;
    STRECT_AREA_ATTRIB stRectAreaAttrib;  //��ȡ������������
    unsigned short datalen,templen;
    unsigned char *pTmp;
    //////////////
    if(BufferLen==0)return 2;
    ////////////
    Sum=*pBuffer++;
    pTmp =pBuffer;
    datalen=BufferLen-1;
    //����
    for(i=0; i<Sum; i++)//���ID
    {  
        templen=Area_GetCurRectAreaDatalen(pTmp,datalen,&AreaID);        
        if(0 == templen)return 2;//��Ϣ����;
        //////////////////////////
        flag =0;
        for(j=0; j<s_stAreaAttrib.CheckRectNum; j++)
        {
            OffSetAddr=RECT_AREA_ADDR_START + j*STRECT_AREA_ATTRIB_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stRectAreaAttrib,OffSetAddr,AREA_HEAD_SIZE);
            if(AreaID==stRectAreaAttrib.AreaID)
            {
                flag=1;//
                break;
            }
        }
        //////////////////
        if(0==flag)return 1;//ʧ��;
        ///////////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }
    //////////////////////////////////////
    pTmp =pBuffer;
    datalen=BufferLen-1;
    for(i=0; i<Sum; i++)
    {
        templen = 0;
        Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pTmp[0],sizeof(unsigned long),1); //�������ID
        for(j=0; j<s_stAreaAttrib.CheckRectNum; j++)
        {
            OffSetAddr=RECT_AREA_ADDR_START + j*STRECT_AREA_ATTRIB_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stRectAreaAttrib,OffSetAddr,AREA_HEAD_SIZE);
            if(AreaID==stRectAreaAttrib.AreaID)
            {
                templen=Area_SaveOneRectArea(OffSetAddr,pTmp,datalen);
                break;
            }
        }
        ///////////////
        Area_RectAreaCheckForResetParam(AreaID);
        /////////////////
        if(0 == templen)break;
        ////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }    
    return 0;    
}
/*************************************************************
** ��������: Area_SetRectAreaCheckProtoclType
** ��������: :��������ʱ,����Ƿ���������־,��Ϊ���ݾ�ͨ�汾
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Area_SetRectAreaCheckProtoclType(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long AreaID;
    unsigned char i,Sum;
    unsigned short datalen,templen;
    unsigned char *pTmp;
    //////////////    
    if(BufferLen==0)return AREA_PROTOCOL_TYPE_EMPTY;
    ////////////////////////    
    Sum=*pBuffer++;
    ////////////////////
    datalen=BufferLen-1;
    pTmp=pBuffer;
    ////////////////////
    for(i=0; i<Sum; i++)
    {
        templen=Area_GetRectAreaDatalenStandard(pTmp,datalen,&AreaID);
        if(0 == templen)//����
        {
            break;
        }
        pTmp += templen;
        //////////////////////////
        if(datalen == templen)//���,��ȷ
        {
            return AREA_PROTOCOL_TYPE_STANDARD;
        }
        else
        if(datalen > templen)//����,��������
        {
            datalen -= templen;
        }
        else////С��,����
        {
            break;
        }
    }
    /////////////////////
    datalen=BufferLen-1;
    pTmp=pBuffer;
    ////////////////////
    for(i=0; i<Sum; i++)
    {
        templen=Area_GetRectAreaDatalenJiuTong(pTmp,datalen,&AreaID);
        if(0 == templen)//����
        {
            break;
        }
        pTmp += templen;
        ///////////////////
        if(datalen == templen)//���,��ȷ
        {
            return AREA_PROTOCOL_TYPE_JIUTONG;//������,��ȷ
        }
        else
        if(datalen > templen)//����,��������
        {
            datalen -= templen;
        }
        else////С��,����
        {
            break;
        }
    }
    return AREA_PROTOCOL_TYPE_EMPTY;
}
/*************************************************************
** ��������: Area_SetRectArea
** ��������: :���þ�������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Area_SetRectArea(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned char AckResult,type;
    unsigned char protoclType;
    if(BufferLen==0)return 2;
	
	  if((1 == BBXYTestFlag)||(1 == BBGNTestFlag))
    {
        Area_ClearRoundArea();
			  Area_ClearRectArea();
			  Polygon_ClearArea();
    }
		
    /////////////////
    type=*pBuffer++;
    //////////////////////
    protoclType=Area_SetRectAreaCheckProtoclType(pBuffer,BufferLen-1);
    if(AREA_PROTOCOL_TYPE_EMPTY==protoclType)return 2;
    s_stRectArea.protoclType = protoclType;
    //////////////
    AckResult = 0;
    

    if(0== type)//����
    {
        AckResult=Area_RectAreaUpdate(pBuffer,BufferLen-1);
    }
    else
    if(1 == type)//���
    {   
        AckResult=Area_RectAreaAdd(pBuffer,BufferLen-1);
    }
    else
    if(2 == type)//�޸�
    {  
        AckResult=Area_RectAreaModify(pBuffer,BufferLen-1);
    }
    if(0==AckResult)
    {
        s_stAreaAttrib.preSpeed = 0xff;
    }
    return AckResult;
}
/*************************************************************
** ��������: Area_DelRectArea
** ��������: :ɾ����������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Area_DelRectArea(unsigned char *pBuffer, unsigned short BufferLen) 
{
    unsigned char i,j,Sum,flag;
    unsigned long OffSetAddr,AreaID;
    STRECT_AREA_ATTRIB stRectArea;  //�����������Խṹ
    unsigned char *pTmp;
    
    if(BufferLen==0) return 2;
    Sum=*pBuffer++;    
    if(Sum)
    {
        pTmp =pBuffer;
        for(i=0; i<Sum; i++)
        {
            Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pTmp[0],sizeof(unsigned long),1); //�������ID
            pTmp+=sizeof(unsigned long);
            flag =0;
            for(j=0; j<s_stAreaAttrib.CheckRectNum; j++)
            {
                OffSetAddr=RECT_AREA_ADDR_START + j*STRECT_AREA_ATTRIB_SIZE;
                sFLASH_ReadBuffer((unsigned char *)&stRectArea,OffSetAddr,AREA_HEAD_SIZE);
                if(AreaID==stRectArea.AreaID)
                {
                   flag =1;
                   break;
                }
            }
            ///////////////////////
            if(0 == flag)return 2;
            ///////////////////////
        }
        ///////////////////////////
        pTmp =pBuffer;
        for(i=0; i<Sum; i++)
        {
            Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pTmp[0],sizeof(unsigned long),1); //�������ID
            pTmp+=sizeof(unsigned long);
            for(j=0; j<s_stAreaAttrib.CheckRectNum; j++)
            {
                OffSetAddr=RECT_AREA_ADDR_START + j*STRECT_AREA_ATTRIB_SIZE;
                sFLASH_ReadBuffer((unsigned char *)&stRectArea,OffSetAddr,AREA_HEAD_SIZE);
                if(AreaID==stRectArea.AreaID&&AreaID)//��Ҫɾ��������ID
                {
                    stRectArea.AreaID = 0;  //ɾ������,ֻ���ID=0
                    s_stAreaAttrib.ValidRectNum--;
                    OffSetAddr=RECT_AREA_ADDR_START + j*STRECT_AREA_ATTRIB_SIZE;
                    stRectArea.Head =HEAD_FLAG;
                    Area_MdyDataInFlash((unsigned char*)&stRectArea,OffSetAddr,AREA_HEAD_SIZE);
                }
            }
            ////////////////////////////
            Area_RectAreaCheckForResetParam(AreaID);
            ////////////////////////////
        } 
    }
    else//ɾ������
    {
        Area_ClearRectArea();
    }
    //////////////
    return 0;
}
/*********************************************************************
//��������  :Area_GetRoundAreaDatalenStandard
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
unsigned short Area_GetRoundAreaUserDatalen(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen;
    STROUND_AREA_ATTRIB stRoundAreaAttrib;  //��ȡԲ����������
    datalen =0;    
    Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.AreaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); 
    datalen +=sizeof(unsigned long);
    //����Զ�����������   
    Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.UserAreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //�����������
    datalen +=sizeof(unsigned short ); 
    ///////�Զ���λ///////////////////////
    if(PUBLIC_CHECKBIT(stRoundAreaAttrib.UserAreaAttrib,E_USER_IN_PLAY))
    {     
        //������ʾ������
        Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0);
        datalen +=sizeof(unsigned char);  
        //��ʾ�����
        datalen += stRoundAreaAttrib.InVoiceSize;    
    }
    /////////////////////////////
    if(PUBLIC_CHECKBIT(stRoundAreaAttrib.UserAreaAttrib,E_USER_OUT_PLAY))
    {
        //������ʾ������
        Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); 
        datalen += sizeof(unsigned char);  
        //��ʾ�����
        datalen += stRoundAreaAttrib.OutVoiceSize;
    }
    ////////////////////////////
    if(datalen>BufferLen)return 0;
    ///////////////////////////
    *AreaID = stRoundAreaAttrib.AreaID;
    return datalen;
}

/*********************************************************************
//��������  :Area_GetRoundAreaDatalenStandard
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
unsigned short Area_GetRoundAreaDatalenStandard(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen;
    STROUND_AREA_ATTRIB stRoundAreaAttrib;  //��ȡԲ����������
    datalen =0;
    //�������ID
    Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.AreaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); 
    datalen +=sizeof(unsigned long);
    //�����������   
    Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.AreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //�����������
    datalen +=sizeof(unsigned short ); 
    //������ĵ�γ��
    datalen +=sizeof(unsigned long);  
    //������ĵ㾭��
    datalen +=sizeof(unsigned long);  
    //��ð뾶
    datalen +=sizeof(unsigned long);  
    //////����ʱ��////////////////////////
    if(PUBLIC_CHECKBIT(stRoundAreaAttrib.AreaAttrib,E_AREA_MOMENT))
    {
        //��ʼʱ��
        datalen += 6;  
        //����ʱ��
        datalen += 6;  
    }
    /////�������/////////////////////
    if(PUBLIC_CHECKBIT(stRoundAreaAttrib.AreaAttrib,E_AREA_LIMIT_SPEED))
    {
        //�������
        datalen +=sizeof(unsigned short);    
        //���ٳ���ʱ��
        datalen +=sizeof(unsigned char); 
    }
    ////////////////////
    if(datalen>BufferLen)return 0;
    ///////////////////////////
    *AreaID = stRoundAreaAttrib.AreaID;
    return datalen;
}
/*********************************************************************
//��������  :Area_GetRoundAreaDatalenStandard
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
unsigned short Area_GetRoundAreaDatalenJiuTong(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen;
    STROUND_AREA_ATTRIB stRoundAreaAttrib;  //��ȡԲ����������
    datalen =0;
    //�������ID
    Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.AreaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); 
    datalen +=sizeof(unsigned long);
    //�����������   
    Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.AreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //�����������
    datalen +=sizeof(unsigned short ); 
    //������ĵ�γ��
    datalen +=sizeof(unsigned long);  
    //������ĵ㾭��
    datalen +=sizeof(unsigned long);  
    //��ð뾶
    datalen +=sizeof(unsigned long);  
    //////����ʱ��////////////////////////
    if(PUBLIC_CHECKBIT(stRoundAreaAttrib.AreaAttrib,E_AREA_MOMENT))
    {
        //��ʼʱ��
        datalen += 6;  
        //����ʱ��
        datalen += 6;  
    }
    /////�������/////////////////////
    if(PUBLIC_CHECKBIT(stRoundAreaAttrib.AreaAttrib,E_AREA_LIMIT_SPEED))
    {
        //�������
        datalen +=sizeof(unsigned short);    
        //���ٳ���ʱ��
        datalen +=sizeof(unsigned char); 
    }
    //////////////////////////
    ///////�Զ���λ///////////////////////
    /////////////////////////////////
    if(PUBLIC_CHECKBIT(stRoundAreaAttrib.AreaAttrib,14))
    {     
        //������ʾ������
        Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); 
        datalen+=sizeof(unsigned char);
        //��ʾ�����
        datalen+=stRoundAreaAttrib.InVoiceSize;    
    }
    ///////////////////////
    if(PUBLIC_CHECKBIT(stRoundAreaAttrib.AreaAttrib,15))
    {
         //������ʾ������
        Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0);
        datalen+=sizeof(unsigned char);
        //��ʾ�����
        datalen+=stRoundAreaAttrib.OutVoiceSize;
    }
    ////////////////////
    if(datalen>BufferLen)return 0;
    ///////////////////////////
    *AreaID = stRoundAreaAttrib.AreaID;
    return datalen;
}
/*********************************************************************
//��������  :Area_GetRoundAreaDatalenStandard
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
unsigned short Area_GetRoundAreaDatalenSuoMei(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen=0;
    return datalen;
}
/*********************************************************************
//��������  :Area_SaveOneRoundArea
//����      :����Բ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned short Area_SaveOneRoundModifyUserData(unsigned long addr,unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long AreaID;
    unsigned short datalen,UserAreaAttrib;
    STROUND_AREA_ATTRIB stRoundAreaAttrib;  //��ȡԲ����������
    if(addr+STROUND_AREA_ATTRIB_SIZE>ROUND_AREA_ADDR_END)return 0;
    sFLASH_ReadBuffer((unsigned char*)&stRoundAreaAttrib,addr,STROUND_AREA_ATTRIB_SIZE); //������   
    datalen =0;
    Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //�������ID
    /////////////////////
    if(AreaID!=stRoundAreaAttrib.AreaID)return 0;
    //////////////////////////////
    datalen +=sizeof(unsigned long);
    Public_Mymemcpy((unsigned char *)&UserAreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //�����������
    datalen +=sizeof(unsigned short );
    /////////�Զ���///////////////////////////
    if(PUBLIC_CHECKBIT(UserAreaAttrib,E_USER_IN_PLAY))
    {        
        PUBLIC_SETBIT(stRoundAreaAttrib.UserAreaAttrib,E_USER_IN_PLAY);
        Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
        datalen +=sizeof(unsigned char);  
        Public_Mymemcpy(stRoundAreaAttrib.InVoiceStr,(unsigned char *)&pBuffer[datalen],stRoundAreaAttrib.InVoiceSize,0); //��ʾ�����
        datalen += stRoundAreaAttrib.InVoiceSize;    
    }
    if(PUBLIC_CHECKBIT(UserAreaAttrib,E_USER_OUT_PLAY))
    {
        PUBLIC_SETBIT(stRoundAreaAttrib.UserAreaAttrib,E_USER_OUT_PLAY);
        Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
        datalen += sizeof(unsigned char);  
        Public_Mymemcpy(stRoundAreaAttrib.OutVoiceStr,(unsigned char *)&pBuffer[datalen],stRoundAreaAttrib.OutVoiceSize,0); //��ʾ�����
        datalen += stRoundAreaAttrib.OutVoiceSize;
    }
    if(datalen>BufferLen)return 0;
    ///////////////////////////
    stRoundAreaAttrib.Head = HEAD_FLAG;
    Area_MdyDataInFlash((unsigned char*)&stRoundAreaAttrib,addr,STROUND_AREA_ATTRIB_SIZE);
		
		//////dxl,2016.5.7 start��Ϊ������ʱä��������������
		if(1 == BBGNTestFlag)
		{
		    if(0x4000 == (0x4000&stRoundAreaAttrib.AreaAttrib))//������ر�ͨ��ģ��
				{
				    Blind_Erase(0xff);
				}
		}
		//////dxl,2016.5.7 end
    return datalen;
}
/*********************************************************************
//��������  :Area_SaveOneRoundArea
//����      :����Բ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned short Area_SaveOneRoundArea(unsigned long addr,unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned short datalen;
    STROUND_AREA_ATTRIB stRoundAreaAttrib;  //��ȡԲ����������

    if(addr+STROUND_AREA_ATTRIB_SIZE>ROUND_AREA_ADDR_END)return 0;
    memset(&stRoundAreaAttrib,0,sizeof(STROUND_AREA_ATTRIB));
    datalen =0;
    Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.AreaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //�������ID
    datalen +=sizeof(unsigned long);
    Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.AreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //�����������
    datalen +=sizeof(unsigned short ); 
    Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.CenterLat,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //������ĵ�γ��
    datalen +=sizeof(unsigned long);  
    Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.CenterLong,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //������ĵ㾭��
    datalen +=sizeof(unsigned long);  
    Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.Radius,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //��ð뾶
    datalen +=sizeof(unsigned long);
    ////////////����ʱ��////////////////////
    if(PUBLIC_CHECKBIT(stRoundAreaAttrib.AreaAttrib,E_AREA_MOMENT))
    {
        Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.StartMoment[0],(unsigned char *)&pBuffer[datalen],6,0); //��ʼʱ��
        datalen += 6;  
        Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.EndMoment[0],(unsigned char *)&pBuffer[datalen],6,0); //����ʱ��
        datalen += 6;  
    }
    ////////////�������////////////////////
    if(PUBLIC_CHECKBIT(stRoundAreaAttrib.AreaAttrib,E_AREA_LIMIT_SPEED))
    {
        Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.LimitSpeed,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //��ʼʱ��
        datalen +=sizeof(unsigned short );    
        Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.speedDuration,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //��ʼʱ��
        datalen +=sizeof(unsigned char); 
    }
    ///////////////////
    if(AREA_PROTOCOL_TYPE_JIUTONG == s_stRoundArea.protoclType)
    {
        if(PUBLIC_CHECKBIT(stRoundAreaAttrib.AreaAttrib,14))
        {
            PUBLIC_CLRBIT(stRoundAreaAttrib.AreaAttrib,14);
            PUBLIC_SETBIT(stRoundAreaAttrib.UserAreaAttrib,E_USER_IN_PLAY);              
            Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
            datalen+=sizeof(unsigned char);  
            Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.InVoiceStr[0],(unsigned char *)&pBuffer[datalen],stRoundAreaAttrib.InVoiceSize,0); //��ʾ�����
            datalen+=stRoundAreaAttrib.InVoiceSize;    
        }
        ///////////////////////
        if(PUBLIC_CHECKBIT(stRoundAreaAttrib.AreaAttrib,15))
        {
            PUBLIC_CLRBIT(stRoundAreaAttrib.AreaAttrib,15);
            PUBLIC_SETBIT(stRoundAreaAttrib.UserAreaAttrib,E_USER_OUT_PLAY);
            Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
            datalen+=sizeof(unsigned char);  
            Public_Mymemcpy((unsigned char *)&stRoundAreaAttrib.OutVoiceStr[0],(unsigned char *)&pBuffer[datalen],stRoundAreaAttrib.OutVoiceSize,0); //��ʾ�����
            datalen+=stRoundAreaAttrib.OutVoiceSize;
        }
    }
    //////////////////////
    if(datalen>BufferLen)return 0;
    ///////////////////////////
    stRoundAreaAttrib.Head = HEAD_FLAG;
    Area_MdyDataInFlash((unsigned char*)&stRoundAreaAttrib,addr,STROUND_AREA_ATTRIB_SIZE);
    return datalen;
}
/*********************************************************************
//��������  :Area_RoundAreaUpdate(unsigned char *pBuffer, unsigned short BufferLen)
//����      :����Բ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned char Area_RoundAreaUpdate(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long OffSetAddr;
    unsigned char i,Sum;
    unsigned short datalen,templen;
    unsigned char *pTmp;
    //////////////
    if(BufferLen==0)return 2;     
    Sum=*pBuffer++; 
    if(Sum>MAX_ROUND_AREA_SUM)return 1;
    ////////////////////    
    /////////////////
    Area_ClearRoundArea();
    //////////////////
    datalen=BufferLen-1;
    pTmp=pBuffer;
    //Բ��
    Area_RoundAreaParameterInitialize();
    for(i=0; i<Sum; i++)
    {
        OffSetAddr=ROUND_AREA_ADDR_START + i*STROUND_AREA_ATTRIB_SIZE;
        templen=Area_SaveOneRoundArea(OffSetAddr,pTmp,datalen);
        if(0 == templen)break;
        s_stAreaAttrib.CheckRoundNum++;
        s_stAreaAttrib.ValidRoundNum++;
        if(s_stAreaAttrib.CheckRoundNum>=MAX_ROUND_AREA_SUM)break;
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }
    return 0;    
}
/*************************************************************
** ��������: Area_GetCurRectAreaDatalen
** ��������: ��ȡ��ǰԲ�νṹ�峤��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short Area_GetCurRoundAreaDatalen(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    if(AREA_PROTOCOL_TYPE_JIUTONG == s_stRectArea.protoclType)
    {
        return Area_GetRoundAreaDatalenJiuTong(pBuffer,BufferLen,AreaID);
    }
    else
    {
       return Area_GetRoundAreaDatalenStandard(pBuffer,BufferLen,AreaID);
    }
}
/*********************************************************************
//��������  :Area_RoundAreaAdd(unsigned char *pBuffer, unsigned short BufferLen)
//����      :����Բ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned char Area_RoundAreaAdd(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long OffSetAddr,AreaID;
    unsigned char i,j,Sum;
    STROUND_AREA_ATTRIB stRoundAreaAttrib;  //��ȡԲ����������
    unsigned short datalen,templen;
    unsigned char *pTmp;
    //////////////
    if(BufferLen==0)return 2;
    ////////////
    Sum=*pBuffer++;
    if(Sum+s_stAreaAttrib.CheckRoundNum>MAX_ROUND_AREA_SUM)return 1;
    pTmp =pBuffer;
    datalen=BufferLen-1;
    //Բ��
    for(i=0; i<Sum; i++)//���ID
    {
        templen=Area_GetCurRoundAreaDatalen(pTmp,datalen,&AreaID);
        if(0 == templen)return 2;//��Ϣ����;
        //////////////////////////
        for(j=0; j<s_stAreaAttrib.CheckRoundNum; j++)
        {
            OffSetAddr=ROUND_AREA_ADDR_START + j*STROUND_AREA_ATTRIB_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stRoundAreaAttrib,OffSetAddr,AREA_HEAD_SIZE);
            if(AreaID==stRoundAreaAttrib.AreaID)
            {
                return 1;////ʧ��
            }
        }
        ///////////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }
    //////////////////////////////////////
    pTmp =pBuffer;
    datalen=BufferLen-1;
    for(i=0; i<Sum; i++)
    {
        OffSetAddr=ROUND_AREA_ADDR_START + s_stAreaAttrib.CheckRoundNum *STROUND_AREA_ATTRIB_SIZE;
        templen=Area_SaveOneRoundArea(OffSetAddr,pTmp,datalen);
        if(0 == templen)break;
        ////////////////////
        s_stAreaAttrib.CheckRoundNum++;//
        s_stAreaAttrib.ValidRoundNum++;
        if(s_stAreaAttrib.CheckRoundNum>=MAX_ROUND_AREA_SUM)break;
        ////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }    
    return 0;    
}
/*********************************************************************
//��������  :Area_RoundAreaModify(unsigned char *pBuffer, unsigned short BufferLen)
//����      :�޸�Բ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned char Area_RoundAreaModify(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long OffSetAddr,AreaID;
    unsigned char i,j,Sum,flag;
    STROUND_AREA_ATTRIB stRoundAreaAttrib;  //��ȡԲ����������
    unsigned short datalen,templen;
    unsigned char *pTmp;
    //////////////
    if(BufferLen==0)return 2;
    ////////////
    Sum=*pBuffer++;
    pTmp =pBuffer;
    datalen=BufferLen-1;
    //Բ��
    for(i=0; i<Sum; i++)//���ID
    {        
        templen=Area_GetCurRoundAreaDatalen(pTmp,datalen,&AreaID);
        if(0 == templen)return 2;//��Ϣ����;
        //////////////////////////
        flag =0;
        for(j=0; j<s_stAreaAttrib.CheckRoundNum; j++)
        {
            OffSetAddr=ROUND_AREA_ADDR_START + j*STROUND_AREA_ATTRIB_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stRoundAreaAttrib,OffSetAddr,AREA_HEAD_SIZE);
            if(AreaID==stRoundAreaAttrib.AreaID)
            {
                flag=1;//
                break;
            }
        }
        //////////////////
        if(0==flag)return 1;//ʧ��
        ///////////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }
    //////////////////////////////////////
    pTmp =pBuffer;
    datalen=BufferLen-1;
    for(i=0; i<Sum; i++)
    {
        templen = 0;
        Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pTmp[0],sizeof(unsigned long),1); //�������ID
        for(j=0; j<s_stAreaAttrib.CheckRoundNum; j++)
        {
            OffSetAddr=ROUND_AREA_ADDR_START + j*STROUND_AREA_ATTRIB_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stRoundAreaAttrib,OffSetAddr,AREA_HEAD_SIZE);
            if(AreaID==stRoundAreaAttrib.AreaID)
            {
                templen=Area_SaveOneRoundArea(OffSetAddr,pTmp,datalen);
                break;
            }
        }
        ///////////////////////
        Area_RoundAreaCheckForResetParam(AreaID);
        /////////////////////
        if(0 == templen)break;
        ////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }    
    return 0;
}
/*************************************************************
** ��������: Area_SetRectAreaCheckProtoclType
** ��������: :��������ʱ,����Ƿ���������־,��Ϊ���ݾ�ͨ�汾
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Area_SetRoundAreaCheckProtoclType(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long AreaID;
    unsigned char i,Sum;
    unsigned short datalen,templen;
    unsigned char *pTmp;
    //////////////    
    if(BufferLen==0)return AREA_PROTOCOL_TYPE_EMPTY;
    ////////////////////////    
    Sum=*pBuffer++;
    ////////////////////
    datalen=BufferLen-1;
    pTmp=pBuffer;
    ////////////////////
    for(i=0; i<Sum; i++)
    {
        templen=Area_GetRoundAreaDatalenStandard(pTmp,datalen,&AreaID);
        if(0 == templen)//����
        {
            break;
        }
        pTmp += templen;
        //////////////////////////
        if(datalen == templen)//���,��ȷ
        {
            return AREA_PROTOCOL_TYPE_STANDARD;
        }
        else
        if(datalen > templen)//����,��������
        {
            datalen -= templen;
        }
        else////С��,����
        {
            break;
        }
    }
    /////////////////////
    datalen=BufferLen-1;
    pTmp=pBuffer;
    ////////////////////
    for(i=0; i<Sum; i++)
    {
        templen=Area_GetRoundAreaDatalenJiuTong(pTmp,datalen,&AreaID);
        if(0 == templen)//����
        {
            break;
        }
        pTmp += templen;
        
        if(datalen == templen)//���,��ȷ
        {
            return AREA_PROTOCOL_TYPE_JIUTONG;//������,��ȷ
        }
        else
        if(datalen > templen)//����,��������
        {
            datalen -= templen;
        }
        else////С��,����
        {
            break;
        }
    }
    return AREA_PROTOCOL_TYPE_EMPTY;
}
/*************************************************************
** ��������: Area_SetRoundArea
** ��������: :����Բ������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Area_SetRoundArea(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned char AckResult,type;
    unsigned char protoclType;
    if(BufferLen==0)return 2;
	
	  if((1 == BBXYTestFlag)||(1 == BBGNTestFlag))
    {
        Area_ClearRoundArea();
			  Area_ClearRectArea();
			  Polygon_ClearArea();
    }
    /////////////////
    type=*pBuffer++;
    ///////////////////
    protoclType=Area_SetRoundAreaCheckProtoclType(pBuffer,BufferLen-1);
    if(AREA_PROTOCOL_TYPE_EMPTY==protoclType)return 2;
    s_stRoundArea.protoclType = protoclType;
    //////////////
    AckResult = 0;
    
    if(0== type)//����
    {
        AckResult=Area_RoundAreaUpdate(pBuffer,BufferLen-1);
    }
    else
    if(1 == type)//���
    {   
        AckResult=Area_RoundAreaAdd(pBuffer,BufferLen-1);
    }
    else
    if(2 == type)//�޸�
    {  
        AckResult=Area_RoundAreaModify(pBuffer,BufferLen-1);
    }
    if(0==AckResult)
    {
        s_stAreaAttrib.preSpeed = 0xff;
    }
    return AckResult;
}
/*************************************************************
** ��������: Area_DelRoundArea
** ��������: :ɾ��Բ������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Area_DelRoundArea(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned char i,j,Sum,flag;
    unsigned long OffSetAddr,AreaID;
    STROUND_AREA_ATTRIB stRoundArea;  //��ȡԲ����������  
    unsigned char *pTmp;
    if(BufferLen==0)return 2;
    Sum=*pBuffer++;
    pTmp =pBuffer;
    if(Sum)
    {
        for(i=0; i<Sum; i++)
        {
            Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pTmp[0],sizeof(unsigned long),1); //�������ID
            pTmp+=sizeof(unsigned long);
            flag =0;
            for(j=0; j<s_stAreaAttrib.CheckRoundNum; j++)
            {
                OffSetAddr=ROUND_AREA_ADDR_START + j*STROUND_AREA_ATTRIB_SIZE;
                sFLASH_ReadBuffer((unsigned char *)&stRoundArea,OffSetAddr,AREA_HEAD_SIZE);
                if(AreaID==stRoundArea.AreaID)
                {
                    flag =1;
                    break;
                }
            }
            if(0 == flag)return 2;
        }        
        ///////////////////////
        pTmp =pBuffer;
        for(i=0; i<Sum; i++)
        {
            Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pTmp[0],sizeof(unsigned long),1); //�������ID
            pTmp+=sizeof(unsigned long);
            for(j=0; j<s_stAreaAttrib.CheckRoundNum; j++)
            {
                OffSetAddr=ROUND_AREA_ADDR_START + j*STROUND_AREA_ATTRIB_SIZE;
                sFLASH_ReadBuffer((unsigned char *)&stRoundArea,OffSetAddr,AREA_HEAD_SIZE);
                if(AreaID==stRoundArea.AreaID&&AreaID)
                {//��Ҫ�޸ĸ���Ŀ
                    stRoundArea.AreaID = 0;  //ɾ������,ֻ���ID=0  
                    s_stAreaAttrib.ValidRoundNum--;
                    OffSetAddr=ROUND_AREA_ADDR_START + j*STROUND_AREA_ATTRIB_SIZE;
                    stRoundArea.Head = HEAD_FLAG;
                    Area_MdyDataInFlash((unsigned char*)&stRoundArea,OffSetAddr,AREA_HEAD_SIZE); 
                }
            }
            //////////////////////////////////////
            Area_RoundAreaCheckForResetParam(AreaID);
            ////////////////////////////////////////
        }
    }
    else
    { //ɾ����������
        Area_ClearRoundArea();
    }
    
    return 0;
}
/*********************************************************************
//��������  :Area_RoundAreaModify(unsigned char *pBuffer, unsigned short BufferLen)
//����      :�޸�Բ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned char Area_RoundAreaModifyUserData(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long OffSetAddr,AreaID;
    unsigned char i,j,Sum,flag;
    STROUND_AREA_ATTRIB stRoundAreaAttrib;  //��ȡԲ����������
    unsigned short datalen,templen;
    unsigned char *pTmp;
    //////////////
    if(BufferLen==0)return 2;
    ////////////
    Sum=*pBuffer++;
    pTmp =pBuffer;
    datalen=BufferLen-1;
    //Բ��
    for(i=0; i<Sum; i++)//���ID
    {        
        templen=Area_GetRoundAreaUserDatalen(pTmp,datalen,&AreaID);
        if(0 == templen)return 2;//��Ϣ����;
        //////////////////////////
        flag =0;
        for(j=0; j<s_stAreaAttrib.CheckRoundNum; j++)
        {
            OffSetAddr=ROUND_AREA_ADDR_START + j*STROUND_AREA_ATTRIB_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stRoundAreaAttrib,OffSetAddr,AREA_HEAD_SIZE);
            if(AreaID==stRoundAreaAttrib.AreaID)
            {
                flag=1;//
                break;
            }
        }
        //////////////////
        if(0==flag)return 1;//ʧ��
        ///////////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }
    //////////////////////////////////////
    pTmp =pBuffer;
    datalen=BufferLen-1;
    for(i=0; i<Sum; i++)
    {
        templen = 0;
        Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pTmp[0],sizeof(unsigned long),1); //�������ID
        for(j=0; j<s_stAreaAttrib.CheckRoundNum; j++)
        {
            OffSetAddr=ROUND_AREA_ADDR_START + j*STROUND_AREA_ATTRIB_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stRoundAreaAttrib,OffSetAddr,AREA_HEAD_SIZE);
            if(AreaID==stRoundAreaAttrib.AreaID)
            {
                templen=Area_SaveOneRoundModifyUserData(OffSetAddr,pTmp,datalen);
                break;
            }
        }
        /////////////////////
        if(0 == templen)break;
        ////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }    
    return 0;
}
/*********************************************************************
//��������  :Area_RectAreaModify(unsigned char *pBuffer, unsigned short BufferLen)
//����      :�޸ľ�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
unsigned char Area_RectAreaModifyUserData(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long OffSetAddr,AreaID;
    unsigned char i,j,Sum,flag;
    STRECT_AREA_ATTRIB stRectAreaAttrib;  //��ȡ������������
    unsigned short datalen,templen;
    unsigned char *pTmp;
    //////////////
    if(BufferLen==0)return 2;
    ////////////
    Sum=*pBuffer++;
    pTmp =pBuffer;
    datalen=BufferLen-1;
    //����
    for(i=0; i<Sum; i++)//���ID
    {        
        templen=Area_GetRectAreaUserDatalen(pTmp,datalen,&AreaID);
        if(0 == templen)return 2;//��Ϣ����;
        //////////////////////////
        flag =0;
        for(j=0; j<s_stAreaAttrib.CheckRectNum; j++)
        {
            OffSetAddr=RECT_AREA_ADDR_START + j*STRECT_AREA_ATTRIB_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stRectAreaAttrib,OffSetAddr,AREA_HEAD_SIZE);
            if(AreaID==stRectAreaAttrib.AreaID)
            {
                flag=1;//
                break;
            }
        }
        //////////////////
        if(0==flag)return 1;//ʧ��;
        ///////////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }
    //////////////////////////////////////
    pTmp =pBuffer;
    datalen=BufferLen-1;
    for(i=0; i<Sum; i++)
    {
        templen = 0;
        Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pTmp[0],sizeof(unsigned long),1); //�������ID
        for(j=0; j<s_stAreaAttrib.CheckRectNum; j++)
        {
            OffSetAddr=RECT_AREA_ADDR_START + j*STRECT_AREA_ATTRIB_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stRectAreaAttrib,OffSetAddr,AREA_HEAD_SIZE);
            if(AreaID==stRectAreaAttrib.AreaID)
            {
                templen=Area_SaveOneRectModifyUserData(OffSetAddr,pTmp,datalen);
                break;
            }
        }
        /////////////////
        if(0 == templen)break;
        ////////////////////
        pTmp +=templen;
        if(datalen>templen)datalen -=templen;
        else break;
    }    
    return 0;    
}
/*************************************************************
** ��������: Area_DisposeEIExpandProtocol
** ��������: ��������������չЭ��
** ��ڲ���: pBuffer:��Ϣ�������׵�ַ,BufferLen��Ϣ�����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Area_DisposeEIExpandProtocol(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned char type;
    unsigned char *p;
    if(BufferLen<3)return 0;
    p = pBuffer;
    type = *p++;//��������
    //�����ֽ�
    p++;
    switch(type)
    {
        case E_ROUND_AREA:
            Area_RoundAreaModifyUserData(p,BufferLen-2);
            break;
        case E_RECT_AREA:
            Area_RectAreaModifyUserData(p,BufferLen-2);
            break;
        case E_POLYGON_AREA:
            #ifdef __POLYGON__H_
            Polygon_AreaModifyUserData(p,BufferLen-2);
            #endif
            break;
    }
    return 1;
}
/*************************************************************
** ��������: Area_DisposeReadAreaProtocol
** ��������: ��������������Э��
** ��ڲ���: pBuffer:��Ϣ�������׵�ַ,BufferLen��Ϣ�����ݳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Area_DisposeReadAreaProtocol(unsigned char *pBuffer, unsigned short BufferLen)
{
    //unsigned char *pTmp;
    //pTmp=pBuffer;
    //if(BufferLen !=4)
    return 0;
}
/*************************************************************
** ��������: Area_GetRoundAreaTotalNum
** ��������: ���Բ����������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: Բ����������
*************************************************************/ 
unsigned char Area_GetRoundAreaTotalNum(void)
{

    return s_stAreaAttrib.ValidRoundNum;
}
/*************************************************************
** ��������: Area_GetRectAreaTotalNum
** ��������: ��þ�����������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: ������������
*************************************************************/ 
unsigned char Area_GetRectAreaTotalNum(void)
{

    return s_stAreaAttrib.ValidRectNum;
}
/*************************************************************
** ��������: AreaManage_ScanTask
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_CheckScanTask(void)
{
    unsigned char speed;
    //���������򱨾�����λ״̬
    if(Io_ReadAlarmMaskBit(ALARM_BIT_IN_OUT_AREA))
    {
        if(1==s_stAreaAttrib.nonavigationCnt)
        {
            s_stAreaAttrib.nonavigationCnt = 0;
            Area_ClearAllAlarm(E_RECT_AREA);
            Area_ClearAllAlarm(E_ROUND_AREA);
        }
        return;
    }
    Area_GetCurGpsCoordinate();//1s��ȡһ�ξ�γ��        
    s_stAreaAttrib.nonavigationCnt =1;    
    s_stRoundArea.preAreaCnt=s_stRoundArea.AreaCnt;
    s_stRectArea.preAreaCnt=s_stRectArea.AreaCnt;
    speed = Area_GetCurSpeed();
    ///////////////////////////
    if(0==speed&&0==s_stAreaAttrib.preSpeed)return;//�ٶ�Ϊ��,������
    /////////////////////////
    s_stAreaAttrib.preSpeed = speed;   
    if(s_stAreaAttrib.CheckRectNum)
    Area_ScanRectArea();
    if(s_stAreaAttrib.CheckRoundNum)
    LZM_PublicSetOnceTimer(&s_stAREATimer[AREA_TIMER_SCAN_ROUND],PUBLICSECS(0.1),Area_ScanRoundArea);
}
/*************************************************************
** ��������: AreaManage_ScanTask
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void AreaManage_ScanTask(void)
{
    if(Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
    {
        GPS_STRUCT stTmpGps;
        Gps_CopygPosition(&stTmpGps);
        if(stTmpGps.Second!=s_stAreaAttrib.preSecond)
        {
            s_stAreaAttrib.preSecond = stTmpGps.Second; 
            LZM_PublicKillTimer(&s_stAREATimer[AREA_TIMER_SCAN_RECT]);
            Area_CheckScanTask();
        }
    }
    else
    {  
        s_stAreaAttrib.preSecond = 0xff;
        s_stAreaAttrib.preSpeed = 0xff;
        if(s_stAreaAttrib.nonavigationCnt<50)s_stAreaAttrib.nonavigationCnt++;//10s
        else
        if(s_stAreaAttrib.nonavigationCnt == 50)
        {
            s_stAreaAttrib.nonavigationCnt =100;
            LZM_PublicKillTimer(&s_stAREATimer[AREA_TIMER_SCAN_RECT]);
            LZM_PublicKillTimer(&s_stAREATimer[AREA_TIMER_SCAN_ROUND]);
            Area_RectAreaResetParam();//modify by joneming 20150105
            Area_RoundAreaResetParam();//modify by joneming 20150105
            //Area_ClearAllAlarm(E_RECT_AREA);
            //Area_ClearAllAlarm(E_ROUND_AREA);
        }
    }
}

/*************************************************************
** ��������: AreaManage_ScanTask
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void AreaManage_StartScanTask(void)
{
    LZM_PublicSetCycTimer(&s_stAREATimer[AREA_TIMER_TASK],PUBLICSECS(0.2),AreaManage_ScanTask);
}
/*************************************************************
** ��������: AreaManage_TimeTask
** ��������: �����ܶ�ʱ��������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
FunctionalState AreaManage_TimeTask(void)
{
    LZM_PublicTimerHandler(s_stAREATimer,AREA_TIMERS_MAX);
    return ENABLE;
}
/*************************************************************
** ��������: Area_GetInOutAreaAlarmSubjoinInfo
** ��������: ��ý������򱨾�������Ϣ��(����������ϢID�����ȡ�������Ϣ��) :��������ϢIDΪ0x12����������
** ��ڲ���: ��Ҫ����������򱨾�������Ϣ�����׵�ַ
** ���ڲ���: 
** ���ز���: �������򱨾�������Ϣ�����ܳ���,0:��ʾδ��������
*************************************************************/
unsigned char Area_GetInOutAreaAlarmSubjoinInfo(unsigned char *data) 
{
    unsigned char len; 
    unsigned char buffer[20]={0};

    if(!Io_ReadAlarmBit(ALARM_BIT_IN_OUT_AREA))return 0;
    len =0;
    buffer[len++]= IN_OUT_AREA_OVERHEAD_INFO_ID;
    buffer[len++]= IN_OUT_AREA_OVERHEAD_INFO_LEN;    
    buffer[len++]= s_stAreaAttrib.INOUTPostType;
    Public_ConvertLongToBuffer(s_stAreaAttrib.InOutAreaID,&buffer[len]);
    len += 4;
    buffer[len++]= s_stAreaAttrib.Orientation;
    memcpy(data,buffer,len);

    return len;
    
}
/*************************************************************
** ��������: Area_ReadAndCheckParam
** ��������: ���������ʼ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_ReadAndCheckParam(void) 
{
    unsigned long OffSetAddr;
    unsigned short i;
    
    STRECT_AREA_ATTRIB stRectArea;    //��ȡ������������
    STROUND_AREA_ATTRIB stRoundArea;  //��ȡԲ����������
   
    s_stAreaAttrib.preSecond        = 0xff;
    s_stAreaAttrib.nonavigationCnt  = 0xff;
    s_stAreaAttrib.InOutAreaAlarm   = 0;
    s_stAreaAttrib.CloseGPRSFlag    = 0;
    s_stAreaAttrib.CollectGnssFlag  = 0;
    s_stAreaAttrib.InAreaFlag       = 0;
    s_stAreaAttrib.OpenDoorAlarmFlag= 0;
    ////////////
    //���γ�ʼ��    
    Area_RectAreaParameterInitialize();
    ////////////////////////
    for(i=0; i<MAX_RECT_AREA_SUM; i++)
    {
        OffSetAddr=RECT_AREA_ADDR_START + i*STRECT_AREA_ATTRIB_SIZE ;  
        sFLASH_ReadBuffer((unsigned char *)&stRectArea,OffSetAddr,AREA_HEAD_SIZE);//������������    
        if(HEAD_FLAG == stRectArea.Head)
        {
            if(0xFFFFFFFF == stRectArea.AreaID)
            {
                Area_ClearRectArea();
                break;
            }
            else
            {
                s_stAreaAttrib.CheckRectNum++;
                if(stRectArea.AreaID)
                {
                    s_stAreaAttrib.ValidRectNum++;
                }
            }
        }
        else 
        if(stRectArea.Head != 0xFFFFFFFF)
        {
            Area_ClearRectArea();
            break;
        }
        else 
        if(0xFFFFFFFF == stRectArea.Head)
        {
            break;
        }
    }    
    //Բ�γ�ʼ��
    Area_RoundAreaParameterInitialize();
    for(i=0; i<MAX_ROUND_AREA_SUM; i++)
    {
        OffSetAddr=ROUND_AREA_ADDR_START + i*STROUND_AREA_ATTRIB_SIZE ;  
        sFLASH_ReadBuffer((unsigned char *)&stRoundArea,OffSetAddr,AREA_HEAD_SIZE);//������������   
        if(HEAD_FLAG == stRoundArea.Head)
        {
            if(0xFFFFFFFF == stRoundArea.AreaID)
            {
                Area_ClearRoundArea();
                break;
            }
            else
            {
                s_stAreaAttrib.CheckRoundNum++;
                if(stRoundArea.AreaID)
                {
                    s_stAreaAttrib.ValidRoundNum++;
                }
            }
        }
        else 
        if(0xFFFFFFFF != stRoundArea.Head)
        {
            Area_ClearRoundArea();
            break;
        }
        else 
        if(0xFFFFFFFF == stRoundArea.Head)
        {
            break;
        }
    }
    //////////////////////////////  
    LZM_PublicSetOnceTimer(&s_stAREATimer[AREA_TIMER_TASK],PUBLICSECS(15),AreaManage_StartScanTask);    
}
/*************************************************************
** ��������: Area_ParameterInitialize
** ��������: ���������ʼ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Area_ParameterInitialize(void) 
{
    LZM_PublicKillTimerAll(s_stAREATimer,AREA_TIMERS_MAX); 
    //////////////////////////////
    SetTimerTask(TIME_AREA,SYSTICK_0p1SECOND);
    //////////////////////////////////////////////////////////
    LZM_PublicSetOnceTimer(&s_stAREATimer[AREA_TIMER_TASK],PUBLICSECS(15),Area_ReadAndCheckParam);    
}
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

