/********************************************************************
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :Gps_App.c      
//����      :GPSģ��
//�汾��    :
//������    :dxl
//����ʱ��  :2011.8
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :
***********************************************************************/ 
/*************************�޸ļ�¼*************************/
//***********************����(���ڰ��������)*******************
//2011.8.25--1.����GPSģ��,��GPSģ��ֳ�3�����֣�
//-----------GPS_RxIsr(void)���������ݽ���
//-----------GpsParse_EvTask(void):����Э�����
//-----------Gps_TimeTask(void):����ģ�����
//20118.25--2.GPSģ���ȶ��Դ�ʩ��
//-----------(1)�����ϵ縴λ,GPSģ��Ҳ�ϵ縴λ,��Ҫ�ȹ��ٿ�
//-----------(2)У������Ư�ƴ����ﵽһ������ʱ,��λ�����־,��GPSģ�鸴λ
//--------------��λ:ģ���ȹر�5��,���ٴ�,ֻ��ACC ONʱ�Ż�������־
//-----------(3)ACC OFFʱ,��GPSģ��,ÿСʱ��1����
//-----------(4)����40��û���յ�����,��λ�����־
//-----------(5)Ư������10��,��λ�����־
//-----------(6)У���������10��,��λ�����־
//2011.8.25--3.ȥƯ�Ƶķ�����
//-----------(1)γ�ȷ���,�������ʱ��>=250����/Сʱ,��Ϊ��Ư��
//-----------(2)���ȷ���,�������ʱ��>=250����/Сʱ,��Ϊ��Ư��
//-----------(3)�ٶ���������ʱ,�ٶȱ仯��/ʱ��>=20����/Сʱ��Ϊ��Ư��
//-----------(4)��Ȼ����,������С��3��ˮƽ�������Ӵ��ڵ���10,��Ϊ��Ư��
//**********************�޸�*********************
//2012.6.5--4.ȥƯ�Ʒ�����Ϊ��
//-----------(1)γ�ȷ���,�������ʱ��>=250����/Сʱ,��Ϊ��Ư��
//-----------(2)���ȷ���,�������ʱ��>=250����/Сʱ,��Ϊ��Ư��
//-----------(3)�ٶ���������ʱ,�ٶȱ仯��/ʱ��>=9����/Сʱ��Ϊ��Ư��
//-----------(4)����ȥƯ��ʹ�ܿ��أ�����10��2D�����󣬿���ȥƯ�ƣ�����10��Ư�ƺ󣬹ر�ȥƯ�ƣ�
//****************�ļ�����**************************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
//***************��������***********************
static u16  GprmcVerifyErrorCount = 0;//GPRMCУ��������
static u16  GpggaVerifyErrorCount = 0;//GPGGAУ��������
static u16  GprmcParseErrorCount = 0;//GPRMC�����������
static u16  GpggaParseErrorCount = 0;//GPGGA�����������
static u16  GpsParseEnterErrorCount = 0;//û�н��������������
static u8   StopExcursionEnable = 0;//����ֹͣʱȥƯ��ʹ�ܿ���,0Ϊ��ʹ��,1Ϊʹ��
static u8   GpsRunFlag = 0;//ȥƯ����ʻ��־,1Ϊ��ʻ��0Ϊͣʻ
u32 gPositionTime = 0;//gPosition��Чʱ��ʱ�� 
u32 PositionTime = 0;//Position��Чʱ��ʱ�� 

u8  LastLocationFlag = 0;//��һ�ζ�λ״̬
u8  LocationFlag = 0;//��ǰ��λ״̬
//static u16    AccOffCount = 0;
//static u16    AccOnCount = 0; BY WYF
u8  LastLocationSpeed = 0;//��һ�ζ�λ���ٶ� 
GPS_STRUCT  gPosition;//��ǰ��Чλ��
u8  GpsOnOffFlag = 0;//GPS���ر�־,0ΪGPS��,1ΪGPS��
GPS_STRUCT   Position;//��ʱ������

//*****************�ⲿ����********************
extern  u8  AccOffGpsControlFlag;//ACC OFFʱGPSģ����Ʊ�־,0Ϊ��,1Ϊ��
extern u8   GprmcBuffer[];//���GPRMC����
extern u8   GprmcBufferBusyFlag;//GprmcBuffer����æ��־
extern u8   GpggaBuffer[];//���GPGGA����
extern u8   GpggaBufferBusyFlag;//GpggaBuffer����æ��־
extern u8   GpgsvBuffer[];//���GPGGA����
extern u8   GpgsvBufferBusyFlag;//GpggaBuffer����æ��־
extern u8   SpeedFlag;//�ٶ�����,0Ϊ����,1ΪGPS
extern u8   GBTestFlag;//0:������������ģʽ;1:������ģʽ���ñ�־ͨ���˵���ѡ��
extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ����Э����ģʽ
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
extern u8  XYTestMachine3Flag;//0Ϊ����ģʽ��1ΪЭ����3�Ż�ģʽ���������ٶ� 
//****************��������*********************
/*********************************************************************
//��������  :Gps_ReadStaNum(void)
//����      :��ȡGPS�Ķ�λ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Gps_ReadStaNum(void)
{
    return gPosition.SatNum;
}
/*********************************************************************
//��������  :GPS_AdjustRtc(GPS_STRUCT *Position)
//����      :GPSУʱ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
ErrorStatus Gps_AdjustRtc(GPS_STRUCT *Position)
{
    TIME_T time;
    u32 timecount;
    
    time.year = Position->Year;
    time.month = Position->Month;
    time.day = Position->Date;
    time.hour = Position->Hour;
    time.min = Position->Minute;
    time.sec = Position->Second;
    
    if(SUCCESS == CheckTimeStruct(&time))
    {
    
        timecount = ConverseGmtime(&time);
        timecount += 8*3600;
        Gmtime(&time, timecount);
        if(SUCCESS == CheckTimeStruct(&time))
        {
            if(0 == GBTestFlag)//dxl,2015.10.31,������ʱֻ��ͨ������Уʱָ��Уʱ
            {
              SetRtc(&time); 
            }
            return SUCCESS;     
        }
        else
        {
            return ERROR;
        }
    }
    else
    {
        return ERROR;
    }
}   
/*********************************************************************
//��������  :GPS_GpsDataIsOk(u8 *pBuffer, u8 BufferLen)
//����      :�ж�GPS�����Ƿ���ȷ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :��ȷ����1�����󷵻�0
//��ע      :
*********************************************************************/
u8 Gps_DataIsOk(u8 *pBuffer, u8 BufferLen)
{
    u8  i;
    u8  j =0;
    u8  temp;
    u8  sum = 0;
    u8  count = 0;
    u8  High;
    u8  Low;
    u8  verify;

    for(i=1; i<BufferLen; i++)//��1������ʼ��'$'
    {
        temp = *(pBuffer+i);
        if((0x0a == temp)||(0x0d == temp))
        {
            break;//����ѭ��
        }
        else if('*' == temp)
        {
            j = i;  
            break;
        }
        else
        {
            sum ^= temp;//���
            if(',' == temp)
            {
                count++;
            }
        }
    }
    High = *(pBuffer+j+1);
    Low = *(pBuffer+j+2);
    if((High >= '0')&&(High <= '9'))
    {
        High = High - 0x30;
    }
    else if((High >= 'A')&&(High <= 'F'))
    {
        High = High - 0x37;
    }
    else
    {
        return 0;
    }
    if((Low >= '0')&&(Low <= '9'))
    {
        Low = Low - 0x30;
    }
    else if((Low >= 'A')&&(Low <= 'F'))
    {
        Low = Low - 0x37;
    }
    else
    {
        return 0;
    }
    verify = (High << 4)|Low;
    if(verify == sum)
    {
        return 1;
    }
    else
    {
        return 0;
    }   
}
/*********************************************************************
//��������  :GPS_GprmcIsLocation(u8 *pBuffer, u8 BufferLen)
//����      :�ж�GPRMC�����Ƿ�λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :��ȷ����1�����󷵻�0
//��ע      :
*********************************************************************/
u8 Gps_GprmcIsLocation(u8 *pBuffer, u8 BufferLen)
{
    u8  i;
    u8  temp;
    u8  count = 0;

    for(i=0; i<BufferLen; i++)
    {
        temp = *(pBuffer+i);
        if(',' == temp)
        {
            count++;
        }
        else if('A' == temp)
        {
            if(2 == count)
            {
                return 1;
            }   
        }
    }
    
    return 0;   
}

/*********************************************************************
//��������  :GPS_GprmcParse(u8 *pBuffer, u8 BufferLen)
//����      :����GPRMC����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gps_GprmcParse(u8 *pBuffer, u8 BufferLen)
{
    u8  i = 0;
    u8  j = 0;
    u8  k = 0;
    u8  l = 0;
    u8  m = 0;
    s8  z = 0;
    u8  temp;
    u8  flag = 0;
    u8  count = 0;
    u16 temp2;
    u16 temp3;

    for(i=0; i<BufferLen; i++)
    {
        temp = *(pBuffer+i);
        if((0x0a == temp)||(0x0d == temp))
        {
            break;
        }
        else if('.' == temp)
        {
            l = i;//.�ŵ�λ��
        }
        else if(',' == temp)
        {
            k = i;//��ǰ���ŵ�λ��
            count++;
            switch(count)
            {
                case 2://��2������,����ʱ����
                    {
                        if(7 == (l-j))
                        {
                            //����ʱ��,С������������
                            Position.Hour = (*(pBuffer+j+1)-0x30)*10+(*(pBuffer+j+2)-0x30);
                            Position.Minute = (*(pBuffer+j+3)-0x30)*10+(*(pBuffer+j+4)-0x30);
                            Position.Second = (*(pBuffer+j+5)-0x30)*10+(*(pBuffer+j+6)-0x30);   
                        }
                        else if(1 == (k-j))//û������
                        {
                            
                        }
                        else
                        {
                            flag = 1;//����
                        }
                    
                        break;
                    }
                case 3://��3������,������Ч��־
                    {
                        if(2 == (k-j))
                        {
                                                        //������Ч��־
                            if('A' == *(pBuffer+j+1))
                            {
                                Position.Status = 1;
                            }
                            else if('V' == *(pBuffer+j+1))
                            {
                                Position.Status = 0;
                            }
                            else
                            {
                                flag = 1;//����
                            }
                            
                        }
                        else if(1 == (k-j))//û������
                        {
                            
                        }
                        else
                        {
                            flag = 1;
                        }
                        break;
                    }
                case 4://��4������,����γ��ֵ����ȷ��0.0001�֣���ȡС�������λ
                    {
                        if((k > l)&&(5 == l-j))
                        {
                                                        //����γ��ֵ
                            Position.Latitue_D = (*(pBuffer+j+1)-0x30)*10+(*(pBuffer+j+2)-0x30);
                            Position.Latitue_F = (*(pBuffer+j+3)-0x30)*10+(*(pBuffer+j+4)-0x30);
                            temp2 = 0;
                                                        //������δ����ڷֵ�С��λ����4λʱ����������,dxl,2013.6.27
                            for(m=l+1; m<l+5; m++)
                            {
                                temp3 = (*(pBuffer+m)-0x30);
                                for(z=0; z<l+4-m; z++)
                                {
                                    temp3 = temp3 * 10; 
                                }
                                temp2 += temp3;
                            }
                            Position.Latitue_FX = temp2;
                                                        
                            
                        }
                        else if(1 == (k-j))//û������
                        {
                            
                        }
                        else
                        {
                            flag = 1;
                        }
                        break;
                    }
                case 5://��5������,����γ�ȷ���
                    {
                        if(2 == (k-j))
                        {
                                                        //����γ�ȷ���
                            if('N' == *(pBuffer+j+1))
                            {
                                Position.North = 1;
                                
                            }
                            else if('S' == *(pBuffer+j+1))
                            {
                                Position.North = 0;
                                
                            }
                            else
                            {
                                flag = 1;
                            }
                        }
                        else if(1 == (k-j))//û������
                        {
                            
                        }
                        else
                        {
                            flag = 1;
                        }
                        break;
                    }
                case 6://��6������,��������ֵ����ȷ��0.0001��
                    {
                        if((k > l)&&(6 == l-j))
                        {
                                                        //��������ֵ
                            Position.Longitue_D = (*(pBuffer+j+1)-0x30)*100+(*(pBuffer+j+2)-0x30)*10+*(pBuffer+j+3)-0x30;
                            Position.Longitue_F = (*(pBuffer+j+4)-0x30)*10+(*(pBuffer+j+5)-0x30);
                            temp2 = 0;
                            for(m=l+1; m<l+5; m++)
                            {
                                temp3 = (*(pBuffer+m)-0x30);
                                for(z=0; z<l+4-m; z++)
                                {
                                    temp3 = temp3 * 10; 
                                }
                                temp2 += temp3;
                            }
                            Position.Longitue_FX = temp2;
                        }
                        else if(1 == (k-j))//û������
                        {
                            
                        }
                        else
                        {
                            flag = 1;
                        }
                        break;
                    }
                case 7://��7������,�������ȷ���
                    {
                        if(2 == (k-j))
                        {
                                                        //�������ȷ���
                            if('E' == *(pBuffer+j+1))
                            {
                                Position.East = 1;
                                
                            }
                            else if('W' == *(pBuffer+j+1))
                            {
                                Position.East = 0;
                                
                            }
                            else
                            {
                                flag = 1;
                            }
                        }
                        else if(1 == (k-j))//û������
                        {
                            
                        }
                        else
                        {
                            flag = 1;
                        }
                        break;
                    }
                case 8://��8������,�����ٶȣ���ȷ��������С����������
                    {
                        if((k>l)&&(l>j))
                        {
                            temp2 = 0;
                            for(m=j+1; m<l; m++)
                            {
                                temp3 = (*(pBuffer+m)-0x30);
                                for(z=0; z<l-1-m; z++)
                                {
                                    temp3 = temp3 * 10; 
                                }
                                temp2 += temp3;
                            }
                            Position.Speed = temp2;
                                                        Position.SpeedX = *(pBuffer+l+1)-0x30;
                                                        if(Position.SpeedX >= 10)
                                                        {
                                                              //��������
                                                              Position.SpeedX = 0;
                                                        }
                        }
                        else if(1 == (k-j))//û������
                        {
                            
                        }
                        else
                        {
                            flag = 1;
                        }
                        break;
                    }
                case 9://��9������,�������򣬾�ȷ��������С����������
                    {
                        if((k>l)&&(l>j))
                        {
                            temp2 = 0;
                            for(m=j+1; m<l; m++)
                            {
                                temp3 = (*(pBuffer+m)-0x30);
                                for(z=0; z<l-1-m; z++)
                                {
                                    temp3 = temp3 * 10; 
                                }
                                temp2 += temp3;
                            }
                            Position.Course = temp2;        
                        }
                        else if(1 == (k-j))//û������
                        {
                            Position.Course = 0;
                        }
                        else
                        {
                            flag = 1;
                        }
                        break;
                    }
                case 10://��10������,����������
                    {
                        if(7 == (k-j))
                        {
                            //����
                            Position.Date = (*(pBuffer+j+1)-0x30)*10+(*(pBuffer+j+2)-0x30);
                            Position.Month = (*(pBuffer+j+3)-0x30)*10+(*(pBuffer+j+4)-0x30);
                            Position.Year = (*(pBuffer+j+5)-0x30)*10+(*(pBuffer+j+6)-0x30);
                                                        //Position.Hour += 8;//��8Сʱ,�б��ⲻ��������Σ���Ϊ�б���ʱ��Уʱ��
                                                        //if(Position.Hour >= 24)//ʵ��ʹ��ʱ��8Сʱ����Уʱ������
                                                        //{
                                                               //Position.Hour -= 24;
                                                               //Position.Date++;
                                                       // }
                        }
                        else if(1 == (k-j))//û������
                        {
                            
                        }
                        else
                        {
                            flag = 1;
                        }
                        break;
                    }
                default : break;
                
            }
            j = i;//��һ�����ŵ�λ��
        }
        if(1 == flag)//�����⵽��������ǰ����
        {
            break;
        }
        
    }
    if(1 == flag)
    {
        GprmcParseErrorCount++;
    }
    else
    {
        GprmcParseErrorCount = 0;   
    }
}
/*********************************************************************
//��������  :GPS_GpggaParse(u8 *pBuffer, u8 BufferLen)
//����      :����GPGGA����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gps_GpggaParse(u8 *pBuffer, u8 BufferLen)
{
    u8  i = 0;
    u8  j = 0;
    u8  k = 0;
    u8  l = 0;
    u8  m = 0;
    s8  z = 0;
    u8  temp;
    s16 temp2;
    s16 temp3;
    u8  flag = 0;
    u8  count = 0;
    for(i=0; i<BufferLen; i++)
    {
        temp = *(pBuffer+i);
        if((0x0a == temp)||(0x0d == temp))
        {
            break;
        }
        else if('.' == temp)
        {
            l = i;
        }
        else if(',' == temp)
        {
            k = i;
            count++;
            switch(count)
            {
            
                case 8://��8�����ţ���������
                    {
                        if(2 == (k-j))//ֻ��һλ��
                        {
                            Position.SatNum = (*(pBuffer+j+1)-0x30);
                        }
                        else if(3 == (k-j))//��λ��
                        {
                            Position.SatNum = (*(pBuffer+j+1)-0x30)*10+(*(pBuffer+j+2)-0x30);
                        }
                        else if(1 == (k-j))//û������
                        {
                            
                        }
                        else
                        {
                            flag = 1;
                        }
                        break;
                    }
                case 9://��9�����ţ�����ˮƽ�������ӣ���ȷ��������С������
                    {
                        temp2 = 0;
                        for(m=j+1; m<l; m++)
                        {
                            temp3 = (*(pBuffer+m)-0x30);
                            for(z=0; z<l-m-1; z++)
                            {
                                temp3 = temp3 * 10; 
                            }
                            temp2 += temp3;
                        }
                        Position.HDOP = temp2;
                        break;
                    }
                case 10://��10�����ţ����������뺣ƽ��ĸ߶ȣ���ȷ��������С������
                    {
                        temp2 = 0;
                        if(*(pBuffer+j+1) == '-')//������
                        {
                            for(m=j+2; m<l; m++)
                            {
                                temp3 = (*(pBuffer+m)-0x30);
                                for(z=0; z<l-m-1; z++)
                                {
                                    temp3 = temp3 * 10; 
                                }
                                temp2 += temp3;
                            }
                            Position.High = -temp2;
                        }
                        else//������
                        {
                            for(m=j+1; m<l; m++)
                            {
                                temp3 = (*(pBuffer+m)-0x30);
                                for(z=0; z<l-m-1; z++)
                                {
                                    temp3 = temp3 * 10; 
                                }
                                temp2 += temp3;
                            }
                            Position.High = temp2;
                        }
                        break;
                    }
                                case 12://�����߳����ֵ
                                  {
                                        temp2 = 0;
                    if(*(pBuffer+j+1) == '-')//��ƫ��
                    {
                        for(m=j+2; m<l; m++)
                        {
                            temp3 = (*(pBuffer+m)-0x30);
                            for(z=0; z<l-m-1; z++)
                            {
                                temp3 = temp3 * 10; 
                            }
                            temp2 += temp3;
                        }
                        Position.HighOffset = -temp2;
                    }
                    else//��ƫ��
                    {
                        for(m=j+1; m<l; m++)
                        {
                            temp3 = (*(pBuffer+m)-0x30);
                            for(z=0; z<l-m-1; z++)
                            {
                                temp3 = temp3 * 10; 
                            }
                            temp2 += temp3;
                        }
                        Position.HighOffset = temp2;
                    }
                                  }
                default : break;
            }
            j = i;
        }
    }
    if(1 == flag)
    {
        GpggaParseErrorCount++;
    }
    else
    {
        GpggaParseErrorCount = 0;   
    }
}
/*********************************************************************
//��������  :GPS_CopygPosition(GPS_STRUCT *dest, GPS_STRUCT *src)
//����      :����һ�����µ�gpsλ�����ݣ�����һ����Ч�Ķ�λ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gps_CopygPosition(GPS_STRUCT *dest)
{
    dest->Year      = gPosition.Year;       //��
    dest->Month         = gPosition.Month;      //��
    dest->Date      = gPosition.Date;       //��
    dest->Hour      = gPosition.Hour;       //ʱ
    dest->Minute        = gPosition.Minute;     //��
    dest->Second        = gPosition.Second;     //��
    dest->North     = gPosition.North;      //1:��γ,0:��γ
    dest->Latitue_D     = gPosition.Latitue_D;  //γ��,��
    dest->Latitue_F     = gPosition.Latitue_F;  //γ��,��
    dest->Latitue_FX    = gPosition.Latitue_FX; //γ��,�ֵ�С��,��λΪ0.0001��
    dest->East      = gPosition.East;       //1:����0:����
    dest->Longitue_D    = gPosition.Longitue_D; //����,��,���180��
    dest->Longitue_F    = gPosition.Longitue_F; //����,��       
    dest->Longitue_FX   = gPosition.Longitue_FX;    //����,�ֵ�С��,��λΪ0.0001��
    dest->Speed         = gPosition.Speed;      //�ٶ�,��λΪ����/Сʱ
    dest->SpeedX        = gPosition.SpeedX;     //�ٶȵ�С��
    dest->Course        = gPosition.Course;     //����,��λΪ��
    dest->High      = gPosition.High;       //����,��λΪ��
        dest->HighOffset    = gPosition.HighOffset;     //����ƫ��,��λΪ��
    dest->SatNum        = gPosition.SatNum;     //��������
    dest->HDOP      = gPosition.HDOP;       //ˮƽ��������
    dest->Status        = gPosition.Status; //1:��Ч��λ 0:��Ч��λ
    dest->Error         = gPosition.Error;  //1:GPSģ���������������  0:ģ������
}
/*********************************************************************
//��������  :GPS_CopyPosition(GPS_STRUCT *dest, GPS_STRUCT *src)
//����      :����һ�������յ���gpsλ�����ݣ�����һ������Ч�Ķ�λ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gps_CopyPosition(GPS_STRUCT *dest)
{
    dest->Year      = Position.Year;        //��
    dest->Month         = Position.Month;       //��
    dest->Date      = Position.Date;        //��
    dest->Hour      = Position.Hour;        //ʱ
    dest->Minute        = Position.Minute;      //��
    dest->Second        = Position.Second;      //��
    dest->North     = Position.North;       //1:��γ,0:��γ
    dest->Latitue_D     = Position.Latitue_D;   //γ��,��
    dest->Latitue_F     = Position.Latitue_F;   //γ��,��
    dest->Latitue_FX    = Position.Latitue_FX;  //γ��,�ֵ�С��,��λΪ0.0001��
    dest->East      = Position.East;        //1:����0:����
    dest->Longitue_D    = Position.Longitue_D;  //����,��,���180��
    dest->Longitue_F    = Position.Longitue_F;  //����,��       
    dest->Longitue_FX   = Position.Longitue_FX; //����,�ֵ�С��,��λΪ0.0001��
    dest->Speed         = Position.Speed;       //�ٶ�,��λΪ����/Сʱ
    dest->SpeedX        = Position.SpeedX;      //�ٶȵ�С��
    dest->Course        = Position.Course;      //����,��λΪ��
    dest->High      = Position.High;        //����,��λΪ��
        dest->HighOffset    = Position.HighOffset;      //����ƫ��,��λΪ��
    dest->SatNum        = Position.SatNum;      //��������
    dest->HDOP      = Position.HDOP;        //ˮƽ��������
    dest->Status        = Position.Status;  //1:��Ч��λ 0:��Ч��λ
    dest->Error         = Position.Error;   //1:GPSģ���������������  0:ģ������
}
/*********************************************************************
//��������  :GPS_UpdatagPosition(void)
//����      :���±���gPosition
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gps_UpdatagPosition(void)
{
    gPosition.Year      = Position.Year;        //��
    gPosition.Month     = Position.Month;       //��
    gPosition.Date      = Position.Date;        //��
    gPosition.Hour      = Position.Hour;        //ʱ
    gPosition.Minute    = Position.Minute;      //��
    gPosition.Second    = Position.Second;      //��
    gPosition.North     = Position.North;       //1:��γ,0:��γ
    gPosition.Latitue_D     = Position.Latitue_D;   //γ��,��
    gPosition.Latitue_F     = Position.Latitue_F;   //γ��,��
    gPosition.Latitue_FX    = Position.Latitue_FX;  //γ��,�ֵ�С��,��λΪ0.0001��
    gPosition.East      = Position.East;        //1:����0:����
    gPosition.Longitue_D    = Position.Longitue_D;  //����,��,���180��
    gPosition.Longitue_F    = Position.Longitue_F;  //����,��       
    gPosition.Longitue_FX   = Position.Longitue_FX; //����,�ֵ�С��,��λΪ0.0001��
    gPosition.Speed     = Position.Speed;       //�ٶ�,��λΪ����/Сʱ
    gPosition.SpeedX    = Position.SpeedX;      //�ٶȵ�С��
    gPosition.Course    = Position.Course;      //����,��λΪ��
    gPosition.High      = Position.High;        //����,��λΪ��
    gPosition.HighOffset    = Position.HighOffset;      //����ƫ��,��λΪ��
    gPosition.SatNum    = Position.SatNum;      //��������
    gPosition.HDOP      = Position.HDOP;        //ˮƽ��������
    gPosition.Status    = Position.Status;  //1:��Ч��λ 0:��Ч��λ
    gPosition.Error     = Position.Error;   //1:GPSģ���������������  0:ģ������
}
/*********************************************************************
//��������  :GPS_ReadGpsSpeed(void)
//����      :��ȡGPS�ٶ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Gps_ReadSpeed(void)
{
    u8	Speed,checkSpeed;

	  if(1 == GBTestFlag)
		{
		    Speed = (gPosition.Speed*18520+gPosition.SpeedX*1852)/10000;
		}
		else
		{
        Speed = (gPosition.Speed*18520+gPosition.SpeedX*1852+5000)/10000;//?????0.5km
		}
    checkSpeed = 5;
    if(Task_GetCurSystemIsDeepFlag())//??//add by joneming
    {
        checkSpeed = 1;//add by joneming
    }
    if(Speed <= checkSpeed)//GPS????5??/???????0,dxl,2014.7.26
    {
        Speed = 0;
    }

		
		#if(TACHOGRAPHS_19056_TEST)
			return Tachographs_Test_Get_Speed(); 
		#else
			return Speed;
		#endif
    
    
    /*
    
    if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
    {
       return 100;
    }
    else
    {
       return 0;
    }
    */
    
}
/*********************************************************************
//��������  :GPS_ReadGpsStatus(void)
//����      :��ȡGPS��״̬
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Gps_ReadStatus(void)
{
    return gPosition.Status;
}
/*********************************************************************
//��������  :GPS_ReadGpsCourse(void)
//����      :��ȡGPS�ķ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u16 Gps_ReadCourse(void)
{
    return gPosition.Course;
}
/*********************************************************************
//��������  :GPS_ReadGpsCourseDiv2(void)
//����      :��ȡGPS�ķ��򣨳�����2��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Gps_ReadCourseDiv2(void)
{
    u8  Course;

    Course = gPosition.Course >> 1;

    return Course;
}
/*********************************************************************
//��������  :GPS_PowerOnUpdataPosition(void)
//����      :�ϵ���¾�γ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gps_PowerOnUpdataPosition(void)
{
    u8  Buffer[12];
    u8  PramLen;
    
    //PramLen = EepromPram_ReadPram(E2_LAST_LOCATION_ID, Buffer);
    PramLen = FRAM_BufferRead(Buffer, 11, FRAM_LAST_LOCATION_ADDR);
    if(11 != PramLen)//���ݴ���
    {
        
    }
    else
    {
        gPosition.North         = Buffer[0];    //1:��γ0:��γ,1�ֽ� 
        gPosition.Latitue_D     = Buffer[1];//��,1�ֽ�
        gPosition.Latitue_F     = Buffer[2];//��,1�ֽ�
        gPosition.Latitue_FX    = Buffer[3]*256 + Buffer[4];//��С������,��λΪ0.0001��,2�ֽ�,���ֽ�
        gPosition.East          = Buffer[5];//1:����0:����,1�ֽ�
        gPosition.Longitue_D    = Buffer[6];//�� ���180��,1�ֽ�,
        gPosition.Longitue_F    = Buffer[7];//�֣�1�ֽڣ�           
        gPosition.Longitue_FX   = Buffer[8]*256 + Buffer[9];//���ȷֵ�С������,��λΪ0.0001��,2�ֽ�,���ֽ�
        gPosition.Speed = 0;
        gPosition.SpeedX = 0;
        gPosition.Course = 0;
        gPosition.High = 0;
                gPosition.HighOffset = 0;
        gPosition.SatNum = 0;
        gPosition.HDOP = 0;
        gPosition.Status =  0; 
        gPosition.Error = 0;
    }
}
/*********************************************************************
//��������  :GPS_SavePositionToFram(void)
//����      :��λ�ñ��浽eeprom��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ִ������ACC ON->ACC OFF || ����->������
*********************************************************************/
void Gps_SavePositionToFram(void)
{
    u8  Buffer[12];
    
    Buffer[0] = gPosition.North;    //1:��γ0:��γ,1�ֽ� 
    Buffer[1] = gPosition.Latitue_D;//��,1�ֽ�
    Buffer[2] = gPosition.Latitue_F;//��,1�ֽ�
    Buffer[3] = (gPosition.Latitue_FX&0xff00)>>8;//��С������,��λΪ0.0001��,2�ֽ�,���ֽ�
    Buffer[4] = gPosition.Latitue_FX&0x00ff;//��С������,��λΪ0.0001��,2�ֽ�,���ֽ�
    Buffer[5] = gPosition.East;//1:����0:����,1�ֽ�
    Buffer[6] = gPosition.Longitue_D;//�ȣ�1�ֽ�
    Buffer[7] = gPosition.Longitue_F;//1�ֽ�            
    Buffer[8] = (gPosition.Longitue_FX&0xff00)>>8;//���ȷֵ�С������,��λΪ0.0001��,2�ֽ�,���ֽ�
    Buffer[9] = gPosition.Longitue_FX&0x00ff;//���ȷֵ�С������,��λΪ0.0001��,2�ֽ�,���ֽ�
        Buffer[10]= gPosition.Status ;  //1:��Ч��λ 0:��Ч��λ
    
    FRAM_BufferWrite(FRAM_LAST_LOCATION_ADDR, Buffer, 11);
        
}
/*********************************************************************
//��������  :Gps_GetRunFlag
//����      :��ȡȥƯ����ʻ��־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :1Ϊ��ʻ��0Ϊͣʻ
//��ע      :
*********************************************************************/
u8 Gps_GetRunFlag(void)
{
    return GpsRunFlag;
}

/*********************************************************************
//��������  :Gps_IsInRunning
//����      :�жϵ�ǰ�Ǵ�����ʻ״̬����ͣʻ״̬
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :1Ϊ��ʻ��0Ϊͣʻ
//��ע      :1�����1��
*********************************************************************/
void Gps_IsInRunning(void)
{
    static u32 UpCount = 0;
    static u32 DownCount = 0;    
    u8 Speed;
    u8 checkSpeed = 5;//add by joneming        
  
    if(Task_GetCurSystemIsDeepFlag())//??//add by joneming
    {
        checkSpeed  = 1;//add by joneming
    }
    Speed = (Position.Speed*18520+Position.SpeedX*1852+5000)/10000;//add by joneming,?????0.5km
    if(Speed > checkSpeed)
    {
        DownCount = 0;
        UpCount++;
        if(UpCount >= 10)
        {
            GpsRunFlag = 1;
        }
    }
    else
    {
        UpCount = 0;
        DownCount++;
        if(DownCount >= 10)
        {
            GpsRunFlag = 0;
        }
    }
}
/*********************************************************************
//��������  :GpsParse_EvTask(void)
//����      :GPS��������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Gps_EvTask(void)
{
    u8  VerifyFlag = 0;
    u8  flag;
    u8  Num;
    u32 s1;
    u32 s2;
        u8  ACC;//����ACCȥƯ��,dxl,2014.7.25

    static  u16 DriftCount = 0;
        static  u16 noNavigationCount = 0;//������������������У����������0
        static  u16     VerifyErrorCount = 0;//У����������У����ȷ��0
        static  u16     noUpdatePositionCount = 0;//��λģ������ǵ�����־����û�дﵽ���¾�γ�ȵ�Ҫ��
        static  u32     NavigationCount = 0;//dxl,2014.7.28,�������������������У����������0���ñ������ڴӲ������������˳�ǰ��3�������� 
        static  u8      AdjustRtcFlag = 0;//Уʱ��־���ϵ��Ӳ�����������Уʱ1�Σ����ÿ��50СʱУʱ1��
       
    
        ACC = Io_ReadStatusBit(STATUS_BIT_ACC);
	
	
	  //ACC OFF��Ϊ������򲿱���ʱ����������λ����
	  if(((1 == BBXYTestFlag)||(1 == BBGNTestFlag)||(1 == GBTestFlag))&&(0 == ACC))
		{
					GprmcVerifyErrorCount = 0;
			    return ;
	  }
    
    
    //�ж������Ƿ���Ч
    VerifyFlag = Gps_DataIsOk(GprmcBuffer, GPRMC_BUFFER_SIZE);
    if(1 != VerifyFlag)
    {
        GprmcVerifyErrorCount++;
    }
    else
    {
        GprmcVerifyErrorCount = 0;
        //�ж������Ƿ���Ч
        VerifyFlag = Gps_DataIsOk(GpggaBuffer, GPGGA_BUFFER_SIZE);
        if(1 != VerifyFlag)
        {
            GpggaVerifyErrorCount++;
        }
        else
        {
            GpggaVerifyErrorCount = 0;
        }
    }
        
    //У���Ƿ�ͨ��
    if(1 == VerifyFlag)//ͨ��
    {
          VerifyErrorCount = 0;
        //�б���ʱ���ν���
         GpsParseEnterErrorCount = 0;
        //����ģ������־
         Io_WriteAlarmBit(ALARM_BIT_GNSS_FAULT, RESET);
         
         if((0 == GpsOnOffFlag)||
        ((1 == GpsOnOffFlag)&&(0 == AccOffGpsControlFlag)&&(0 == ACC)))//GPSģ��ر�״̬,dxl,2014.9.24ȥ��ACC OFFʱGPS����5�����ڼ�Ľ���    
        {
            //�ٶ���0
                  Position.Speed = 0;
          gPosition.Speed = 0;  
                  Position.SpeedX = 0;
          gPosition.SpeedX = 0;  
                  return ;
        }
    
        //�ж��Ƿ�λ
        flag = Gps_GprmcIsLocation(GprmcBuffer, GPRMC_BUFFER_SIZE);
				
				if(1 == XYTestMachine3Flag)//Э����3�Ż�����ʱ�������ٶ�ģʽ������GPS����
				{
				    flag = 0;
				}
				
        if(1 != flag)
        {
             NavigationCount = 0;
                        //��λæ��־
            //GprmcBufferBusyFlag = 1;
                        //����
            //Gps_GprmcParse(GprmcBuffer, GPRMC_BUFFER_SIZE);//Ϊ�б��ͼ����
                        //���æ��־
            //GprmcBufferBusyFlag = 0;
                        
            LocationFlag = 0;//��ǰ�㵼��״̬
					  if(1 == GBTestFlag)//������ʱ����Уʱ
						{
						
						}
						else
						{
					      AdjustRtcFlag = 0;//dxl,2016.5.9�´ε���ʱ�Ż�ȥУʱ
						}
            noNavigationCount++;
            if(noNavigationCount >= 10)//����10���ֲ�����,�ٶ���0
            {
                noNavigationCount = 0;
                //�ٶ���0
                Position.Speed = 0;
                gPosition.Speed = 0;
                Position.SpeedX = 0;
                gPosition.SpeedX = 0;
                //if(1 == GBTestFlag)//������ʱGNSS���߶Ͽ�����һ��ᱻ���ε��������ϲ������ߣ�����Ҫ��Ϊ����ʾ���ٶ��������ٶ��쳣��
                //{
                    StopExcursionEnable = 0;
                  gPosition.Status = 0;
                  //������,������־��0
                  Io_WriteStatusBit(STATUS_BIT_NAVIGATION, RESET);
                //}
            }
            //�ж�·���·�����Ļ�,�ٶȺ͵�����־��0
            if((1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHUT))
               ||(1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHORT))
              ||(0 == StopExcursionEnable))
            {
                StopExcursionEnable = 0;
                gPosition.Status = 0;
                Position.Speed = 0;
                gPosition.Speed = 0;
                Position.SpeedX = 0;
                gPosition.SpeedX = 0;
                //������,������־��0
                Io_WriteStatusBit(STATUS_BIT_NAVIGATION, RESET);
            }
        }
        else
        {
            
                
            noNavigationCount = 0;
            //*************����GPRMC************
            //��λæ��־
            GprmcBufferBusyFlag = 1;
            //����
            Gps_GprmcParse(GprmcBuffer, GPRMC_BUFFER_SIZE);
            //���æ��־
            GprmcBufferBusyFlag = 0;
            //��¼ʱ��
            PositionTime = RTC_GetCounter();
            //*************����GPGGA*************
            //��λæ��־
            GpggaBufferBusyFlag = 1;
            //����
            Gps_GpggaParse(GpggaBuffer, GPGGA_BUFFER_SIZE);
            //���æ��־
            GpggaBufferBusyFlag = 0;
					
					  if(0 == AdjustRtcFlag)//dxl,2016.5.14Уʱ��Ҫ���ڽ����ĺ��棬�����ڵ���-->������-->����ʱУʱ�������⣬���ʱ��У׼����һ�������㣬֮ǰ��������
            {
                if(SUCCESS == Gps_AdjustRtc(&Position))
                {
                    Gps_SavePositionToFram();
                    AdjustRtcFlag = 1;
                }
            }
            //**************ȥƯ��**********************
              //�б���ʱ���ο�ʼ
            flag = 0;
            //v=s/t >= 200km/h(55m/s)����Ϊ��Ư��,1�ֵȼ���1�����1852��
            //γ�Ⱦ���
            s1 = ((Position.Latitue_D*60+Position.Latitue_F)*1000+Position.Latitue_FX/10)*2;//��2����1.852
            s2 = ((gPosition.Latitue_D*60+gPosition.Latitue_F)*1000+gPosition.Latitue_FX/10)*2; 
            if((s1 > s2)&&(0 != s1)&&(0 != s2))
            {
                                
                if(((s1-s2)/(PositionTime - gPositionTime) > 55)&&(0 != Position.Latitue_D)&&(0 != gPosition.Latitue_D)&&(0 != gPositionTime))//���ϵ�û��������ά��Ϊ0
                {
                    flag = 1;//��Ư�Ƶ�
                }
            }
            else if((0 != s1)&&(0 != s2))
            {
                if(((s2-s1)/(PositionTime - gPositionTime) > 55)&&(0 != Position.Latitue_D)&&(0 != gPosition.Latitue_D)&&(0 != gPositionTime))
                {
                    flag = 1;//��Ư�Ƶ�
                }
            }
            //���Ⱦ���
            s1 = ((Position.Longitue_D*60+Position.Longitue_F)*1000+Position.Longitue_FX/10)*2;//��2����1.852
            s2 = ((gPosition.Longitue_D*60+gPosition.Longitue_F)*1000+gPosition.Longitue_FX/10)*2; 
            if((s1 > s2)&&(0 != s1)&&(0 != s2))
            {
                if(((s1-s2)/(PositionTime - gPositionTime) > 55)&&(0 != Position.Longitue_D)&&(0 != gPosition.Longitue_D)&&(0 != gPositionTime))
                {
                    flag = 1;//��Ư�Ƶ�
                }
            }
            else if((0 != s1)&&(0 != s2))
            {
                if(((s2-s1)/(PositionTime - gPositionTime) > 55)&&(0 != Position.Longitue_D)&&(0 != gPosition.Longitue_D)&&(0 != gPositionTime))
                {
                    flag = 1;//��Ư�Ƶ�
                }
            }
            //����ٶȱ仯,��ǰ�ٶ�С����һ�ε��ٶȱ仯�ʲ�������
            //if(Position.Speed >= LastLocationSpeed)
                        if((Position.Speed > LastLocationSpeed)&&(0 != gPositionTime))//dxl,2013.6.4
            {
                if((Position.Speed-LastLocationSpeed) >= (PositionTime - gPositionTime)*6)
                {
                    flag = 1;
                }
            }
            //�����ٶȣ����ٶ�ȥƯ�ƣ�������300���㣨5���ӣ���Ư��ʱ������ʱ�������ȥƯ�ƣ��Ⱦ���ʱ��
            //�����߶�·�����Ļ���Ҫ���¶�λ��StopExcursionEnable = 0,dxl,2013.10.16
            if(1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHUT))
            {
                StopExcursionEnable = 0;
            }
            if(1 == flag)
            {
                DriftCount++;
                                if(DriftCount == 10)
                                {
                                      //�ٶ���0
                              gPosition.Speed = 0;  
                              gPosition.SpeedX = 0;  
                                }
                else if(DriftCount >= 300)
                {
                    flag = 0;
                }    
            }
            else
            {
                DriftCount = 0;
                                
                                if(0 == Gps_GetRunFlag())//�ж��Ƿ���ʻ,1Ϊ��ʻ
                    {
                        if(1 == StopExcursionEnable)
                        {
                              flag = 1;//ͣʻ���������� 
                        }
                                        //�ٶ���0,ע�����ﲻ����ֲ�����Position.Speed,dxl,2015.4.20
                                gPosition.Speed = 0;  
                                gPosition.SpeedX = 0;  
                    }
                    else
                    {
                        StopExcursionEnable = 0;
                    }
            }
                                  
            if(0 == LastLocationFlag)
            {
                Num = 3;
            }
            else
            {
                Num = 2;
            }
            if(0 == AccOffGpsControlFlag)//ACC OFFʱGPSģ��ر�
            {
                //��ACC״̬����
            }
            else
            {
                //����ACC״̬����
                ACC = 1;
            }
						
						//dxl,2016.5.7���ӣ����������ʱȥ��ȥƯ�ƹ���
						//if((1 == BBXYTestFlag)||(1 == BBGNTestFlag)||(1 == GBTestFlag))
						//{
						    //Num = 0;
							  //Position.HDOP = 7;
							  //flag = 0;
						//}
						//dxl,2016.5.7 end
            if((Position.SatNum>Num)&&(Position.HDOP<=8)&&(Position.Speed<=110)&&(0 == flag)&&(1 == ACC))
            {
                //�б���ʱ���ο�ʼ
                               //���±���
                NavigationCount++;
                
                if(NavigationCount > 3)//ǰ3���������˳���
                {
                                    if(DriftCount >= 300)
                                    {
                                            DriftCount = 0;
                                    }
                                    noUpdatePositionCount = 0;
                    Gps_UpdatagPosition();
                    //��¼ʱ��
                    gPositionTime = RTC_GetCounter();
                    //��һ���ٶ�
                    LastLocationSpeed = gPosition.Speed;
                    //��λ������־
                    Io_WriteStatusBit(STATUS_BIT_NAVIGATION, SET);
                    LocationFlag = 1;
                    //�ҵ���һ����ֹ��,����ͣ��ȥƯ�ƣ��б���ʱ���ο�ʼ
                    //if(0 == gPosition.Speed)
                    if(0 == Gps_GetRunFlag())//dxl,2014.7.28
                    {
                        StopExcursionEnable = 1;
                    }
                }
            }
            else
            {
                LocationFlag = 0;
                                noUpdatePositionCount++;
                                if(noUpdatePositionCount >= 10)
                                {
                                          noUpdatePositionCount = 0;
                                          gPosition.Speed = 0;
                                  gPosition.SpeedX = 0;
                                }
            }            
        }
    }
    else//У�鲻ͨ��
    {
        noNavigationCount = 0;
                NavigationCount = 0;
                VerifyErrorCount++;
                if(VerifyErrorCount >= 10)
                {
                        VerifyErrorCount = 0;
                //�ٶ���0
                Position.Speed = 0;
                gPosition.Speed = 0;
                        Position.SpeedX = 0;
                gPosition.SpeedX = 0;
                }
        //������
        //gPosition.Status = 0;
        //LocationFlag = 0;
        //������־��0
        //Io_WriteStatusBit(STATUS_BIT_NAVIGATION, RESET);,��ӡʱ��������
        
    }   

    //��¼��1�ζ�λ״̬
    LastLocationFlag = gPosition.Status;
    
    
}
/*********************************************************************
//��������  :Gps_TimeTask(void)
//����      :GPSʱ������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :1�����1��
*********************************************************************/
FunctionalState  Gps_TimeTask(void)
{
    static u8   count = 0;
    static u32  AccOffGpsOffCount = 0;
    static u8   AccOffSaveFlag = 0;
  static u8   ErrorCount = 0;
  static u32    EnterCount = 0;
    u8  ACC;
  static u8 AccOnFirstLocationReport = 0;//ACC ON��1�ζ�λ�������ϱ�1��λ����Ϣ
    
    ACC = Io_ReadStatusBit(STATUS_BIT_ACC);
    
    //�������һ����Ч�㵽����
    count++;
    if(count > 5)
    {
        count = 0;
        if(0 == AccOffGpsControlFlag)//��ACC����
        {
            if(0 == ACC)//ACC ��
            {
                if(0 == AccOffSaveFlag)
                {
                    Gps_SavePositionToFram();
                    AccOffSaveFlag = 1;
                }
            }
            else// ACC��
            {
                AccOffSaveFlag = 0;
                Gps_SavePositionToFram();
            }
        }
        else
        {
            AccOffSaveFlag = 0;
            Gps_SavePositionToFram();
        }
    }
		
		//*********�жϵ�ǰ����ʻ����ͣʻ*********
    Gps_IsInRunning();
		
		if((1 == BBXYTestFlag)||(1 == BBGNTestFlag)||(1 == GBTestFlag))
		{
			   return ENABLE;
	  }
    
		
		   if(0 == ACC)
        {
              AccOnFirstLocationReport = 0;
        }
        else
        {
              if(0 == AccOnFirstLocationReport)
              {
                    if(1 == gPosition.Status) 
                    {

                          //SetEvTask(EV_REPORT); //������ʾ,λ���ϱ�
                            Report_UploadPositionInfo(CHANNEL_DATA_1);
                            Report_UploadPositionInfo(CHANNEL_DATA_2);
                            AccOnFirstLocationReport = 1;
                    }
              }
        }
		
       if(0 == ACC)
           {
            if(0 == AccOffGpsOffCount%7200)
            {
                GPS_POWER_ON();
            }
            else if(240 == AccOffGpsOffCount%7200)
            {
                if(0 == AccOffGpsControlFlag)
                {
                    //�ϵ籨��ʱACC���ΪOFF,����ʱ��Ӧ�ر�GPS
                    if(0 == Io_ReadAlarmBit(ALARM_BIT_POWER_SHUT))
                    {
                          GPS_POWER_OFF(); 
                    }
                    else
                    {
                    
                    }
                }
            }
            AccOffGpsOffCount++;
           }
       else
       {
           AccOffGpsOffCount = 0;
           if(0 == Io_ReadAlarmBit(ALARM_BIT_GNSS_FAULT))//dxl,2014.10.8
           {
                GPS_POWER_ON();
           }
        
       }
    
        
        //***************������************************
        //������1
        if(1 == GpsOnOffFlag)//GPSģ�鿪
        {
            GpsParseEnterErrorCount++;
            
            //*******************ÿ��50СʱУʱ1�Σ�������ʻ��¼�Ǽ�������Уʱ*********
            EnterCount++;
            if((EnterCount >= 3600*50)&&(1 == LocationFlag))
            {
                            EnterCount = 0;
            //Уʱ���б���ʱ����������
#ifdef HUOYUN_DEBUG_OPEN
#else
                Gps_AdjustRtc(&Position);
#endif
            }
        }
   
        if((GpsParseEnterErrorCount >= 40)
        ||(GprmcVerifyErrorCount >= 40)
        ||(GpggaVerifyErrorCount >= 40))//һ����������������ģ��,�ȹ�ģ��5����,���ٴ�
        {
                     
                //������
                gPosition.Status = 0;
                //�ٶ���0
                gPosition.Speed = 0;
                gPosition.SpeedX = 0;
                //������
                Position.Status = 0;
                //�ٶ���0
                Position.Speed = 0;
                Position.SpeedX = 0;
                //������һ��״̬
                LastLocationFlag = 0;
                LocationFlag = 0;
                //������־��0
                Io_WriteStatusBit(STATUS_BIT_NAVIGATION, RESET);
                //��λ����
                gPosition.Error = 1;
                //��λGPSģ���쳣��־
                Io_WriteAlarmBit(ALARM_BIT_GNSS_FAULT, SET);
                
                ErrorCount++;//dxl,2014.10.8
                if(1 == ErrorCount)
                {
                      GPS_POWER_OFF();
                }
                else if(ErrorCount >= 5)
                {
                    GPS_POWER_ON();
                     //�������������0
                    GpsParseEnterErrorCount = 0;
                    GprmcVerifyErrorCount = 0;
                    GpggaVerifyErrorCount = 0;   
                    ErrorCount = 0;
                } 
        }
        else
        {
                 ErrorCount = 0;
        }
        
    
    
        return ENABLE;
}
