/*******************************************************************************
 * File Name:			Route.c 
 * Function Describe:	
 * Relate Module:		��·���
 * Writer:				Joneming
 * Date:				2013-10-17
 * ReWriter:			
 * Date:				
 ***����ƽ̨�㷨��д,ȥ����ʱ�ж�
 *******************************************************************************/
//****************�����ļ�*************************
#include "include.h"
#include "VDR_Doubt.h"
#include "VDR_Overspeed.h"

//****************�궨��********************
#define ONE_TIME_READ_NUM           10  //ÿһ���жϵĶ�����
//////////////////////////////////////
#define IN_OUT_ROUTE_OVERHEAD_INFO_ID               0x12 //������·����
#define IN_OUT_ROUTE_OVERHEAD_INFO_LEN              0x06;//������·����������Ϣ����
//////////////////////////////////////
#define LOAD_OVER_DRIVE_OVERHEAD_INFO_ID            0x13 //·����ʻʱ�䲻��/��������
#define LOAD_OVER_DRIVE_OVERHEAD_INFO_LEN           0x07;//·����ʻʱ�䲻��/��������������Ϣ����
//****************�ṹ����*******************
extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ����Э����ģʽ
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ

typedef enum{
    IN_ROUTE=0,    //��״̬,����·��
    OUT_ROUTE      //��״̬,������·��
}E_INOUTROUTE;

//��·�㷨״̬
typedef enum{
    ROUTE_FIND_STATUS=0,            //���ҵ�ǰ����������·
    ROUTE_FIND_ROAD_SEGMENT,        //���ҵ�ǰ�����Ǹ�·��
    ROUTE_CHECK_IN_ROAD_SEGMENT,    //����Ƿ���ĳ��·��
    ROUTE_HOLD_ROAD_SEGMENT,        //������ĳ��·��
    ROUTE_CHANGE_ROAD_SEGMENT,      //�ı�Ϊĳ��·��
    ROUTE_CHECK_OUT_ROUTE,          //�ڵ�ǰ��·ʱ������·���
}T_ROUTE_STATUS; 

//��·����                                                              
typedef enum{
    E_ROUTE_MOMENT      ,       //������·��ʱ����ʱ��,��·������ʻʱ�����ԡ�
    E_ROUTE_SPEED       ,       //����·�����б���,��·��������:Ϊ����
    E_INROUTE_TO_DRIVER ,       //��·�߱�����˾��
    E_INROUTE_TO_SERVER ,       //��·�߱�����ƽ̨
    E_OUTROUTE_TO_DRIVER,       //����·������˾��
    E_OUTROUTE_TO_SERVER,        //����·������ƽ̨
    E_OUTROUTE_MAX
}E_ROUTE_FLAG;

//-------------------------------------------------
//��·�ṹ   
//-------------------------------------------------
typedef struct          //�ܳ�40�ֽ�
{
    u16  Head;            //ͷ��־
    u8   reserve[2];      //����    
    u8   startTime[6];    //��ʼʱ��
    u8   endTime[6];      //����ʱ��
    u32  routeID;         //��·ID
    u16  routeAttrib;     //��·����
    u16  nodeSum;         //�ܵĽڵ���
    T_GPS_POS LeftUpPos;  //����_���ϵ�γ��
    T_GPS_POS RightDowPos;//����_���µ�γ��
}STROUTE_HEAD;

#define STROUTE_HEAD_SIZE sizeof(STROUTE_HEAD)

//-------------------------------------------------
//�ڵ�ṹ  
//-------------------------------------------------
typedef struct             //�ܳ�28�ֽ�
{
    u32 NodeID;            //�յ�ID
    u32 roadSegID;         //·��ID
    T_GPS_POS point;       //�ڵ㾭γ��
    u16 longLimitTime;     //���ʻʱ��
    u16 shortLimitTime;    //�����ʻʱ��
    u16 driverTime;        //��¼��ǰ·������ʻʱ�䡣
    u16 limitSpeed;        //����ֵ
    u8  speedDuration;     //����������ʱ��     
    u8  roadWidth;         //·��
    u8  roadAttrib;        //·������
    u8  reserve;           //���� 
}ST_ROUTE_NODE;
/////////////////////////
#define STROUTE_NODE_SIZE sizeof(ST_ROUTE_NODE)
//-------------------------------------------------
//��·Ӧ�ýṹ
//-------------------------------------------------
typedef struct
{
    u32 Latitude;//γ��
    u32 Longitude;//����
    ///////////////////////    
    u32 startTime;       //��·ͨ��ʱ���
    u32 endTime;         //��·ͨ��ʱ��� 
    /////////////////��·����//////////////
    u32 routeID;          //��ǰ��·ID
    u16 routeAttrib;      //��·���� 
    u16 nodeSum;          //��ǰ��·�ڵ�����
    T_GPS_POS LeftUpPos;  //����_���ϵ�γ��
    T_GPS_POS RightDowPos;//����_���µ�γ��
    T_GPS_POS point2;     //�ڶ����ڵ㾭γ�� 
    /////////////////////
    ST_ROUTE_NODE stNode;          //�ڵ���Ϣ   
    ///////////////////////////
    STPLAY_ATTRIB stPlay; //ƫ��·�߱�������
    /////////////     
    double roadDistance; //�߶ξ���
    u32  nodeAddr;         //�ڵ��׵�ַ
    u32  startdriverTime;  //��ʼ��ʻ��ʱʱ��
    u16  curNode;          //��ǰ�ڵ��
    u8   status;           //��·״̬ 0:��·����  1:��·����
    u8   curRoute;         //��ǰ��·��� 
    u8   routeSum;         //��·���� 
    u8   minRoadWidth;     //��С·��
    u8   timeType;          //��·��ĳ��ʱ��ο������͡����Ϊ0-���������գ�ʱ���롣���Ϊ1����ÿ��ģ�ʱ���롣
    u8   preSecond;
    u8   preSpeed;
    u8   nonavigationCnt;
    u8   overTimeFlag;
    u8   needplayflag;    
}ST_ROUTE_ATTRIB; 

static ST_ROUTE_ATTRIB s_stRouteAttrib;            //������·����
//-------------------------------------------------
//��·��γ�Ⱥͱ���ȫ�ֽṹ
//-------------------------------------------------
typedef struct 
{
    u32 INOUTTypeID;     //�������·ID
    u8 INOUTPostType;    //0:���ض�λ��  1:Բ��  2:����  3:����� 4:·��
    u8 Orientation;      //0����1:��
    //·����ʻʱ�䲻��/����������Ϣ
    u32 roadSegID;        //·��ID
    u16 driverTime;       //��ʻʱ��
    u8 driverResult;      //0:���� 1:����
}T_ROUTE_ALARM;

static T_ROUTE_ALARM s_stRouteAlarm;
//////////////////////////////////////////////
#define ROUTE_RESET_STATUS_COUNT        50///����������10s�����״̬.(0.2s)
////////////////////////////////////////
#define ROUTE_ADDR_START             (u32)(FLASH_ROUTE_START_SECTOR * FLASH_ONE_SECTOR_BYTES)   //��·��ſ�ʼλ��
////////////////////////////////////////////
#define ONE_ROUTE_AREA_SIZE          (u32)(STROUTE_HEAD_SIZE+(ROUTE_NODE_SUM*STROUTE_NODE_SIZE))
////////////////////////
#define ROUTE_HEAD_FLAG    0xFEDC

enum 
{
    ROUTE_TIMER_TASK,                       //  
    ROUTE_TIMER_TEST,						//   
    ROUTE_TIMERS_MAX
}STROUTETIME;

static LZM_TIMER s_stRouteTimer[ROUTE_TIMERS_MAX];
/////////////////
//****************��������*******************
void Route_GoNextStatus(u8 NextStatus); //Route״̬ת��
void Route_GetNodeData(u16 CurNode);//�����·����
void Route_CheckDriverTime(void) ; //�����ʻʱ��,�Ƿ�����Ҫ��
void Route_DisposeInOutAlarm(E_INOUTROUTE InOutAlarm);
void Route_SetSpeedParam(void);
void Route_CheckScanTask(void);
void Route_ParameterInitialize(void);

//****************��������******************
/////////////�����������ʹ��/////////////////////////////////////
#define DEF_PI180        0.01745329252 //=PI/180.0//
#define EARTH_RADIUS     6378.137
#define DEF_PI180_EX     0.00000001745329252 //=PI/180.0/1000000.0//(�ϴ��ľ�γ��,�ѷŴ�1000000��)
#define EARTH_RADIUS_EX  12756274.0 //=2*EARTH_RADIUS*1000;
#if 0
/*************************************************************OK
** ��������: Route_GetPointDistance
** ��������: ������λ�õ�ֱ�ӵľ�������(��)
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/ 
double Route_GetDistanceEx(unsigned long lat1, unsigned long lng1, unsigned long lat2, unsigned long lng2)
{
    double radLat1 = lat1* DEF_PI180/1000000.0;
    double radLat2 = lat2* DEF_PI180/1000000.0;
    double a = radLat1 - radLat2;
    double b = lng1* DEF_PI180/1000000.0 - lng2 * DEF_PI180/1000000.0;
    double s = 2.0 * asin(
    sqrt(pow(sin(a / 2), 2)
    +
    cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)))
    * EARTH_RADIUS
    * 1000.0;
    return s;
}
#endif
/*************************************************************OK
** ��������: Route_GetPointDistance
** ��������: ������λ�õ�ֱ�ӵľ�������(��)
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/ 
double Route_GetDistance(unsigned long lat1, unsigned long lng1, unsigned long lat2, unsigned long lng2)
{
    double radLat1 = lat1* DEF_PI180_EX;
    double radLat2 = lat2* DEF_PI180_EX;
    double a = radLat1 - radLat2;
    double b = ((double)lng1-(double)lng2)* DEF_PI180_EX;
    double s = asin(
    sqrt(pow(sin(a / 2), 2)
    +
    cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2)))
    * EARTH_RADIUS_EX;
    return s;
}

/*************************************************************OK
** ��������: Route_GetPointDistance
** ��������: ������λ�õ�ֱ�ӵľ�������(��)
** ��ڲ���: Point1,Point2
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/ 
double Route_GetPointDistance(T_GPS_POS *Point1,T_GPS_POS *Point2)
{
    return Route_GetDistance(Point1->Lat, Point1->Long,Point2->Lat, Point2->Long);
}
/*************************************************************OK
** ��������: Route_GetNearestDistance
** ��������: �����p3���߶�AB�ľ���
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/ 
double Route_GetNearestDistance(T_GPS_POS *curPoint,T_GPS_POS *PA,T_GPS_POS *PB,double Pab)   
{
    double d1,d2,d;
    double dd,dd2,dd1;
    double ss;
    d1 = Route_GetPointDistance(PB,curPoint);
    if(d1<=0.000001)
        return 0.0f;  
    ////////////////////////////////
    d =  Pab; 
    if(d<=0.000001)
        return d1;//���PA��PB������ͬ�����˳������������ؾ���
    /////////////////////
    d2 = Route_GetPointDistance(PA,curPoint);   
    if(d2<=0.000001)
        return 0.0f;  
    ///////////
    dd1 = d1*d1;
    dd2 = d2*d2;
    dd = d*d;
    //------------------------------ 
    if(dd1>=dd2+dd)
        return d2; 
    if(dd2>=dd1+dd)
        return d1; 
    //////////////////////
    dd=(d1+d2+d)/2;     //�ܳ���һ��   
    ss=sqrt(dd*(dd-d1)*(dd-d2)*(dd-d));  //���׹�ʽ�����   
    return (2.0*ss/d);
}
/*************************************************************OK
** ��������: Route_SetSpeedParam
** ��������: ����·�κ��ʼ�����ټ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Route_SetSpeedParam(void)
{

    unsigned char tmp = 0;
    //tmp=(s_stRouteAttrib.stNode.speedDuration < 10) ? 10:s_stRouteAttrib.stNode.speedDuration;           //��֤����10��
    tmp = s_stRouteAttrib.stNode.speedDuration;//dxl,2017.4.24,���Բ�Ҫ������
	  if(PUBLIC_CHECKBIT(s_stRouteAttrib.stNode.roadAttrib,E_ROUTE_SPEED))     //���ٶ�����      
    {
        //SpeedMonitor_SetSpeedParamValid(E_SPEED_TYPE_ROUTE,s_stRouteAttrib.routeID,s_stRouteAttrib.stNode.limitSpeed,tmp);
			VDROverspeed_SetRoadParameter(E_ROUTE,s_stRouteAttrib.routeID,s_stRouteAttrib.stNode.limitSpeed&0xff,tmp);
    }
    else
    {
       // SpeedMonitor_ClearSpeedParamValid(E_SPEED_TYPE_ROUTE);
			VDROverspeed_ResetRoadParameter(E_ROUTE);
    }    
}
/*************************************************************OK
** ��������: Route_ClearSpeedParam
** ��������: ������ٱ���
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/  
void Route_ClearSpeedParam(void)
{
    //SpeedMonitor_ClearSpeedParamValid(E_SPEED_TYPE_ROUTE); dxl,2015.9,
	  VDROverspeed_ResetRoadParameter(E_ROUTE);
}
/************************************************************
��������: ���������·����
��������: 
��ڲ���:
���ڲ���:
************************************************************/ 
void Route_ClearAlarm(void) 
{
    s_stRouteAttrib.status = ROUTE_FIND_STATUS;              //���õ���������Ѱ·��
	  if(1 == BBGNTestFlag)//dxl,2016.5.24�������ܼ��ʱ����·����λ������ģ�鸺��
		{
		    Io_WriteAlarmBit(ALARM_BIT_RESERVER15,RESET);         //�����ȥ·��
		}
		else
		{
        Io_WriteAlarmBit(ALARM_BIT_IN_OUT_ROAD,RESET);         //�����ȥ·��
		}
    Io_WriteAlarmBit(ALARM_BIT_LOAD_EXCURSION,RESET);      //���ƫ��·�߱���  
    Io_WriteAlarmBit(ALARM_BIT_LOAD_OVER_DRIVE,RESET);     //�����ʻʱ�䱨��
    Route_ClearSpeedParam(); //������ٱ��� 
    s_stRouteAttrib.startdriverTime = Timer_Val();                     //��ʻʱ�����¼���.��Ϊ��������֪���´ε������ڱ�·�Σ����ǵ�����·��ȥ�ˡ�
}
/*************************************************************
** ��������: Route_ResetParam
** ��������: ��·���ò���
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_ResetParam(void) 
{
    Route_ClearAlarm();
    ///////////////////
    s_stRouteAttrib.curNode = 0;
    s_stRouteAttrib.curRoute = 0;
    s_stRouteAttrib.preSpeed = 0xff;
}
/*************************************************************
** ��������: ClrRoute
** ��������: ����·���������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void ClrRoute(void)
{
    Route_ParameterInitialize();
    Area_EraseFlash(FLASH_ROUTE_START_SECTOR,FLASH_ROUTE_END_SECTOR);      //������·
}
/*************************************************************
** ��������: Route_GoNextStatusAtOnce
** ��������: ����������һ��״̬
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_GoNextStatusAtOnce(u8 NextStatus)
{
    s_stRouteAttrib.status = NextStatus;
    ////////////////////////////////
    LZM_PublicSetOnceTimer(&s_stRouteTimer[ROUTE_TIMER_TEST],LZM_AT_ONCE, Route_CheckScanTask);
}
/*************************************************************
** ��������: Route_GoNextStatus
** ��������: ״̬ת��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_GoNextStatus(u8 NextStatus)
{
    LZM_PublicKillTimer(&s_stRouteTimer[ROUTE_TIMER_TEST]);
    s_stRouteAttrib.status = NextStatus;
}
/*************************************************************
** ��������: Route_CheckIsInRoute
** ��������: �жϳ����Ƿ�����·��
** ��ڲ���: ��·���˾�γ��,·��
** ���ڲ���: 
** ���ز���: 1:����·��  0:����
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
u8 Route_CheckIsInRoute( T_GPS_POS *curPos,T_GPS_POS *Dot1, T_GPS_POS *Dot2,u16 Width)
{
    double distance;
    unsigned short tmpWidth;
    tmpWidth = Width;
    if(tmpWidth < s_stRouteAttrib.minRoadWidth)
        tmpWidth = s_stRouteAttrib.minRoadWidth; //·�����޶�,����С��30��
    ////////////////////
    tmpWidth>>=1;
    ////////////////
    distance = Route_GetNearestDistance(curPos,Dot1,Dot2,s_stRouteAttrib.roadDistance);
    if(distance > tmpWidth) return 0;//������·��
    return 1;//����·��    
}
/*************************************************************
** ��������: Route_CheckIsInRouteEx
** ��������: �жϳ����Ƿ�����·��
** ��ڲ���: ��·���˾�γ��,·��
** ���ڲ���: 
** ���ز���: 1:����·��  0:����
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Route_CheckIsInRouteEx(void)
{
    return Route_CheckIsInRoute((T_GPS_POS *)&s_stRouteAttrib.Latitude,&s_stRouteAttrib.stNode.point, &s_stRouteAttrib.point2, s_stRouteAttrib.stNode.roadWidth);
}
/*************************************************************
** ��������: Route_CheckTime
** ��������: �����·�Ƿ����ʱ�䷶Χ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 0:������ʱ�䷶Χ,1:����ʱ�䷶Χ
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Route_CheckTime(void)
{
    u32 curTime = 0;
    //�Ƿ���Ҫ���ʱ��
    if(!PUBLIC_CHECKBIT(s_stRouteAttrib.routeAttrib,E_ROUTE_MOMENT)) return 1; //����Ҫ���ʱ��                  
    curTime=Area_GetConvertCurTime(s_stRouteAttrib.timeType); //��ǰʱ��  
    return Public_CheckTimeIsInRange(curTime,s_stRouteAttrib.startTime,s_stRouteAttrib.endTime);    
}
/*************************************************************
** ��������: Route_CheckTimeEx
** ��������: �����·�Ƿ����ʱ�䷶Χ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 0:������ʱ�䷶Χ,1:����ʱ�䷶Χ
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Route_CheckTimeEx(void)
{
    if(Route_CheckTime())return 1;
    //���������·����,���²�����·����
    Route_ClearAlarm();
    return 0;
}
/*************************************************************
** ��������: Route_GetNodeData
** ��������: ��ȡ��·��ǰ�ڵ���Ϣ������һ�ڵ��GPSλ����Ϣ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_GetNodeData(u16 CurNode)
{   
    u32 OffSetAddr; 
    ST_ROUTE_NODE NodeData;
    OffSetAddr= s_stRouteAttrib.nodeAddr + CurNode*STROUTE_NODE_SIZE; //��Žڵ�ĵ�ַ  
    sFLASH_ReadBuffer((u8 *)&s_stRouteAttrib.stNode,OffSetAddr,STROUTE_NODE_SIZE);                                  //�����ڵ�����
    //��������һ���ڵ�����         
    OffSetAddr += STROUTE_NODE_SIZE; //��Žڵ�ĵ�ַ
    sFLASH_ReadBuffer((u8 *)&NodeData,OffSetAddr,STROUTE_NODE_SIZE);                                         //�����ڵ�����  
    s_stRouteAttrib.point2    = NodeData.point;//����GPS������Ϣ
    //////////////////////
    s_stRouteAttrib.roadDistance = Route_GetPointDistance(&s_stRouteAttrib.stNode.point,&s_stRouteAttrib.point2);  
}
/*************************************************************
** ��������: Route_GetNodeDataAndCheckInRoute
** ��������: ��ȡ��·��ǰ�ڵ���Ϣ������һ�ڵ��GPSλ����Ϣ,���жϳ����Ƿ�����·��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Route_GetNodeDataAndCheckInRoute(u16 CurNode)
{
    Route_GetNodeData(CurNode);
    return Route_CheckIsInRouteEx();
}
/*************************************************************
** ��������: Route_FindRouteAreaTask
** ��������: ���ҳ������ڵ���·����
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_FindRouteAreaTask(void)
{
    unsigned char flag,i,check;    
    unsigned long addr;
    STROUTE_HEAD stRoute;	    //��ȡ��·����
    flag = 0;
    check = 0;    
    for(i=0; i<s_stRouteAttrib.routeSum;i++)
    {
        addr = ROUTE_ADDR_START + i*ONE_ROUTE_AREA_SIZE ;  
        sFLASH_ReadBuffer((u8 *)&stRoute,addr,STROUTE_HEAD_SIZE); //������·����
        if(stRoute.routeID==0) //����ID=0
            continue;
        //������о��εģ�����Ϊ0,0,��˵���Ƕ���·��ڵ�ʧ�ܲ������ݣ�����ֱ������?        
        if((stRoute.LeftUpPos.Lat == 0)||(stRoute.LeftUpPos.Long == 0))//����
            continue;
        s_stRouteAttrib.timeType    = Area_GetConverseBCDTimeType(&stRoute.startTime[0]); 
        s_stRouteAttrib.startTime   = Area_ConverseBcdGmtime(&stRoute.startTime[0],s_stRouteAttrib.timeType); //ת��ʱ��Ϊ�룬���ڶԱȡ�
        s_stRouteAttrib.endTime     = Area_ConverseBcdGmtime(&stRoute.endTime[0],s_stRouteAttrib.timeType);  
        //////////////////////////////
        s_stRouteAttrib.routeID     = stRoute.routeID;
        s_stRouteAttrib.routeAttrib = stRoute.routeAttrib;
        s_stRouteAttrib.nodeSum     = stRoute.nodeSum;
        s_stRouteAttrib.LeftUpPos   = stRoute.LeftUpPos;
        s_stRouteAttrib.RightDowPos = stRoute.RightDowPos;
        /////////////////////////////
        //memcpy(&s_stRouteAttrib.routeID,&stRoute.routeID,24);
        //�����·�Ƿ��ڿ���ʱ�����
        if(!Route_CheckTime())//��������ʱ�����
            continue;
        if(PUBLIC_CHECKBIT(s_stRouteAttrib.routeAttrib,E_OUTROUTE_TO_SERVER))//����·������ƽ̨
        {
            flag = 1;
        }
        //���㵱ǰ���Ƿ��ھ�����,�Ż��㷨��
        if(!ComparePos(&stRoute.LeftUpPos,&stRoute.RightDowPos,(T_GPS_POS *)&s_stRouteAttrib.Latitude))
        {
            check =1;
            s_stRouteAttrib.curRoute = i;//
            s_stRouteAttrib.nodeAddr = addr + STROUTE_HEAD_SIZE;
            break;
        }        
    }
    ////////////////////////////////
    if(check)//�ҵ���·
    {
        s_stRouteAttrib.curNode = 0;
        Route_GoNextStatusAtOnce(ROUTE_FIND_ROAD_SEGMENT);
    }
    else//δ�ҵ��κ���·
    if(flag)//Ҫ�����·������ƽ̨
    {
        if(0==Io_ReadAlarmBit(ALARM_BIT_LOAD_EXCURSION)) //���������ƫ��·�߱��������ظ�����ֹ���õ����ظ��ϴ�������Ϣ��
        {
            Io_WriteAlarmBit(ALARM_BIT_LOAD_EXCURSION,SET); //�����·�ߣ�����δ���κ���·�ڣ�������ƫ��·�߱���
        }
    }
    else
    {
         Io_WriteAlarmBit(ALARM_BIT_LOAD_EXCURSION,RESET);
    }
}
/*************************************************************
** ��������: Route_FindRoadSegmentTask
** ��������: ���ҵ�ǰ�����Ǹ�·��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_FindRoadSegmentTask(void)
{
    unsigned char i;
    //�����·�Ƿ������õĿ���ʱ�����.���������²���·�ߡ�
    if(!Route_CheckTimeEx()) return;
    ///////////////////////////
    for(i=0 ;s_stRouteAttrib.curNode < (s_stRouteAttrib.nodeSum-1) && i<ONE_TIME_READ_NUM; s_stRouteAttrib.curNode++,i++)//�ڵ�洢�Ǵ�0��ʼ,������11���ڵ㣬��洢��0~10.����Ϊ·�ι��������ڵ㣬����9~10�������һ��·���ˡ�
    {  
        //��ȡ��ǰ�ڵ���Ϣ����һ���GPS��Ϣ
        if(Route_GetNodeDataAndCheckInRoute(s_stRouteAttrib.curNode))
        {
            Route_GoNextStatusAtOnce(ROUTE_CHECK_IN_ROAD_SEGMENT); //��ѭ�ڵ���Ϻ�,��ǰ�㲻���κ�·����.��ѯ��һ����·
            return;
        }
    }
    //////////////////////////////
    if(s_stRouteAttrib.curNode >= (s_stRouteAttrib.nodeSum-1))    
    {
        Route_GoNextStatus(ROUTE_FIND_STATUS); //��ѭ�ڵ���Ϻ�,��ǰ�㲻���κ�·����.��ѯ��һ����·��
    }
    else//�ӿ�����
    {
        LZM_PublicSetOnceTimer(&s_stRouteTimer[ROUTE_TIMER_TEST],LZM_AT_ONCE,Route_CheckScanTask);
    }
}
/*************************************************************
** ��������: Route_FindRoadSegmentTask
** ��������: ���ҵ�ǰ�����Ǹ�·��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_CheckInRoadSegmentTask(void)
{
    if(Route_CheckIsInRouteEx())
    {
        Route_DisposeInOutAlarm(IN_ROUTE);                        //����·������
        s_stRouteAttrib.startdriverTime = RTC_GetCounter();       //��ʻʱ��,��¼��ʼʱ�䡣
        Route_SetSpeedParam();
        Route_GoNextStatus(ROUTE_HOLD_ROAD_SEGMENT);                    //�ҵ���ʻ��·��
    }
    else//������ѭ,����·�� 
    {
        Route_GoNextStatus(ROUTE_CHECK_IN_ROAD_SEGMENT);
    }
}
/*************************************************************
** ��������: Route_CheckNextRoadSegment
** ��������: ���ҵ�ǰ�����Ǹ�·��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_GoToChangeRoadSegment(unsigned short NextNode)
{
    s_stRouteAttrib.curNode = NextNode;
    Route_GoNextStatusAtOnce(ROUTE_CHANGE_ROAD_SEGMENT);
}
/*************************************************************
** ��������: Route_CheckNextRoadSegment
** ��������: ���ҵ�ǰ�����Ǹ�·��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_CheckNextRoadSegment(unsigned short NextNode)
{
    //�жϵ�ǰGPS���Ƿ�����һ·�߶��С�
    if(Route_GetNodeDataAndCheckInRoute(NextNode))
    {
        //��Ҫ��һ���ж��Ƿ������һ��·��
        Route_GoToChangeRoadSegment(NextNode);
    }                                
    else//��Ҫ��һ���ж��Ƿ��뿪·��
    {  
        s_stRouteAttrib.curNode = 0;
        Route_GoNextStatusAtOnce(ROUTE_CHECK_OUT_ROUTE);
    } 
}
/*************************************************************
** ��������: Route_FindRoadSegmentTask
** ��������: ���ҵ�ǰ�����Ǹ�·��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_HoldRoadSegmentTask(void)
{
    //�����·�Ƿ������õĿ���ʱ�����.���������²���·�ߡ�
    if(!Route_CheckTimeEx()) return;
    //////////////
    if(!Route_CheckIsInRouteEx())
    {
        if(0 == s_stRouteAttrib.curNode)//��·�׶�,�������·��                               
        {
            Route_CheckNextRoadSegment(s_stRouteAttrib.curNode+1);
        }
        else //��·ĩ��,�������·��
        if(s_stRouteAttrib.curNode == s_stRouteAttrib.nodeSum-2)
        {
            Route_CheckNextRoadSegment(s_stRouteAttrib.curNode-1);                                                                                                                            
        }
        else//��·�м�·��
        {
             //�������·��
            if(Route_GetNodeDataAndCheckInRoute(s_stRouteAttrib.curNode+1))
            {
                Route_GoToChangeRoadSegment(s_stRouteAttrib.curNode+1);
            } 
            else //���������Ҳ������ͷ������·��.
            {
                Route_CheckNextRoadSegment(s_stRouteAttrib.curNode-1); 
            }                                                           
        }
    }
}
/*************************************************************
** ��������: Route_CheckChangeRoadSegmentTask
** ��������: ����Ƿ�ı���·��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_CheckChangeRoadSegmentTask(void)
{
    if(Route_CheckIsInRouteEx())
    {
        Route_CheckDriverTime();
        Route_SetSpeedParam();
        Route_GoNextStatus(ROUTE_HOLD_ROAD_SEGMENT);//�ص����Ͻ׶ε�·�߽ڵ���
    }
    else
    {
        s_stRouteAttrib.startdriverTime = RTC_GetCounter();
        Route_SetSpeedParam();
        Route_GoNextStatus(ROUTE_HOLD_ROAD_SEGMENT);//�������·�ι��̣���������3��ȷ�ϵ�����·��ʱ�䶼��������������=2�������⣬���õ���0���¼����·��
    }
}
/*************************************************************
** ��������: Route_CheckOutRouteTask
** ��������: ����Ƿ������·
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_CheckOutRouteTask(void)
{
    unsigned char i;
    for(i=0 ;s_stRouteAttrib.curNode < (s_stRouteAttrib.nodeSum-1) &&i<ONE_TIME_READ_NUM; s_stRouteAttrib.curNode++,i++)
    {
        //��ȡ��ǰ�ڵ���Ϣ����һ���GPS��Ϣ
        if(Route_GetNodeDataAndCheckInRoute(s_stRouteAttrib.curNode))
        {
            Route_GoNextStatusAtOnce(ROUTE_CHANGE_ROAD_SEGMENT);
            return;
        }
    }
    ///////////////////////////////////
    if(s_stRouteAttrib.curNode >= (s_stRouteAttrib.nodeSum-1))                      //δ�����������κ�·���У�ȷ���뿪·�ߡ�
    {
        Route_DisposeInOutAlarm(OUT_ROUTE);
        Route_GoNextStatus(ROUTE_FIND_STATUS);
    }
    else//�ӿ�����
    {
        LZM_PublicSetOnceTimer(&s_stRouteTimer[ROUTE_TIMER_TEST],LZM_AT_ONCE,Route_CheckScanTask);
    }
}
/*************************************************************
** ��������: Route_SetDriverTimeAlarm
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_SetDriverTimeAlarm(unsigned char flag,unsigned short drivertime)
{
    s_stRouteAlarm.roadSegID    = s_stRouteAttrib.stNode.roadSegID;
    s_stRouteAlarm.driverTime   = drivertime;
    s_stRouteAlarm.driverResult = flag;//��ʻ��̫�� 
    Io_WriteAlarmBit(ALARM_BIT_LOAD_OVER_DRIVE,SET);     //��ʹʱ���������̱���
}
/*************************************************************
** ��������: Route_CheckDriverTime
** ��������: �����ʻʱ��,�Ƿ�����Ҫ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_CheckDriverTime(void)
{
    unsigned long curtime;
    unsigned short drivertime;             //��¼��ʻʱ��(��λ��)  
    curtime = RTC_GetCounter();
    if(PUBLIC_CHECKBIT(s_stRouteAttrib.stNode.roadAttrib,E_ROUTE_MOMENT))              //����ʻʱ������               
    {
        if(curtime>s_stRouteAttrib.startdriverTime)
        {
            drivertime = curtime-s_stRouteAttrib.startdriverTime;
        }
        else
        {
            drivertime = 0;
        }
        //////////////////////
        if(drivertime > s_stRouteAttrib.stNode.longLimitTime) 
        {                 
            Route_SetDriverTimeAlarm(1,drivertime);//��ʻ����
        }
        else
        if(drivertime < s_stRouteAttrib.stNode.shortLimitTime) 
        {
            Route_SetDriverTimeAlarm(0,drivertime);////��ʻ����
        }
    }
    else
    {
        Io_WriteAlarmBit(ALARM_BIT_LOAD_OVER_DRIVE,RESET);//�޸�����,��Ҫ����ñ�־.
    }
    ////��ʻʱ����/////////////////////////////////
    s_stRouteAttrib.startdriverTime = curtime;
}
/************************************************************
��������: ��·����
��������: 
��ڲ���: RouteAlarmItem:��·����
���ڲ���:
************************************************************/ 
void Route_DisposeInOutAlarm(E_INOUTROUTE InOutAlarm) 
{
    s_stRouteAlarm.Orientation      = InOutAlarm;   //��������
    s_stRouteAlarm.INOUTPostType    = E_ROUTE;      //��·
    s_stRouteAlarm.INOUTTypeID      = s_stRouteAttrib.routeID;//��·ID
    switch(InOutAlarm)
    {
        case IN_ROUTE:
            ////////////////////////////////////////
            Io_WriteAlarmBit(ALARM_BIT_LOAD_EXCURSION,RESET);     //���ƫ��·�߱��� 
            /////////////////////////////////////////
            if(PUBLIC_CHECKBIT(s_stRouteAttrib.routeAttrib,E_INROUTE_TO_DRIVER))     //�����ȥ������˾������ʹ����������
            {
                //PlayMulTTSVoiceStr("���ѽ����趨·��!");
                Public_PlayTTSVoiceStr("���ѽ����趨·��!");//dxl,2014.8.8�����蹤�ύ�޸�

               // LcdShowCaptionEx((char *)"���ѽ���ָ����·",17);    //fanqinghai

            }
            if(PUBLIC_CHECKBIT(s_stRouteAttrib.routeAttrib,E_INROUTE_TO_SERVER))     //�����·����:����·���������ʱ������������
            {
							  if(1 == BBGNTestFlag)//dxl,2016.5.24�������ܼ��ʱ����·����λ������ģ�鸺��
		            {
									  Io_WriteAlarmBit(ALARM_BIT_RESERVER15,SET);
		            }
		            else
		            {
                    Io_WriteAlarmBit(ALARM_BIT_IN_OUT_ROAD,SET);      //��ʶ����·����
							  }
            }
            else
            {
							  if(1 == BBGNTestFlag)//dxl,2016.5.24�������ܼ��ʱ����·����λ������ģ�鸺��
		            {
		                Io_WriteAlarmBit(ALARM_BIT_RESERVER15,RESET);
		            }
		            else
		            {
                    Io_WriteAlarmBit(ALARM_BIT_IN_OUT_ROAD,RESET);   //����޽�·�߱�������,�����ϴ�����г�·�߱�������Ҫ����� 
								}
            }
            break; 
        case OUT_ROUTE:
            s_stRouteAttrib.needplayflag = 0;
            if(PUBLIC_CHECKBIT(s_stRouteAttrib.routeAttrib,E_OUTROUTE_TO_DRIVER)) 
            {
                s_stRouteAttrib.needplayflag = 1;
               // LcdShowCaptionEx((char *)"�����뿪ָ����·",17);    //fanqinghai

            }
            ////////////////////////////////////
            if(PUBLIC_CHECKBIT(s_stRouteAttrib.routeAttrib,E_OUTROUTE_TO_SERVER))    //�����·����:����·���������ʱ������������
            {
                Io_WriteAlarmBit(ALARM_BIT_LOAD_EXCURSION,SET);      //·��ƫ�뱨��
							  if(1 == BBGNTestFlag)
								{
								    Io_WriteAlarmBit(ALARM_BIT_RESERVER15,SET);
								}
								else
								{
                    Io_WriteAlarmBit(ALARM_BIT_IN_OUT_ROAD,SET);      //��ʶ����·����    
								}
            }
            else
            {
							  if(1 == BBGNTestFlag)
								{
								    Io_WriteAlarmBit(ALARM_BIT_RESERVER15,RESET);
								}
								else
								{
                    Io_WriteAlarmBit(ALARM_BIT_IN_OUT_ROAD,RESET);    //��ʶ����·���� 
								}
            } 
            Route_ClearSpeedParam();//��·�ߣ��볬�ٱ�����
            break; 
        default:
            break;
    }
}
/*********************************************************************
//����:��ȡGPS����
//����:
//���:���ݳ���
//��ע:
*********************************************************************/
void Route_GetCurGpsCoordinate(void)
{
    unsigned char buffer[28];
    Public_GetCurBasicPositionInfo(buffer);
    Public_Mymemcpy((unsigned char *)&s_stRouteAttrib.Latitude,(unsigned char *)&buffer[8],sizeof(unsigned long),1); 
    Public_Mymemcpy((unsigned char *)&s_stRouteAttrib.Longitude,(unsigned char *)&buffer[12],sizeof(unsigned long),1); 
}
/*******************************************************************************
** ��������: Route_GetInOutAlarmSubjoinInfo
** ��������: ��ȡ����·�߱���������Ϣ��(����������ϢID�����ȡ�������Ϣ��) :��������ϢIDΪ0x12����������
** ��ڲ���: ��Ҫ�������·�߱���������Ϣ�����׵�ַ
** ���ڲ���: 
** ���ز���: ����·�߱���������Ϣ�����ܳ���,0:��ʾδ����·��,
*******************************************************************************/
unsigned char Route_GetInOutAlarmSubjoinInfo(unsigned char *data) 
{
    unsigned char len; 
    unsigned char buffer[20]={0};
		
		if(1 == BBGNTestFlag)//dxl,2016.5.24�������ʱ����·�߱���������route�ļ����ƣ�����������ģ��
		{
		    //if(!Io_ReadAlarmBit(ALARM_BIT_RESERVER15))return 0;
		}
		else
		{
        if(!Io_ReadAlarmBit(ALARM_BIT_IN_OUT_ROAD))return 0;
		}
		
    len =0;
    buffer[len++]= IN_OUT_ROUTE_OVERHEAD_INFO_ID;
    buffer[len++]= IN_OUT_ROUTE_OVERHEAD_INFO_LEN;
    buffer[len++]= s_stRouteAlarm.INOUTPostType;
    Public_ConvertLongToBuffer(s_stRouteAlarm.INOUTTypeID,&buffer[len]);
    len += 4;
    buffer[len++]= s_stRouteAlarm.Orientation;
    memcpy(data,buffer,len);
    return len;
}

/*************************************************************
** ��������: Route_GetDriveAlarmInfoSubjoinInfo
** ��������: ��ȡ��ʻʱ�䱨��������Ϣ��(����������ϢID�����ȡ�������Ϣ��) :��������ϢIDΪ0x12����������
** ��ڲ���: ��Ҫ������ʻʱ�䱨��������Ϣ�����׵�ַ
** ���ڲ���: 
** ���ز���: ��ʻʱ�䱨��������Ϣ�����ܳ���,0:��ʾδ����,
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Route_GetDriveAlarmInfoSubjoinInfo(unsigned char *data) 
{
    unsigned char len; 
    unsigned char buffer[20]={0};
    /*static u8 time ;
    char DisBuffer[40] ;
    static u8 lastdrect ; */


    if(!Io_ReadAlarmBit(ALARM_BIT_LOAD_OVER_DRIVE))return 0;
    len =0;
    buffer[len++]= LOAD_OVER_DRIVE_OVERHEAD_INFO_ID;
    buffer[len++]= LOAD_OVER_DRIVE_OVERHEAD_INFO_LEN;
    Public_ConvertLongToBuffer(s_stRouteAlarm.roadSegID,&buffer[len]);
    len += 4;
    Public_ConvertShortToBuffer(s_stRouteAlarm.driverTime,&buffer[len]);
    len += 2;
    buffer[len++]= s_stRouteAlarm.driverResult;
    memcpy(data,buffer,len);

    return len;
}

/*************************************************************
** ��������: Route_GetDeviationAlarmInfo
** ��������: ��ȡ·��ƫ�뱨��������Ϣ(����������ϢID�����ȡ�������Ϣ��) :��������ϢIDΪ0x13����������
** ��ڲ���: ��Ҫ����·��ƫ�뱨��������Ϣ�����׵�ַ
** ���ڲ���: 
** ���ز���: ·��ƫ�뱨��������Ϣ�����ܳ���,0:��ʾδ����,
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Route_GetDeviationAlarmInfo(unsigned char *data) 
{
    unsigned char len; 
    unsigned char buffer[20]={0};
   /* static u8 time ;
    char DisBuffer[40] ;
    static u8 lastdrect ;
		*/
    if(1 == BBGNTestFlag)
		{
		    if(!Io_ReadAlarmBit(ALARM_BIT_RESERVER15))return 0;
		}
		else
		{
        if(!Io_ReadAlarmBit(ALARM_BIT_IN_OUT_ROAD))return 0;
		}
    len =0;
    buffer[len++]= IN_OUT_ROUTE_OVERHEAD_INFO_ID;
    buffer[len++]= IN_OUT_ROUTE_OVERHEAD_INFO_LEN;
    buffer[len++]= s_stRouteAlarm.INOUTPostType;
    Public_ConvertLongToBuffer(s_stRouteAlarm.roadSegID,&buffer[len]);
    len += 4;
    buffer[len++]= s_stRouteAlarm.Orientation;
    memcpy(data,buffer,len);

    return len;
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
void Route_CheckForResetParam(unsigned long RouteID) 
{
    if(s_stRouteAttrib.routeID!=RouteID)return;
    Route_ResetParam();
}
/*************************************************************
** ��������: Route_DisposeSetRoad
** ��������: :����·��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Route_DisposeSetRoad(unsigned char flag,unsigned char *pBuffer, unsigned short BufferLen)
{
    static T_GPS_POS MinLeftUpPos;   //ȡ��γ����СֵΪ������һ������.
    static T_GPS_POS MaxLeftUpPos;   //ȡ��γ�����ֵΪ������һ������
    static u16 MaxWidth = 0;         //���·��    
    static u16 CurRoute  = 0;        //���浱ǰ��·��Flash�еı��,������gRoutePcb.CurRoute����Ϊ���ܵ�ǰ·�߼���ʱ���������ã������·��ʱ�������������ܳ������⡣
    static u16 NodeNum  = 0;         //���浱ǰ��·��Flash�еĽڵ���
    static short NodeCount = 0;        //�ڵ����
    STROUTE_HEAD stRoute,stTmpRoute; //��ȡ��·����
    ST_ROUTE_NODE stNodeData;
    unsigned short datalen;
    u32 ExpLatLog = 0;               //·��=��������ľ�γ��ֵ
    u32	addr,tmpAddr;        
    u8 *pTmp;               //�·�Э��������ָ��
    u8 i;               //
    ///////////////////   
	  if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))
		{
		    ClrRoute();
		}
	
    pTmp =pBuffer;
    datalen = 0;
    //////////////////
    if(flag<2)
    {
        CurRoute = 0;
        NodeNum  = 0;
        MaxWidth = 0;        
        NodeCount = 0;
        if(BufferLen<8)return 2;//��Ϣ����
        memset(&stRoute,0,STROUTE_HEAD_SIZE);                                   //���Ϊ0��ͬʱ���Ͻǣ����½�����Ϊ0.
        memset(&MinLeftUpPos,0,sizeof(T_GPS_POS));                              //������Сֵ�����ֵΪ0.
        memset(&MaxLeftUpPos,0,sizeof(T_GPS_POS));
        /////////////////////
        stRoute.routeID = Public_ConvertBufferToLong(&pTmp[datalen]);
        datalen += 4;
        stRoute.routeAttrib =Public_ConvertBufferToShort(&pTmp[datalen]);
        datalen += 2;
        //////////////////////
        if(PUBLIC_CHECKBIT(stRoute.routeAttrib, 0))//�����Ƿ����ʱ����Ϣ
        {
            if(BufferLen<20) return 2;//��Ϣ����
            memcpy(stRoute.startTime,&pTmp[datalen],6);
            datalen += 6;
            memcpy(stRoute.endTime,&pTmp[datalen],6);
            datalen += 6;
        }
        stRoute.nodeSum = Public_ConvertBufferToShort(&pTmp[datalen]);          
        datalen += 2; 
        NodeCount = stRoute.nodeSum;//��¼�ڵ�����
        if(NodeCount > ROUTE_NODE_SUM||0==NodeCount)return 1;//����ڵ������������ڵ����򷵻�ʧ�ܡ�
        ///////////////////////
        if(0==flag)//δ�ְ������
        {
            if(BufferLen < datalen+NodeCount*18)return 2;//��Ϣ����
        }
        ///����·��Ϣд��Flash////////////
        addr = 0;        
        for(i =0; i< s_stRouteAttrib.routeSum; i++)//�ж�������·ID�Ƿ��Ѵ���
        {
            tmpAddr=ROUTE_ADDR_START + i*ONE_ROUTE_AREA_SIZE;  
            sFLASH_ReadBuffer((u8 *)&stTmpRoute,tmpAddr,STROUTE_HEAD_SIZE); //������·����	
            if(stTmpRoute.routeID==stRoute.routeID)
            {
                CurRoute = i;
                addr = tmpAddr;
                break;
            }
            else
            if(0==stTmpRoute.routeID)
            {
                CurRoute = i;
                addr = tmpAddr;
            }
        }
        /////////////////////////
        if(0==addr)//
        {
            if(s_stRouteAttrib.routeSum >= ROUTE_NUM_MAX)return 1;//����������ֵ,����ʧ��                      
            CurRoute = s_stRouteAttrib.routeSum;
            addr = ROUTE_ADDR_START + CurRoute*ONE_ROUTE_AREA_SIZE;
            s_stRouteAttrib.routeSum++;
        }
        /////////////////////
        stRoute.Head = ROUTE_HEAD_FLAG;
        Area_MdyDataInFlash((u8*)&stRoute,addr,STROUTE_HEAD_SIZE);  
        Route_CheckForResetParam(stRoute.routeID);
    }
    ///////////////////////
    do
    {
        if(datalen+18 > BufferLen)return 2;//��Ϣ����
        NodeCount--;//�ڵ�����-1
        //��ýڵ���Ϣ
        stNodeData.NodeID= Public_ConvertBufferToLong(&pTmp[datalen]);
        datalen += 4;
        stNodeData.roadSegID = Public_ConvertBufferToLong(&pTmp[datalen]);
        datalen += 4;
        stNodeData.point.Lat  = Public_ConvertBufferToLong(&pTmp[datalen]);
        datalen += 4;
        stNodeData.point.Long = Public_ConvertBufferToLong(&pTmp[datalen]);
        datalen += 4;
        ////////
        stNodeData.roadWidth = pTmp[datalen++];
			  if(stNodeData.roadWidth <= 30)//dxl,2016.5.16,����·����ǰ1�뱨ƫ�뱨������˼Ӵ�·��
				{
				    stNodeData.roadWidth = 40;
				}
        stNodeData.roadAttrib = pTmp[datalen++];
        if(PUBLIC_CHECKBIT(stNodeData.roadAttrib, 0))//��ʻʱ��
        {
            stNodeData.longLimitTime = Public_ConvertBufferToShort(&pTmp[datalen]);
            datalen += 2;
            stNodeData.shortLimitTime = Public_ConvertBufferToShort(&pTmp[datalen]);
            datalen += 2;
        }
        /////////////////
        if(PUBLIC_CHECKBIT(stNodeData.roadAttrib, 1))//����
        {
            stNodeData.limitSpeed = Public_ConvertBufferToShort(&pTmp[datalen]);
            datalen += 2;
            stNodeData.speedDuration = pTmp[datalen++];
        }
        ///////////////////////
        //������·�Ĵ����
        if(MinLeftUpPos.Lat == 0)                             //���Ϊ0,�����һ����γ��ֵ
        {
            MinLeftUpPos.Lat = stNodeData.point.Lat;
            MaxLeftUpPos.Lat = stNodeData.point.Lat;
        }
        else 
        if(MinLeftUpPos.Lat > stNodeData.point.Lat)  //ȡ��Сֵ
        {
            MinLeftUpPos.Lat = stNodeData.point.Lat;
        }
        else 
        if(MaxLeftUpPos.Lat < stNodeData.point.Lat)  //ȡ���ֵ
        {
            MaxLeftUpPos.Lat = stNodeData.point.Lat;
        }
        /////////////////////////////
        if(MinLeftUpPos.Long == 0) 
        {
            MinLeftUpPos.Long =  stNodeData.point.Long;
            MaxLeftUpPos.Long =  stNodeData.point.Long;
        }
        else 
        if(MinLeftUpPos.Long > stNodeData.point.Long)
        {
            MinLeftUpPos.Long =  stNodeData.point.Long;
        }
        else 
        if(MaxLeftUpPos.Long < stNodeData.point.Long)
        {
            MaxLeftUpPos.Long =  stNodeData.point.Long;
        }
        ////////////////////
        if(MaxWidth == 0)
        {
            MaxWidth = stNodeData.roadWidth;
        }
        else 
        if(MaxWidth < stNodeData.roadWidth)                                     //��·������ֵ
        {
            MaxWidth =  stNodeData.roadWidth;
        }
        ////////////////////////
        //�ѽڵ���Ϣд��Flash
        addr=ROUTE_ADDR_START + CurRoute*ONE_ROUTE_AREA_SIZE + STROUTE_HEAD_SIZE+NodeNum*STROUTE_NODE_SIZE; //��Žڵ�ĵ�ַ   
        Area_MdyDataInFlash((u8*)&stNodeData,addr,STROUTE_NODE_SIZE);
        NodeNum++;
    }
    while(datalen<BufferLen);
    //���=0,˵���յ��ְ������нڵ���Ϣ
    if(NodeCount == 0)
    {
        addr = ROUTE_ADDR_START + CurRoute*ONE_ROUTE_AREA_SIZE ;  
        sFLASH_ReadBuffer((u8 *)&stRoute,addr,STROUTE_HEAD_SIZE); //������·����.
        //ExpLatLog = MaxWidth/30.9 *(0.0003*1000000)(30.9��/��=30.9ԼΪ31,1��=1/60/60ԼΪ0.000278��,����ԼΪ0.0003.) 
        ExpLatLog = (MaxWidth*100*3)/31;                                     //�����ã���γ��Ūһ���������ģ�γ�Ȼ��ã����������˳���Ǹ�λ�����ֵ������롣
        MinLeftUpPos.Lat -= ExpLatLog;                                       //��·���������,��ֹ������ת��ʱ����Щ·���ϵĵ㲻�ھ����ϡ�
        MinLeftUpPos.Long -= ExpLatLog;
        MaxLeftUpPos.Lat += ExpLatLog;
        MaxLeftUpPos.Long += ExpLatLog;
        ///////////////////////////////
        stRoute.LeftUpPos = MinLeftUpPos;                             //����һ����·������С�㹹�ɵĴ���Σ������㷨�Ż���
        stRoute.RightDowPos = MaxLeftUpPos;
        stRoute.Head = ROUTE_HEAD_FLAG;
        Area_MdyDataInFlash((u8*)&stRoute,addr,STROUTE_HEAD_SIZE);     //�޸���·����.        
        Route_CheckForResetParam(stRoute.routeID);
        s_stRouteAttrib.preSpeed = 0xff;
    }
    return 0;               //����ֵ
}
/*************************************************************
** ��������: Route_DisposeSetRoad
** ��������: :ɾ��·��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char Route_DisposeDelRoad(unsigned char *pBuffer, unsigned short BufferLen)
{
    u8	i,j,flag;
    u32	OffSetAddr,RouteID;
    u8 *pTmp;
    u8 Sum;    
    STROUTE_HEAD stTmpRoute; //��ȡ��·����
    
    Sum = *pBuffer++;              //ɾ��·������    
    if(Sum)
    {
        pTmp =pBuffer;
        for(i=0; i<Sum; i++)
        {
            Public_Mymemcpy((unsigned char *)&RouteID,(unsigned char *)&pTmp[0],sizeof(unsigned long),1); //���·��ID
            pTmp+=sizeof(unsigned long);
            flag =0;
            for(j=0; j<s_stRouteAttrib.routeSum; j++)
            {
                OffSetAddr=ROUTE_ADDR_START + j*ONE_ROUTE_AREA_SIZE ;  
                sFLASH_ReadBuffer((u8 *)&stTmpRoute,OffSetAddr,STROUTE_HEAD_SIZE);//������·����	
                if(RouteID==stTmpRoute.routeID)
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
            Public_Mymemcpy((unsigned char *)&RouteID,(unsigned char *)&pTmp[0],sizeof(unsigned long),1); //���·��ID
            pTmp+=sizeof(unsigned long);
            flag =0;
            for(j=0; j<s_stRouteAttrib.routeSum; j++)
            {
                OffSetAddr=ROUTE_ADDR_START + j*ONE_ROUTE_AREA_SIZE ;
                sFLASH_ReadBuffer((u8 *)&stTmpRoute,OffSetAddr,STROUTE_HEAD_SIZE);//������·����	
                if(RouteID==stTmpRoute.routeID)
                {
                    stTmpRoute.routeID = 0;
                    Area_MdyDataInFlash((u8*)&stTmpRoute,OffSetAddr,STROUTE_HEAD_SIZE);
                }
            }
            Route_CheckForResetParam(RouteID);
        }          
    }
    else
    {                                              //����=0��ɾ��������·
        ClrRoute();    
    }                  
    //ֻ��Ӧ��
    return 0;
}
/********************************************************************
* ���� : Route_CheckScanTask
* ����: 
* ���: 
* ȫ�ֱ���: 
*
* ��ע: 
********************************************************************/  
void Route_CheckScanTask(void)
{
    switch(s_stRouteAttrib.status)
    {
        case ROUTE_FIND_STATUS:
            Route_FindRouteAreaTask();
            break;
        case ROUTE_FIND_ROAD_SEGMENT:
            Route_FindRoadSegmentTask();
            break;
        case ROUTE_CHECK_IN_ROAD_SEGMENT:
            Route_CheckInRoadSegmentTask();
            break;
        case ROUTE_HOLD_ROAD_SEGMENT:
            Route_HoldRoadSegmentTask();
            break;
        case ROUTE_CHANGE_ROAD_SEGMENT:
            Route_CheckChangeRoadSegmentTask();
            break;
        case ROUTE_CHECK_OUT_ROUTE:
            Route_CheckOutRouteTask();
            break;
    }
}
/*************************************************************
** ��������: Route_PlayRoadExcursionAlarmVoice
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Route_PlayRoadExcursionAlarmVoice(void)
{
    if(!Public_PlayE2ParamAlarmVoice(E2_ROUTE_EXCURSION_ALARM_VOICE_ID))
    {
        Public_PlayTTSVoiceStr("�밴�涨��·��ʻ");
    }
}


/*************************************************************
** ????: SpeedMonitor_CheckPlayFlag
** ????: ????????
** ????: VaildFlag??????,stPlay???????
** ????: 
** ????: 1:????,0:?????
** ????: ?
** ????:
*************************************************************/
#define ONCE_MIN_PLAY_TIME                      5//????????
#define ONCE_DEFAULT_PLAY_TIME                  10//??????10?

unsigned char SpeedMonitor_CheckPlayFlag(unsigned char VaildFlag,STPLAY_ATTRIB *stPlay)
{
    unsigned short temp,tmp;    
    if(VaildFlag&&stPlay->OnceNumber)
    {
        stPlay->GroupTimeCnt++;//????????
        if(stPlay->GroupTime)//???????
        {
            temp = stPlay->GroupTimeCnt%stPlay->GroupTime;//5????
        }
        else
        {
            temp = stPlay->GroupTimeCnt;
        }
        //////////////////////////////////////
        if(temp==1)//??????????
        {
            stPlay->OnceCnt = 0;//???????????? 
        }
        /////////////////////////
        tmp = stPlay->OnceInterval;
        if(0==tmp)								//???
        {
            tmp=ONCE_DEFAULT_PLAY_TIME;
        }
        else
        if(tmp < ONCE_MIN_PLAY_TIME)
        {
            tmp = ONCE_MIN_PLAY_TIME;//????????
        }
        if(1 ==(temp%tmp))					//????????
        {
            if(stPlay->OnceCnt<stPlay->OnceNumber)
            {
                stPlay->OnceCnt++;
                return 1;
            }
        }
    }
    else
    {
        stPlay->OnceCnt = 0;
        stPlay->GroupTimeCnt = 0;
    }
    return 0;
}
/*************************************************************
** ????: SpeedMonitor_CheckPlayFlag
** ????: ????????
** ????: VaildFlag??????,stPlay???????
** ????: 
** ????: 1:????,0:?????
** ????: ?
** ????:
*************************************************************/
unsigned char Route_CheckPlayFlag(unsigned char VaildFlag,STPLAY_ATTRIB *stPlay)
{
    unsigned short temp,tmp;    
    if(VaildFlag&&stPlay->OnceNumber)
    {
        stPlay->GroupTimeCnt++;
        if(stPlay->GroupTime)
        {
            temp = stPlay->GroupTimeCnt%stPlay->GroupTime;
        }
        else
        {
            temp = stPlay->GroupTimeCnt;
        }
        //////////////////////////////////////
        if(temp==1)
        {
            stPlay->OnceCnt = 0;
        }
        /////////////////////////
        tmp = stPlay->OnceInterval;
        if(0==tmp)
        {
            tmp=ONCE_DEFAULT_PLAY_TIME;
        }
        else
        if(tmp < ONCE_MIN_PLAY_TIME)
        {
            tmp = ONCE_MIN_PLAY_TIME;
        }
        if(1 ==(temp%tmp))
        {
            if(stPlay->OnceCnt<stPlay->OnceNumber)
            {
                stPlay->OnceCnt++;
                return 1;
            }
        }
    }
    else
    {
        stPlay->OnceCnt = 0;
        stPlay->GroupTimeCnt = 0;
    }
    return 0;
}
/*************************************************************
** ��������: Route_CheckAlarmVoiceTask
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Route_CheckAlarmVoiceTask(void)
{

    unsigned char flag;
    flag = 0;
    if(s_stRouteAttrib.needplayflag&&Io_ReadAlarmBit(ALARM_BIT_LOAD_EXCURSION))
    {
        if(VDRDoubt_GetCarRunFlag()) 
        flag = 1;
    }    
    ////////////
    if(Route_CheckPlayFlag(flag,&s_stRouteAttrib.stPlay))
    {
        Route_PlayRoadExcursionAlarmVoice();
    }
	
    
}

/********************************************************************
* ���� : ��鳵���Ƿ�����·��
* ����: 
* ���: 
* ȫ�ֱ���: 
*
* ��ע:ÿ1sִ��һ��
********************************************************************/  
void Route_CheckGpsDataChange(void)
{
    unsigned char speed;
    GPS_STRUCT stTmpGps;
    Gps_CopygPosition(&stTmpGps);    
    if(stTmpGps.Second != s_stRouteAttrib.preSecond)
    {
			  if(1 == BBGNTestFlag)
				{
				//������·�߱�������λ״̬
        if(Io_ReadAlarmMaskBit(ALARM_BIT_RESERVER15))
        {
            if(1==s_stRouteAttrib.nonavigationCnt)
            {
                s_stRouteAttrib.nonavigationCnt =0;
                Route_ClearAlarm();
            }
            return;
        }
				}
				else
				{
        //������·�߱�������λ״̬
        if(Io_ReadAlarmMaskBit(ALARM_BIT_IN_OUT_ROAD))
        {
            if(1==s_stRouteAttrib.nonavigationCnt)
            {
                s_stRouteAttrib.nonavigationCnt =0;
                Route_ClearAlarm();
            }
            return;
        }
			  }
        s_stRouteAttrib.nonavigationCnt = 1; 
        speed = Area_GetCurSpeed();
        ///////////////////
        Route_GetCurGpsCoordinate();        
        ///////////////////////////
        if(speed||s_stRouteAttrib.preSpeed)
        {
            LZM_PublicKillTimer(&s_stRouteTimer[ROUTE_TIMER_TEST]);
            Route_CheckScanTask();
        }
        //////////////////////////
        Route_CheckAlarmVoiceTask();
        ///////////////
        s_stRouteAttrib.preSpeed = speed;
        s_stRouteAttrib.preSecond = stTmpGps.Second;
    }
}
/********************************************************************
* ���� : ��鳵���Ƿ�����·��
* ����: 
* ���: 
* ȫ�ֱ���: 
*
* ��ע:ÿ1sִ��һ��
********************************************************************/  
void Route_ScanTask(void)
{		//Io_WriteStatusBit(STATUS_BIT_NAVIGATION,SET);
    
    if(Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
    {
        Route_CheckGpsDataChange();
    }
    else
    {
        s_stRouteAttrib.stPlay.GroupTimeCnt =0;
        s_stRouteAttrib.stPlay.OnceCnt =0;
        /////////////////
        s_stRouteAttrib.preSecond = 0xff;
        s_stRouteAttrib.preSpeed = 0xff;
        if(s_stRouteAttrib.nonavigationCnt<ROUTE_RESET_STATUS_COUNT)s_stRouteAttrib.nonavigationCnt++;//10s
        else
        if(s_stRouteAttrib.nonavigationCnt == ROUTE_RESET_STATUS_COUNT)
        {
            s_stRouteAttrib.nonavigationCnt ++;
            LZM_PublicKillTimer(&s_stRouteTimer[ROUTE_TIMER_TEST]);
            //Route_ClearAlarm();
            Route_ResetParam();//modify by joneming20150429
        }
    }
}
/*************************************************************
** ��������: Route_StartScanTask
** ��������: 
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_StartScanTask(void)
{
   LZM_PublicSetCycTimer(&s_stRouteTimer[ROUTE_TIMER_TASK],PUBLICSECS(0.2),Route_ScanTask);
}
/*************************************************************
** ��������: Route_UpdatePram
** ��������: ���û��޸���·��ر�����,��Ҫ����·��ģ����ر���
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void Route_UpdatePram(void)
{
    u8 PramLen;
    u8 Buffer[20];
    ////////////////////
    //��EEPROM��ȡ���ݸ��±���
    PramLen = EepromPram_ReadPram(E2_ROUTE_EXCURSION_ALARM_NUMBER_ID, Buffer);
    if(PramLen)
    {
        s_stRouteAttrib.stPlay.OnceNumber =Public_ConvertBufferToLong(Buffer);
    }
    else
    {
        s_stRouteAttrib.stPlay.OnceNumber = 3;
    }
    PramLen = EepromPram_ReadPram(E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID, Buffer);
    if(PramLen)
    {
        s_stRouteAttrib.stPlay.GroupTime =Public_ConvertBufferToLong(Buffer)*60;
    }
    else
    {
        s_stRouteAttrib.stPlay.GroupTime = 300;
    }
}
/*************************************************************
** ��������: Route_ParameterInitialize
** ��������: ��·������ʼ��
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Route_ParameterInitialize(void) 
{
    Route_ResetParam();
    s_stRouteAttrib.routeSum =0;
    ///////////////////
    s_stRouteAttrib.nonavigationCnt = 0xff;
    ///////////////////////////////
    Route_UpdatePram();
    ////////////////////////////
    s_stRouteAttrib.minRoadWidth = 30;//��С·��50,�ͼ�ʱΪ30
}
/********************************************************************
* ���� : ��ʼ����·
* ����:  
* ���: 
* ȫ�ֱ���: 
*
* ��ע: ȷ����·����,��Ҫ�ĸ�λ����
********************************************************************/ 
void Route_ReadAndCheckParam(void) 
{
    u8 i;
    u32	OffSetAddr;
    STROUTE_HEAD  stRouteData;	  //��ȡ��·����
    //////////////////////
    memset(&s_stRouteAttrib,0,sizeof(s_stRouteAttrib));
    //////////////////////////
    Route_ParameterInitialize();
    //�����·��
    for(i=0; i<ROUTE_NUM_MAX; i++)
    {
        OffSetAddr=ROUTE_ADDR_START + i*ONE_ROUTE_AREA_SIZE ;  
        sFLASH_ReadBuffer((u8 *)&stRouteData,OffSetAddr,STROUTE_HEAD_SIZE);//������·����
        if(ROUTE_HEAD_FLAG == stRouteData.Head)
        {
            if(0xFFFFFFFF == stRouteData.routeID)
            {
                ClrRoute();
                break;
            }
            else
            {
                s_stRouteAttrib.routeSum++;
            }
        }
        else 
        if(0xFFFF != stRouteData.Head)
        {
            ClrRoute();
            break;
        }
        else 
        if(0xFFFF == stRouteData.Head)
        {
            break;
        }
    }
    //////////////////////////////////////////////////////////
    LZM_PublicSetOnceTimer(&s_stRouteTimer[ROUTE_TIMER_TASK],PUBLICSECS(10),Route_StartScanTask);    
}
/********************************************************************
* ���� : ��ʼ����·
* ����:  
* ���: 
* ȫ�ֱ���: 
*
* ��ע: ȷ����·����,��Ҫ�ĸ�λ����
********************************************************************/ 
void InitRoute(void) 
{    
    LZM_PublicKillTimerAll(s_stRouteTimer,ROUTE_TIMER_TASK); 
    ///////////////////
    SetTimerTask(TIME_ROUTE,LZM_TIME_BASE);
    //////////////////////////////////////////////////////////
    LZM_PublicSetOnceTimer(&s_stRouteTimer[ROUTE_TIMER_TASK],PUBLICSECS(20),Route_ReadAndCheckParam);
}

/********************************************************************
* ���� : ��鳵���Ƿ�����·��
* ����: 
* ���: 
* ȫ�ֱ���: 
*
* ��ע:ÿ1sִ��һ��
********************************************************************/  
FunctionalState Route_TimeTask(void)
{
    LZM_PublicTimerHandler(s_stRouteTimer,ROUTE_TIMERS_MAX);
    return ENABLE;   
}
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

