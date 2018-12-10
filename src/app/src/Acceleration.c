/*
********************************************************************************
*
* Filename      : Acceleration.h
* Version       : V1.00
* Programmer(s) : zengliang
* @date         : 2016-10-28 
* 
********************************************************************************
*/


/*
********************************************************************************
*                               INCLUDES
********************************************************************************
*/
#include "include.h"
#include "Public.h"
#include "VDR_Doubt.h"
#include "VDR_Speed.h"
#include "VDR_Pulse.h"
/*
********************************************************************************
*                               DEFIEN START
********************************************************************************
*/
#ifdef ACCELERATION_H

/*
********************************************************************************
*                             LOCAL DEFINES
********************************************************************************
*/


/*
********************************************************************************
*                         LOCAL DATA TYPES
********************************************************************************
*/

/*
********************************************************************************
*                          LOCAL VARIABLES
********************************************************************************
*/
enum ThreeEnerStatus{
	ENER_STATUS_SHARP_ACCELERATE = 0, //������
	ENER_STATUS_SHARP_SLOWDOWN, 			//������
	ENER_STATUS_SHARP_TRUN,						//��ת��
	ENER_STATUS_MAX
};

static u8 Initflag=0;
static u8 acce_flg=0;//�����ٱ�־,�����ٱ�־,��ת��
static u8 Report[150],Report_len=0;
static u8 Acc_Threshold[4]={0,0,0,0};//{15,10,7,5};
static u8 Slowdwn_Threshold[2]={0,0};//{25,20};
static u8 ThreeEmerClrDlyTime[ENER_STATUS_MAX] = {0,};

static u8 ThreeEmerCheckDisable[3] = {0,0,0};//�������ܱ�־

static u16 EmerCornerAngleMax=15;
#define EMER_TURN_ANGLE_SPEED    25
#define	EMER_CLR_DLY_COUNT				5
/*
********************************************************************************
*                          GLOBAL VARIABLES
********************************************************************************
*/

/*
********************************************************************************
*                          EXTERN VARIABLES
********************************************************************************
*/



/*
********************************************************************************
*                           FUNCTIONS EXTERN
********************************************************************************
*/


/*
********************************************************************************
*                              FUNCTIONS
********************************************************************************
*/
void CleanInitFlag(void)
{
      Initflag=0;
}


void ThreeEmerAlarmBit(u32 bit, FlagStatus state)
{
	if(bit >= 32)return;
    if(SET==state)
    {
        PUBLIC_SETBIT(acce_flg, bit);
    }
    else if(RESET==state)
    {
        PUBLIC_CLRBIT(acce_flg, bit);
    }
} 

u8 ThreeEmerCleckAlarmBit(u32 bit)
{

	if(PUBLIC_CHECKBIT(acce_flg,bit))
  {
		return 0;
	}
	return 1;
} 

/**
* @brief  ��ƽ̨����͸������
* @param  None
* @retval None
*/
void Acceleration_8900Send(u8 *Acc,u8 len)
{
      EIExpand_PotocolSendData(0xf8f0,Acc,len);
}

/**
* @brief  ���ü����ټ����ٷ�ֵ�ӿ�
* @param  None
* @retval None
*/
void Set_Emer_Threshold(u8 *Threshold,u8 emer_type_flag)
{
  u8 i=0;
  if(emer_type_flag==0)
  {
    for(i=0;i<4;i++)
    {
      Acc_Threshold[i]=Threshold[i];
    }
  }
  else if(emer_type_flag==1)
  {
    for(i=0;i<2;i++)
    {
      Slowdwn_Threshold[i]=Threshold[i];
    }        
  }
	else
	{
		EmerCornerAngleMax=Threshold[0];
		EmerCornerAngleMax=EmerCornerAngleMax<<8|Threshold[1];
	}
}

/**
* @brief  ��ȡ�����ټ����ٷ�ֵ�ӿ�
* @param  None
* @retval None
*/
u8* Read_Emer_Threshold(u8 emer_type_flag)
{
  if(emer_type_flag==0)
  {
    return Acc_Threshold;
  }
  else if(emer_type_flag==1)
  {
    return Slowdwn_Threshold;       
  }
  return NULL;
}

//zengliang add 2016-12-7
extern u8 SpeedFlag;
u8 Get_Speed(u8 *CurrentSpeed)
{
	static u8 chk_valid = 0;
	static u8 pos_valid_cnt = 0;
  //����ʹ�������ٶ�
  if(VDRPulse_GetPulseSpeed())
  {
    *CurrentSpeed = VDRPulse_GetPulseSpeed();
		chk_valid = 1;
  }
  else
  {
    *CurrentSpeed = Gps_ReadSpeed();
		if(!Gps_ReadStatus())//δ��λ
		{
			pos_valid_cnt = 0;
			chk_valid = 0;
		}
		else
		{
			if(0 == chk_valid)
			{
				if(pos_valid_cnt++ > 5)
					chk_valid = 1;
			}
		}
  }
  
  return chk_valid;
}


/**
* @brief 3��״̬�����ϱ�
* @param  ReportType:�ϱ�����
* @retval None
*/
static void Emer_Acce_Occur_Report(u8 ReportType)
{
	//����Ƿ��Ѿ��м������ϱ�
	if(ThreeEmerCleckAlarmBit(ReportType)){
		//Public_PlayTTSVoiceStr("������"); 
		//LcdShowCaptionEx((char *)"������",2); 
	 ThreeEmerAlarmBit(ReportType,SET);
	 //����������ʱ�䣬�ϱ�
	 Report_len= Report_GetPositionInfo(Report);
	 RadioProtocol_PostionInformationReport(CHANNEL_DATA_1|CHANNEL_DATA_2,Report,Report_len);
	 ThreeEmerAlarmBit(ReportType,RESET);//�ϱ�֮�������ֻ�ϱ�һ��
	}

}
/**
* @brief 3��״̬����ϱ�
* @param  ReportType:�ϱ�����
* @retval None
*/
static void Emer_Acce_Clear_Report(u8 ReportType)
{
	
	if(!ThreeEmerCleckAlarmBit(ReportType))	
	{
		if(ThreeEmerClrDlyTime[ReportType]++ > EMER_CLR_DLY_COUNT)	
		{
			ThreeEmerClrDlyTime[ReportType] = 0;
			//Public_PlayTTSVoiceStr("���������"); 
			ThreeEmerAlarmBit(ReportType,RESET);
			Report_len= Report_GetPositionInfo(Report);
			RadioProtocol_PostionInformationReport(CHANNEL_DATA_1|CHANNEL_DATA_2,Report,Report_len);  
		}
	}
}
//ֱ�����ٶ�
void Emer_acce_check(u8 flag)
{
	static u8 oldspeed = 0;
	u8 newspeed ;
	u8 chk_valid = 0;
	//static u8 pos_valid_cnt = 0;
	static float acce;
	static u8 *emer_acce;
	u8	i;
	
	if(flag == 0)
	{
		ThreeEmerAlarmBit(0,RESET);
		chk_valid = Get_Speed(&oldspeed);
		emer_acce=Read_Emer_Threshold(0);
		return ;
	}
	
	emer_acce=Read_Emer_Threshold(0);  
	
	chk_valid = Get_Speed(&newspeed);
	
	if(0 == chk_valid)
	{
		Emer_Acce_Clear_Report(ENER_STATUS_SHARP_ACCELERATE); 
	}
	else
	{
	//ʹ��GPS�ٶ���δ��λ״̬�²����3��״̬
		if((newspeed > oldspeed)&&oldspeed)
		{
			acce = newspeed - oldspeed; //��λ��km/h 
			//ThreeEmerAlarmBit(0,RESET);
			if(oldspeed >= 90){
				i = 3;
			}
			else{
				i = oldspeed/30;
			}
			
			if(acce >= emer_acce[i]) 
			{
				Emer_Acce_Occur_Report(ENER_STATUS_SHARP_ACCELERATE);
			}
			else 
			{
				Emer_Acce_Clear_Report(ENER_STATUS_SHARP_ACCELERATE);
			}
		}
		else
		{ 	
			Emer_Acce_Clear_Report(ENER_STATUS_SHARP_ACCELERATE); 
		}
	}
	oldspeed = newspeed;
}

/**
* @brief �������ж�
* @param  None
* @retval None
*/
//ֱ�����ٶ�
void Emer_slowdwn_check(u8 flag)
{
	static u8 oldspeed = 0;
	u8 newspeed;
	u8 chk_valid = 0;
	//static u8 pos_valid_cnt = 0;
	static float acce;
	static u8 *emer_slowdwn;
	
	if(flag == 0)
	{
		ThreeEmerAlarmBit(1,RESET);
		chk_valid = Get_Speed(&oldspeed);
		emer_slowdwn=Read_Emer_Threshold(1);
		return ;
	}
	
	chk_valid = Get_Speed(&newspeed);
	
	emer_slowdwn=Read_Emer_Threshold(1);
	
	if(0 == chk_valid)
	{
		Emer_Acce_Clear_Report(ENER_STATUS_SHARP_SLOWDOWN); 
	}
	else
	{
		if((newspeed < oldspeed)&&oldspeed)
		{
			acce = oldspeed - newspeed;  
			//ThreeEnerAlarmBit(1,RESET); 
			if(oldspeed < 30)
			{
				if(acce >= emer_slowdwn[0])
				{
					Emer_Acce_Occur_Report(ENER_STATUS_SHARP_SLOWDOWN);
				}
				else
				{
					Emer_Acce_Clear_Report(ENER_STATUS_SHARP_SLOWDOWN);
				}
			}
			else
			{
				if(acce >= emer_slowdwn[1])
				{
					Emer_Acce_Occur_Report(ENER_STATUS_SHARP_SLOWDOWN);
				}
				else
				{
					Emer_Acce_Clear_Report(ENER_STATUS_SHARP_SLOWDOWN);
				}
			}
		}
		else
		{ 
			Emer_Acce_Clear_Report(ENER_STATUS_SHARP_SLOWDOWN);
		} 
	}
	oldspeed = newspeed;
}

/**
* @brief ���ٶȳ�ʼ��
* @param  None
* @retval None
*/
void Acceleration_Init(void)
{
    u8 A_Threshold[4]={0,0,0,0};
    u8 L_Threshold[2]={0,0};
		u8 Angle[2]={0,0};
    u8 read_flag=0,i=0;
    if(Initflag)
    {
        return;//�ѳ�ʼ�����
    }
    read_flag=FRAM_BufferRead(A_Threshold,4,FRAM_ACCELERATION_ADDR);
    
    //�������Ƿ�Ϸ�  
    if(read_flag)//У��ͨ��
    {
    	for(i=0;i<4;i++)    
		{
		    Acc_Threshold[i]=A_Threshold[i];
		}
		if(A_Threshold[0]==0||A_Threshold[1]==0||A_Threshold[2]==0||A_Threshold[3]==0)
		{
			ThreeEmerCheckDisable[0] = 1;
		}
    }
	else//default
	{
		FRAM_BufferWrite(FRAM_ACCELERATION_ADDR,Acc_Threshold,FRAM_ACCELERATION_LEN);
	}
    
    read_flag=FRAM_BufferRead(L_Threshold,2,FRAM_DECELERATION_ADDR);
    //�������Ƿ�Ϸ�
    if(read_flag)//У��ͨ��
    {
    	Slowdwn_Threshold[0]=L_Threshold[0];
	    Slowdwn_Threshold[1]=L_Threshold[1];
		if(L_Threshold[0]==0||L_Threshold[1]==0)
	    {		
				ThreeEmerCheckDisable[1] = 1;
	    }
    }
	else
	{
		FRAM_BufferWrite(FRAM_DECELERATION_ADDR,Slowdwn_Threshold,FRAM_DECELERATION_LEN);
	}
		
    read_flag=FRAM_BufferRead(Angle,2,FRAM_TURN_ANGLE_ADDR);
    //�������Ƿ�Ϸ�
    if(read_flag)//У��ͨ��
    {
		EmerCornerAngleMax=Angle[0];
		EmerCornerAngleMax=EmerCornerAngleMax<<8|Angle[1];
		if(Angle[0]==0&&Angle[1]==0)
	    {
			ThreeEmerCheckDisable[2] = 1;
	    }
    }
	else
	{
		EmerCornerAngleMax=15;
		L_Threshold[0]=0;L_Threshold[1]=15;
		FRAM_BufferWrite(FRAM_TURN_ANGLE_ADDR,L_Threshold,FRAM_TURN_ANGLE_LEN);
	}		
    
    Initflag = 1;
    //Emer_acce_check(0);
    //Emer_slowdwn_check(0);
}
 

void Emer_Turn_check(void)//��ת���ж�
{
    u32 CornerAngle = 0;
    static u32 LastCornerAngle = 0;
    u8  Acc = 0;
    u8  RunFlag = 0;
    u8  Navigation = 0;
    u8  Speed;
    Acc = Io_ReadStatusBit(STATUS_BIT_ACC);
    Navigation = Io_ReadStatusBit(STATUS_BIT_NAVIGATION);
    RunFlag = VDRDoubt_GetCarRunFlag();
    CornerAngle = Gps_ReadCourse();
    Get_Speed(&Speed);
    
    if((1 == Acc)&&(1 == Navigation)&&(1 == RunFlag))
    {
        if(CornerAngle >= LastCornerAngle)
        {
            if((CornerAngle - LastCornerAngle) >= 180)
            {
                if(((360+LastCornerAngle - CornerAngle) >= EmerCornerAngleMax)&&(Speed>EMER_TURN_ANGLE_SPEED))
                {
                				Emer_Acce_Occur_Report(ENER_STATUS_SHARP_TRUN);
								}
								else
								{
									Emer_Acce_Clear_Report(ENER_STATUS_SHARP_TRUN);
								}
            }
            else
            {
                if(((CornerAngle - LastCornerAngle) >= EmerCornerAngleMax)&&(Speed>EMER_TURN_ANGLE_SPEED))
                { 
                		Emer_Acce_Occur_Report(ENER_STATUS_SHARP_TRUN);
                }
								else
								{
									Emer_Acce_Clear_Report(ENER_STATUS_SHARP_TRUN);
								}
            }
           
        }
        else
        {
            if((LastCornerAngle - CornerAngle) >= 180)
            {
                if(((360+CornerAngle - LastCornerAngle) >= EmerCornerAngleMax)&&(Speed>EMER_TURN_ANGLE_SPEED))
                {
                    Emer_Acce_Occur_Report(ENER_STATUS_SHARP_TRUN);
                }
								else
								{
									Emer_Acce_Clear_Report(ENER_STATUS_SHARP_TRUN);
								}
            }  
            else
            {
                if(((LastCornerAngle - CornerAngle) >= EmerCornerAngleMax)&&(Speed>EMER_TURN_ANGLE_SPEED))
                {
                    Emer_Acce_Occur_Report(ENER_STATUS_SHARP_TRUN);
                }
								else
								{
									Emer_Acce_Clear_Report(ENER_STATUS_SHARP_TRUN);
								}
            }
        }
    }
    else
    {
    	Emer_Acce_Clear_Report(ENER_STATUS_SHARP_TRUN);    
    }
    LastCornerAngle = CornerAngle;
}
/**
  * @brief  λ����Ϣ������Ϣ 0xe9
  * @param  ָ�����ݵ�ַ
  * @retval �������ݳ���
  */
u16  Acceleration_PosEx(u8 *pDst)
{
    u16 acc;
    acc = (u16)acce_flg;
    if((acc == 0))
    {
        return 0;
    }
    acc=acc<<8;
    *pDst++ = 0xE9;//������ϢID
    *pDst++ = 2;//���ӳ���
    memcpy(pDst,&acc,2);
    return (4);
}
/**
* @brief ���ٶ��ж�����
* @param  None
* @retval None
*/
FunctionalState Acceleration_TimeTask(void)
{
		if(ReadPeripheral2TypeBit(6))return DISABLE;
		//δ��A2��OBDģ��
		Acceleration_Init();//��ʼ��
		if(0 == ThreeEmerCheckDisable[0])
		{
			Emer_acce_check(1);//�������ж�
		}
		else
		{
			Emer_Acce_Clear_Report(ENER_STATUS_SHARP_ACCELERATE);
		}
		if(0 == ThreeEmerCheckDisable[1])
		{
			Emer_slowdwn_check(1);//�������ж�
		}
		else
		{
			Emer_Acce_Clear_Report(ENER_STATUS_SHARP_SLOWDOWN);
		}
		if(0 == ThreeEmerCheckDisable[2])
		{
			Emer_Turn_check();//��ת���ж�
		}
		else
		{
			Emer_Acce_Clear_Report(ENER_STATUS_SHARP_TRUN);
		}
		return ENABLE;
}
/*
********************************************************************************
*                 DEFIEN END
********************************************************************************
*/
#endif 
