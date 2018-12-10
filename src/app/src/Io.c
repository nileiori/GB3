/******************************************************************** 
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :Io.c       
//����      :IO��AD�ɼ�
//�汾��    :
//������    :dxl
//����ʱ��  :2012.3
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :
***********************************************************************/
//***************�����ļ�*****************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
//****************�궨��****************
//***************��������***************
// 0.0��ACC �أ�1�� ACC ��;���ʱ��1��
// 1.0��δ��λ��1����λ
// 2.0����γ��1����γ
// 3.0��������1������
// 4.0����Ӫ״̬��1��ͣ��״̬
// 5.0����γ��δ�����ܲ�����ܣ�1����γ���Ѿ����ܲ������
// 6.����
// 7.����
// 9.[9:8]��ʾ��00�ճ�,01����;10����;11����
// 10.0��������·������1��������·�Ͽ�
// 11.0��������·������1��������·�Ͽ�
// 12.0�����Ž�����1�����ż���
// 13.0����1 �أ�1����1 ����ǰ�ţ�;���ʱ��1��
// 14.0����2 �أ�1����2 �������ţ�;���ʱ��1��
// 15.0����3 �أ�1����3 �������ţ�;���ʱ��1��
// 16.0��0����4 �أ�1����4 ������ʻϯ�ţ�;���ʱ��1��
// 17.0����5 �أ�1����5 �����Զ��壩;���ʱ��1��
// 18.0��δʹ��GPS ���ǽ��ж�λ��1��ʹ��GPS ���ǽ��ж�λ
// 19.0��δʹ�ñ������ǽ��ж�λ��1��ʹ�ñ������ǽ��ж�λ
// 20.0��δʹ��GLONASS ���ǽ��ж�λ��1��ʹ��GLONASS ���ǽ��ж�λ
// 21.0��δʹ��Galileo ���ǽ��ж�λ��1��ʹ��Galileo ���ǽ��ж�λ
//22-31 ����
const u8 DetectStatusHighDelay[STATUS_BIT_MAX] = {
    20,0,0,0,0,0,0,0,       
    0,0,0,0,0,20,20,20,         
    20,20,0,0,0,0,0,0,     
    0,0,0,0,0,0,0,0,      
};
const u8 DetectStatusLowDelay[STATUS_BIT_MAX] = {
    20,0,0,0,0,0,0,0,       
    0,0,0,0,0,20,20,20,         
    20,20,0,0,0,0,0,0,     
    0,0,0,0,0,0,0,0,      
};
// 0    �����;�ߵ�ƽ���ʱ��0.15�룬�͵�ƽ���1��
// 1    Զ���;�ߵ�ƽ���ʱ��0.15�룬�͵�ƽ���1��
// 2    ��ת���;���ʱ��0.15�룬�͵�ƽ���1��
// 3    ��ת���;���ʱ��0.15�룬�͵�ƽ���1��
// 4    �ƶ���ɲ����;���ʱ��0.15��
// 5    ����;���ʱ��1��
// 6    ���;���ʱ��0.15�룬�͵�ƽ���1��
// 7    ʾ�ȵ�;���ʱ��0.15�룬�͵�ƽ���1��
// 8    ����;���ʱ��0.15��
// 9    �յ�;���ʱ��1��
// 10   �յ�
// 11   ����������
// 12   ABS����
// 13   ����������              
// 14   ���������
// 15   15-31����
const u8 DetectExtCarStatusHighDelay[CAR_STATUS_BIT_MAX] = {
    3,3,3,3,3,20,3,3,       
    3,20,0,0,0,0,0,0,       
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
const u8 DetectExtCarStatusLowDelay[CAR_STATUS_BIT_MAX] = {
    //20,20,20,20,20,20,20,20,      
    //20,20,0,0,0,0,0,0,    
    3,3,3,3,3,20,3,3,   
    3,20,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};
// 0    1����������,�յ�Ӧ������;���ʱ��2��
// 1    1������,��־ά���������������
// 2    1��ƣ�ͼ�ʻ,��־ά���������������
// 3    1��Σ��Ԥ��,�յ�Ӧ�������
// 4    1��GNSSģ�����,��־ά���������������
// 5    1��GNSS���߶Ͽ�,��־ά���������������;���ʱ��1��
// 6    1��GNSS���߶�·,��־ά���������������;���ʱ��1��
// 7    1������Ƿѹ,��־ά���������������;���ʱ��15��
// 8    1���������,��־ά���������������;���ʱ��15��
// 9    1��LCD��ʾ������,��־ά���������������
// 10   1��TTS����,��־ά���������������
// 11   1������ͷ����,��־ά���������������
// 12    1����·����֤IC ��ģ�����,��־ά���������������
// 13    1������Ԥ��,��־ά���������������
// 14    1��ƣ�ͼ�ʻԤ��,��־ά���������������
// 15    ����
// 16    ����
// 17   ����
// 18   1�������ۼƼ�ʻ��ʱ,��־ά���������������
// 19   1����ʱͣ��,��־ά���������������
// 20   1����������,�յ�Ӧ�������
// 21   1������·��,�յ�Ӧ������
// 22   1��·����ʻʱ�䲻��/����,�յ�Ӧ������
// 23   1��·��ƫ�뱨��,��־ά���������������
// 24   1������VSS ����,��־ά���������������
// 25   1�����������쳣,��־ά���������������
// 26   1����������(ͨ������������),��־ά���������������
// 27   1�������Ƿ����,�յ�Ӧ�������
// 28   1�������Ƿ�λ��,�յ�Ӧ�������
// 29    1����ײԤ��,��־ά���������������
// 30    1���෭Ԥ��,��־ά���������������
// 31    1���Ƿ����ű������ն�δ��������ʱ�����жϷǷ����ţ�,�յ�Ӧ�������
const u8 DetectAlarmHighDelay[ALARM_BIT_MAX] = {
    40,0,0,0,0,20,20,0,         
    0,0,0,0,0,0,0,0,          
    0,0,0,0,0,0,0,0,        
    0,0,0,0,0,0,0,0,    
};
const u8 DetectAlarmLowDelay[ALARM_BIT_MAX] = {
    40,0,0,0,0,20,20,0,         
    0,0,0,0,0,0,0,0,          
    0,0,0,0,0,0,0,0,        
    0,0,0,0,0,0,0,0,    
};

u32 Status = 0;//32λ״̬λ,��Ӧ����λ����Ϣ�㱨���״̬λ����
u32 Alarm = 0;//32λ����λ,��Ӧ����λ����Ϣ�㱨��ı���λ����
//********************������״̬λ����************
static u32  ExtCarStatus = 0;//��չ�ĳ���״̬λ,��Ӧ����λ����Ϣ�㱨�����չ�����ź�״̬����
static u32  IoStatus = 0;//IO״̬λ,��Ӧ����λ����Ϣ�㱨�����չIO״̬����
static u32  SuoMeiStatus = 0;//����״̬λ������IDΪ0xEF,
static u32  SelfDefine2 = 0;//�Զ����2������ID0xE1
static u32  SelfDefine2MaskWord = 0;//�Զ����2������
static u32      SelfDefine2HighValid = 0;//�Զ���2����Ч��־����ӦλΪ1��ʾ����Ч��0Ϊ����Ч
static u32      KeyAlarm = 0;//�ؼ�����������Ӧλ����Ϊ�ؼ�����ʱ���ñ������ܱ��������ֿ��ƣ������ñ������跢��SMS�������ϴ�
static u8   MicControl = 0;//��˷������
//********************������״̬λ��Ӧ���ⲿIO�ڸߵ���Ч����************
static u32  AlarmHighValid = 0xffffffff;  //�ߵ�ѹ��Ч״̬��־��ÿ��λ��Alarm��Ӧ��1Ϊ����Ч��0Ϊ����Ч
static u32  StatusHighValid = 0x0;        //�ߵ�ѹ��Ч״̬��־��ÿ��λ��IoStatus��Ӧ��1Ϊ����Ч��0Ϊ����Ч
static u32  ExtCarStatusHighValid = 0x0;  //�ߵ�ѹ��Ч״̬��־��ÿ��λ��ExtCarStatus��Ӧ��1Ϊ����Ч��0Ϊ����Ч

//********************���ڿ���λ************
static u8   SpeakerControl = 0;//������������,��2���ط�ʹ����:ͨ��,tts��������
static u8   OverSpeedControl = 0;//���ٿ����֣���3���ط�ʹ����:���泬��,������,��·����
static u8   OvertimeControl = 0;//��ʱ�����֣���2���ط�ʹ����:���泬ʱ��ҹ�䳬ʱ
static u8   PreOvertimeControl = 0;//��ʱԤ�������֣���2���ط�ʹ����:���泬ʱ��ҹ�䳬ʱ
static u8   RecordControl = 0;//¼��������,��3���ط�ʹ����:��ʱ��ѭ��¼��,��������¼��,�ճ�ת�س�¼��

//********************����������***************
u32 AlarmMaskWord = 0;//����������,���ö�Ӧλ��ñ���λ�����ٱ���,�൱�ڸñ�������û����
u8  MicBaseOnOffFlag = 0;//���ƫ�õ�ѹON/OFF��־λ1,1ΪON��0ΪOFF
//********************������״̬�仯���տ���*************
u32 AlarmPhotoSwitch = 0;//�������տ���,1Ϊʹ��,0��ʹ��
u32 AlarmPhotoStoreFlag = 0;//��������洢��־,1Ϊ�洢,0Ϊ���洢
u32 StatusPhotoSwitch = 0;//״̬�仯���տ���,1Ϊʹ��,0��ʹ��
u32 StatusPhotoStoreFlag = 0;//״̬�仯���մ洢��־,1Ϊ�洢,0Ϊ���洢
//*********************����������*****************
u32 BeepOnTime = 0;
u32 BeepOffTime = 0;
u8  BeepOnOffFlag = 0;//���������ر�־

//*********************ͨ��ģ�飬��λģ��ָʾ�ƿ���************
u32 GprsLedOnTime = 0;
u32 GprsLedOffTime = 0;
u32 GpsLedOnTime = 0;
u32 GpsLedOffTime = 0;
u8  GprsLedOnOffFlag = 0;//���������ر�־
u8  GpsLedOnOffFlag = 0;//���������ر�־
u8  GprsLedState = 0;//GPRS LED��״̬,0��������Ϩ��,1��������˸,2�����ϳ���
u8  GpsLedState = 0;//GPS LED��״̬,0��������Ϩ��,1��������˸,2�����ϳ���

static u8   LastOneKeyStatus = 0;//��һ��һ��ͨ��״̬,1Ϊ�ж��������˼�
static u8   DoorSelectFlag = 0;//���ŵ�ƽѡ���־,0Ϊ����Ч,1Ϊ����Ч
//***************12/24V��Դ����Ӧ**************
static u16  PowerLowMaxValue = 0;//����0V
static u16  PowerShutValue = 50;//����5.0V
u8  CarVoltageType = 0;//������ƽ����:0Ϊ12V,1Ϊ24V
//****************�ⲿ����*****************

extern u8   GpsOnOffFlag;//GPS���ر�־,0ΪGPS��,1ΪGPS��
extern CAMERA_STRUCT Camera[];
extern u32     PhotoID;//ͼ���ý��ID
extern u8  BBXYTestFlag;//0Ϊ����ģʽ��1Ϊ����Э����ģʽ
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ

//****************��������*****************
//**********************��һ���֣���ʼ��****************
/*********************************************************************
//��������  :Io_Init(void)
//����      :IO��ʼ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_Init(void)
{
        GpioInInit(DLIGHTI);
        GpioInInit(LLIGHTI);
        GpioInInit(BREAK);
        GpioInInit(ACC);
        GpioInInit(DOORI_N);
        GpioInInit(DOORI_P);
        GpioInInit(DOORI2);
        GpioOutInit(LAMP);
        GpioOutInit(LOCK_OUT);
        GpioOutInit(UNLOCK_OUT);
        GpioInInit(XLIGHTI);
        GpioInInit(RLIGHTI);
        GpioInInit(BKHI);
        GpioInInit(SWITCHI);  
        GpioInInit(HORNI); 
        GpioInInit(VIRBRATIONI); 
        GpioOutInit(COIL); 
        GpioOutInit(HORN_OUT); 
        
        GpioInInit(DETCT1I); 
        GpioInInit(DETCT2I); 
        
        GpioOutInit(V_COL);
    
        GpioOutInit(BUZZER);
        //GpioInInit(MIC_EN);//dxl,2014.9.30ԭ��������Ϊ�������������MIC_INͬʱҲ�������ֽ��ɱ��汾�����������Ϊ���룬
                            //�ڼ���꽵�ɱ��汾�������Զ�����Ϊ�����
				GpioOutInit(MIC_EN);//dxl,2016.3.30
        GpioOutInit(SD_POWER);//dxl,2013.5.16
        
        GpioOutInit(I2S_AD_DA);//dxl,2013.5.23
        GpioOutInit(I2S_PWDAN);//dxl,2013.5.23
        GpioOutInit(I2S_PWADN);//dxl,2013.5.23
        
        GpioOutInit(PWR_TYPE);//dxl,2013.5.26
    
        GpioInInit(SD_DETECT);//dxl,2013.8.16
        
        Io_InitStatusHighValid();        //��ʼ��״̬����Ч��ƽ
        Io_InitExtCarStatusHighValid();  //��ʼ����չ����״̬����Ч��ƽ
        Io_InitAlarmHighValid();         //��ʼ����������Ч��ƽ
        Io_InitDefine2StatusHighValid();//��ʼ���Զ���״̬��2����Ч��ƽ
    
}
/*********************************************************************
//��������  :Io_InitStatusHighValid(void)
//����      :��ʼ���ߵ�ƽ��Ч��־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_InitStatusHighValid(void)
{
    StatusHighValid = 0;
    Io_WriteStatusHighValidBit(STATUS_BIT_ACC, SET);//ACC����Ч
    Io_WriteStatusHighValidBit(STATUS_BIT_DOOR1, SET);//ǰ�Ÿ���Ч,��1
    Io_WriteStatusHighValidBit(STATUS_BIT_DOOR2, SET);//���Ÿ���Ч,��2
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_LITTLE_LIGHT, SET);//С�Ƹ���Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_NEAR_LIGHT, SET);//����Ƹ���Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_GAS_PRESS, SET);//����ѹ������Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_BRAKE, SET);//ɲ������Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_HORN, SET);//�����ȸ���Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_BIG_LIGHT, SET);//��Ƹ���Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_LEFT_LIGHT, SET);//��Ƹ���Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_RIGHT_LIGHT, SET);//�ҵƸ���Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_FRONT_DOOR, SET);//ǰ�Ÿ���Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_MIDDLE_DOOR, SET);//���Ÿ���Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_AIR_CONDITION, SET);//�յ�����Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_ENGINE, SET);//����������Ч
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_EXTG1, SET);//�ⲿ����1
    //Io_WriteStatusHighValidBit(DEF_STATUS_BIT_EXTG2, SET);//�ⲿ����2
}
/*********************************************************************
//��������  :Io_InitExtCarStatusHighValid(void)
//����      :��ʼ���ߵ�ƽ��Ч��־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_InitExtCarStatusHighValid(void)
{
    Io_WriteExtCarStatusHighValidBit(CAR_STATUS_BIT_SIDE_LIGHT, SET);    //С�Ƹ���Ч,ʾ�ȵ�
    Io_WriteExtCarStatusHighValidBit(CAR_STATUS_BIT_NEAR_LIGHT, SET);    //����Ƹ���Ч
    Io_WriteExtCarStatusHighValidBit(CAR_STATUS_BIT_BRAKE, SET);         //ɲ������Ч
    Io_WriteExtCarStatusHighValidBit(CAR_STATUS_BIT_HORN, SET);          //�����ȸ���Ч
    Io_WriteExtCarStatusHighValidBit(CAR_STATUS_BIT_FAR_LIGHT, SET);     //��Ƹ���Ч,Զ���
    Io_WriteExtCarStatusHighValidBit(CAR_STATUS_BIT_LEFT_LIGHT, SET);    //��Ƹ���Ч
    Io_WriteExtCarStatusHighValidBit(CAR_STATUS_BIT_RIGHT_LIGHT, SET);   //�ҵƸ���Ч
    Io_WriteExtCarStatusHighValidBit(CAR_STATUS_BIT_AIR_CONDITION, SET); //�յ�����Ч
}
/*********************************************************************
//��������  :Io_InitDefine2StatusHighValid(void)
//����      :��ʼ���ߵ�ƽ��Ч��־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_InitDefine2StatusHighValid(void)
{

}
/*********************************************************************
//��������  :Io_InitAlarmHighValid(void)
//����      :��ʼ���ߵ�ƽ��Ч��־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_InitAlarmHighValid(void)
{
    AlarmHighValid = 0xffffffff;
    Io_WriteAlarmHighValidBit(ALARM_BIT_EMERGENCY, RESET);//������������Ч
    //Io_WriteAlarmHighValidBit(ALARM_BIT_ANT_SHUT, RESET);//���߿�·����Ч
    Io_WriteAlarmHighValidBit(ALARM_BIT_ANT_SHORT, RESET);//���߶�·����Ч
}
/*********************************************************************
//��������  :Io_UpdataPram(void)
//����      :���±���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_UpdataPram(void)
{       
    u8  Buffer[5];
    u8  BufferLen;
        
        //���ؼ�������
        BufferLen = EepromPram_ReadPram(E2_KEY_FLAG_ID,Buffer);
    if(BufferLen == E2_KEY_FLAG_LEN)
    {
        KeyAlarm = 0;
        KeyAlarm |= Buffer[0] << 24;
        KeyAlarm |= Buffer[1] << 16;
        KeyAlarm |= Buffer[2] << 8;
        KeyAlarm |= Buffer[3];
    }
    //������������
    BufferLen = EepromPram_ReadPram(E2_ALARM_MASK_ID,Buffer);
    if(BufferLen == E2_ALARM_MASK_LEN)
    {
        AlarmMaskWord = 0;
        AlarmMaskWord |= Buffer[0] << 24;
        AlarmMaskWord |= Buffer[1] << 16;
        AlarmMaskWord |= Buffer[2] << 8;
        AlarmMaskWord |= Buffer[3];
    }
    //�������������տ���
    BufferLen = EepromPram_ReadPram(E2_ALARM_PHOTO_SWITCH_ID,Buffer);
    if(BufferLen == E2_ALARM_PHOTO_SWITCH_LEN)
    {
        AlarmPhotoSwitch = 0;
        AlarmPhotoSwitch |= Buffer[0] << 24;
        AlarmPhotoSwitch |= Buffer[1] << 16;
        AlarmPhotoSwitch |= Buffer[2] << 8;
        AlarmPhotoSwitch |= Buffer[3];
    }
    //���������մ洢��־
    BufferLen = EepromPram_ReadPram(E2_ALARM_PHOTO_STORE_FLAG_ID,Buffer);
    if(BufferLen == E2_ALARM_PHOTO_STORE_FLAG_LEN)
    {
        AlarmPhotoStoreFlag = 0;
        AlarmPhotoStoreFlag |= Buffer[0] << 24;
        AlarmPhotoStoreFlag |= Buffer[1] << 16;
        AlarmPhotoStoreFlag |= Buffer[2] << 8;
        AlarmPhotoStoreFlag |= Buffer[3];
    }
    //״̬�仯���տ���
    BufferLen = EepromPram_ReadPram(E2_STATUS_PHOTO_SWITCH_ID,Buffer);
    if(BufferLen == E2_STATUS_PHOTO_SWITCH_ID_LEN)
    {
        StatusPhotoSwitch = 0;
        StatusPhotoSwitch |= Buffer[0] << 24;
        StatusPhotoSwitch |= Buffer[1] << 16;
        StatusPhotoSwitch |= Buffer[2] << 8;
        StatusPhotoSwitch |= Buffer[3];
    }
    //���������մ洢��־
    BufferLen = EepromPram_ReadPram(E2_STATUS_PHOTO_STORE_FLAG_ID,Buffer);
    if(BufferLen == E2_STATUS_PHOTO_STORE_FLAG_ID_LEN)
    {
        StatusPhotoStoreFlag = 0;
        StatusPhotoStoreFlag |= Buffer[0] << 24;
        StatusPhotoStoreFlag |= Buffer[1] << 16;
        StatusPhotoStoreFlag |= Buffer[2] << 8;
        StatusPhotoStoreFlag |= Buffer[3];
    }
    //����һ�ν�������״̬
    BufferLen = FRAM_BufferRead(Buffer, FRAM_EMERGENCY_FLAG_LEN, FRAM_EMERGENCY_FLAG_ADDR);
    if(1 == BufferLen)
    {
        if(1 == Buffer[0])
        {
            Io_WriteAlarmBit(ALARM_BIT_EMERGENCY, SET);
            
        }
        else
        {
            Io_WriteAlarmBit(ALARM_BIT_EMERGENCY, RESET);
        }
    }
    //�����ŵ�ƽ����
    BufferLen = EepromPram_ReadPram(E2_DOOR_SET_ID,Buffer);
    if(E2_DOOR_SET_LEN == BufferLen)
    {
        DoorSelectFlag = Buffer[0];
    }
        //DoorSelectFlag = 1;//�б걱�����ʱ���,����1�ߵ�ƽ��Ч
    //��¼�����Ʊ�־
    if(FRAM_RECORD_CONTROL_LEN != FRAM_BufferRead(&RecordControl, FRAM_RECORD_CONTROL_LEN, FRAM_RECORD_CONTROL_ADDR))
    {
        RecordControl = 0;//�쳣ʱĬ��Ϊ0
    }
    //��ȡ��λģʽ
    BufferLen = EepromPram_ReadPram(E2_GPS_SET_MODE_ID,Buffer);
    if(E2_GPS_SET_MODE_LEN == BufferLen)//bit0:GPS��λ;bit1:������λ;bit2:GLONASS��λ;bit3:Galileo��λ
    {
        if(0x01 == (0x01&Buffer[0]))
        {
            Io_WriteStatusBit(STATUS_BIT_GPS, SET);
        }
        else
        {
            Io_WriteStatusBit(STATUS_BIT_GPS, RESET);
        }
        
        if(0x02 == (0x02&Buffer[0]))
        {
            Io_WriteStatusBit(STATUS_BIT_COMPASS, SET);
        }
        else
        {
            Io_WriteStatusBit(STATUS_BIT_COMPASS, RESET);
        }
        
        if(0x04 == (0x04&Buffer[0]))
        {
            Io_WriteStatusBit( STATUS_BIT_GLONASS, SET);
        }
        else
        {
            Io_WriteStatusBit( STATUS_BIT_GLONASS, RESET);
        }
        
        if(0x08 == (0x08&Buffer[0]))
        {
            Io_WriteStatusBit(STATUS_BIT_GALILEO, SET);
        }
        else
        {
            Io_WriteStatusBit(STATUS_BIT_GALILEO, RESET);
        }
        
    }
}
//**********************�ڶ����֣��ⲿ����****************
/*********************************************************************
//��������  :Io_TimeTask
//����      :IO������λ�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      : 50ms����һ��
*********************************************************************/
FunctionalState Io_TimeTask(void)
{
    u8  OneKeyStatus;
        u32 CurrentTime = 0;
    u8  Acc;
    

    Io_DetectExtCarStatus(BREAK,CAR_STATUS_BIT_BRAKE);//���ɲ��
        Io_DetectStatus(ACC, STATUS_BIT_ACC); //���ACC
        
        Acc = Io_ReadStatusBit(STATUS_BIT_ACC);

  
    Io_DetectExtCarStatus(BKHI,CAR_STATUS_BIT_AIR_CONDITION);         //���յ�
        if((1 == GpsOnOffFlag)&&(1 == Acc))//GPS�Ǵ򿪵�
    {
        Io_DetectAlarm(GPS_OC, ALARM_BIT_ANT_SHUT);//���GPS���߿�·
        Io_DetectAlarm(GPS_SC, ALARM_BIT_ANT_SHORT);//���GPS���߶�·
            
    }
    else//ACC OFFʱ��������ߣ�����״̬��ά��֮ǰ��
    {
        //Io_WriteAlarmBit(ALARM_BIT_ANT_SHUT, RESET);
        //Io_WriteAlarmBit(ALARM_BIT_ANT_SHORT, RESET);
    }
    Io_DetectAlarm(SWITCHI, ALARM_BIT_EMERGENCY);//����������
    
        Io_DetectExtCarStatus(DLIGHTI,CAR_STATUS_BIT_FAR_LIGHT);    //���Զ��ƣ���ƣ��������3����Ч
   
        Io_DetectExtCarStatus(XLIGHTI,CAR_STATUS_BIT_NEAR_LIGHT);//,ԭ������Ƽ������16Pin�ϣ����ڸ�Ϊ32PIN��С�ƴ�

        Io_DetectExtCarStatus(DETCT1I,CAR_STATUS_BIT_SIDE_LIGHT); //,ʾ�ȵƸ�Ϊ��16PIN�ϵĽ���Ƽ��,ע����ԭ����С�Ƽ����ͬ���޸�
    Io_DetectExtCarStatus(LLIGHTI,CAR_STATUS_BIT_LEFT_LIGHT);    //�����ת��
                
    Io_DetectExtCarStatus(RLIGHTI,CAR_STATUS_BIT_RIGHT_LIGHT);   //�����ת��
    Io_DetectExtCarStatus(HORNI,CAR_STATUS_BIT_HORN);            //��⳵����,�޸ĳ�ɳ��������ʾ�汾ʱ����
            
        Io_DetectStatus(DOORI2, STATUS_BIT_DOOR2);                   //����г��ţ�����2��,�޸ĳ�ɳ��������ʾ�汾ʱ����
    
    Io_DetectFrontDoor();                                        //���ǰ��,ǰ����Ҫ������⣬���ܵ���ͨ�ú���
    OneKeyStatus = Io_DetectPhoneKey();                          //���һ��ͨ��
    if((0 == OneKeyStatus)&&(1 == LastOneKeyStatus))             //���¼������ɿ�
    {
        SetEvTask(EV_CALLING);     //����һ��ͨ��   
    }
    LastOneKeyStatus = OneKeyStatus;
    Io_DetectMainPower();//��������Ƿ�����
    //Io_DetectBackupPower();//��ⱸ��

        //***********������*************
    CurrentTime = Timer_Val();        
    if(1 == BeepOnOffFlag)                                         //BEEP_ON()����λ��־ BeepOnOffFlag
    {
        if((CurrentTime - BeepOnTime) >= (1*SYSTICK_0p1SECOND))
        {
            BEEP_OFF();
        }
    }
     
    return ENABLE;
}
/*********************************************************************
//��������  :Io_ReadStatus
//����      :����׼״̬��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u32 Io_ReadStatus(void)
{
    return Status;
}
/*********************************************************************
//��������  :Io_ReadStatusBit(u8 index)
//����      :����׼״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Io_ReadStatusBit(STATUS_BIT bit)
{
    u8  state = 0;

    if(bit < 32)
    {
        state = (Status & (1<< bit))>> bit;
    }
    return state;
}
/*********************************************************************
//��������  :Io_WriteStatusBit
//����      :д��׼״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteStatusBit(STATUS_BIT bit, FlagStatus state)
{
    if(bit < 32)
    {
        if(SET==state)
        {
            Status |= 1<<bit;
        }
        else if(RESET==state)
        {
            Status &= ~(1<<bit);
        }
    
    }
}
/*********************************************************************
//��������  :Io_ReadExtCarStatus
//����      :����չ����״̬��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u32 Io_ReadExtCarStatus(void)
{
    return ExtCarStatus;
}
/*********************************************************************
//��������  :Io_ReadExtCarStatusBit(u8 index)
//����      :����չ����״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Io_ReadExtCarStatusBit(CAR_STATUS_BIT bit)
{
    u8  state = 0;

    if(bit < 32)
    {
        state = (ExtCarStatus & (1<< bit))>> bit;
    }
    return state;
}
/*********************************************************************
//��������  :Io_WriteExtCarStatusBit
//����      :д��չ����״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteExtCarStatusBit(CAR_STATUS_BIT bit, FlagStatus state)
{
    if(bit < 32)
    {
        if(SET==state)
        {
            ExtCarStatus |= 1<<bit;
        }
        else if(RESET==state)
        {
            ExtCarStatus &= ~(1<<bit);
        }
    
    }
}
/*********************************************************************
//��������  :Io_ReadIoStatus
//����      :��IO״̬��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u16 Io_ReadIoStatus(void)
{
    return IoStatus;
}
/*********************************************************************
//��������  :Io_ReadIoStatusBit(u8 index)
//����      :��IO״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Io_ReadIoStatusBit(IO_STATUS_BIT bit)
{
    u8  state = 0;

    if(bit < 16)
    {
        state = (IoStatus & (1<< bit))>> bit;
    }
    return state;
}
/*********************************************************************
//��������  :Io_WriteIoStatusBit
//����      :дIO״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteIoStatusBit(IO_STATUS_BIT bit, FlagStatus state)
{
    if(bit < 16)
    {
        if(SET==state)
        {
            IoStatus |= 1<<bit;
        }
        else if(RESET==state)
        {
            IoStatus &= ~(1<<bit);
        }
    
    }
}
/*********************************************************************
//��������  :Io_ReadSuoMeiStatus
//����      :��SuoMei״̬��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u32 Io_ReadSuoMeiStatus(void)
{
    return SuoMeiStatus;
}
/*********************************************************************
//��������  :Io_ReadSuoMeiStatusBit(u8 index)
//����      :��SuoMei״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Io_ReadSuoMeiStatusBit(SUOMEI_STATUS_BIT bit)
{
    u8  state = 0;

    if(bit < 32)
    {
        state = (SuoMeiStatus & (1<< bit))>> bit;
    }
    return state;
}
/*********************************************************************
//��������  :Io_WriteSuoMeiStatusBit
//����      :дSuoMei״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteSuoMeiStatusBit(SUOMEI_STATUS_BIT bit, FlagStatus state)
{
    if(bit < 32)
    {
        if(SET==state)
        {
            SuoMeiStatus |= 1<<bit;
        }
        else if(RESET==state)
        {
            SuoMeiStatus &= ~(1<<bit);
        }
    
    }
}

/*********************************************************************
//��������  :Io_ReadAlarmMaskBit(u8 index)
//����      :����������λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :�ؼ�����λ��ӦΪ1��ֱ�ӷ���0���ؼ�����λ��ӦλΪ0���������α���λ
*********************************************************************/
u8 Io_ReadAlarmMaskBit(ALARM_BIT bit)
{
    u8  tmp = 0;

    if(bit < 32)
    {
                if((KeyAlarm& (1<< bit))==(1 << bit))
                {
                      tmp = 0;
                }
                else
                {
              tmp = (AlarmMaskWord& (1<< bit))>> bit;
                }
    }
    return tmp;
}
/*********************************************************************
//��������  :Io_ReadAlarm
//����      :��������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u32 Io_ReadAlarm(void)
{
#ifdef HUOYUN_DEBUG_OPEN
    return (Alarm&((~AlarmMaskWord)|KeyAlarm));
#else
        return (Alarm&(~AlarmMaskWord));
#endif
}
/*********************************************************************
//��������  :Io_ReadKeyAlarm
//����      :���ؼ�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u32 Io_ReadKeyAlarm(void)
{
    return KeyAlarm;
}
/*********************************************************************
//��������  :Io_ReadSelfDefine2
//����      :���Զ���״̬��2
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u32 Io_ReadSelfDefine2(void)
{
    return (SelfDefine2&(~SelfDefine2MaskWord));
}
/*********************************************************************
//��������  :Io_ReadSelfDefine2Bit(u8 index)
//����      :���Զ����2��λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Io_ReadSelfDefine2Bit(DEFINE_BIT bit)
{
    u8  tmp = 0;

    if(bit < 32)
    {
        tmp = ((SelfDefine2&(~SelfDefine2MaskWord)) & (1<< bit))>> bit;
    }
    return tmp;
}
/*********************************************************************
//��������  :Io_ReadAlarmBit(u8 index)
//����      :������λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Io_ReadAlarmBit(ALARM_BIT bit)
{
    u8  tmp = 0;

    if(bit < 32)
    {
#ifdef HUOYUN_DEBUG_OPEN
        tmp = ((Alarm&((~AlarmMaskWord)|KeyAlarm)) & (1<< bit))>> bit;
#else
                tmp = ((Alarm&(~AlarmMaskWord)) & (1<< bit))>> bit;
#endif
    }
    return tmp;
}
/*********************************************************************
//��������  :Io_ReadKeyAlarmBit(ALARM_BIT bit)
//����      :���ؼ�������ĳһ��λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Io_ReadKeyAlarmBit(ALARM_BIT bit)
{
    u8  tmp = 0;

    if(bit < 32)
    {
        tmp = (KeyAlarm & (1<< bit))>> bit;
    }
    return tmp;
}
/*********************************************************************
//��������  :Io_WriteAlarmBit
//����      :дĳһ����λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteAlarmBit(ALARM_BIT bit, FlagStatus state)
{
    if(bit < 32)
    {
        
        if(SET==state)
        {
            Alarm |= 1<<bit;
        }
        else if(RESET==state)
        {
            Alarm &= ~(1<<bit);
        }
    }
}
/*********************************************************************
//��������  :Io_WriteKeyAlarmBit
//����      :д�ؼ�������ĳһ����λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteKeyAlarmBit(ALARM_BIT bit, FlagStatus state)
{
    if(bit < 32)
    {
        
        if(SET==state)
        {
            KeyAlarm |= 1<<bit;
        }
        else if(RESET==state)
        {
            KeyAlarm &= ~(1<<bit);
        }
    }
}
/*********************************************************************
//��������  :Io_WriteSelfDefine2Bit
//����      :д�Զ����2��λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteSelfDefine2Bit(DEFINE_BIT bit, FlagStatus state)
{
    if(bit < 32)
    {
        
        if(SET==state)
        {
            SelfDefine2 |= 1<<bit;
        }
        else if(RESET==state)
        {
            SelfDefine2 &= ~(1<<bit);
        }
    }
}
/*********************************************************************
//��������  :Io_ReadOverSpeedConrtolBit
//����      :�����ٿ���λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
FlagStatus Io_ReadOverSpeedConrtolBit(OVER_SPEED_BIT bit)
{
    
    
    if(bit < OVER_SPEED_BIT_MAX)
    {
        if((1 << bit) == ((1 << bit)&OverSpeedControl))
        {
            return SET;
        }
        else
        {
            return RESET;
        }
    }
    else//����
    {
        return RESET;
    }
}
/*********************************************************************
//��������  :Io_WriteOverSpeedConrtolBit
//����      :д���ٿ���λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteOverSpeedConrtolBit(OVER_SPEED_BIT bit, FlagStatus state)
{
    if(bit < OVER_SPEED_BIT_MAX)
    {
        
        if(SET==state)
        {
            OverSpeedControl |= 1<<bit;
        }
        else if(RESET==state)
        {
            OverSpeedControl &= ~(1<<bit);
        }
    }
    if(0 == OverSpeedControl)
    {
        //������ٱ�־
        Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED, RESET);
    }
    else
    {
        //��λ���ٱ�־
        Io_WriteAlarmBit(ALARM_BIT_OVER_SPEED, SET);
    }
}
/*********************************************************************
//��������  :Io_ReadPreOvertimeConrtolBit
//����      :����ʱԤ������λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
FlagStatus Io_ReadPreOvertimeConrtolBit(PRE_OVER_TIME_BIT bit)
{
    
    
    if(bit < PRE_OVER_TIME_BIT_MAX)
    {
        if((1 << bit) == ((1 << bit)&PreOvertimeControl))
        {
            return SET;
        }
        else
        {
            return RESET;
        }
    }
    else//����
    {
        return RESET;
    }
}
/*********************************************************************
//��������  :Io_WritePreOvertimeConrtolBit
//����      :д��ʱԤ������λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WritePreOvertimeConrtolBit(PRE_OVER_TIME_BIT bit, FlagStatus state)
{
    if(bit < PRE_OVER_TIME_BIT_MAX)
    {
        
        if(SET==state)
        {
            PreOvertimeControl |= 1<<bit;
        }
        else if(RESET==state)
        {
            PreOvertimeControl &= ~(1<<bit);
        }
    }
    if(0 == PreOvertimeControl)
    {
        //������ٱ�־
        Io_WriteAlarmBit(ALARM_BIT_TIRED_DRIVE_PRE_ALARM, RESET);
    }
    else
    {
        //��λ���ٱ�־
        Io_WriteAlarmBit(ALARM_BIT_TIRED_DRIVE_PRE_ALARM, SET);
    }
}
/*********************************************************************
//��������  :Io_ReadOvertimeConrtolBit
//����      :����ʱ����λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
FlagStatus Io_ReadOvertimeConrtolBit(OVER_TIME_BIT bit)
{
    
    
    if(bit < OVER_TIME_BIT_MAX)
    {
        if((1 << bit) == ((1 << bit)&OvertimeControl))
        {
            return SET;
        }
        else
        {
            return RESET;
        }
    }
    else//����
    {
        return RESET;
    }
}
/*********************************************************************
//��������  :Io_WriteOvertimeConrtolBit
//����      :д��ʱ����λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteOvertimeConrtolBit(OVER_TIME_BIT bit, FlagStatus state)
{
    if(bit < OVER_TIME_BIT_MAX)
    {
        
        if(SET==state)
        {
            OvertimeControl |= 1<<bit;
        }
        else if(RESET==state)
        {
            OvertimeControl &= ~(1<<bit);
        }
    }
    if(0 == OvertimeControl)
    {
        //������ٱ�־
        Io_WriteAlarmBit(ALARM_BIT_TIRED_DRIVE, RESET);
    }
    else
    {
        //��λ���ٱ�־
        Io_WriteAlarmBit(ALARM_BIT_TIRED_DRIVE, SET);
    }
}
/*********************************************************************
//��������  :Io_ReadRecordConrtol
//����      :��¼�������ֽ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
u8 Io_ReadRecordConrtol(void)
{
    return RecordControl;
}
/*********************************************************************
//��������  :Io_WriteRecordConrtolBit
//����      :д¼������λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteRecordConrtolBit(RECORD_BIT bit, FlagStatus state)
{
    u8  CloseFlag;
    u8  OpenFlag;
    
    
    if(bit < RECORD_BIT_MAX)
    {
        
        if(SET==state)
        {
            RecordControl |= 1<<bit;
            OpenFlag = (1 << bit);
        }
        else if(RESET==state)
        {
            RecordControl &= ~(1<<bit);
            CloseFlag = ~(1 << bit);
        }
        if(0 == RecordControl)
        {
            //�ر�¼��
//          RecordTask_Close(); dxl,2015.9,
            //�ر�־д������
            FRAM_BufferWrite(FRAM_RECORD_CLOSE_ADDR, &CloseFlag, FRAM_RECORD_CLOSE_LEN);
        }
        else
        {
            if(SET == state)
            {
                //����¼��
//              RecordTask_Start(8000,60); dxl,2015.9,
                //����־д������
                FRAM_BufferWrite(FRAM_RECORD_OPEN_ADDR, &OpenFlag, FRAM_RECORD_OPEN_LEN);
            }
        }
        //��־д������
        FRAM_BufferWrite(FRAM_RECORD_CONTROL_ADDR, &RecordControl, FRAM_RECORD_CONTROL_LEN);
        
    }
    
}
/*********************************************************************
//��������  :Io_ReadRecordConrtolBit
//����      :��¼������λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
FlagStatus Io_ReadRecordConrtolBit(RECORD_BIT bit)
{
    
    
    if(bit < RECORD_BIT_MAX)
    {
        if((1 << bit) == ((1 << bit)&RecordControl))
        {
            return SET;
        }
        else
        {
            return RESET;
        }
    }
    else//����
    {
        return RESET;
    }
}
/*********************************************************************
//��������  :Io_WriteMicConrtolBit
//����      :дĳһ��˿���λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteMicConrtolBit(MIC_BIT bit, FlagStatus Status)
{
    if(bit < MIC_BIT_MAX)
    {
        
        if(SET==Status)
        {
            MicControl |= 1<<bit;
        }
        else if(RESET==Status)
        {
            MicControl &= ~(1<<bit);
        }
    }
    if(0 == MicControl)
    {
        MIC_BASE_OFF();//�ر�
    }
    else
    {
        MIC_BASE_ON();//��
    }
}
/*********************************************************************
//��������  :Io_WriteSpeakerConrtolBit
//����      :дĳһ����������λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteSpeakerConrtolBit(SPEAKER_BIT bit, FlagStatus state)
{
    if(bit < SPEAKER_BIT_MAX)
    {
        
        if(SET==state)
        {
            SpeakerControl |= 1<<bit;
        }
        else if(RESET==state)
        {
            SpeakerControl &= ~(1<<bit);
        }
    }
    if(0 == SpeakerControl)
    {
        SPEAKER_OFF();//�ر�
    }
    else
    {
        SPEAKER_ON();//��
    }
    
    
}

//********************�������֣��ڲ�����*******************
/*********************************************************************
//��������  :Io_DetectStatus
//����      :�������״̬λ��IO����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_DetectStatus(GPIO_OUT_IN_NUM IoNum, u8 index)
{
    u8  state;
    static u8   HighCount[32] = {0};
    static u8   LowCount[32] = {0};

    state = GpioInGetState(IoNum);
    if(1 == state)
    {
        HighCount[index]++;
        if(HighCount[index] >=DetectStatusHighDelay[index])
        {
            HighCount[index] = DetectStatusHighDelay[index];
        }
        LowCount[index] = 0;
    }
    else
    {
        HighCount[index] = 0;
        LowCount[index]++;
        if(LowCount[index] >=DetectStatusLowDelay[index])
        {
            LowCount[index] = DetectStatusLowDelay[index];
        }

    }
    if(HighCount[index] >=DetectStatusHighDelay[index])
    {
        state = (StatusHighValid & (1<< index))>> index;
        if(1==state)
        {
            Status |= 1<<index;
        }
        else
        {
            Status &= ~(1<<index);
        }
    }
    else if(LowCount[index] >=DetectStatusLowDelay[index])
    {
        state = (StatusHighValid & (1<< index))>> index;
        if(0==state)
        {
            Status |= 1<<index;
        }
        else
        {
            Status &= ~(1<<index);
        }

    }
		
		if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))
		{
		    if(1 == Io_ReadStatusBit(STATUS_BIT_ACC))
				{
				    Io_WriteIoStatusBit(IO_STATUS_BIT_SLEEP, RESET);  
				}
				else
				{
				    Io_WriteIoStatusBit(IO_STATUS_BIT_SLEEP, SET);  
				}
		}

}
/*********************************************************************
//��������  :Io_DetectExtCarStatus
//����      :���������չ����״̬�ֵ�IO����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_DetectExtCarStatus(GPIO_OUT_IN_NUM IoNum,u8 index)
{
    u8  state;
    static u8   HighCount[32] = {0};
    static u8   LowCount[32] = {0};

    state = GpioInGetState(IoNum);
    if(1 == state)
    {
        HighCount[index]++;
        if(HighCount[index] >=DetectExtCarStatusHighDelay[index])
        {
            HighCount[index] = DetectExtCarStatusHighDelay[index];
        }
        LowCount[index] = 0;
    }
    else
    {
        HighCount[index] = 0;
        LowCount[index]++;
        if(LowCount[index] >=DetectExtCarStatusLowDelay[index])
        {
            LowCount[index] = DetectExtCarStatusLowDelay[index];
        }

    }
    if(HighCount[index] >=DetectExtCarStatusHighDelay[index])
    {
        state = (ExtCarStatusHighValid & (1<< index))>> index;
        if(1==state)
        {
            ExtCarStatus |= 1<<index;
        }
        else
        {
            ExtCarStatus &= ~(1<<index);
        }
    }
    else if(LowCount[index] >=DetectExtCarStatusLowDelay[index])
    {
        state = (ExtCarStatusHighValid & (1<< index))>> index;
        if(0==state)
        {
            ExtCarStatus |= 1<<index;
        }
        else
        {
            ExtCarStatus &= ~(1<<index);
        }

    }

}

/*********************************************************************
//��������  :Io_DetectDefine2Status
//����      :��������Զ���״̬�ֵ�IO����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_DetectDefine2Status(GPIO_OUT_IN_NUM IoNum,u8 index)
{
    u8  state;
    static u8   HighCount[32] = {0};
    static u8   LowCount[32] = {0};

    state = GpioInGetState(IoNum);
    if(1 == state)
    {
        HighCount[index]++;
        if(HighCount[index] >=3)
        {
            HighCount[index] = 3;
        }
        LowCount[index] = 0;
    }
    else
    {
        HighCount[index] = 0;
        LowCount[index]++;
        if(LowCount[index] >=3)
        {
            LowCount[index] = 3;
        }

    }
    if(HighCount[index] >=3)
    {
        state = (SelfDefine2HighValid & (1<< index))>> index;
        if(1==state)
        {
            SelfDefine2 |= 1<<index;
        }
        else
        {
            SelfDefine2 &= ~(1<<index);
        }
    }
    else if(LowCount[index] >=3)
    {
        state = (SelfDefine2HighValid & (1<< index))>> index;
        if(0==state)
        {
            SelfDefine2 |= 1<<index;
        }
        else
        {
            SelfDefine2 &= ~(1<<index);
        }

    }

}

/*********************************************************************
//��������  :Io_DetectAlarm
//����      :��ⱨ��IO��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_DetectAlarm(GPIO_OUT_IN_NUM IoNum, u8 index)
{
    u8  state;
    u8  flag;
    static u8   HighCount[32] = {0};
    static u8   LowCount[32] = {0};

    state = GpioInGetState(IoNum);
    if(1 == state)
    {
        HighCount[index]++;
        if(HighCount[index] >=DetectAlarmHighDelay[index])
        {
            HighCount[index] = DetectAlarmHighDelay[index];
        }
        LowCount[index] = 0;
    }
    else
    {
        HighCount[index] = 0;
        LowCount[index]++;
        if(LowCount[index] >=DetectAlarmLowDelay[index])
        {
            LowCount[index] = DetectAlarmLowDelay[index];
        }

    }
    if(HighCount[index] >=DetectAlarmHighDelay[index])
    {
        state = (AlarmHighValid & (1<< index))>> index;
        if(1==state)
        {
            Alarm |= 1<<index;
        }
        else
        {
            if(ALARM_BIT_EMERGENCY == index)//��������״̬�ɼ���������
            {
                
            }
            else
            {
                Alarm &= ~(1<<index);
            }
        }
    }
    else if(LowCount[index] >=DetectAlarmLowDelay[index])
    {
        state = (AlarmHighValid & (1<< index))>> index;
        if(0==state)
        {
            Alarm |= 1<<index;
            if(ALARM_BIT_EMERGENCY == index)//��������״̬�ɼ���������
            {
                flag = 1;
                FRAM_BufferWrite(FRAM_EMERGENCY_FLAG_ADDR, &flag, FRAM_EMERGENCY_FLAG_LEN);
            }
            
        }
        else
        {
            if(ALARM_BIT_EMERGENCY == index)//��������״̬�ɼ���������
            {
                
            }
            else
            {
                Alarm &= ~(1<<index);
            }
        }

    }

}
/*********************************************************************
//��������  :Io_DetectFrontDoor
//����      :���ǰ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :120ms����һ��
*********************************************************************/
void Io_DetectFrontDoor(void)
{
    u8  state;
    static u8   HighCountP = 0;
    static u8   LowCountP = 0;
    static u8   HighCountN = 0;
    static u8   LowCountN = 0;
    
    if(1 == DoorSelectFlag)//�ߵ�ƽ��Ч
    {
        //���DOOR+
        state = GpioInGetState (DOORI_P);
        if(1 == state)
        {
            HighCountP++;
            if(HighCountP >=3)
            {
                HighCountP = 3;
            }
            LowCountP = 0;
        }
        else
        {
            HighCountP = 0;
            LowCountP++;
            if(LowCountP >=3)
            {
                LowCountP = 3;
            }
        }
        if(HighCountP >=3)
        {
            Status |= 1<<STATUS_BIT_DOOR1;
        }
        else if(LowCountP >=3)
        {
            Status &= ~(1<<STATUS_BIT_DOOR1);
        }
    }
    else if(0 == DoorSelectFlag)
    {
        //���DOOR-
        state = GpioInGetState (DOORI_N);
        if(1 == state)
        {
            HighCountN++;
            if(HighCountN >=3)
            {
                HighCountN = 3;
            }
            LowCountN = 0;
        }
        else
        {
            HighCountN = 0;
            LowCountN++;
            if(LowCountN >=3)
            {
                LowCountN = 3;
            }

        }
        if(HighCountN >=3)
        {
            Status &= ~(1<<STATUS_BIT_DOOR1);
        }
        else if(LowCountN >=3)
        {   
            Status |= 1<<STATUS_BIT_DOOR1;
        }
    
    }
    else
    {
        Status &= ~(1<<STATUS_BIT_DOOR1);//����ѡ�����,����ʼ��Ϊ��
    }
    
        
}
/*********************************************************************
//��������  :Io_DetectPhoneKey
//����      :���һ��ͨ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :�͵�ƽ��Ч
*********************************************************************/
u8 Io_DetectPhoneKey(void)
{
    u8  state;

    static u8   HighCount = 0;
    static u8   LowCount = 0;
    
    
    state = GpioInGetState (VIRBRATIONI);
    if(1 == state)
    {
        HighCount++;
        if(HighCount >=3)
        {
            HighCount = 3;
        }
        LowCount = 0;
    }
    else
    {
        HighCount = 0;
        LowCount++;
        if(LowCount >=3)
        {
            LowCount = 3;
        }

    }
    if(LowCount >=3)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    
}
/*********************************************************************
//��������  :Io_DetectMainPower
//����      :��������ѹ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :100ms����һ��
*********************************************************************/
void Io_DetectMainPower(void)
{
    static u8   LowAlarmCount = 0;
    static u8   NoLowAlarmCount = 0;
    static u8   ShutAlarmCount = 0;
    static u8   NoShutAlarmCount = 0;
    static u32  AdSum = 0;//ADת��ֵ�ۼƺ�
    static u16  AdMin = 0xffff;//ADת��ֵ��Сֵ
    static u16  AdMax = 0;//ADת��ֵ���ֵ
    static u8   AdCount = 0;//ADת������
    u16 Ad;
        u8     AlarmCountMax;
        u8     noAlarmCountMax;
        u8     AdCountMax;

        AlarmCountMax = 15;//���ܹ淶Ҫ����15�룬����ͳһ�ĳ����
        noAlarmCountMax = 15;
        //AlarmCountMax = 4;//ʵ��Ӧ��ʱΪ30��,10��,�������Ų��Ծ�Ϊ5��
        //noAlarmCountMax = 4;
        //AlarmCountMax = 3;//�������ʱʹ��
        //noAlarmCountMax = 1;
        AdCountMax = 10;
        
    AdCount++;
    Ad = Ad_GetValue(ADC_MAIN_POWER); 
    if(AdMin > Ad)
    {
        AdMin = Ad;
    }
    if(AdMax < Ad)
    {
        AdMax = Ad;
    }
    AdSum += Ad;
    if(AdCount >=  AdCountMax)
    {
        //����ƽ��ֵ
        AdSum -= AdMin;
        AdSum -= AdMax;
        AdSum = AdSum >> 3;
        //ת����0.1VΪ��λ��ֵ
        AdSum = AdSum*33*9/0xfff;
        //��λ�����Ƿѹ����
        if((AdSum < PowerLowMaxValue)&&(AdSum > PowerShutValue))
        {
            LowAlarmCount++;
            if(LowAlarmCount >=AlarmCountMax)//ʵ��Ӧ��ȡֵ
            {
                    LowAlarmCount = AlarmCountMax;
            }
            NoLowAlarmCount = 0;
        }
        else
        {
            LowAlarmCount = 0;
            NoLowAlarmCount++;
            if(NoLowAlarmCount >=noAlarmCountMax)//ʵ��Ӧ��ȡֵ
            {
                NoLowAlarmCount = noAlarmCountMax;
            }
        }
        if(LowAlarmCount >=AlarmCountMax)
        {
            Io_WriteAlarmBit(ALARM_BIT_POWER_LOW, SET);//����Ƿѹ��λ
            Io_WriteAlarmBit(ALARM_BIT_POWER_SHUT, RESET);//����������
        }
        else if(NoLowAlarmCount >=noAlarmCountMax)
        {
            Io_WriteAlarmBit(ALARM_BIT_POWER_LOW, RESET);//����Ƿѹ���
        }
        //��λ������ϵ籨��
        if(AdSum < PowerShutValue)
        {
            ShutAlarmCount++;
            if(ShutAlarmCount >=AlarmCountMax)
            {
                ShutAlarmCount = AlarmCountMax;
            }
            NoShutAlarmCount = 0;
        }
        else
        {
            ShutAlarmCount = 0;
            NoShutAlarmCount++;
            if(NoShutAlarmCount >=noAlarmCountMax)
            {
                NoShutAlarmCount = noAlarmCountMax;
            }
        }
        if(ShutAlarmCount >=AlarmCountMax)
        {
            Io_WriteAlarmBit(ALARM_BIT_POWER_SHUT, SET);//���������λ
            Io_WriteAlarmBit(ALARM_BIT_POWER_LOW, RESET);//����Ƿѹ���
        }
        else if(NoShutAlarmCount >=noAlarmCountMax)
        {
            Io_WriteAlarmBit(ALARM_BIT_POWER_SHUT, RESET);//����������
        }
        //������ʼ��
        AdCount = 0;
        AdMin = 0xffff;
        AdMax = 0;
        AdSum = 0;  
    }
    
}
/*********************************************************************
//��������  :Io_DetectLoad
//����      :�����س�״̬�����س�����������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ֻ���������汾����,��ADC1���,100ms����һ��
//              :�ճ���3.2V~4.5V
//              :�س���2.0V~3.2V
//              :��������·��0V~2.0V
//              :��������·��4.5V~
*********************************************************************/
void Io_DetectLoad(void)
{
    static u8   AlarmCount = 0;
        static u8   UnAlarmCount = 0;
        static u8       StatusCount = 0;
        static u8       UnStatusCount = 0;
    static u32  AdSum = 0;//ADת��ֵ�ۼƺ�
    static u16  AdMin = 0xffff;//ADת��ֵ��Сֵ
    static u16  AdMax = 0;//ADת��ֵ���ֵ
    static u8   AdCount = 0;//ADת������
        
       // static u16      LoadSensorEmptyMinValue = 32;//������Сֵ
       // static u16      LoadSensorEmptyMaxValue = 45;//�������ֵ
    u16 Ad;
        u8     AlarmCountMax = 3;
        u8     UnAlarmCountMax = 3;
        u8     AdCountMax = 10;
        u8     StatusCountMax = 3;
        u8     UnStatusCountMax = 3;
        
    AdCount++;
    Ad = Ad_GetValue(ADC_EXTERN1); 
    if(AdMin > Ad)
    {
        AdMin = Ad;
    }
    if(AdMax < Ad)
    {
        AdMax = Ad;
    }
    AdSum += Ad;
    if(AdCount >=  AdCountMax)
    {
        //����ƽ��ֵ
        AdSum -= AdMin;
        AdSum -= AdMax;
        AdSum = AdSum >> 3;
        //ת����0.1VΪ��λ��ֵ
        AdSum = AdSum*33*9/0xfff;
        //��λ��������س�����������
        if((AdSum < LOAD_SENSOR_SHORT_VALUE)||(AdSum > LOAD_SENSOR_SHUT_VALUE))
        {
            AlarmCount++;
            if(AlarmCount >=AlarmCountMax)//ʵ��Ӧ��ȡֵ
            {
                    AlarmCount = AlarmCountMax;
            }
            UnAlarmCount = 0;
        }
        else
        {
                AlarmCount = 0;
            UnAlarmCount++;
            if(UnAlarmCount >= UnAlarmCountMax)//ʵ��Ӧ��ȡֵ
            {
                UnAlarmCount = UnAlarmCountMax;
            }
                        
                              //��λ��������س�״̬
                if((AdSum > LOAD_SENSOR_FULL_MIN_VALUE)&&(AdSum < LOAD_SENSOR_FULL_MAX_VALUE))
                {
                  StatusCount++;
                  if(StatusCount >=StatusCountMax)//ʵ��Ӧ��ȡֵ
                  {
                          StatusCount = StatusCountMax;
                  }
                  UnStatusCount = 0;
                }
                else
                {
                      StatusCount = 0;
                  UnStatusCount++;
                  if(UnStatusCount >=UnStatusCountMax)//ʵ��Ӧ��ȡֵ
                  {
                    UnStatusCount = UnStatusCountMax;
                  }
                }
                if(StatusCount ==StatusCountMax)
                {
                  //��λ���س�
                              Io_WriteStatusBit(STATUS_BIT_FULL_LOAD1,SET);
                              Io_WriteStatusBit(STATUS_BIT_FULL_LOAD2,SET);
                        
                }
                else if(UnStatusCount == UnStatusCountMax)
                {
                //������س�
                            Io_WriteStatusBit(STATUS_BIT_FULL_LOAD1,RESET);
                            Io_WriteStatusBit(STATUS_BIT_FULL_LOAD2,RESET);
                }
        }
        if(AlarmCount ==AlarmCountMax)
        {
            //��λ���س�����������
                        Io_WriteSelfDefine2Bit(DEFINE_BIT_25, SET);
                        
        }
        else if(UnAlarmCount == UnAlarmCountMax)
        {
            //������س�����������
                    Io_WriteSelfDefine2Bit(DEFINE_BIT_25, RESET);
        }
                
          
    
        //������ʼ��
        AdCount = 0;
        AdMin = 0xffff;
        AdMax = 0;
        AdSum = 0;  
    }
    
}

/*********************************************************************
//��������  :Io_PowerOnDetectMainPower
//����      :�ϵ��������ѹ,�ж���12V���绹��24V����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_PowerOnDetectMainPower(void)
{
    
    u32 AdSum = 0;//ADת��ֵ�ۼƺ�
    u16 AdMin = 0xffff;//ADת��ֵ��Сֵ
    u16 AdMax = 0;//ADת��ֵ���ֵ
    u8  Count = 0;//ADת������
    u16 Ad;

    for(Count = 0; Count<10; Count++)
    {
        Ad = Ad_GetValue(ADC_MAIN_POWER); 
        if(AdMin > Ad)
        {
            AdMin = Ad;
        }
        if(AdMax < Ad)
        {
            AdMax = Ad;
        }
        AdSum += Ad;
    }
    if(Count >= 10)
    {
        //����ƽ��ֵ
        AdSum -= AdMin;
        AdSum -= AdMax;
        AdSum = AdSum >> 3;
        //ת����0.1VΪ��λ��ֵ
        AdSum = AdSum*33*9/0xfff;
        //if(AdSum >= 180)//���ڵ���18V��Ϊ��24Vϵͳ,������12Vϵͳ,dxl,2014.1.15����,��180V�ĳ�21V
        //if(AdSum > 210)//,dxl,2014.1.15
        if(AdSum > 190)//,dxl,2014.6.24,��21V��Ϊ24Vϵͳ���ж�ֵ����bug,��Ϊ19V
        {
            CarVoltageType = 1;
            //PowerLowMinValue = 210;//Ƿѹֵ��Χ
            PowerLowMaxValue = 220;
            PWR_24V();
        }
        else
        {
            CarVoltageType = 0;
            //PowerLowMinValue = 105;//Ƿѹֵ��Χ
            PowerLowMaxValue = 110;
            PWR_12V();
        }
        
    }
    
}
/*********************************************************************
//��������  :Io_WriteStatusHighValidBit
//����      :дStatusHighValidĳһ״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteStatusHighValidBit(STATUS_BIT bit, FlagStatus state)
{
    if(bit < 32)
    {
        if(SET==state)
        {
            StatusHighValid |= 1<<bit;
        }
        else if(RESET==state)
        {
            StatusHighValid &= ~(1<<bit);
        }
    
    }
}
/*********************************************************************
//��������  :Io_WriteExtCarStatusHighValidBit
//����      :дExtCarStatusHighValidĳһ״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteExtCarStatusHighValidBit(CAR_STATUS_BIT bit, FlagStatus state)
{
    if(bit < 32)
    {
        if(SET==state)
        {
            ExtCarStatusHighValid |= 1<<bit;
        }
        else if(RESET==state)
        {
            ExtCarStatusHighValid &= ~(1<<bit);
        }
    
    }
}
/*********************************************************************
//��������  :Io_WriteSelfDefine2HighValidBit
//����      :дSelfDefine2HighValidĳһ״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteSelfDefine2HighValidBit(DEFINE_BIT bit, FlagStatus state)
{
    if(bit < 32)
    {
        if(SET==state)
        {
            SelfDefine2HighValid |= 1<<bit;
        }
        else if(RESET==state)
        {
            SelfDefine2HighValid &= ~(1<<bit);
        }
    
    }
}
/*********************************************************************
//��������  :Io_WriteAlarmHighValidBit
//����      :дAlarmHighValidĳһ״̬λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ע���벻Ҫ���жϺ����ڵ��ô˺���
*********************************************************************/
void Io_WriteAlarmHighValidBit(ALARM_BIT bit, FlagStatus state)
{
    if(bit < 32)
    {
        if(SET==state)
        {
            AlarmHighValid |= 1<<bit;
        }
        else if(RESET==state)
        {
            AlarmHighValid &= ~(1<<bit);
        }
    
    }
}
/*********************************************************************
//��������  :Io_GetCarVoltageType
//����      :��ȡ������ѹ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
u8 Io_GetCarVoltageType(void)
{
    return CarVoltageType;
}
/*********************************************************************
//��������  :Io_ClearAlarmWord
//����      :��������ָ�λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_ClearAlarmWord(void)
{
    u8 flag = 0;
    Alarm = 0;
    FRAM_BufferWrite(FRAM_EMERGENCY_FLAG_ADDR, &flag, FRAM_EMERGENCY_FLAG_LEN);
}
/*********************************************************************
//��������  :Io_ClearStatusWord
//����      :���״̬�ָ�λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_ClearStatusWord(void)
{
    Status = 0;
}
/*********************************************************************
//��������  :Io_ClearExtCarStatusWord
//����      :�����չ����״̬�ָ�λ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void Io_ClearExtCarStatusWord(void)
{
    ExtCarStatus = 0;
}