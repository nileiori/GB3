/*******************************************************************************
 * File Name:			PolygonManager.c 
 * Function Describe:	
 * Relate Module:		������
 * Writer:				mark
 * Date:				2013-10-10
 ******* ��ӿ��ű������ܡ��ر�ͨѶģ�鹦�ܡ��ɼ�GNSS��ϸ��λ���ݹ��ܣ�
 ******* �޸Ľ��������������ʾ����λ��:��ԭ����λ14��15�޸�Ϊ:12��13
 *******************************************************************************/
#include"include.h"
#include "VDR_Overspeed.h"

/******************************************************************************
*����״̬��ö�����Ͷ���
******************************************************************************/

/*****************************************************************************
*��ز�����Ԥ����
*****************************************************************************/
#define POLYGON_MAX_AREA_ADDR                       0xFFFFFFFF
#define POLYGON_AREA_NUM_MAX                        20//20 at the most
#define ONE_POLYGON_VERTEX_NUM_MAX                  200//(256=12ms)
#define ONE_BLOCK_READ_VERTEX_NUM                   64//���ζ�ȡ�Ķ�����
#define ONE_BLOCK_READ_VERTEX_DATALEN               (ONE_BLOCK_READ_VERTEX_NUM<<3)//���ζ�ȡ�����ݳ���
///////////////////////////////////
#define  POLYGON_SAVE_FLAG              0xDCBAABCD
#define  POLYGON_HEAD_ID_SIZE           8//Define the size of head flag for polygon area
#define  SIZE_STPOLYGON_PARAM           sizeof(STPOLYGON_PARAM)//Define the size of area attrib for polygon area
#define  ONE_POLYGON_AREA_SIZE          (SIZE_STPOLYGON_PARAM+(ONE_POLYGON_VERTEX_NUM_MAX<<3))
//////////////////////
#define  VOICE_BYTE       50                         //Define the value for voice_byte as 50
////////////////////////////
#ifndef    FLASH_ONE_SECTOR_BYTES
#define    FLASH_ONE_SECTOR_BYTES                      ONE_SECTOR_BYTES
#endif

#ifndef    FLASH_POLYGON_AREA_START_SECTOR
#define    FLASH_POLYGON_AREA_START_SECTOR             POLYGON_AREA_START_SECTOR                                 //Define the start flash sector for polygon area
#endif

#ifndef    FLASH_POLYGON_AREA_END_SECTOR
#define    FLASH_POLYGON_AREA_END_SECTOR               POLYGON_AREA_END_SECTOR                                  //Define  the end  flash  sector for polygon area 
#endif

#define    POLYGON_AREA_ADDR_START    (unsigned long)(FLASH_POLYGON_AREA_START_SECTOR*FLASH_ONE_SECTOR_BYTES)    //Define the start address for polygon area
#define    POLYGON_AREA_ADDR_END      (unsigned long)(FLASH_POLYGON_AREA_END_SECTOR*FLASH_ONE_SECTOR_BYTES)      //Define the end   address for polygon area

/*****************************************************************************
*���򱨾���ʾ��ö�ٶ���
******************************************************************************/
typedef enum
{
    ALARM_ITEM_IN_AREA,                 //Define one alarm as in polygon area
    ALARM_ITEM_OUT_AREA                 //Define one alarm as out polygon area
}E_POLYGON_ALARM_ITEM;
/*****************************************************************************
*������������Խṹ
******************************************************************************/
typedef struct //160�ֽ�
{
    unsigned long   Head;                   //�����־
    unsigned long   areaID;                 //����ID
    unsigned short  areaAttrib;             //��������
    unsigned short  vertexNum;              //����ζ�����
    unsigned char   startMoment[6];         //��ʼʱ��
    unsigned char   endMoment[6];           //����ʱ��    
    unsigned short  limitspeed;             //����ֵ
    unsigned short  userAreaAttrib;         //�Զ�����������
    unsigned char   speedDuration;          //����ʱ��
    unsigned char   InVoiceSize;            //������������С
    unsigned char   OutVoiceSize;           //������������С
    unsigned char   reserve[13];
    T_GPS_POS       minpoint;               //��С�㾭γ��
    T_GPS_POS       maxpoint;               //���㾭γ��
    unsigned char   InVoiceStr[VOICE_BYTE]; //Define an array of invoice
    unsigned char   OutVoiceStr[VOICE_BYTE];//Define an array of outvoice
}STPOLYGON_PARAM;
///////////////////
/*****************************************************************************
*������ƿ鶨��
******************************************************************************/
typedef struct
{
    T_GPS_POS      minpoint;            //��С�㾭γ��
    T_GPS_POS      maxpoint;            //���㾭γ��
    T_GPS_POS      endpoint;            //�յ㾭γ��
    T_GPS_POS      curpoint;            //��ǰ�㾭γ��
    unsigned long  addr;                //Define the address for area ,no any area is in as the address is max_area_num
    unsigned long  vertexAddr;          //Define the address for area ,no any area is in as the address is max_area_num
    unsigned long  startTime;           //Define  starttime
    unsigned long  endTime;               //Define  endtime
    unsigned long  areaID;                //Define  the area id for a defined area    
    unsigned short areaAttrib;            //Define  the attrib for a defined area
    unsigned short vertexNum;              //Define the number of vertexs for polygon area
    unsigned short userAreaAttrib;        //Define  the user attrib for a defined area
    unsigned short limitSpeed;            //Define  the limit speed value for a defined area
    unsigned char  speedDuration;         //Define  an duration for overspeed    
    unsigned char  checkAreaNum;          //Define  the number of polygon area
    unsigned char  validAreaNum;          //Define  the number of polygon area    
    unsigned char  preSecond;             //Define  the second value of current moment
    unsigned char  noNavigationCnt;       //Define  a counter as no navigation
    unsigned char  areaCnt;               //Define  an area counter
    unsigned char  preAreaCnt;            //Define  a current area counter
    unsigned char  areaFlag;              //Define  the flag for a defined area
    unsigned char  timeType;              //Define  the time type
    unsigned char  preSpeed;
    unsigned char  protoclType;
    unsigned char  voiceStr[VOICE_BYTE];  //Define  an array for prompt voice    
}STPOLYGON_ATTRIB;//
////////////////////////////////////
static STPOLYGON_ATTRIB s_stPolygonAttrib;//Define  a STPOLYGON_ATTRIB type area
///////////////////////
enum
{
    POL_TIMER_TASK,
    POL_TIMER_SCAN,
    POL_TIMER_TEST,
    POL_TIMERS_MAX
}STPOLYGONAREATIME;                                  //Define an enum type variable  stareatime
//////////////////////////

extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ������ģʽ
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ

static  LZM_TIMER s_stPOLTimer[POL_TIMERS_MAX]; //Define a LZM_TIMER type array s_stPOLTimer
////////////////////////////
extern void Area_ClearAllAlarm(unsigned char type);
extern void Area_SetCollectGnssFlag(unsigned char type);
extern void Area_SetCloseGPRSFlag(unsigned char type);
extern void Area_SetInAreaFlag(unsigned char type);
/*****************************************************************************
*��������
******************************************************************************/
/*************************************************************
** ��������: Area_RoundAreaResetParam
** ��������: Բ���������ò���
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Polygon_ResetParam(void) 
{
    s_stPolygonAttrib.preSpeed = 0xff;
    s_stPolygonAttrib.areaCnt   = 0;
    s_stPolygonAttrib.addr      = POLYGON_MAX_AREA_ADDR;
    s_stPolygonAttrib.areaFlag  = AREA_STATUS_MAX;
    ////////////////////////////
    Area_ClearAllAlarm(E_POLYGON_AREA);
}
/*****************************************************************************
*Function Name:  Polygon_AreaParameterInit()
*Input  Parameter:  None
*Output Parameter:  None
*Function Describtion:Initialize the polygon area parameter 
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
void Polygon_AreaParameterInit(void) 
{
    Polygon_ResetParam();
    s_stPolygonAttrib.checkAreaNum = 0;
    s_stPolygonAttrib.validAreaNum = 0;
}
/*************************************************************
** ��������: Polygon_CheckForResetParam
** ��������: ����Ƿ����ò���
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Polygon_CheckForResetParam(unsigned long AreaID) 
{
    if(s_stPolygonAttrib.areaFlag==AREA_STATUS_IN&&s_stPolygonAttrib.areaID==AreaID)
    {
        Polygon_ResetParam();
    }
}
/*****************************************************************************
*Function Name: Polygon_ClearArea()
*Input  Parameter:  None
*Output Parameter:  None
*Function Describtion:clear Polygon area flash sector  
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
void Polygon_ClearArea(void)
{
    Polygon_AreaParameterInit();
    Area_EraseFlash(FLASH_POLYGON_AREA_START_SECTOR,FLASH_POLYGON_AREA_END_SECTOR); //erase flash
}
/*****************************************************************************
*Function Name: Polygon_ComparePos(T_GPS_POS *Polygon,unsigned char VertexSum,T_GPS_POS *s_stPolygonAttrib.curpoint)
*Input  Parameter:  polygon,vertexsum,current position
*Output Parameter:  
*Function Describtion:Check the relation between current position and polygon area  
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
void Polygon_ReadEndPointData(void)
{
    unsigned long addr;
    addr = ((s_stPolygonAttrib.vertexNum-1)<<3);
    addr += s_stPolygonAttrib.vertexAddr;
    sFLASH_ReadBuffer((unsigned char *)&s_stPolygonAttrib.endpoint,addr,8);
}
/*****************************************************************************
*Function Name: Polygon_GetCurGpsCoordinate()
*Input  Parameter:  None
*Output Parameter:  None
*Function Describtion:Get GPS Coordinate of current location  
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
void Polygon_GetCurGpsCoordinate(void)
{  
    unsigned char buffer[28];
    Public_GetCurBasicPositionInfo(buffer);
    Public_Mymemcpy((unsigned char *)&s_stPolygonAttrib.curpoint.Lat,(unsigned char *)&buffer[8],4,1); 
    Public_Mymemcpy((unsigned char *)&s_stPolygonAttrib.curpoint.Long,(unsigned char *)&buffer[12],4,1);    
}
/*****************************************************************************
*Function Name: Polygon_CheckIsInMinRectArea
*Input  Parameter: polygon area
*Output Parameter:  
*Function Describtion:
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
unsigned char Polygon_CheckIsInMinRectArea(void)  
{
   if((s_stPolygonAttrib.curpoint.Lat < s_stPolygonAttrib.minpoint.Lat)
      ||(s_stPolygonAttrib.curpoint.Lat > s_stPolygonAttrib.maxpoint.Lat)
      ||(s_stPolygonAttrib.curpoint.Long < s_stPolygonAttrib.minpoint.Long)
      ||(s_stPolygonAttrib.curpoint.Long > s_stPolygonAttrib.maxpoint.Long))
        return 1;
   return 0;
}
/*****************************************************************************
*Function Name: Polygon_CalTrigonOrientation
*Input  Parameter: 
*Output Parameter:  
*Function Describtion:
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
unsigned char Polygon_CalTrigonOrientation(T_GPS_POS *curpoint,T_GPS_POS *point1,T_GPS_POS *point2)
{
    double dVal;
    double temp1,temp2,temp3;
    if(point2->Long==point1->Long)
    {
        dVal = point1->Lat;
    }
    else
    {
        temp1 = (double)curpoint->Long  - (double)point1->Long;
        temp2 = (double)point2->Lat     - (double)point1->Lat;
        temp3 = (double)point2->Long    - (double)point1->Long;
        dVal  = (temp1*temp2)/temp3;
        dVal  = point1->Lat + dVal;
    }
    //////////////////
    if(dVal == curpoint->Lat)//�ڱ���
    {
        return 0xff;
    }
    else
    if(dVal < curpoint->Lat)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    //////////////////
    //return (dVal<curpoint->Lat)?1:0;
}
/*************************************************************
** ��������: Polygon_ComparePosBlock
** ��������: �ֿ��ж϶����
** ��ڲ���: addr
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Polygon_ComparePosBlock(unsigned long addr,unsigned char pointnum,unsigned char oddNodes,T_GPS_POS *point2)
{
    T_GPS_POS vertex[ONE_BLOCK_READ_VERTEX_NUM];
    T_GPS_POS *curpoint,*point1;
    unsigned short datalen;
    unsigned char i,result;
    curpoint = &s_stPolygonAttrib.curpoint;
    datalen  = (pointnum<<3);
    sFLASH_ReadBuffer((unsigned char *)vertex,addr,datalen);
    for(i=0; i< pointnum; i++)
    {
        point1 =&vertex[i];
        if((point1->Lat==curpoint->Lat)&&(point1->Long==curpoint->Long))//֮ǰ���㷨����߽�ʱ��׼
        {
            return 0xff;//Ϊ���е�һ������
        }
        else
        if(point1->Long==curpoint->Long&&point2->Long==curpoint->Long)//������ͬ
        {
            if((point1->Lat<curpoint->Lat&&curpoint->Lat<point2->Lat)
             ||(point1->Lat>curpoint->Lat&&curpoint->Lat>point2->Lat))
            {
                return 0xff;//Ϊ����һ����
            }
        }
        else
        if((point1->Long< curpoint->Long && point2->Long>=curpoint->Long)
         ||(point1->Long>=curpoint->Long && point2->Long< curpoint->Long))
        {
            result = Polygon_CalTrigonOrientation(curpoint,point1,point2);
            if(0xff==result)return 0xff;//���Ȳ�ͬ,Ϊ����һ����
            oddNodes ^= result;
        }
        ///////////////////////////
        //point2 = point1;//�����Ǹ���ַ,modify by joneming20150108 
        ///////���������Ż�Ѹı���ֵ����ȥ///////
        *point2 = *point1;//�����Ǹ�ֵ,modify by joneming20150108 
    }
    return oddNodes;
}
/*************************************************************
** ��������: Polygon_ComparePos
** ��������: �Ƿ��ڶ������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 0:����,1:��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Polygon_ComparePos(void)
{
    T_GPS_POS point;
    unsigned long addr; 
    unsigned char oddNodes = 0;
    unsigned char i,block,pointnum;    
    ///////////////////////////////////    
    point   = s_stPolygonAttrib.endpoint;
    addr    = s_stPolygonAttrib.vertexAddr;
    /////////////////////    
    block   = s_stPolygonAttrib.vertexNum/ONE_BLOCK_READ_VERTEX_NUM;
    ///////////////////////
    for(i=0; i<block; i++)
    {
        oddNodes = Polygon_ComparePosBlock(addr,ONE_BLOCK_READ_VERTEX_NUM,oddNodes,&point);
        if(0xff==oddNodes)return 1;//Ϊ���е�һ������
        addr += ONE_BLOCK_READ_VERTEX_DATALEN;
    }
    //////////////////////
    pointnum = s_stPolygonAttrib.vertexNum%ONE_BLOCK_READ_VERTEX_NUM;    
    if(pointnum)
    {
        oddNodes = Polygon_ComparePosBlock(addr,pointnum,oddNodes,&point);
    }
    return oddNodes;
}

/*************************************************************
** ��������: Polygon_IsInArea
** ��������: �Ƿ��ڶ������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 0:����,1:��
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Polygon_IsInArea(void)  
{
    if(Polygon_CheckIsInMinRectArea())return 0;
    return Polygon_ComparePos();
}
/*************************************************************
** ��������: Polygon_CheckTime
** ��������: �����·�Ƿ����ʱ�䷶Χ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 0:������ʱ�䷶Χ,1:����ʱ�䷶Χ
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Polygon_CheckTime(void)
{
    u32 curTime = 0;
    //�Ƿ���Ҫ���ʱ��
    if(!PUBLIC_CHECKBIT(s_stPolygonAttrib.areaAttrib,E_AREA_MOMENT))return 1; //����Ҫ���ʱ��                  
    curTime=Area_GetConvertCurTime(s_stPolygonAttrib.timeType); //��ǰʱ��  
    return Public_CheckTimeIsInRange(curTime,s_stPolygonAttrib.startTime,s_stPolygonAttrib.endTime);    
}
/*****************************************************************************
*Function Name: Polygon_DisposeDriveOutArea(void)
*Input  Parameter: None
*Output Parameter: None
*Function Describtion:Dispose the status as drive out of polygon area  
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
void Polygon_ClearInOutAlarm(void)                //Clear in/out polygon area alarm
{
    Area_ClearInOutAreaAlarmBit(E_POLYGON_AREA);
}
/*****************************************************************************
*Function Name: Polygon_DisposeDriveOutArea(void)
*Input  Parameter: None
*Output Parameter: None
*Function Describtion:Dispose the status as drive out of polygon area  
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
void Polygon_DisposeDriveOutArea(void) 
{
    Area_ClearAllAlarm(E_POLYGON_AREA);
    if(PUBLIC_CHECKBIT(s_stPolygonAttrib.areaAttrib,E_OUT_SENDTO_DRIVER)) 
    {
        Public_PlayTTSVoiceStr(s_stPolygonAttrib.voiceStr);
    }    
    /////////////////////////////
    if(PUBLIC_CHECKBIT(s_stPolygonAttrib.areaAttrib,E_OUT_SENDTO_SERVER)) 
    {
        Area_SetInOutAreaAlarmInfo(E_POLYGON_AREA, s_stPolygonAttrib.areaID,AREA_STATUS_OUT);
    }
    //////////////////////////
    if(PUBLIC_CHECKBIT(s_stPolygonAttrib.userAreaAttrib,E_USER_NO_CHANGE_LIMIT_SPEED))//�����򲻸�������ֵ
    { 
        if(PUBLIC_CHECKBIT(s_stPolygonAttrib.areaAttrib,E_AREA_LIMIT_SPEED))//
        {
//            SpeedMonitor_SetSpeedParamValid(E_SPEED_TYPE_GENERAL,0,s_stPolygonAttrib.limitSpeed,s_stPolygonAttrib.speedDuration); dxl,2015.9,
        }
        ////////////////////////////////
    }
}
/*****************************************************************************
*Function Name: Area_DisposeDriveInRectArea
*Input  Parameter: None
*Output Parameter: None
*Function Describtion:Dispose the status as drive in the polygon area  
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
void Polygon_DisposeDriveInArea(void) 
{
    Area_ClearAllAlarm(E_POLYGON_AREA);
    ///////////////////////
    if(PUBLIC_CHECKBIT(s_stPolygonAttrib.areaAttrib,E_AREA_LIMIT_SPEED))
    {
//        SpeedMonitor_SetSpeedParamValid(E_SPEED_TYPE_POLYGON_AREA,s_stPolygonAttrib.areaID,s_stPolygonAttrib.limitSpeed,s_stPolygonAttrib.speedDuration); dxl,2015.9,
			VDROverspeed_SetAreaParameter(E_POLYGON_AREA,s_stPolygonAttrib.areaID,s_stPolygonAttrib.limitSpeed&0xff,s_stPolygonAttrib.speedDuration);
    }
    /////////////////Ԥ������////////////////
    ////////////////////////////
    if(PUBLIC_CHECKBIT(s_stPolygonAttrib.areaAttrib,E_IN_SENDTO_DRIVER)) 
    {
        Public_PlayTTSVoiceStr(s_stPolygonAttrib.voiceStr);
    }
    ///////////////////////
    if(PUBLIC_CHECKBIT(s_stPolygonAttrib.areaAttrib,E_AREA_ENTER_TURNOFF_GPRS))
    {
        Area_SetCloseGPRSFlag(E_POLYGON_AREA);
    }
    //////////////////////////
    if(PUBLIC_CHECKBIT(s_stPolygonAttrib.areaAttrib,E_AREA_ENTER_COLLECT_GNSS))
    {
        Area_SetCollectGnssFlag(E_POLYGON_AREA);
    }
    ///////////////////////////////
    if(PUBLIC_CHECKBIT(s_stPolygonAttrib.areaAttrib,E_IN_SENDTO_SERVER)) 
    {
        Area_SetInOutAreaAlarmInfo(E_POLYGON_AREA, s_stPolygonAttrib.areaID,AREA_STATUS_IN);
    }
    //////////////
    Area_SetInAreaFlag(E_POLYGON_AREA);
}
/*****************************************************************************
*Function Name: Polygon_ChoiceAreaArmDest(unsigned char AlarmItem)
*Input  Parameter: AlarmItem
*Output Parameter: None
*Function Describtion:Select an area alarm destination 
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
void Polygon_ChoiceAreaArmDest(unsigned char AlarmItem) 
{
    switch(AlarmItem)
    {
        case ALARM_ITEM_OUT_AREA:
            Polygon_DisposeDriveOutArea();                    
            break;        
        case  ALARM_ITEM_IN_AREA:
            Polygon_DisposeDriveInArea();            
            break;                                                                  
        default:
            break;    
    }
}
/*****************************************************************************
*Function Name: Polygon_ScanArea(void)
*Input  Parameter: None
*Output Parameter: None
*Function Describtion:Execute the task scan polygon area  
*Writer:mark
*Update Data:2013.10.11
******************************************************************************/
void Polygon_ScanArea(void) 
{
    unsigned char i,j,len;
    unsigned long addr;
    STPOLYGON_PARAM stPolygonParam;                                  //Define a STPOLYGON_PARAM type stPolygonParam
    if(AREA_STATUS_IN != s_stPolygonAttrib.areaFlag)                              //Out of the polygon area
    {
        if(POLYGON_MAX_AREA_ADDR == s_stPolygonAttrib.addr)                               //No any polygon area be in
        {
            for(i=0; s_stPolygonAttrib.areaCnt < s_stPolygonAttrib.checkAreaNum && i<10; s_stPolygonAttrib.areaCnt++,i++)
            {
                addr = POLYGON_AREA_ADDR_START + s_stPolygonAttrib.areaCnt*ONE_POLYGON_AREA_SIZE;  
                sFLASH_ReadBuffer((unsigned char *)&stPolygonParam,addr,SIZE_STPOLYGON_PARAM); //Read the data in flash	
                if(0 == stPolygonParam.areaID)//Override the status ID=0
                    continue;
                s_stPolygonAttrib.vertexAddr    = addr + SIZE_STPOLYGON_PARAM;
                s_stPolygonAttrib.areaAttrib    = stPolygonParam.areaAttrib;
                s_stPolygonAttrib.timeType      = Area_GetConverseBCDTimeType(stPolygonParam.startMoment);
                s_stPolygonAttrib.startTime     = Area_ConverseBcdGmtime(&stPolygonParam.startMoment[0],s_stPolygonAttrib.timeType);
                s_stPolygonAttrib.endTime       = Area_ConverseBcdGmtime(&stPolygonParam.endMoment[0],s_stPolygonAttrib.timeType);
                if(0 == Polygon_CheckTime())//ʱ�䲻����
                    continue;
                s_stPolygonAttrib.minpoint  = stPolygonParam.minpoint;
                s_stPolygonAttrib.maxpoint  = stPolygonParam.maxpoint;
                if(Polygon_CheckIsInMinRectArea())//������С������
                {
                    continue;
                }
                s_stPolygonAttrib.vertexNum = stPolygonParam.vertexNum;                
                Polygon_ReadEndPointData();
                //////////////////
                if(Polygon_ComparePos())//
                {
                    s_stPolygonAttrib.addr = addr;
                }
                //////////////////////////
                break;//����ζ���ܶ�,һ��ֻ����һ�ζ�����ж�
            }       
            if(s_stPolygonAttrib.areaCnt>=s_stPolygonAttrib.checkAreaNum)s_stPolygonAttrib.areaCnt = 0;  //Rescanning
            if((POLYGON_MAX_AREA_ADDR == s_stPolygonAttrib.addr) && (s_stPolygonAttrib.preAreaCnt != s_stPolygonAttrib.areaCnt))
                LZM_PublicSetOnceTimer(&s_stPOLTimer[POL_TIMER_SCAN],PUBLICSECS(0.1),Polygon_ScanArea);
        }
        /////////////////////////////////
        if(s_stPolygonAttrib.addr != POLYGON_MAX_AREA_ADDR)                                 //Get the duration as remain in polygon area
        {
            s_stPolygonAttrib.areaFlag = AREA_STATUS_IN;
            sFLASH_ReadBuffer((unsigned char *)&stPolygonParam,s_stPolygonAttrib.addr,SIZE_STPOLYGON_PARAM);//Read data in flash                
            if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,E_IN_SENDTO_DRIVER)) 
            {
                if(PUBLIC_CHECKBIT(stPolygonParam.userAreaAttrib,E_USER_IN_PLAY)&&stPolygonParam.InVoiceSize)
                {
                    len=stPolygonParam.InVoiceSize;
                    if(len>=VOICE_BYTE)len=VOICE_BYTE-1;
                    for(j=0; j<len;j++)
                    {
                        s_stPolygonAttrib.voiceStr[j]=stPolygonParam.InVoiceStr[j];                    
                    }
                    s_stPolygonAttrib.voiceStr[j]='\0';
                }
                else
                {
                    strcpy((char *)s_stPolygonAttrib.voiceStr,"����ʻ��ָ������");
                }
            }
            ///////////////////////
            s_stPolygonAttrib.areaID            = stPolygonParam.areaID; 
            s_stPolygonAttrib.areaAttrib        = stPolygonParam.areaAttrib;
            s_stPolygonAttrib.userAreaAttrib    = stPolygonParam.userAreaAttrib;            
            s_stPolygonAttrib.limitSpeed        = stPolygonParam.limitspeed; 
            s_stPolygonAttrib.speedDuration     = stPolygonParam.speedDuration;            
//            if(s_stPolygonAttrib.speedDuration<10)s_stPolygonAttrib.speedDuration = 10;      dxl,2017.4.24,���Բ�Ҫ��ȥ������          
            Polygon_ChoiceAreaArmDest(ALARM_ITEM_IN_AREA);
        }
    }
    else
    if(AREA_STATUS_IN == s_stPolygonAttrib.areaFlag)                              //In the area
    {
        if(0 == Polygon_CheckTime())//ʱ�䲻����
        {
            Polygon_ResetParam();
        }
        else
        if(0==Polygon_IsInArea())//Get into the Polygon area
        {
            s_stPolygonAttrib.areaFlag = AREA_STATUS_OUT;                         
            sFLASH_ReadBuffer((unsigned char *)&stPolygonParam,s_stPolygonAttrib.addr,SIZE_STPOLYGON_PARAM);//Read data in flash	
            if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,E_OUT_SENDTO_DRIVER)) 
            {
                if(PUBLIC_CHECKBIT(stPolygonParam.userAreaAttrib,E_USER_OUT_PLAY)&&stPolygonParam.OutVoiceSize)
                {
                    len=stPolygonParam.OutVoiceSize;
                    if(len>=VOICE_BYTE)len=VOICE_BYTE-1;
                    for(j=0; j<len;j++)
                    {
                        s_stPolygonAttrib.voiceStr[j]=stPolygonParam.OutVoiceStr[j];
                    }
                    s_stPolygonAttrib.voiceStr[j]='\0'; 
                }
                else
                {
                    strcpy((char *)s_stPolygonAttrib.voiceStr,"�����뿪ָ������");
                }
            }
            s_stPolygonAttrib.areaID = stPolygonParam.areaID;
            s_stPolygonAttrib.areaAttrib = stPolygonParam.areaAttrib;
            s_stPolygonAttrib.userAreaAttrib = stPolygonParam.userAreaAttrib;
            Polygon_ChoiceAreaArmDest(ALARM_ITEM_OUT_AREA); 
            ///////////////////////////
            s_stPolygonAttrib.addr = POLYGON_MAX_AREA_ADDR;
        }
    }
    
    if((AREA_STATUS_IN==s_stPolygonAttrib.areaFlag)&&PUBLIC_CHECKBIT(s_stPolygonAttrib.areaAttrib,E_AREA_PROHIBIT_OPENDOOR))
    {
        Area_CheckOpenDoorAlarm(E_POLYGON_AREA);
    }
}
/*************************************************************
** ��������: Polygon_GetAreaUserDatalen
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
*************************************************************/
unsigned short Polygon_GetAreaUserDatalen(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen,userAreaAttrib;
    STPOLYGON_PARAM stPolygonParam; 
    datalen = 0;
    Public_Mymemcpy((unsigned char *)&stPolygonParam.areaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); 
    datalen+=sizeof(unsigned long);
    Public_Mymemcpy((unsigned char *)&userAreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); 
    datalen+=sizeof(unsigned short );    
    if(PUBLIC_CHECKBIT(userAreaAttrib,E_USER_IN_PLAY))
    {     
        PUBLIC_SETBIT(stPolygonParam.userAreaAttrib,E_USER_IN_PLAY);
        Public_Mymemcpy((unsigned char *)&stPolygonParam.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); 
        datalen+=sizeof(unsigned char);
        datalen+=stPolygonParam.InVoiceSize;
    }
    if(PUBLIC_CHECKBIT(userAreaAttrib,E_USER_OUT_PLAY))
    {
        PUBLIC_SETBIT(stPolygonParam.userAreaAttrib,E_USER_OUT_PLAY);
        Public_Mymemcpy((unsigned char *)&stPolygonParam.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0);
        datalen+=sizeof(unsigned char);
        datalen+=stPolygonParam.OutVoiceSize;
    }
    if(datalen>BufferLen)return 0;
    *AreaID =stPolygonParam.areaID;
    return datalen;
}
 /*************************************************************
** ��������: Polygon_GetAreaUserDatalen
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
*************************************************************/
unsigned short Polygon_GetAreaDatalenStandard(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen; 
    STPOLYGON_PARAM stPolygonParam; 
    datalen =0;
    Public_Mymemcpy((unsigned char *)&stPolygonParam.areaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); 
    datalen+=sizeof(unsigned long);
    Public_Mymemcpy((unsigned char *)&stPolygonParam.areaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); 
    datalen+=sizeof(unsigned short ); 
    if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,E_AREA_MOMENT))
    {
        datalen += 6;                //Startmoment
        datalen += 6;                //Endmoment
    }
    if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,E_AREA_LIMIT_SPEED))
    {
        datalen +=sizeof(unsigned short);  //Limited speed
        datalen +=sizeof(unsigned char);   //The duration of limited speed status
    }   
    Public_Mymemcpy((unsigned char *)&stPolygonParam.vertexNum,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1);
    datalen+=sizeof(unsigned short );      //Get the number of polygon vertex
    if(stPolygonParam.vertexNum>ONE_POLYGON_VERTEX_NUM_MAX)return 0;
    datalen += (stPolygonParam.vertexNum<<3);
    if(datalen>BufferLen)return 0;
    *AreaID =stPolygonParam.areaID;
    return datalen;
}
 /*************************************************************
** ��������: Area_GetRectAreaDatalenJiuTong
** ��������: :���ö����ʱ,��ͨЭ�鳤��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned short Polygon_GetAreaDatalenJiuTong(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen;
    STPOLYGON_PARAM stPolygonParam; //��ȡ�������������
    datalen =0;
    Public_Mymemcpy((unsigned char *)&stPolygonParam.areaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); 
    datalen+=sizeof(unsigned long);
    Public_Mymemcpy((unsigned char *)&stPolygonParam.areaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); 
    datalen+=sizeof(unsigned short ); 
    if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,E_AREA_MOMENT))
    {
        datalen += 6;                //Startmoment
        datalen += 6;                //Endmoment
    }
    if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,E_AREA_LIMIT_SPEED))
    {
        datalen +=sizeof(unsigned short);  //Limited speed
        datalen +=sizeof(unsigned char);   //The duration of limited speed status
    }
    ///////�Զ���λ///////////////////////
    if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,14))
    {     
        //������ʾ������
        Public_Mymemcpy((unsigned char *)&stPolygonParam.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); 
        datalen+=sizeof(unsigned char);
        //��ʾ�����
        datalen+=stPolygonParam.InVoiceSize;    
    }
    ///////////////////////
    if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,15))
    {
        //������ʾ������
        Public_Mymemcpy((unsigned char *)&stPolygonParam.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0);
        datalen+=sizeof(unsigned char);
        //��ʾ�����
        datalen+=stPolygonParam.OutVoiceSize;
    } 
    ///////////������/////////////////////
    Public_Mymemcpy((unsigned char *)&stPolygonParam.vertexNum,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1);
    datalen+=sizeof(unsigned short );      //Get the number of polygon vertex
    if(stPolygonParam.vertexNum>ONE_POLYGON_VERTEX_NUM_MAX)return 0;
    datalen += (stPolygonParam.vertexNum<<3);
    if(datalen>BufferLen)return 0;
    *AreaID =stPolygonParam.areaID;
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
unsigned short Polygon_GetAreaDatalenSuoMei(unsigned char *pBuffer, unsigned short BufferLen,unsigned long *AreaID)
{
    unsigned short datalen=0;
    return datalen;
}
/*************************************************************
** ��������: Polygon_SetAreaCheckProtoclType
** ��������: :��������ʱ,����Ƿ���������־,��Ϊ���ݾ�ͨ�汾
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Polygon_SetAreaCheckProtoclType(unsigned char *pBuffer, unsigned short datalen,unsigned long *AreaID)
{
    unsigned short templen;
    //////////////    
    if(datalen==0)return AREA_PROTOCOL_TYPE_EMPTY;
    ///////////////////////
    templen = Polygon_GetAreaDatalenStandard(pBuffer,datalen,AreaID);//����ֻ�·�һ�������
    if(datalen == templen)//���,��ȷ
    {
        return AREA_PROTOCOL_TYPE_STANDARD;
    }
    /////////////////////////
    templen=Polygon_GetAreaDatalenJiuTong(pBuffer,datalen,AreaID);    
    if(datalen == templen)//���,��ȷ
    {
        return AREA_PROTOCOL_TYPE_JIUTONG;//������,��ȷ
    }
    /////////////////////
    return AREA_PROTOCOL_TYPE_EMPTY;
}
 /*************************************************************
** ��������: Polygon_GetAreaUserDatalen
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
*************************************************************/
unsigned short Polygon_SaveOneModifyUserData(unsigned long addr,unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long AreaID;
    unsigned short datalen,userAreaAttrib;
    STPOLYGON_PARAM stPolygonParam;  //��ȡ�������������    
    if(addr+ONE_POLYGON_AREA_SIZE>POLYGON_AREA_ADDR_END)return 0;
    sFLASH_ReadBuffer((unsigned char*)&stPolygonParam,addr,SIZE_STPOLYGON_PARAM); //������
    datalen = 0;
    Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //�������ID
    if(AreaID!=stPolygonParam.areaID)return 0;
    datalen+=sizeof(unsigned long);
    Public_Mymemcpy((unsigned char *)&userAreaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //�����������
    datalen+=sizeof(unsigned short ); 
    //////////�Զ���/////////////////    
    if(PUBLIC_CHECKBIT(userAreaAttrib,E_USER_IN_PLAY))//modify by joneming20150108 
    {        
        PUBLIC_SETBIT(stPolygonParam.userAreaAttrib,E_USER_IN_PLAY);
        Public_Mymemcpy((unsigned char *)&stPolygonParam.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
        datalen+=sizeof(unsigned char);  
        Public_Mymemcpy(stPolygonParam.InVoiceStr,(unsigned char *)&pBuffer[datalen],stPolygonParam.InVoiceSize,0); //��ʾ�����
        datalen+=stPolygonParam.InVoiceSize;    
    }
    if(PUBLIC_CHECKBIT(userAreaAttrib,E_USER_OUT_PLAY))
    {
        PUBLIC_SETBIT(stPolygonParam.userAreaAttrib,E_USER_OUT_PLAY);
        Public_Mymemcpy((unsigned char *)&stPolygonParam.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
        datalen+=sizeof(unsigned char);  
        Public_Mymemcpy(stPolygonParam.OutVoiceStr,(unsigned char *)&pBuffer[datalen],stPolygonParam.OutVoiceSize,0); //��ʾ�����
        datalen+=stPolygonParam.OutVoiceSize;
    }
    ////////////////////////////
    if(datalen>BufferLen)return 0;
    //////////////////////////
    Area_MdyDataInFlash((unsigned char*)&stPolygonParam,addr,SIZE_STPOLYGON_PARAM);
    return datalen;
}
/*************************************************************
** ��������: Polygon_SaveOneArea
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 0:����ɹ�,1����ʧ��
*************************************************************/ 
unsigned char Polygon_SaveOneArea(unsigned long addr,unsigned char *pBuffer, unsigned short BufferLen)
{
    STPOLYGON_PARAM stPolygonParam;  //��ȡ�������������
    unsigned short datalen,i=0,index,tmplen;
    unsigned long tmpaddr; 
    T_GPS_POS point,vertex[32];    
    if(addr+ONE_POLYGON_AREA_SIZE>POLYGON_AREA_ADDR_END)return 1;
    memset(&stPolygonParam,0,sizeof(STPOLYGON_PARAM));
    datalen = 0;
    Public_Mymemcpy((unsigned char *)&stPolygonParam.areaID,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); //�������ID
    datalen+=sizeof(unsigned long);
    Public_Mymemcpy((unsigned char *)&stPolygonParam.areaAttrib,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //�����������
    datalen+=sizeof(unsigned short ); 
    //////////����ʱ��///////////////////
    if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,E_AREA_MOMENT))
    {
        Public_Mymemcpy((unsigned char *)&stPolygonParam.startMoment[0],(unsigned char *)&pBuffer[datalen],6,0); //��ʼʱ��
        datalen += 6;  
        Public_Mymemcpy((unsigned char *)&stPolygonParam.endMoment[0],(unsigned char *)&pBuffer[datalen],6,0); //����ʱ��
        datalen += 6;  
    }
    //////////�������////////////
    if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,E_AREA_LIMIT_SPEED))
    {
        Public_Mymemcpy((unsigned char *)&stPolygonParam.limitspeed,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1); //���
        datalen+=sizeof(unsigned short );    
        Public_Mymemcpy((unsigned char *)&stPolygonParam.speedDuration,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //���ٳ���ʱ��
        datalen+=sizeof(unsigned char);  
    }
    ///////////�Զ���//////////////
    if(AREA_PROTOCOL_TYPE_JIUTONG == s_stPolygonAttrib.protoclType)
    {
        if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,14))
        {
            PUBLIC_CLRBIT(stPolygonParam.areaAttrib,14);
            PUBLIC_SETBIT(stPolygonParam.userAreaAttrib,E_USER_IN_PLAY);               
            Public_Mymemcpy((unsigned char *)&stPolygonParam.InVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
            datalen+=sizeof(unsigned char);  
            Public_Mymemcpy((unsigned char *)&stPolygonParam.InVoiceStr[0],(unsigned char *)&pBuffer[datalen],stPolygonParam.InVoiceSize,0); //��ʾ�����
            datalen+=stPolygonParam.InVoiceSize;    
        }
        ///////////////////////
        if(PUBLIC_CHECKBIT(stPolygonParam.areaAttrib,15))
        {
            PUBLIC_CLRBIT(stPolygonParam.areaAttrib,15);
            PUBLIC_SETBIT(stPolygonParam.userAreaAttrib,E_USER_OUT_PLAY);            
            Public_Mymemcpy((unsigned char *)&stPolygonParam.OutVoiceSize,(unsigned char *)&pBuffer[datalen],sizeof(unsigned char),0); //������ʾ������
            datalen+=sizeof(unsigned char);  
            Public_Mymemcpy((unsigned char *)&stPolygonParam.OutVoiceStr[0],(unsigned char *)&pBuffer[datalen],stPolygonParam.OutVoiceSize,0); //��ʾ�����
            datalen+=stPolygonParam.OutVoiceSize;
        }
    }
    ///////����ζ�����///////////////////
    Public_Mymemcpy((unsigned char *)&stPolygonParam.vertexNum,(unsigned char *)&pBuffer[datalen],sizeof(unsigned short ),1);
    datalen += sizeof(unsigned short );//Get the number of polygon vertex
    tmpaddr = addr + SIZE_STPOLYGON_PARAM;
    index = 0;
    for(i=0;i< stPolygonParam.vertexNum; i++)
    {
        Public_Mymemcpy((unsigned char *)&point.Lat,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); 
        datalen+=sizeof(unsigned long);             //Get the length of vertex latitude
        Public_Mymemcpy((unsigned char *)&point.Long,(unsigned char *)&pBuffer[datalen],sizeof(unsigned long),1); 
        datalen+=sizeof(unsigned long);             //Get the length of vertex longitude
        //////�������Сֵ/////////////
        if(0 == i)//
        {
           stPolygonParam.minpoint = point;
           stPolygonParam.maxpoint = point;
        }
        else
        {
            if(stPolygonParam.minpoint.Lat > point.Lat)
            {
                stPolygonParam.minpoint.Lat = point.Lat;
            }
            if(stPolygonParam.minpoint.Long > point.Long)
            {
                stPolygonParam.minpoint.Long = point.Long;
            }
            ///////////////////////
            if(stPolygonParam.maxpoint.Lat < point.Lat)
            {
                stPolygonParam.maxpoint.Lat = point.Lat;
            }
            if(stPolygonParam.maxpoint.Long < point.Long)
            {
                stPolygonParam.maxpoint.Long = point.Long;
            }
        }
        //////�ݴ�//////////////////////////
        vertex[index++]=point;
        if(index>=32)//�ֿ�д��ȽϿ�
        {
            tmplen =(index<<3);            
            Area_MdyDataInFlash((unsigned char*)&vertex,tmpaddr,tmplen);
            tmpaddr += tmplen;
            index = 0;
        }
    }
    ////////////////////////
    if(index)//����δд��Ķ�������
    {
        tmplen =(index<<3);
        Area_MdyDataInFlash((unsigned char*)&vertex,tmpaddr,tmplen);
    }
    //////////////////////
    stPolygonParam.Head = POLYGON_SAVE_FLAG;
    ///////////////////////////
    Area_MdyDataInFlash((unsigned char*)&stPolygonParam,addr,SIZE_STPOLYGON_PARAM);
    return 0;
}

/*************************************************************
** ��������: Polygon_SetArea
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
*************************************************************/
unsigned char Polygon_SetArea(unsigned char *pBuffer, unsigned short BufferLen)
{
    STPOLYGON_PARAM stPolygonParam;  //��ȡ�������������
    unsigned long tmpAddr =0;
	  unsigned long address =0;
	  unsigned long AreaID =0;
	  unsigned long tmpAreaID =0;
    unsigned short datalen;
    unsigned char AckResult,i,protoclType;
    unsigned char *pTmp;
    //////////////////
    if(BufferLen==0)return 2;
	
	  if((1== BBXYTestFlag)||(1 == BBGNTestFlag))
    {
        Area_ClearRoundArea();
			  Area_ClearRectArea();
			  Polygon_ClearArea();
    }
		
    /////////////////
    pTmp = pBuffer;
    datalen = BufferLen;
    
    protoclType = Polygon_SetAreaCheckProtoclType(pTmp,datalen,&AreaID);//����ֻ�·�һ�������
    if(AREA_PROTOCOL_TYPE_EMPTY==protoclType)return 2;
    s_stPolygonAttrib.protoclType = protoclType;
    ///////////////////////////
    address =0;
    for(i = s_stPolygonAttrib.checkAreaNum; i>0; i--)
    {
        tmpAddr=POLYGON_AREA_ADDR_START + (i-1)*ONE_POLYGON_AREA_SIZE;
        sFLASH_ReadBuffer((unsigned char *)&stPolygonParam,tmpAddr,POLYGON_HEAD_ID_SIZE);
        if(AreaID==stPolygonParam.areaID)
        {
            
            address   = tmpAddr;
            tmpAreaID = stPolygonParam.areaID;
            break;
        }
        else
        if(0==stPolygonParam.areaID)
        {
            address   = tmpAddr;
            tmpAreaID = stPolygonParam.areaID;
        }
    }
    ////////////////////////////
    if(0 == address)//
    {
        if(s_stPolygonAttrib.checkAreaNum >= POLYGON_AREA_NUM_MAX)return 1;
        ////////////////////////
        address = POLYGON_AREA_ADDR_START + s_stPolygonAttrib.checkAreaNum*ONE_POLYGON_AREA_SIZE;
        s_stPolygonAttrib.checkAreaNum++;
        s_stPolygonAttrib.validAreaNum++;
    }
    else
    {
        if(0==tmpAreaID)//�滻IDΪ0�Ķ��������
        {
            s_stPolygonAttrib.validAreaNum++;
        }
    }
    ///////////////////////
    pTmp = pBuffer;
    datalen = BufferLen;
    AckResult = Polygon_SaveOneArea(address,pTmp,datalen);
    Polygon_CheckForResetParam(AreaID);
    s_stPolygonAttrib.preSpeed = 0xff;
    return AckResult;
}
/*************************************************************
** ��������: Polygon_SetArea
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
*************************************************************/ 
unsigned char Polygon_DeleteArea(unsigned char *pBuffer, unsigned short BufferLen) 
{
    unsigned char i,j,Sum,flag;
    unsigned long addr,AreaID;
    STPOLYGON_PARAM stPolygonParam;  
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
            for(j=0; j< s_stPolygonAttrib.checkAreaNum; j++)
            {
                addr=POLYGON_AREA_ADDR_START + j*ONE_POLYGON_AREA_SIZE;
                sFLASH_ReadBuffer((unsigned char *)&stPolygonParam,addr,POLYGON_HEAD_ID_SIZE);
                if(AreaID==stPolygonParam.areaID)
                {
                    flag = 1;
                    break;
                }
            }
            if(0 == flag)return 2;
        }
        pTmp =pBuffer;
        for(i=0; i<Sum; i++)
        {
            Public_Mymemcpy((unsigned char *)&AreaID,(unsigned char *)&pTmp[0],sizeof(unsigned long),1); //Get area ID
            pTmp+=sizeof(unsigned long);
            for(j=0; j<s_stPolygonAttrib.checkAreaNum; j++)
            {
                addr = POLYGON_AREA_ADDR_START + j*ONE_POLYGON_AREA_SIZE;
                sFLASH_ReadBuffer((unsigned char *)&stPolygonParam,addr,POLYGON_HEAD_ID_SIZE);
                if(AreaID==stPolygonParam.areaID&&AreaID)                          //Deleted area ID 
                {
                    stPolygonParam.areaID = 0;                             //Delete area and clear ID
                    s_stPolygonAttrib.validAreaNum--;
                    addr = POLYGON_AREA_ADDR_START + j*ONE_POLYGON_AREA_SIZE;
                    stPolygonParam.Head = POLYGON_SAVE_FLAG;
                    Area_MdyDataInFlash((unsigned char*)&stPolygonParam,addr,POLYGON_HEAD_ID_SIZE);
                    Polygon_CheckForResetParam(AreaID);
                }
            }
        } 
    }
    else//Clear all polygon area
    {
        Polygon_ClearArea();
    }
    return 0;
}
/*************************************************************
** ��������: Polygon_AreaModifyUserData
** ��������: �޸Ķ���������Զ��岿��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
*************************************************************/ 
unsigned char Polygon_AreaModifyUserData(unsigned char *pBuffer, unsigned short BufferLen)
{
    unsigned long addr,AreaID;
    unsigned char i,j,Sum,flag;
    STPOLYGON_PARAM stPolygonParam;  //��ȡ�������������
    unsigned short datalen,templen;
    unsigned char *pTmp;
    //////////////
    if(BufferLen==0)return 2;
    ////////////
    Sum=*pBuffer++;
    pTmp =pBuffer;
    datalen=BufferLen-1;
    //�����
    for(i=0; i<Sum; i++)//���ID
    {        
        templen=Polygon_GetAreaUserDatalen(pTmp,datalen,&AreaID);
        if(0 == templen)return 2;//��Ϣ����;
        //////////////////////////
        flag =0;
        for(j=0; j<s_stPolygonAttrib.checkAreaNum; j++)
        {
            addr=POLYGON_AREA_ADDR_START + j*ONE_POLYGON_AREA_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stPolygonParam,addr,POLYGON_HEAD_ID_SIZE);
            if(AreaID==stPolygonParam.areaID)
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
        for(j=0; j<s_stPolygonAttrib.checkAreaNum; j++)
        {
            addr=POLYGON_AREA_ADDR_START + j*ONE_POLYGON_AREA_SIZE;
            sFLASH_ReadBuffer((unsigned char *)&stPolygonParam,addr,POLYGON_HEAD_ID_SIZE);
            if(AreaID==stPolygonParam.areaID)
            {
                templen=Polygon_SaveOneModifyUserData(addr,pTmp,datalen);
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
** ��������: Polygon_GetAreaTotalNum
** ��������: ��ö������������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: �������������
*************************************************************/ 
unsigned char Polygon_GetAreaTotalNum(void)
{
    return s_stPolygonAttrib.validAreaNum;
}
/*************************************************************
** ��������: Polygon_ScanTask
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Polygon_CheckScanTask(void)
{
    unsigned char speed;
    //���������򱨾�����λ״̬
    if(Io_ReadAlarmMaskBit(ALARM_BIT_IN_OUT_AREA))
    {
        if(1==s_stPolygonAttrib.noNavigationCnt)
        {
            s_stPolygonAttrib.noNavigationCnt = 0;
            Polygon_ResetParam();
        }
        return;
    }
    Polygon_GetCurGpsCoordinate();//1s��ȡһ�ξ�γ��        
    s_stPolygonAttrib.noNavigationCnt = 1;
    s_stPolygonAttrib.preAreaCnt=s_stPolygonAttrib.areaCnt;
    speed = Area_GetCurSpeed();
    ///////////////////////////
    if(0==speed&&0==s_stPolygonAttrib.preSpeed)return;//�ٶ�Ϊ��,������
    /////////////////////////
    s_stPolygonAttrib.preSpeed = speed;
    Polygon_ScanArea();
}
/*************************************************************
** ��������: Polygon_ScanTask
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Polygon_ScanTask(void)
{
    if(Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
    {
        GPS_STRUCT stTmpGps;
        Gps_CopygPosition(&stTmpGps);
        if(stTmpGps.Second!=s_stPolygonAttrib.preSecond)
        {
            s_stPolygonAttrib.preSecond = stTmpGps.Second;
            LZM_PublicKillTimer(&s_stPOLTimer[POL_TIMER_SCAN]);
            Polygon_CheckScanTask();
        }
    }
    else
    {  
        s_stPolygonAttrib.preSecond = 0xff;
        s_stPolygonAttrib.preSpeed = 0xff;
        if(s_stPolygonAttrib.noNavigationCnt<50)s_stPolygonAttrib.noNavigationCnt++;//10s
        else
        if(s_stPolygonAttrib.noNavigationCnt == 50)
        {
            s_stPolygonAttrib.noNavigationCnt =100;
            //Area_ClearAllAlarm(E_POLYGON_AREA);
            Polygon_ResetParam();//modify by joneming 20150105
        }
    }
}

/*************************************************************
** ��������: Polygon_ScanTask
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Polygon_StartScanTask(void)
{
    LZM_PublicSetCycTimer(&s_stPOLTimer[POL_TIMER_TASK],PUBLICSECS(0.2),Polygon_ScanTask);
}
/*************************************************************
** ��������: Polygon_TimeTask
** ��������: �����ܶ�ʱ��������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
FunctionalState Polygon_TimeTask(void)
{
    LZM_PublicTimerHandler(s_stPOLTimer,POL_TIMERS_MAX);
    return ENABLE;
}
/*************************************************************
** ��������: Polygon_ParameterInitialize
** ��������: ���������ʼ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Polygon_ReadAndCheckParam(void) 
{
    unsigned long addr;
    unsigned short i;    
    STPOLYGON_PARAM stPolygonParam;	  //��ȡ�������������
    s_stPolygonAttrib.preSecond = 0xff;
    s_stPolygonAttrib.preSpeed = 0xff;
    s_stPolygonAttrib.noNavigationCnt =0xff;  
    ////////////    
    //����γ�ʼ��
    Polygon_AreaParameterInit();
    for(i=0; i<POLYGON_AREA_NUM_MAX; i++)
    {
        addr=POLYGON_AREA_ADDR_START + i*ONE_POLYGON_AREA_SIZE ;  
        sFLASH_ReadBuffer((unsigned char *)&stPolygonParam,addr,POLYGON_HEAD_ID_SIZE);//������������	
        if(POLYGON_SAVE_FLAG == stPolygonParam.Head)
        {
            if(0xFFFFFFFF == stPolygonParam.areaID)
            {
                Polygon_ClearArea();
                break;
            }
            else
            {
                s_stPolygonAttrib.checkAreaNum++;
                if(stPolygonParam.areaID)s_stPolygonAttrib.validAreaNum++;
            }
        }
        else 
        if(stPolygonParam.Head != 0xFFFFFFFF)
        {
            Polygon_ClearArea();
            break;
        }
        else 
        if(0xFFFFFFFF == stPolygonParam.Head)
        {
            break;
        }
    }
    //////////////////////////////////////////////////////////
    LZM_PublicSetOnceTimer(&s_stPOLTimer[POL_TIMER_TASK],PUBLICSECS(20),Polygon_StartScanTask);    
 }
/*************************************************************
** ��������: Polygon_ParameterInitialize
** ��������: ���������ʼ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Polygon_ParameterInitialize(void) 
{
    LZM_PublicKillTimerAll(s_stPOLTimer,POL_TIMERS_MAX);
    ///////////////////////////
    SetTimerTask(TIME_POLYGON,LZM_AT_ONCE);
    //////////////////////////////////////////////////////////
    LZM_PublicSetOnceTimer(&s_stPOLTimer[POL_TIMER_TASK],PUBLICSECS(10),Polygon_ReadAndCheckParam);    
 }

/*******************************************************************************
 *                             end of module
 *******************************************************************************/
