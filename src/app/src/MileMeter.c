/*************************************************************************
*
* Copyright (c) 2008,�������������¼����������޹�˾
* All rights reserved.
*
* �ļ����� : MileMeter.c 
* ����     :
*
* ��ǰ�汾 :
* ������   :
* �޸�ʱ�� :
*
* ��ʷ�汾 :
* ������   :
* ���ʱ�� :
*
* ��ע :
*************************************************************************/

#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
#include "VDR.h"

d64 MileSum;//m
d64 OldMileSum;
Point_Struct OldPoint;
Point_Struct Point;
u8 GpsPosSpeed;
u8 GpsPosSpeedTimeCount;
//d64 Max,Average,TwoPointDistanceTest[N-1];
static u8	GpsMinuteSpeed = 0;//GPSÿ����ƽ���ٶ�
//static u16	GpsSpeedSum = 0;//GSP˲ʱ�ٶ��ۻ���
//static u8	MinuteSpeedCount = 0;//ƽ���ٶȼ���
static u32	OldMileTime;//���ͳ�Ƶ���һ��λ�õ��ʱ��
static u32	MileTime;//���ͳ�Ƶĵ�ǰλ�õ��ʱ��
extern GPS_STRUCT	gPosition;//��ǰ��Чλ��
extern u8	SpeedFlag;//�ٶ�����,0Ϊ����,1ΪGPS
extern  u8	AccOffGpsControlFlag;//ACC OFFʱGPSģ����Ʊ�־,0Ϊ��,1Ϊ��
void MileMeter_GpsPosSpeedCount(void);
/********************************************************************
* ���� : DecimalDegree
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/

void MileMeter_DecimalDegree(Point_Struct *Point,GPS_STRUCT *GpsData)
{

    d64 temp;
	
	temp = GpsData->Longitue_FX/10000.0;
	temp = temp + GpsData->Longitue_F;
	temp = temp/60.0;
	temp = temp + GpsData->Longitue_D;
	Point->LonDegree = temp;
	temp = GpsData->Latitue_FX/10000.0;
	temp = temp + GpsData->Latitue_F;
	temp = temp/60.0;
	temp = temp + GpsData->Latitue_D;
	Point->LatDegree = temp;
	
}
/********************************************************************
* ���� :
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/

void MileMeter_InitialLonLat(GPS_STRUCT *GpsData)
{
	Point_Struct Point ;
	TIME_T	tt;
	if((OldPoint.LonDegree == 0)&&(OldPoint.LatDegree == 0)) //������Ҫ���ݿ�����
	{
		MileMeter_DecimalDegree(&Point,GpsData);
		OldPoint.LonDegree = Point.LonDegree;
		OldPoint.LatDegree = Point.LatDegree;
		tt.year = GpsData->Year;
		tt.month = GpsData->Month;
		tt.day = GpsData->Date;
		tt.hour = GpsData->Hour;
		tt.min = GpsData->Minute;
		tt.sec = GpsData->Second;
		OldMileTime = ConverseGmtime(&tt);
		
	}
	
}


/********************************************************************
* ���� : CalDistance
* ���� : ���������ľ���
*
* ����: ����ľ��Ⱥ�γ�ȣ��Զ�Ϊ��λ
* ���: ����֮��ľ��룬����Ϊ��λ
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/
d64 MileMeter_CalDistance(Point_Struct *Point1,Point_Struct *Point2)
{

	d64 distance,temp,RadLon1,RadLat1,RadLon2,RadLat2;
	u8	flag = 0;
		
	RadLon1 = Point1->LonDegree*PI/180.0;
	RadLat1 = Point1->LatDegree*PI/180.0;
	RadLon2 = Point2->LonDegree*PI/180.0;
	RadLat2 = Point2->LatDegree*PI/180.0;
	
	//γ�Ȼ򾭶ȱ仯Ҫ����һ����Χ,(1��==1852��,�Ƴ�1��==0.000009��)
	temp = (d64)fabs(Point1->LonDegree-Point2->LonDegree);
	if(temp > 0.000009)
	{
		flag = 1;
	}
	temp = (d64)fabs(Point1->LatDegree-Point2->LatDegree);
	if(temp > 0.000009)
	{
		flag = 1;
	}
	if(1 == flag)
	{
		temp = 2.0-2.0*cos(RadLat1)*cos(RadLat2)*cos(RadLon2-RadLon1)-2.0*sin(RadLat2)*sin(RadLat1);
		if(temp > 0)//��֤Ϊ��
		{
			distance = REarth*sqrt(temp);
			return distance;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	
			
}
/********************************************************************
* ���� : GpsMile_UpdatePram(void)
* ���� : ����GPS�����
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/
void GpsMile_UpdatePram(void)
{
	u8	TmpData[5];
	//u8	Buffer[20];
	u8	PramLen;
	//u8	length;
	u32	Tmp = 0;
	
	PramLen = FRAM_BufferRead(TmpData, FRAM_VDR_MILEAGE_LEN, FRAM_VDR_MILEAGE_ADDR);			//��ȡ�����
	//���У���ֽ��Ƿ���ȷ
	if(FRAM_VDR_MILEAGE_LEN == PramLen)//У�����ȷ
	{
		Tmp = 0;
		Tmp |= TmpData[0] << 24;					//���ֽ�
		Tmp |= TmpData[1] << 16;		//�θ��ֽ�
		Tmp |= TmpData[2] << 8;		//
		Tmp |= TmpData[3];		//��
	}
	else
	{
                
		//�����ݵ�,dxl,2015.4.9
                PramLen = FRAM_BufferRead(TmpData, FRAM_VDR_MILEAGE_BACKUP_LEN, FRAM_VDR_MILEAGE_BACKUP_ADDR);	
                if(FRAM_VDR_MILEAGE_BACKUP_LEN == PramLen)//У�����ȷ
	        {
		        Tmp = 0;
		        Tmp |= TmpData[0] << 24;					//���ֽ�
		        Tmp |= TmpData[1] << 16;		//�θ��ֽ�
		        Tmp |= TmpData[2] << 8;		//
		        Tmp |= TmpData[3];		//��
	        }
                else
                {
		        Tmp = 0;
                }
	}
	MileSum = (d64)Tmp * 10.0;//����洢����������10mΪ��λ�������
}
/********************************************************************
* ���� : GpsMile_SetPram(u32 TmpMilege)
* ���� : ����GPS�����
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:TmpMilege��λΪ0.1����,������˺�������Ҫ�����ŵ��ø���gps��̺�������̺���
********************************************************************/
void GpsMile_SetPram(u32 TmpMilege)
{
	u8 	TmpData[5]={0};
        u32     MileSum = 0;
        
	MileSum = TmpMilege * 10;
	TmpData[3] = MileSum;		//���ֽ�,dxl,2013.11.20֮ǰʹ�õ���TmpMilege,����
	TmpData[2] = MileSum>>8;	//���ֽ�
	TmpData[1] = MileSum>>16;	//���ֽ�
	TmpData[0] = MileSum>>24;	//���ֽ�
	FRAM_BufferWrite(FRAM_VDR_MILEAGE_ADDR, TmpData, FRAM_VDR_MILEAGE_LEN); //�洢�����
        
  FRAM_BufferWrite(FRAM_VDR_MILEAGE_BACKUP_ADDR, TmpData, FRAM_VDR_MILEAGE_BACKUP_LEN); //�洢�����
        
  //������Ӧ����
	GpsMile_UpdatePram();
	
  //�����������,
  VDRPulse_UpdateParameter();
        
}
/********************************************************************
* ���� :
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/
FunctionalState GpsMile_TimeTask(void)
{

	d64 TwoPointDistance;
	u8	RunState = 0;
	u8	TmpData[5];
	u32	Mile;
	TIME_T	tt;
	static  u8	count = 0;
	
	//����ÿ����ƽ���ٶ�
	MileMeter_MinuteSpeed();
	
	
	if(0x00== (0x01&SpeedFlag))//����
	{
		return ENABLE;
	}
        
        
  //����ʻ��־
  //if(0 == AccOffGpsControlFlag)//ACC OFFʱGPSģ��ر�
 // {
    //RunState = VDRDoubt_GetCarRunFlag(); 
  //}
  //else//GPSģ��ʼ���Ǵ򿪵�
  //{
    RunState = Gps_GetRunFlag();
 //}
	
	count++;
	if(count >= 6)//3���Ӽ���һ�����,ע�Ȿ���������ԭ����1���Ϊ�����ڵ�0.5�룬��˼���ֵ��Ϊ6
	{
	
		count = 0;
	 	//���ACC���Ҵ�����ʻ״̬
        	if(1 == RunState)
        	{
			MileMeter_InitialLonLat(&gPosition);
			MileMeter_DecimalDegree(&Point, &gPosition);
			TwoPointDistance = MileMeter_CalDistance(&OldPoint, &Point);
			tt.year = gPosition.Year;
			tt.month = gPosition.Month;
			tt.day = gPosition.Date;
			tt.hour = gPosition.Hour;
			tt.min = gPosition.Minute;
			tt.sec = gPosition.Second;
			MileTime = ConverseGmtime(&tt);
			if(TwoPointDistance > StillFloatDistanceAverageStandard)
			{
				
				if(TwoPointDistance > 55*(MileTime-OldMileTime))//ƽ��ʱ��С��200����/Сʱ
				{
				
				}
				else
				{
					MileSum += TwoPointDistance;//����Ʈ��
					//�����ֵ��������
					Mile = (u32)(MileSum/10.0);//Ϊ����������̱���һ��,���Գ���10,��λ��0.01����
					TmpData[3] = Mile;	//���ֽ�
					TmpData[2] = Mile>>8;	//���ֽ�
					TmpData[1] = Mile>>16;	//���ֽ�
					TmpData[0] = Mile>>24;	//���ֽ�
					FRAM_BufferWrite(FRAM_VDR_MILEAGE_ADDR, TmpData, FRAM_VDR_MILEAGE_LEN); //�洢�����
          FRAM_BufferWrite(FRAM_VDR_MILEAGE_BACKUP_ADDR, TmpData, FRAM_VDR_MILEAGE_BACKUP_LEN); //�洢�����
                                        
				}
				OldPoint.LonDegree = Point.LonDegree;
				OldPoint.LatDegree = Point.LatDegree;
				OldMileTime = MileTime;
			}
		}
	}
	
	return ENABLE;
}
	
/********************************************************************
* ���� :
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/
void MileMeter_GpsPosSpeedCount(void)
{
	s32 MileSubTemp;
	
	GpsPosSpeedTimeCount ++;
	if(GpsPosSpeedTimeCount >= 20)
	{
		GpsPosSpeedTimeCount = 0;
		MileSubTemp = (s32)(MileSum - OldMileSum);
		if(MileSubTemp > 0)
		{
        		GpsPosSpeed = (u8)(MileSubTemp*6/100);
	    		OldMileSum = MileSum;
		}
	}


}
/********************************************************************
* ���� :
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/
void MileMeter_MinuteSpeed(void)
{
       static TIME_T t2;
       static u8  MinuteSpeedData[60] = {0};
       
       TIME_T CurTime;
       u8     i;
       u16    sum;
       
       /*
        static  u8  InitFlag = 0;
        TIME_T tt;
        
	RTC_GetCurTime(&tt);
        if(0 == tt.sec)
        {
                if(0 == InitFlag)
                {
                        GpsSpeedSum = 0;
                        InitFlag = 1;
                }
                
        }
        else
        {
               InitFlag = 0; 
        }
        GpsSpeedSum += Gps_ReadSpeed();
        GpsMinuteSpeed = GpsSpeedSum/120;//ע������120����Ϊ��0.5�����һ�θú����������1������60
        */
                RTC_GetCurTime(&CurTime);
                //����ÿ����ƽ���ٶ�
                if(CurTime.sec != t2.sec)//��仯ʱ��
                {  
                        if(59 == CurTime.sec)
                        {
                                i = CurTime.sec;
                                if(i >= 60)
                                {
                                        i = 0;
                                }
                                MinuteSpeedData[i] = Gps_ReadSpeed();
                                
                                sum = 0;
                                for(i=0; i<60; i++)
                                {
                                        sum += MinuteSpeedData[i];
                                        //LastMinuteSpeedData[i] = MinuteSpeedData[i];
                                        //if(0 == LastMinuteSpeedData[i])
                                        //{
                                        
                                        //}
                                }
                                GpsMinuteSpeed = sum/60;
                                
                                for(i=0; i<60; i++)
                                {
                                        MinuteSpeedData[i] = 0;
                                }      
                                
                        }
                        else
                        {
                                i = CurTime.sec;
                                if(i >= 60)
                                {
                                        i = 0;
                                }
                                MinuteSpeedData[i] = Gps_ReadSpeed();
                        }
                        t2.sec = CurTime.sec;
                        
                }
}
/********************************************************************
* ���� :MileMeter_GetMinuteSpeed
* ���� :��ȡGPSÿ����ƽ���ٶ�
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/
u8 MileMeter_GetMinuteSpeed(void)
{
	return  GpsMinuteSpeed;
}
/********************************************************************
* ���� :ReadMileSum
* ���� :��gps�ۼ���ʻ���
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:MileSum��λΪ��
********************************************************************/
u32 ReadMileSum(void)
{
	return (u32)MileSum/100;//��λΪ0.1����
}
/********************************************************************
* ���� :
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:��λΪ0.01����
********************************************************************/
u32 MileMeter_GetTotalMile(void)
{
	return (u32)MileSum/10;//MileSum��λΪ��
}
/********************************************************************
* ���� : MileMeter_ClearOldPoint
* ���� : ��һ���������0
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/
void MileMeter_ClearOldPoint(void)
{
		OldPoint.LonDegree = 0;
		OldPoint.LatDegree = 0;
}
/********************************************************************
* ���� :
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/
u8 ReadGpsPosSpeed(void)
{
	return GpsPosSpeed;
}
#if Test//

/********************************************************************
* ���� :
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/

void MileMeter_DecimalDegreeTest(Point_Struct *Point,Test_Pos_Data *TestGpsData)
{

    d64 temp;
	
	temp = TestGpsData->LongitueMF/10000.0;
	temp = temp + TestGpsData->LongitueM;
	temp = temp/60.0;
	temp = temp + TestGpsData->LongitueD;
	Point->LonDegree = temp;
	temp = TestGpsData->LatitueMF/10000.0;
	temp = temp + TestGpsData->LatitueM;
	temp = temp/60.0;
	temp = temp + TestGpsData->LatitueD;
	Point->LatDegree = temp;
	
}


/*Ư�Ʋ���*/

/********************************************************************
* ���� :
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/

void FloatTest(d64 *Max,d64 *Average,Point_Struct Point[],u8 Num)
{

	d64 MaxTemp = 0,AverageTemp = 0,DistanceTemp = 0;
	u8 i;
		
	for(i=0;i<Num-1;i++)
	{
	
	    DistanceTemp = MileMeter_CalDistance(&Point[i],&Point[i+1]);
		TwoPointDistanceTest[i] = DistanceTemp;
	    AverageTemp += DistanceTemp;
		if(MaxTemp < DistanceTemp)
			MaxTemp = DistanceTemp;
		
	}
	*Max = MaxTemp;
	*Average = AverageTemp/(Num-1);
//	*Average = (AverageTemp-MaxTemp)/(Num-2);
		
}

/********************************************************************
* ���� :
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/

void FloatResult(void)
{
	u8 i;
	
	Test_Pos_Data Data[N]=
	{
		{114,05,5420,22,33,5930},

		{114,05,5440,22,33,5870},
		
		{114,05,5430,22,33,5800},

		{114,05,5420,22,33,5760},

		{114,05,5440,22,33,5770},

		{114,05,5450,22,33,5800},

		{114,05,5460,22,33,5840},

		{114,05,5460,22,33,5850},

		{114,05,5450,22,33,5840},

		{114,05,5450,22,33,5850},

		{114,05,5440,22,33,5870},

		{114,05,5420,22,33,5880},

		{114,05,5400,22,33,5890},

		{114,05,5380,22,33,5900},

		{114,05,5370,22,33,5910},

		{114,05,5360,22,33,5930},

		{114,05,5350,22,33,5940},

		{114,05,5340,22,33,5950},

		{114,05,5330,22,33,5970},

		{114,05,5340,22,33,6010},

		{114,05,5370,22,33,6010}
	};

	Point_Struct PointData[N];
	for(i=0;i<N;i++)
	MileMeter_DecimalDegreeTest(&PointData[i],&Data[i]);
    FloatTest(&Max,&Average,PointData,N);

}


/*����ۼӲ���*/

/********************************************************************
* ���� :
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/
void MileMeter_DistanceSumTest(Test_Pos_Data *TestGpsData)
{
    Point_Struct Point;
	d64 TwoPointDistance;
	
	MileMeter_DecimalDegreeTest(&Point,TestGpsData);
	TwoPointDistance = MileMeter_CalDistance(&OldPoint, &Point);
//	if(TwoPointDistance > StillFloatDistance)
	MileSum += TwoPointDistance;//����Ʈ��
	OldPoint.LonDegree = Point.LonDegree;
	OldPoint.LatDegree = Point.LatDegree;
	

}


/********************************************************************
* ���� :
* ���� :
*
* ����:
* ���:
*
* ȫ�ֱ���:
* ���ú���:
*
* �ж���Դ:
*
* ��ע:
********************************************************************/

void SimulateDistanceSumTest(void)
{
	u8 i ;
	Test_Pos_Data Data[N]=
	{
		{114,05,5430,22,33,5780},
		{114,05,5420,22,33,5750},

		{114,05,5430,22,33,5750},

		{114,05,5460,22,33,5760},

		{114,05,5460,22,33,5710},

		{114,05,5410,22,33,5620},

		{114,05,5380,22,33,5580},

		{114,05,5380,22,33,5580},
	
		{114,05,5390,22,33,5580},

		{114,05,5440,22,33,5600}
		
	};
	
	MileMeter_DecimalDegreeTest(&OldPoint,&Data[0]);
	for(i = 1;i < N;i++)
    MileMeter_DistanceSumTest(&Data[i]);
	
}

// https://www.cnblogs.com/softfair/p/lat_lon_distance_bearing_new_lat_lon.html
const double Ea = 6378137;     //   ����뾶  
const double Eb = 6356725;     //   ���뾶 
static void MileMeter_GetJWDB(double GLAT, double GLON, double distance, double angle, double *outBJD, double *outBWD)

{

    double dx = distance * 1000 * sin(angle * PI / 180.0);

    double dy = distance * 1000 * cos(angle * PI / 180.0);



    //double ec = 6356725 + 21412 * (90.0 - GLAT) / 90.0;

    // 21412 �ǳ���뾶�뼫�뾶�Ĳ�

    double ec = Eb + (Ea-Eb) * (90.0 - GLAT) / 90.0;



    double ed = ec * cos(GLAT * PI / 180);

    *outBJD = (dx / ed + GLON * PI / 180.0) * 180.0 / PI;

    *outBWD = (dy / ec + GLAT * PI / 180.0) * 180.0 / PI;

}

#define EARTH_RADIUS            6378.137
// Calculate radian
static d64 MileMeter_Radian(d64 d)  
{  
     return d * PI / 180.0;
}  

d64 MileMeter_CalDistance1(Point_Struct *Point1,Point_Struct *Point2)
{  
     d64 radLat1 = MileMeter_Radian(Point1->LatDegree);  
     d64 radLat2 = MileMeter_Radian(Point2->LatDegree);  
     d64 a = radLat1 - radLat2;  
     d64 b = MileMeter_Radian(Point1->LonDegree) - MileMeter_Radian(Point2->LonDegree);  
	 
     d64 dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));  
     dst = dst * EARTH_RADIUS;  
     //dst= round(dst * 10000) / 10000;  
     return dst;  
} 
static double HaverSin(double theta)
{
   double v = sin(theta / 2);
   return v * v;
}
static double ConvertDegreesToRadians(double degrees)
{
    return degrees * PI / 180;
}

static double ConvertRadiansToDegrees(double radian)
{
    return radian * 180.0 / PI;
}

/// <summary>
/// �����ľ���1��γ��1������2��γ��2. ����2����γ��֮��ľ��롣
/// </summary>
/// <param name="lat1">����1</param>
/// <param name="lon1">γ��1</param>
/// <param name="lat2">����2</param>
/// <param name="lon2">γ��2</param>
/// <returns>���루���ǧ�ף�</returns>
static double MileMeter_CalDistance2(Point_Struct *Point1,Point_Struct *Point2)
{
   //��haversine��ʽ�������������ľ��롣
   //��γ��ת���ɻ���
   double lat1 = ConvertDegreesToRadians(Point1->LatDegree);
   double lon1 = ConvertDegreesToRadians(Point1->LonDegree);
   double lat2 = ConvertDegreesToRadians(Point2->LatDegree);
   double lon2 = ConvertDegreesToRadians(Point2->LonDegree);

   //��ֵ
   double vLon = fabs(lon1 - lon2);
   double vLat = fabs(lat1 - lat2);

   //h is the great circle distance in radians, great circle����һ�������ϵ����棬����Բ�ļ������ĵ�һ���ܳ�����Բ��
   double h = HaverSin(vLat) + cos(lat1) * cos(lat2) * HaverSin(vLon);

   double distance = 2 * EARTH_RADIUS * asin(sqrt(h));

   return distance;
}

d64 dst ;
double outBJD;
double outBWD;

void MileMeter_Dis(void)
{  
	Point_Struct p1 = {116.41615,39.90744};  
	Point_Struct p2 = {116.30746,39.90744};
	
	// test: float as arguments
	dst = MileMeter_CalDistance1(&p1, &p2);  
	dst = MileMeter_CalDistance2(&p1, &p2);  
	MileMeter_GetJWDB(39.90744, 116.41615, 98.5623, 25.6, &outBJD, &outBWD);
} 

/*
//ÿ��6sͬһλ��Ư��
{
	{114,05,5430,22,33,5780},

	{114,05,5420,22,33,5750},

	{114,05,5430,22,33,5750},

	{114,05,5460,22,33,5760},

	{114,05,5460,22,33,5710},

	{114,05,5410,22,33,5620},

	{114,05,5380,22,33,5580},

	{114,05,5380,22,33,5580},
	
	{114,05,5390,22,33,5580},

	{114,05,5440,22,33,5600}
}
*/
//ÿ��3sͬһλ��Ư��

/*
	{
		{114,05,5420,22,33,5930},

		{114,05,5440,22,33,5870},
		
		{114,05,5430,22,33,5800},

		{114,05,5420,22,33,5760},

		{114,05,5440,22,33,5770},

		{114,05,5450,22,33,5800},

		{114,05,5460,22,33,5840},

		{114,05,5460,22,33,5840},

		{114,05,5450,22,33,5840},

		{114,05,5450,22,33,5850},

		{114,05,5440,22,33,5870},

		{114,05,5420,22,33,5880},

		{114,05,5400,22,33,5890},

		{114,05,5380,22,33,5900},

		{114,05,5370,22,33,5910},

		{114,05,5360,22,33,5930},

		{114,05,5350,22,33,5940},

		{114,05,5340,22,33,5950},

		{114,05,5330,22,33,5970},

		{114,05,5340,22,33,6010},

		{114,05,5370,22,33,6010}
	}
*/
#endif

