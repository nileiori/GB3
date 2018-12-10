/*******************************************************************************
 * File Name:           E2ParamApp.c 
 * Function Describe:   E2����Ӧ��
 * Relate Module:       
 * Writer:              Joneming
 * Date:                2013-10-16
 * ReWriter:            
 * Date:                
 *******************************************************************************/
#include "include.h"
#include "VDR.h"
/*******************************************************************************/ 
////////////////////////////////////////////
#if 1

#define NEED_CONVERT_OLD_SELF_DEFINE_ID     //�������,��ת��֮ǰ�Զ������IDΪ0x0200��ʼ�Ĳ���
#define NEED_CONVERT_SUOMEI_PARAM_ID        //�������,��ת�������Զ������IDΪ0xC000��ʼ�Ĳ���
////////////////////
#define E2_PRAM_SUOMEI_START_ID 0xC000//���������Ŀ�ʼID
#define E2_PRAM_SUOMEI_END_ID   0xD000//���������Ľ���ID
/////////////////////////////////////
//////////////////////////////////////////
extern u16 DelayAckCommand ;//��ʱӦ������
extern const u8 EepromPramLength[];
extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ����Э����ģʽ
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
extern u8  GBTestFlag;//0:������������ģʽ;1:������ģʽ
////////////////////////////////
#define E2PARAM_START_YEAR      00//����ʱ���ʽת��2000��ʼ
#define STANDARD_TIME_DIFF      28800//(������������α�׼ʱ��Ϊ8��ʱ��8*3600)
/*********************************************************************
//��������  :ConverseGmtime
//����      :�ѽṹ������ʱ��ת����32λ�޷��ŵ�ͳһ����ֵ
//����      :TIME_T �ṹ������ʱ��
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :32λ�޷��ŵ�ͳһ����ֵ
//��ע      :
*********************************************************************/
u32 E2ParamApp_ConverseGmtime(TIME_T *tt)
{
    s16 i;
    s16 year;
    u32 TotalDay;
    u32 TotalSec;

    TotalDay = 0;
    TotalSec = 0;
    year = tt->year;

    if((year >= E2PARAM_START_YEAR)&&(year <= END_YEAR))    //�ж����Ƿ���Ϸ�Χ
    {
        for(i = E2PARAM_START_YEAR; i < tt->year; i++)  //����year��֮ǰ������
        {
            TotalDay += (365+LeapYear(i));
        }
        TotalDay += YearDay(tt->year, tt->month, tt->day);  //����year���������
        TotalSec = (TotalDay-1)*DAY_SECOND + tt->hour*HOUR_SECOND+ tt->min*60 + tt->sec;    //�����ܵ�����
    }
    
    return (TotalSec-STANDARD_TIME_DIFF);
}
/*********************************************************************
//��������  :Gmtime
//����      :��32λ�޷��ŵ�ͳһ����ֵת���ɽṹ������ʱ��
//����      :ָ��TIME_T���͵�ָ�룬����ֵ
//���      :ָ��TIME_T���͵�ָ��
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void E2ParamApp_Gmtime(TIME_T *tt, u32 counter)
{
    s16 i;
    u32 sum ;
    u32 temp;
    u32 counter1 = 0;
    u32 timeVal;
    u8 flag;
    unsigned short jdays[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    timeVal=counter+STANDARD_TIME_DIFF;
    //////////////////
    if(timeVal > 0xBC191380)//������ʼ���һ����,����ת��
    {
        tt->year =E2PARAM_START_YEAR;
        tt->month = 1;
        tt->day = 1;
        tt->hour = 0;
        tt->min = 0;
        tt->sec= 0;
        return;//������ʼ���һ����,����ת��
    }
    ///////////////////////
    sum = 0;
    temp = 0;    
    for(i=E2PARAM_START_YEAR; sum<=timeVal&&i<END_YEAR; i++)   /* ������� */
    {
        sum += (LeapYear(i)*DAY_SECOND + YEAR_SECOND);
        if(sum <= timeVal)
        {
            temp= sum;
        }
    }
    ///////////////////////////
    tt->year =i-1;

    flag=LeapYear(tt->year);
    counter1=timeVal-temp;
    /////////////////////////
    sum=counter1;
    counter1=counter1%DAY_SECOND;
    temp =(sum-counter1)/DAY_SECOND;
    temp++;//TotalSec = (TotalDay-1)*DAY_SECOND + tt->hour*HOUR_SECOND+ tt->min*60 + tt->sec;   //�����ܵ�����
    for(i=12; i>0; i--)//
    {            
        if((temp>(jdays[i-1]+flag)&&i>2)||(temp>jdays[i-1]&&i<=2))            
        {
            tt->month=i;
            break;
        }
    }
    //////////////////////////////////////
    tt->day=temp-jdays[tt->month-1];
    if(tt->month>2)tt->day-=flag;

    tt->hour =counter1/HOUR_SECOND;
    ///////////////////////////
    counter1=counter1%HOUR_SECOND;
    /////////////////////
    tt->min =counter1/MIN_SECOND;

    tt->sec=counter1%MIN_SECOND;
    /////////////////////
}
//////////////////////////////////////////
/*************************************************************
** ��������: E2ParamApp_ConvertOldSelfDefinePramID
** ��������: ת���ɵ��Զ������ID(0x0200��ʼ)
** ��ڲ���: ����ID
** ���ڲ���: 
** ���ز���: �������Զ����ID(0xF200��ʼ)
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
#ifdef NEED_CONVERT_OLD_SELF_DEFINE_ID
unsigned short E2ParamApp_ConvertOldSelfDefinePramID(unsigned short usParamID)
{
    unsigned short paramID;
    paramID=usParamID;
    //////////////////////
    if(0x0200<=usParamID&&usParamID<=0x23B)//
    {
        paramID=usParamID+0xF000;
    }
    else
    if(0x0241==usParamID||usParamID==0x242)//32pin��LED�ӿ�\32pin��POS
    {
        paramID=usParamID+0xF03F;//0xF280/0xF281
    }
    else
    if(0x0243==usParamID)//������ֵ
    {
        paramID=0xF240;
    }
    else
    if(0x0244==usParamID)//�Զ��屨��������
    {
        paramID=0xF243;
    }
    else
    if(0x0246==usParamID)//��ʻԱδǩ���������ѣ�0 Ϊ�أ�1 Ϊ����
    {
        paramID=0xF24C;
    }
    else
    if(0x0247<=usParamID&&usParamID<=0x249)//��С����Ƕȡ��������Ƕȡ�����ٶ���ֵ
    {
        paramID=usParamID+0xF000-1;
    }
    else
    if(0x024A==usParamID||usParamID==0x24B)//�ػ�ģʽʱ����������ߵ�ѹ
    {
        paramID=usParamID+0xF000;
    }  
    
    ////////////////////////
    return paramID;
}
#endif
#ifdef NEED_CONVERT_SUOMEI_PARAM_ID
/*************************************************************
** ��������: E2ParamApp_ConvertSuomeiPramID
** ��������: ����������IDת�������Զ����ID
** ��ڲ���: ��������ID
** ���ڲ���: 
** ���ز���: �����Զ����ID
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned short E2ParamApp_ConvertSuomeiPramID(unsigned short usParamID)
{
    unsigned short paramID;
    paramID=usParamID;
    if((E2_PRAM_SUOMEI_START_ID > usParamID)||(usParamID>E2_PRAM_SUOMEI_END_ID))
    {
        return paramID;
    }
    else
    {
        if(0xC100== usParamID)//Ԥ������
        {
            paramID = E2_OVERSPEED_PREALARM_VOICE_ID;
        }
        else
        if(0xC101== usParamID)//��������
        {
            paramID = E2_OVERSPEED_ALARM_VOICE_ID;
        }
        else
        if(0xC110== usParamID)//���ٱ�����ʾ�����ظ������Ĵ���
        {
            paramID = E2_OVERSPEED_ALARM_NUMBER_ID;
        }
        else
        if(0xC111== usParamID)//���ٱ�����ʾʱ����
        {
            paramID = E2_OVERSPEED_ALARM_GPROUP_TIME_ID;
        }
        else
        if(0xC112== usParamID)//����Ԥ����ʾ����ʱ�䷧ֵ
        {
            paramID = E2_OVERSPEED_PREALARM_KEEPTIME_ID;
        }
        else
        if(0xC113== usParamID)//����Ԥ����ʾʱ����
        {
            paramID = E2_OVERSPEED_PREALARM_GPROUP_TIME_ID;
        }
        else
        if(0xC114 == usParamID)//����Ԥ����ʾ�Ĵ���
        {
            paramID = E2_OVERSPEED_PREALARM_NUMBER_ID;
        }
        else
        if(0xC115== usParamID)////����Ԥ����ʾֵ
        {
            //���⴦��
        }
        else
        if(0xC21B == usParamID)//ҹ��ʱ�䶨��
        {
            //���⴦��
            //paramID = E2_FORBID_DRIVE_NIGHT_TIME_ID;
        }
        else
        if(0xC220 == usParamID)//����ʱ�ε���������ǰʱ��
        {
            paramID = E2_FORBID_DRIVE_PREALARM_TIME_ID;
        }
        else
        if(0xC221 == usParamID)//����ʱ������ʱ����
        {
            paramID = E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID;
        }
        else
        if(0xC222 == usParamID)//ҹ��ʱ�䶨��
        {
            //���⴦��
            //paramID = E2_OVERSPEED_NIGHT_TIME_ID;
        }
        else
        if(0xC223 == usParamID)//ҹ�䳬��ֵ��Ԥ��ֵ����
        {
            paramID = E2_NIGHT_OVERSPEED_PERCENT_ID;
        }
        else
        if(0xC224 == usParamID)//��ʱ��ʻԤ����ʾʱ����ǰֵ
        {
            //���⴦��
        }
        else
        if(0xC226 == usParamID)//��ʱ��ʻԤ����ʾ�����ظ������Ĵ���
        {
            paramID = E2_TIRED_DRIVE_PREALARM_NUMBER_ID;
        }
        else
        if(0xC227 == usParamID)//��ʱ��ʻԤ����ʾʱ����
        {
            paramID = E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID;
        }
        else
        if(0xC228 == usParamID)//��ʱ��ʻ������ʾ�����ظ������Ĵ���
        {
            paramID = E2_TIRED_DRIVE_ALARM_NUMBER_ID;
        }
        else
        if(0xC229 == usParamID)//��ʱ��ʻ������ʾʱ����
        {
            paramID = E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID;
        }        
        else
        if(0xC230 == usParamID)//����ʱ�ε�����������
        {
            paramID = E2_FORBID_DRIVE_PREALARM_VOICE_ID;
        }
        else
        if(0xC231 == usParamID)//����ʱ�ε��ﱨ������
        {
            paramID = E2_FORBID_DRIVE_ALARM_VOICE_ID;
        }
        else
        if(0xC232 == usParamID)//����ʱ�ε������Ѳ����Ĵ���
        {
            paramID = E2_FORBID_DRIVE_PREALARM_NUMBER_ID;
        }
        else
        if(0xC234 == usParamID)//������ʾ����
        {
            paramID = E2_FORBID_DRIVE_ALARM_NUMBER_ID;
        }
        else
        if(0xC235 == usParamID)//����ʱ�ε��ﱨ����ʱ����
        {
            paramID = E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID;
        }                
        else///
        if(0xC236 == usParamID)//Ԥ��ʱ��
        {
            paramID = E2_NIGHT_DRIVE_PREALARM_TIME_ID;
        }
        else
        if(0xC237 == usParamID)//ҹ��Ԥ������
        {
            paramID = E2_NIGHT_DRIVE_PREALARM_VOICE_ID;
        }
        else
        if(0xC238 == usParamID)//ҹ��Ԥ����ʾ�����ظ������Ĵ���
        {
            paramID = E2_NIGHT_DRIVE_PREALARM_NUMBER_ID;
        }
        else
        if(0xC239 == usParamID)//ҹ��Ԥ����ʾʱ����
        {
            paramID = E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID;
        }
        else
        if(0xC250 == usParamID)//ͣ���ж��ٶ�(����/Сʱ)
        {
            paramID = E2_FORBID_DRIVE_RUN_SPEED_ID;
        }
        else
        if(0xC251 == usParamID)//ͣ���жϣ���������ʱ��
        {
            paramID = E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID;
        }
        else
        if(0xC252 == usParamID)//����������0����������255����������
        {
            paramID = E2_ROUTE_EXCURSION_ALARM_NUMBER_ID;
        }
        else
        if(0xC253 == usParamID)//ƫ����·��������ʱ���� 
        {
            paramID = E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID;
        }
        else
        if(0xC254 == usParamID)//��������
        {
            paramID = E2_ROUTE_EXCURSION_ALARM_VOICE_ID;
        }
        else
        if(0xC255 == usParamID)//ҹ��ʱ�䶨��
        {
            //���⴦��
            //paramID = E2_TIRED_DRIVE_NIGHT_TIME_ID;
        }
        else
        if(0xC257 == usParamID)//ҹ���������ʻʱ��
        {
            paramID = E2_NIGHT_MAX_DRVIE_TIME_ID;
        }
        else
        if(0xC258 == usParamID)//ҹ���ʻ��С��Ϣʱ��
        {
            paramID = E2_NIGHT_MIN_RELAX_TIME_ID;
        }
        else
        if(0xC259 == usParamID)//ҹ��Ԥ������ 
        {
            paramID = E2_TIRED_DRIVE_PREALARM_VOICE_ID;
        }
        else
        if(0xC25A == usParamID)//��������
        {
            paramID = E2_TIRED_DRIVE_ALARM_VOICE_ID;
        }
        else
        if(0xC25B == usParamID)//�¼������������ָ�������¼����ϱ�ʱ�������¼��� BIT0λ��1�����ٱ��� 
        {
            //���⴦��
        }
        else
        if(0xC25C == usParamID)//�������¼����ϱ�ʱ������ �¼������ָ������ϱ�����λ:�룬���� 0����ֹ������
        {
            paramID = E2_OVERSPEED_ALARM_REPORT_TIME_ID;
        }        
        ////////////////////
        return paramID;
    }
}
/*************************************************************
** ��������: E2ParamApp_ConvertSuomeiPramID
** ��������: �������Զ����IDת����������ID
** ��ڲ���: �Զ����ID
** ���ڲ���: 
** ���ز���: ��������ID
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned short E2ParamApp_ConvertPramIDToSuomeiID(unsigned short usParamID)
{
    unsigned short paramID;
    paramID=usParamID;    
    if(usParamID<E2_SYSTEM_DEFINE_ID_MAX)
    {
        return paramID;
    }
    else
    if((E2_PRAM_SUOMEI_START_ID <= usParamID)&&(usParamID<=E2_PRAM_SUOMEI_END_ID))
    {
        return paramID;
    }
    else
    {
        if(E2_OVERSPEED_PREALARM_VOICE_ID == usParamID)//Ԥ������
        {
            paramID = 0xC100;
        }
        else
        if(E2_OVERSPEED_ALARM_VOICE_ID == usParamID)//��������
        {
            paramID =  0xC101;
        }
        else
        if(E2_OVERSPEED_ALARM_NUMBER_ID == usParamID)//���ٱ�����ʾ�����ظ������Ĵ���
        {
            paramID = 0xC110;
        }
        else
        if(E2_OVERSPEED_ALARM_GPROUP_TIME_ID== usParamID)//���ٱ�����ʾʱ����
        {
            paramID = 0xC111;
        }
        else
        if(E2_OVERSPEED_PREALARM_KEEPTIME_ID == usParamID)//����Ԥ����ʾ����ʱ�䷧ֵ
        {
            paramID = 0xC112;
        }
        else
        if(E2_OVERSPEED_PREALARM_GPROUP_TIME_ID== usParamID)//����Ԥ����ʾʱ����
        {
            paramID = 0xC113;
        }
        else
        if(E2_OVERSPEED_PREALARM_NUMBER_ID == usParamID)//����Ԥ����ʾ�Ĵ���
        {
            paramID = 0xC114;
        }
        else
        if(E2_NIGHT_MAX_SPEED0_ID+1 == usParamID)////����Ԥ����ʾֵ
        {
            paramID = 0xC115;//���⴦��
        }
        else
        if(E2_FORBID_DRIVE_NIGHT_TIME_ID == usParamID)//ҹ��ʱ�䶨��
        {
            paramID = 0xC21B;
        }
        else
        if(E2_FORBID_DRIVE_PREALARM_TIME_ID == usParamID)//����ʱ�ε���������ǰʱ��
        {
            paramID = 0xC220;
        }
        else
        if(E2_FORBID_DRIVE_PREALARM_GPOUP_TIME_ID == usParamID)//����ʱ������ʱ����
        {
            paramID = 0xC221;
        }
        else
        if(E2_OVERSPEED_NIGHT_TIME_ID == usParamID)//ҹ��ʱ�䶨��
        {
            paramID = 0xC222;
        }
        else
        if(E2_NIGHT_OVERSPEED_PERCENT_ID == usParamID)//ҹ�䳬��ֵ��Ԥ��ֵ����
        {
            paramID = 0xC223;
        }
        else
        if(E2_NIGHT_MAX_SPEED0_ID+2  == usParamID)//��ʱ��ʻԤ����ʾʱ����ǰֵ
        {
            paramID = 0xC224;//���⴦��
        }
        else
        if(E2_TIRED_DRIVE_PREALARM_NUMBER_ID == usParamID)//��ʱ��ʻԤ����ʾ�����ظ������Ĵ���
        {
            paramID = 0xC226;
        }
        else
        if(E2_TIRED_DRIVE_PREALARM_GPOUP_TIME_ID == usParamID)//��ʱ��ʻԤ����ʾʱ����
        {
            paramID = 0xC227;
        }
        else
        if(E2_TIRED_DRIVE_ALARM_NUMBER_ID == usParamID)//��ʱ��ʻ������ʾ�����ظ������Ĵ���
        {
            paramID = 0xC228;
        }
        else
        if(E2_TIRED_DRIVE_ALARM_GPOUP_TIME_ID == usParamID)//��ʱ��ʻ������ʾʱ����
        {
            paramID = 0xC229;
        }
        else
        if(E2_FORBID_DRIVE_PREALARM_VOICE_ID== usParamID)//����ʱ�ε�����������
        {
            paramID = 0xC230;
        }
        else
        if(E2_FORBID_DRIVE_ALARM_VOICE_ID == usParamID)//����ʱ�ε��ﱨ������
        {
            paramID = 0xC231;
        }
        else
        if(E2_FORBID_DRIVE_PREALARM_NUMBER_ID == usParamID)//����ʱ�ε������Ѳ����Ĵ���
        {
            paramID = 0xC232;
        }
        else
        if(E2_FORBID_DRIVE_ALARM_NUMBER_ID == usParamID)//������ʾ����
        {
            paramID = 0xC234;
        }
        else
        if(E2_FORBID_DRIVE_ALARM_GPOUP_TIME_ID == usParamID)//����ʱ�ε��ﱨ����ʱ����
        {
            paramID = 0xC235;
        }
        else///
        if(E2_NIGHT_DRIVE_PREALARM_TIME_ID == usParamID)//Ԥ��ʱ��
        {
            paramID = 0xC236;
        }
        else
        if(E2_NIGHT_DRIVE_PREALARM_VOICE_ID == usParamID)//ҹ��Ԥ������
        {
            paramID = 0xC237;
        }
        else
        if(E2_NIGHT_DRIVE_PREALARM_NUMBER_ID == usParamID)//ҹ��Ԥ����ʾ�����ظ������Ĵ���
        {
            paramID = 0xC238;
        }
        else
        if(E2_NIGHT_DRIVE_PREALARM_GPOUP_TIME_ID  == usParamID)//ҹ��Ԥ����ʾʱ����
        {
            paramID = 0xC239;
        }
        else
        if(E2_FORBID_DRIVE_RUN_SPEED_ID  == usParamID)//ͣ���ж��ٶ�(����/Сʱ)
        {
            paramID = 0xC250;
        }
        else
        if(E2_FORBID_DRIVE_RUN_SPEED_KEEPTIME_ID == usParamID)//ͣ���жϣ���������ʱ��
        {
            paramID = 0xC251;
        }
        else
        if(E2_ROUTE_EXCURSION_ALARM_NUMBER_ID == usParamID)//����������0����������255����������
        {
            paramID = 0xC252;
        }
        else
        if(E2_ROUTE_EXCURSION_ALARM_GPOUP_TIME_ID == usParamID)//ƫ����·��������ʱ���� 
        {
            paramID = 0xC253;
        }
        else
        if(E2_ROUTE_EXCURSION_ALARM_VOICE_ID == usParamID)//��������
        {
            paramID = 0xC254;
        }
        else
        if(E2_TIRED_DRIVE_NIGHT_TIME_ID == usParamID)//ҹ��ʱ�䶨��
        {
            paramID = 0xC255;
        }
        else
        if(E2_NIGHT_MAX_DRVIE_TIME_ID == usParamID)//ҹ���������ʻʱ��
        {
            paramID = 0xC257;
        }
        else
        if(E2_NIGHT_MIN_RELAX_TIME_ID == usParamID)//ҹ���ʻ��С��Ϣʱ��
        {
            paramID = 0xC258;
        }
        else
        if(E2_TIRED_DRIVE_PREALARM_VOICE_ID == usParamID)//ҹ��Ԥ������ 
        {
            paramID = 0xC259;
        }
        else
        if(E2_TIRED_DRIVE_ALARM_VOICE_ID == usParamID)//��������
        {
            paramID = 0xC25A;
        }
        else
        if(E2_REPORT_FREQ_EVENT_SWITCH_ID == usParamID)//�¼������������ָ�������¼����ϱ�ʱ�������¼��� BIT0λ��1�����ٱ��� 
        {
            paramID = 0xC25B;//���⴦��
        }
        else
        if(E2_OVERSPEED_ALARM_REPORT_TIME_ID == usParamID)//�������¼����ϱ�ʱ������ �¼������ָ������ϱ�����λ:�룬���� 0����ֹ������
        {
            paramID = 0xC25C;
        }
        ////////////////////
        return paramID;
    }
}
#endif
/*************************************************************
** ��������: E2ParamApp_WriteTimeParam
** ��������: ��������ʱ���ʽдʱ�����
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char E2ParamApp_WriteTimeParam(unsigned short usParamID,unsigned char *data,unsigned char datalen)
{
    unsigned char buffer[20]={0};
    unsigned long temp;
    TIME_T stTime;    
    if(datalen!=10)return 0;
    memcpy(buffer,data,2);
    temp = Public_ConvertBufferToLong(&data[2]);
    E2ParamApp_Gmtime(&stTime,temp);
    Public_ConvertTimeToBCDEx(stTime,&buffer[2]);
    /////////////////////////////////
    temp = Public_ConvertBufferToLong(&data[6]);
    E2ParamApp_Gmtime(&stTime,temp);
    Public_ConvertTimeToBCDEx(stTime,&buffer[8]);
    EepromPram_WritePram(usParamID,buffer,14);
    return 10;
    
}
/*************************************************************
** ��������: E2ParamApp_ReadTimeParam
** ��������: �Ѷ���ʱ�����ת������ʱ���ʽ
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char E2ParamApp_ReadTimeParam(unsigned short usParamID,unsigned char *data)
{
    unsigned char buffer[20]={0};
    unsigned long temp;
    unsigned char len;
    TIME_T stTime;
    len=EepromPram_ReadPram(usParamID,buffer);
    if(len != 14)return 0;
    memcpy(data,buffer,2);
    if(!Public_ConvertBCDToTime(&stTime,&buffer[2]))return 0;
    temp = E2ParamApp_ConverseGmtime(&stTime);
    Public_ConvertLongToBuffer(temp,&data[2]);
    /////////////////////////////////
    if(!Public_ConvertBCDToTime(&stTime,&buffer[8]))return 0;
    temp = E2ParamApp_ConverseGmtime(&stTime);
    Public_ConvertLongToBuffer(temp,&data[6]);
    return 10;
}
/*************************************************************
** ��������: E2ParamApp_WriteSpecialParam
** ��������: д�������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char E2ParamApp_WriteSpecialParam(unsigned short usParamID,unsigned char *data,unsigned char datalen)
{
    #ifdef NEED_CONVERT_SUOMEI_PARAM_ID
    unsigned long temp,val;
    unsigned char buffer[20]={0};    
    if(0xC115== usParamID)////����Ԥ����ʾֵ
    {
        if(datalen != 4)return 0;
        temp=Public_ConvertBufferToLong(data);
        if(temp)
        {
            if(EepromPram_ReadPram(E2_MAX_SPEED_ID,buffer))
            {
                val = Public_ConvertBufferToLong(buffer);
                temp =val-temp;
            }
        }
        ///////////////////
        temp *= 10;
        Public_ConvertShortToBuffer(temp,buffer);
        EepromPram_WritePram(E2_SPEED_EARLY_ALARM_DVALUE_ID,buffer,E2_SPEED_EARLY_ALARM_DVALUE_LEN);
        return 4;
    }
    else
    if(0xC21B == usParamID)//ͣ����Ϣ(��ֹ��ʻ)ҹ��ʱ�䶨��
    {
        return E2ParamApp_WriteTimeParam(E2_FORBID_DRIVE_NIGHT_TIME_ID,data,datalen);
    }
    else
    if(0xC222 == usParamID)//����ҹ��ʱ�䶨��
    {
        return E2ParamApp_WriteTimeParam(E2_OVERSPEED_NIGHT_TIME_ID,data,datalen);
    }
    else
    if(0xC224 == usParamID)//��ʱ��ʻԤ����ʾʱ����ǰֵ
    {
        if(datalen != 4)return 0;
        temp = Public_ConvertBufferToLong(data);
        temp *= 60;
        Public_ConvertShortToBuffer(temp,buffer);
        EepromPram_WritePram(E2_TIRE_EARLY_ALARM_DVALUE_ID,buffer,E2_TIRE_EARLY_ALARM_DVALUE_LEN);
        return 4;
    }
    else
    if(0xC250== usParamID)////ͣ���ж��ٶ�(����/Сʱ),ͬʱ���³�ʱ��ʻ���ж��ٶ�
    {
        EepromPram_WritePram(E2_TIRED_DRIVE_RUN_SPEED_ID,data,E2_TIRED_DRIVE_RUN_SPEED_ID_LEN);
    }
    else
    if(0xC25B == usParamID)//�¼������������ָ�������¼����ϱ�ʱ�������¼��� BIT0λ��1�����ٱ��� 
    {
        if(datalen != 4)return 0;
        temp = Public_ConvertBufferToLong(data);
        val = 0;
        if(EepromPram_ReadPram(E2_REPORT_FREQ_EVENT_SWITCH_ID,buffer))
        {
            val = Public_ConvertBufferToLong(buffer);
        }
        ///////����ֻ�����˳��ٻش�/////////////////
        if(PUBLIC_CHECKBIT(temp, 0))//
        {
            PUBLIC_SETBIT(val, 0);
        }
        else
        {
            PUBLIC_CLRBIT(val, 0);
        }
        //////////////////////
        Public_ConvertLongToBuffer(val,buffer);
        EepromPram_WritePram(E2_REPORT_FREQ_EVENT_SWITCH_ID,buffer,E2_REPORT_FREQ_EVENT_SWITCH_ID_LEN);
    }    
    else
    if(0xC255 == usParamID)//������ʻҹ��ʱ�䶨��
    {
        return E2ParamApp_WriteTimeParam(E2_TIRED_DRIVE_NIGHT_TIME_ID,data,datalen);
    }
    #endif
    return 0;
}
/*************************************************************
** ��������: E2ParamApp_ReadSpecialParam
** ��������: ���������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char E2ParamApp_ReadSpecialParam(unsigned short usParamID,unsigned char *data)
{
    unsigned long temp=0,val=0;
    unsigned char buffer[20]={0}; 
    if(E2_CAR_TOTAL_MILE_ID == usParamID)//������̱����//0.1km
    {
        Public_ReadDataFromFRAM(FRAM_VDR_MILEAGE_ADDR,buffer,FRAM_VDR_MILEAGE_LEN);//��λ0.01����
        val  = Public_ConvertBufferToLong(buffer);
        val *= 0.1;
        Public_ConvertLongToBuffer(val,data);
        return 4;
    }
    #ifdef NEED_CONVERT_SUOMEI_PARAM_ID 
    else
    if(0xC115== usParamID)////����Ԥ����ʾֵ
    {
        if(EepromPram_ReadPram(E2_MAX_SPEED_ID,buffer))
        {
            val = Public_ConvertBufferToLong(buffer);
        }
        //////////////////
        if(EepromPram_ReadPram(E2_SPEED_EARLY_ALARM_DVALUE_ID,buffer))
        {
            temp = Public_ConvertBufferToShort(buffer);
            temp *=0.1;
            if(temp&&val>temp)
            {
                temp=val-temp;
            }
            else
            {
                temp = 0;
            }
        }        
        Public_ConvertLongToBuffer(temp,data);
        /////////////////
        return 4;
    }
    else
    if(0xC21B == usParamID)//ͣ����Ϣ(��ֹ��ʻ)ҹ��ʱ�䶨��
    {
        return E2ParamApp_ReadTimeParam(E2_FORBID_DRIVE_NIGHT_TIME_ID,data);
    }
    else
    if(0xC222 == usParamID)//����ҹ��ʱ�䶨��
    {
        return E2ParamApp_ReadTimeParam(E2_OVERSPEED_NIGHT_TIME_ID,data);
    }
    else
    if(0xC224 == usParamID)//��ʱ��ʻԤ����ʾʱ����ǰֵ
    {
        if(EepromPram_ReadPram(E2_TIRE_EARLY_ALARM_DVALUE_ID,buffer))
        {
            temp = Public_ConvertBufferToShort(buffer);
            temp /=60;
        }
        ////////////
        Public_ConvertLongToBuffer(temp,data);
        return 4;
    }
    else
    if(0xC250== usParamID)////ͣ���ж��ٶ�(����/Сʱ),ͬʱ���³�ʱ��ʻ���ж��ٶ�
    {
        //
    }
    else
    if(0xC25B == usParamID)//�¼������������ָ�������¼����ϱ�ʱ�������¼��� BIT0λ��1�����ٱ��� 
    {
        val = 0;
        if(EepromPram_ReadPram(E2_REPORT_FREQ_EVENT_SWITCH_ID,buffer))
        {
            temp = Public_ConvertBufferToLong(buffer);
        }
        ///////����ֻ�����˳��ٻش�/////////////////
        if(PUBLIC_CHECKBIT(temp, 0))//
        {
            PUBLIC_SETBIT(val, 0);
        }
        else
        {
            PUBLIC_CLRBIT(val, 0);
        }
        //////////////////////
        Public_ConvertLongToBuffer(val,data);
        return 4;
    }    
    else
    if(0xC255 == usParamID)//������ʻҹ��ʱ�䶨��
    {
        return E2ParamApp_ReadTimeParam(E2_TIRED_DRIVE_NIGHT_TIME_ID,data);
    }
    #endif
    return 0;
}
/*************************************************************
** ��������: E2ParamApp_ConvertPramID
** ��������: ����Ҫ��ת���������Զ����ID
** ��ڲ���: ����ID
** ���ڲ���: 
** ���ز���: �������Զ����ID
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned short E2ParamApp_ConvertPramID(unsigned short usParamID)
{
    unsigned short paramID;
    paramID=usParamID;
    #ifdef NEED_CONVERT_OLD_SELF_DEFINE_ID
    paramID=E2ParamApp_ConvertOldSelfDefinePramID(paramID);
    #endif
    #ifdef NEED_CONVERT_SUOMEI_PARAM_ID
    paramID=E2ParamApp_ConvertSuomeiPramID(paramID);
    #endif
    return paramID;
}
/*************************************************************
** ��������: E2ParamApp_ConvertTranPramID
** ��������: ����Ҫ��Ѳ���IDת���ϴ�ʶ��Ĳ���ID
** ��ڲ���: ����ID
** ���ڲ���: 
** ���ز���: �ϴ�ʶ��Ĳ���ID
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned short E2ParamApp_ConvertTranPramID(unsigned short usParamID)
{
    unsigned short paramID;
    paramID=usParamID;
    return paramID;
}
/*************************************************************
** ��������: E2ParamApp_ConvertPramIDToGroupIndex
** ��������: �Ѳ���IDת����������±�
** ��ڲ���: ����ID
** ���ڲ���: 
** ���ز���: �����±�
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned short E2ParamApp_ConvertPramIDToGroupIndex(unsigned short PramID)
{
    unsigned short ID;
    ID = 0;
    if(PramID < E2_SYSTEM_DEFINE_ID_MAX)
    {
        ID = PramID & 0xFFF;//ȡ���ֽ�
    }
    else 
    if(PramID >= E2_PRAM_BASE_CUSTOM_ID)
    {
        ID = (PramID-E2_PRAM_BASE_CUSTOM_ID+E2_SYSTEM_DEFINE_ID_MAX) & 0xFFF;
    }
    else//
    {
        ID = 0;
    }
    //////////////////
    return ID;
}
/*******************************************************************************
** ��������: E2ParamApp_CheckIPFormatData
** ��������: �ж������Ƿ�ΪIP��������ʽ
** ��ڲ���: data�����׵�ַ,datalen���ݳ���
** ���ز���: 0��IP��������ʽ,1����IP��������ʽ
** ȫ�ֱ���: ��
** ����ģ��: ��
*******************************************************************************/
unsigned char E2ParamApp_CheckIPFormatData(unsigned char *data,unsigned char datalen)
{
    unsigned char i;
    for(i=0; i<datalen; i++)
    {
        if(((data[i]>='0')&&(data[i]<='9'))||(data[i] == '.'))
        {

        }
        else
        if(((data[i]>='a')&&(data[i]<='z'))||((data[i]>='A')&&(data[i]<='Z')))
        {

        }
        else if(data[i] ==0)
        {
            if(i != (datalen-1))
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
    }
    return 0;
}
/*******************************************************************************
** ��������: E2ParamApp_CheckPhoneFormatData
** ��������: �ж������Ƿ�Ϊ�ֻ��Ÿ�ʽ,BCD[6]
** ��ڲ���: data�����׵�ַ,datalen���ݳ���
** ���ز���: 0���ֻ��Ÿ�ʽ,1�����ֻ��Ÿ�ʽ
** ȫ�ֱ���: ��
** ����ģ��: ��,dxl,2014.4.29����
*******************************************************************************/
unsigned char E2ParamApp_CheckPhoneFormatData(unsigned char *data,unsigned char datalen)
{
    u8 *p = NULL;
    u8  High4Bit = 0;//��4λ
    u8  Low4Bit = 0;//��4λ
    u8  i;
    if(NULL == p)
    {
        if(6 != datalen)
        {
            return 1;//��������
        }
        if((0 == *data)&&(30 == *(data+1)))//dxl,2014.6.11����,���������ֻ���ʱ��У��
        {
            return 0;//�����ֻ������ò�����bcd��ʽ,ǰ�����ֽ��ǹ̶���0H��1eH
        }
        p = data;
        for(i=0; i<datalen; i++)
        {
            High4Bit = (*p&0xf0) >> 4;
            Low4Bit = *p&0x0f;
            if((High4Bit > 9)||(Low4Bit > 9))
            {
                return 1;//��ʽ����
            }
            p++;
        }
        return 0;
        
    }
    else
    {
        return 1;//���ش���
    }
}
/*************************************************************
** ��������: E2ParamApp_DisposeWriteParam
** ��������: ����Э��д����
** ��ڲ���: pBuffer�����׵�ַ,BufferLen���ݳ���
** ���ڲ���: 
** ���ز���: ������
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
ProtocolACK E2ParamApp_DisposeWriteParam(u8 channel,u8 *pBuffer, u16 BufferLen)
{
    u8  *p;
    u8  Buffer[30];
    u8  ReadLen;
    u8  PramLen;
    s16 length;
    u32 PramID,prePramID;
    u16 ID;
    u8  flag;
    u8  ch;
    u16 Address;
    u8  i;
    u8  sum;
    p = pBuffer;
    length = BufferLen;

    //��ֻ���ǵ�ֵ�����
    while(length > 0)
    {
        //������ID
        prePramID=Public_ConvertBufferToLong(p);
        p += 4;
        ////////////////////////
        PramID = E2ParamApp_ConvertPramID(prePramID);
        /////////////////////////
        ID = E2ParamApp_ConvertPramIDToGroupIndex(PramID);
        ///////////////
        flag = 0;
        //��ȡ��������
        PramLen = *p++;
        /////////////////////////
        E2ParamApp_WriteSpecialParam(prePramID,p,PramLen);
        ////////////////////////////
        if(E2_ALL_DRIVER_INFORMATION_ID == PramID)//EGS702ʹ��,д���м�ʻԱ��Ϣ,���ֻ��д2����ʻԱ
        {
            DriverInfo_DisposeRadioProtocol(p,PramLen);//������FLASH�� 
            flag = 1;//EGS702ʹ��ʱ����ҪдE2
        }
        else
        if(E2_HB_IC_DRIVER_ID == PramID)//EGS702ʹ��,д��ʻԱ��Ϣ��IC����
        {
            flag = 1;//����Ҫд��E2��
        }
        //////////////
        if(ID&&ID< E2_PRAM_ID_INDEX_MAX)//����ID��ʶ���,��������
        {
            //������ID�Ͳ��������Ƿ���ȷ
            if(EepromPramLength[ID] >= PramLen)
            {
                //���һЩ�ض��Ĳ����Ƿ���ȷ
                //if((E2_MAIN_SERVER_IP_ID == PramID)||(E2_BACKUP_SERVER_IP_ID == PramID))��V1.01.09�汾��ʼ�����ip�������ĺϷ�����,dxl,2014.7.15
                //{
                    //���������IP�����Ƿ���ȷ
                    //flag = E2ParamApp_CheckIPFormatData(p,PramLen);
                //}
                if(E2_DEVICE_PHONE_ID == PramID)//dxl,2014.4.29,���Ӷ��ֻ��Ų����ĸ�ʽ���
                {
                    flag = E2ParamApp_CheckPhoneFormatData(p,PramLen);
                }
                if(E2_CAR_ONLY_NUM_ID == PramID)/*Ψһ�Ա�ţ�ǿ��д��CCC��֤��š���Ʒ�ͺ�*/
                {
                    memcpy(p,Recorder_CCC_ID,7);
                    //memcpy(p+7,Recorder_Product_VER,16);    ��Ʒ�ͺ��ɳ�ʼ��������ã��ն˲��ٹ̻�,dxl,2013.9.27                              
                }
                //д����
                if(0 == flag)
                {
                    ////ֻ��д�˲Ž����������/////////////////////      
                    EepromPram_WritePram(PramID, p, PramLen);
                    ////////////////////////
                    if(
                      (E2_MAIN_SERVER_IP_ID == PramID)||                  
                      (E2_MAIN_SERVER_UDP_PORT_ID == PramID)||
                      (E2_MAIN_SERVER_TCP_PORT_ID == PramID)
                      )
                    {
											  //����ͨ��1��Ȩ��,�´��������·�ע��ָ��
											  if((1 == BBXYTestFlag)||(1 == BBGNTestFlag))
												{
													
												}
												else//dxl,2016.5.13���걱�����ܼ��ʱ��������Ȩ�룬����ƽ̨Ҳ������ƽ̨��ͬ�ļ�Ȩ��
												{
												    ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
												}
                    }
										
										if(E2_BACKUP_SERVER_IP_ID == PramID)//dxl,2016.5.13�����ñ��ݷ�������Ϊ���������ӹ��ܵĿ�����־
										{
										    //����ͨ��1��Ȩ��,�´��������·�ע��ָ��
											  if(1 == BBGNTestFlag)
												{
                            SetTimerTask(TIME_MULTI_CONNECT, 1);  
													  MultiCenterConnect_SetFlag();
												}
												else if(1 == BBXYTestFlag)
												{
												
												}
												else//dxl,2016.5.13���걱�����ܼ��ʱ��������Ȩ�룬����ƽ̨Ҳ������ƽ̨��ͬ�ļ�Ȩ��
												{
												    ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
												}
										}
                    
                    if(
                      (E2_SECOND_MAIN_SERVER_IP_ID == PramID)||
                      (E2_SECOND_MAIN_SERVER_TCP_PORT_ID == PramID)||
                      (E2_SECOND_MAIN_SERVER_UDP_PORT_ID == PramID)
                      )
                    {
                        //����ͨ��1��Ȩ��,�´��������·�ע��ָ��
											  if((1 == BBXYTestFlag)||(1 == BBGNTestFlag))
												{
												
												}
												else//dxl,2016.5.13���걱�����ܼ��ʱ��������Ȩ�룬����ƽ̨Ҳ������ƽ̨��ͬ�ļ�Ȩ��
												{
                            ClearTerminalAuthorizationCode(CHANNEL_DATA_2);
												}
                    }
                    
                    if( 
                      (E2_CAR_PROVINCE_ID == PramID)||//ʡ��ID
                      (E2_CAR_CITY_ID == PramID)||//����ID
                      (E2_DEVICE_ID == PramID)||//������ID
                      (E2_CAR_PLATE_NUM_ID == PramID)||//���ƺ���
                      (E2_CAR_PLATE_COLOR_ID == PramID)//������ɫ
                      )
                    {
                        //��������ͨ����Ȩ��,�´��������·�ע��ָ��
											  if((1 == BBXYTestFlag)||(1 == BBGNTestFlag))
												{
												
												}
												else//dxl,2016.5.13���걱�����ܼ��ʱ��������Ȩ�룬����ƽ̨Ҳ������ƽ̨��ͬ�ļ�Ȩ��
												{
                            ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
                            ClearTerminalAuthorizationCode(CHANNEL_DATA_2);
												}
                    }

                    if(E2_DEVICE_PHONE_ID == PramID)//д�뱸������,�ն��ֻ���
                    {
                        EepromPram_WritePram(E2_DEVICE_PHONE_BACKUP_ID, p, PramLen);
                    }
                    else if(E2_DEVICE_ID == PramID)//д�뱸������,�ն�ID
                    {
                        EepromPram_WritePram(E2_DEVICE_BACKUP_ID, p, PramLen);
                    }
                    else if(E2_TIME_PHOTO_CONTROL_ID == PramID)//��ʱ���տ��Ʊ���
                    {
                        ch = channel;
											  if((CHANNEL_DATA_1 == ch)||(CHANNEL_DATA_2 == ch))
												{
                            FRAM_BufferWrite(FRAM_PARAMETER_TIME_PHOTO_CHANNEL_ADDR,&ch,FRAM_PARAMETER_TIME_PHOTO_CHANNEL_LEN);
												}
                                    
                        Address = E2_TIME_PHOTO_CONTROL_BACKUP_ADDR;
                        sum = 0;
                        for(i=0; i<4; i++)
                        {
                            E2prom_WriteByte(Address+i, *(p+i));
                            sum += *(p+i);
                        }
                        E2prom_WriteByte(Address+i, sum);
                    }
                    else if(E2_MILE_PHOTO_CONTROL_ID == PramID)//�������տ��Ʊ���
                    {
                        ch = channel;
            FRAM_BufferWrite(FRAM_PARAMETER_DISTANCE_PHOTO_CHANNEL_ADDR,&ch,FRAM_PARAMETER_DISTANCE_PHOTO_CHANNEL_LEN);
                                    
                        Address = E2_MILE_PHOTO_CONTROL_BACKUP_ADDR;
                        sum = 0;
                        for(i=0; i<4; i++)
                        {
                            E2prom_WriteByte(Address+i, *(p+i));
                            sum += *(p+i);
                        }
                        E2prom_WriteByte(Address+i, sum);
                    }
                    else if(E2_CDMA_SLEEP_ID == PramID)//����CDMA���߿��أ�1Ϊ������1Ϊ�أ�Ĭ��Ϊ��
                    {
                        DelayAckCommand = 4;
                        //������ʱ����
                        SetTimerTask(TIME_DELAY_TRIG, 2);//2�����������ģ��
                    }
                    else if(E2_PASSWORD_ID == PramID)//����ά������
                    {
                        #ifdef USE_NAVILCD//��������˵�����
                        if(NaviLcd_GetCurOnlineStatus())//����
                        {
                            NaviLcdSendCmdModifyPassWord(p);
                        }
                        #endif
                    }
                    else if(E2_RESUME_NOUSED_ID == PramID)//�ָ�Ϊ����û��ʹ�ù���״̬
                    {
                        if(*p)//ִ��
                        {
                                                      Public_ShowTextInfo((char *)"���ڲ����洢���������������Ĭ��ֵ����ʱԼ1��30�룬���Եȡ�����",10);
                            //����ä����¼
                            Blind_Erase(0xff);
                            //ɾ�����м�ʻԱ��Ϣ,dxl,2014.3.15����֮���������ӵ�
                            DriverInfoDeleteAll();
                            //ɾ�����е绰��
                            PhoneBookDeleteAll();
                            //ɾ�������¼�����
                            EventReportDeleteAll();
                            //ɾ��������Ϣ�㲥
                            InfoDemandDeleteAll();
                            //ɾ�����е㲥����Ϣ
                            InfoServiceDeleteAll();
                            //ɾ����������Ӧ����Ϣ
                            QuestionResponseDeleteAll();
                            //ɾ�������ı���Ϣ
                            EraseDownloadGB2313FlashArea();
                            //ɾ������ͨ����¼����
                            PhoneBook_CallRecordDeleteAll();
                            //������Ρ�Բ������
                            Area_EraseAllArea();
                            //������������
                            Polygon_ClearArea();
                            //�����·
                            ClrRoute(); 
                            //��յ�ǰ�ݴ�����������                  
                            //TiredDrive_TiredDriverInfoInit(); dxl,2015.9,
                            //������¼�����ݣ�����ʱ��ʻ��¼��
                            VDRData_EraseAllFlashArea();
                                                        VDRLog_EraseFlashArea();
                                                        FRAM_EraseAllDvrArea();
                            //�ָ�״̬λ
                            Io_ClearStatusWord();
                            Io_ClearExtCarStatusWord();
                            //�ָ�����λ
                            Io_ClearAlarmWord();
                            //�ָ���̼���
                            GpsMile_SetPram(0);
                                                        //�ָ�Ϊ��1��ʹ��ʱ�Զ����õĲ���
                            
                            if(1 == GBTestFlag)//dxl,2015.10.29,�Ȳ��ָ�Ĭ��ֵ,��������ͼ�
														{
															   EepromPram_GBTestSet();
														}
														else if(1 == BBGNTestFlag)//dxl,2016.5.9,�Ȳ��ָ�Ĭ��ֵ,���㲿���ͼ�
														{
														    EepromPram_BBGNTestSet();
														}
														else
														{
														    EepromPram_DefaultSet(); 
														}
                        }
                        //��������
                        DelayAckCommand = 5;
                        //������ʱ����
                        SetTimerTask(TIME_DELAY_TRIG, SECOND);//1�����������
                    }
                    //������д��Ĳ���
                    
#ifdef HUOYUN_DEBUG_OPEN//ȫ������ƽ̨����ʱ����Զ������TCP�˿ں�
                    if(E2_MAIN_SERVER_TCP_PORT_ID == PramID)
                    {
                    
                    }
                    else
                    {
                    if(PramLen < 30)
                    { 
                        ReadLen = EepromPram_ReadPram(PramID, Buffer);
                        if(Public_CheckArrayValIsEqual(Buffer,p,ReadLen))//д����
                        {
                            return ACK_ERROR;//д����
                        }
                    }
                    //������Ӧ�ı���
                    EepromPram_UpdateVariable(PramID);
                    }
#else
                    
                    if(PramLen < 30)
                    { 
                        ReadLen = EepromPram_ReadPram(PramID, Buffer);
                        if(Public_CheckArrayValIsEqual(Buffer,p,ReadLen))//д����
                        {
                            return ACK_ERROR;//д����
                        }
                    }
                    //������Ӧ�ı���
                    EepromPram_UpdateVariable(PramID);
#endif
                    
                }
            }
            else//�������Ȳ���ȷ��
            {
                //return 0;
            }
        }
        else//����ID����ʶ��
        {
            //return 0;
        }
        ////////////////////
        length = length - 4 - 1 - PramLen;
        p += PramLen;
    }
    return ACK_OK;
}
/*************************************************************
** ��������: E2ParamApp_ReadOneParam
** ��������: ����Э�������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/  
unsigned char E2ParamApp_ReadOneParam(unsigned long ReadPramID,unsigned long PramID,unsigned char *data)
{
    u8 PramLen;
    u8 Buffer[200];
    PramLen = E2ParamApp_ReadSpecialParam(PramID,Buffer);
    if(PramLen)
    {
        //д����ID
        Public_ConvertLongToBuffer(PramID,data);
        //д��������
        data[4] = PramLen;
        //д��������
        memcpy(&data[5],Buffer,PramLen);
        return PramLen+5;
    }
    //////////////////////////
    PramLen = EepromPram_ReadPram(ReadPramID, Buffer);
    if((PramLen != 0)&&(PramLen < 50))
    {
        //д����ID
        Public_ConvertLongToBuffer(PramID,data);
        //д��������
        data[4] = PramLen;
        //д��������
        memcpy(&data[5],Buffer,PramLen);
        return PramLen+5;
    }
    return 0;
}
/*************************************************************
** ��������: E2ParamApp_DisposeReadAllParam
** ��������: ����Э�������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
** ��    ע: ��ȡ�Ĳ����ܸ�����*pDstBuffer��ID�ţ�ID���ȣ�ID���ݵ�˳���ţ�*DstBufferLen�ܵĶ�ȡ���ȣ������������ܸ����ֽ�
           : ReadType:0Ϊ����(����)��ÿ��ֻ�ܶ�550�ֽڣ�1Ϊ���ߣ�GPRS����ÿ�οɶ�1000�ֽ�
*************************************************************/  
unsigned char E2ParamApp_DisposeReadAllParam(unsigned char *pDstBuffer,s16 *DstBufferLen,unsigned char ReadType)
{
    unsigned char *des;
    unsigned char paramNum,len;
    signed short length;
    unsigned short maxlen;
    unsigned long PramID,readParamID;  
    length =0;
    paramNum =0;    
    des = pDstBuffer;
    if(0==ReadType)
        maxlen = 550;
    else
        maxlen = 1000;
    //**************ƽָ̨���Ĳ���*****************
    for(PramID=1; PramID<E2_SYSTEM_DEFINE_ID_MAX; PramID++)
    {
        len = E2ParamApp_ReadOneParam(PramID,PramID,des);
        if(len)
        {
            if(length+len>maxlen)break;
            des += len;
            length+=len;
            paramNum++;
        }
    }   
    //******************�Զ���Ĳ���******************
    for(readParamID=E2_PRAM_BASE_CUSTOM_ID; readParamID<E2_SELF_DEFINE_ID_MAX; readParamID++)
    {
        if((readParamID != E2_ALL_DRIVER_INFORMATION_ID)&&(readParamID != E2_UPDATA_URL_ID)&&(readParamID != E2_HB_IC_DRIVER_ID))
        {
            PramID=E2ParamApp_ConvertTranPramID(readParamID);
            //������
            len=E2ParamApp_ReadOneParam(readParamID,PramID,des);
            if(len)
            {
                if(length+len>maxlen)break;
                des += len;
                length+=len;
                paramNum++;
            }
        }
    }    
    ////////////////////
    *DstBufferLen=length;
    ////////////////
    return paramNum;
}
/*************************************************************
** ��������: E2ParamApp_DisposeReadParam
** ��������: ����Э�������
** ��ڲ���: ����Ҫ���Ĳ���ID�ŷ��뻺��pIDBuffer�У�ÿ��ID���ֽڣ�����ǰ��IDBufferLenΪ4��������
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
** ��    ע: ��ȡ�Ĳ����ܸ�����*pDstBuffer��ID�ţ�ID���ȣ�ID���ݵ�˳���ţ�*DstBufferLen�ܵĶ�ȡ���ȣ������������ܸ����ֽ�
           : ReadType:0Ϊ����(����)��ÿ��ֻ�ܶ�550�ֽڣ�1Ϊ���ߣ�GPRS����ÿ�οɶ�1000�ֽ�
*************************************************************/  
unsigned char E2ParamApp_DisposeReadParam(unsigned char *pDstBuffer,s16 *DstBufferLen,unsigned char *pIDBuffer,signed short IDBufferLen,unsigned char ReadType)
{
    unsigned char *des,*src;
    unsigned char paramNum,len;
    signed short length,datalen;
    unsigned short maxlen;
    unsigned long PramID,readParamID;    
    paramNum =0;
    length =0;
    des = pDstBuffer;    
    src = pIDBuffer;//    
    datalen =IDBufferLen;
    if(0==ReadType)
        maxlen = 550;
    else
        maxlen = 1000;
    //////////////////
    while(datalen > 0)
    {
        PramID=Public_ConvertBufferToLong(src);        
        readParamID = E2ParamApp_ConvertPramID(PramID);
        //������
        if(readParamID == E2_ALL_DRIVER_INFORMATION_ID)//EGS702ר��//��ʻԱ��Ϣ
        {
            len=DriverInfo_ReadDataForRadioProtocol(des+5);//��FLASH�����м�ʻԱ��Ϣ
            ////////////////////
            Public_ConvertLongToBuffer(PramID,des);
            des[4] = len;
            ///////////////////
            if(length+len+5>maxlen)break;
            des += len+5;
            length+=len+5;
            paramNum++;
        }
        else
        if((readParamID != E2_UPDATA_URL_ID)&&(readParamID != E2_HB_IC_DRIVER_ID))
        {
            len = E2ParamApp_ReadOneParam(readParamID,PramID,des);
            if(len)
            {
                if(length+len>maxlen)break;
                des += len;
                length+=len;
                paramNum++;
            }
        }
        src += 4;
        datalen -=4;
    }
    ////////////////////
    *DstBufferLen=length;
    ////////////////
    return paramNum;
}
#endif
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

