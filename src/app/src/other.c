/********************************************************************
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :Other.c        
//����      :��ɢ�ĺ���
//�汾��    :
//������    :dxl
//����ʱ��  :2012.4 
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :
***********************************************************************/
//***************�����ļ�*****************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
#include "VDR.h"
#include "AreaManage.h"

extern u16     DelayAckCommand;//��ʱӦ������
extern u8   DelaySendChannel;//��ʱ����ͨ����
extern u8   DelaySmsPhone[];//��ʱ���Ͷ��ź���
extern u8      MultiMediaSendFlag;//��ý�巢�ͱ�־��0Ϊ���ڷ��ͣ�1Ϊ�ڷ���
extern u16     CarControlAckSerialNum;//��������Ӧ����ˮ��
extern  TIME_TASK MyTimerTask[];
extern u32     PhotoID;//ͼ���ý��ID
extern u8      MediaType;//��ý�����ͣ�0ͼ��1��Ƶ��2��Ƶ
extern u8      MediaFormat;//��ý���ʽ����,0JPEG,1TIF,2MP3,3WAV,4WMV
extern u8   MediaRatio;//��ý��ֱ���
extern u8      MediaChannelID;//��ý��ͨ��ID
extern u8      MediaEvenCode;//��ý���¼�����
extern u16     MediaCommand;//��ý������  0:ֹͣ  N:����  0XFFFF:¼�� 0XFFFE:һֱ����  BY WYF
extern u16     MediaPhotoSpace; //���ռ��ʱ�� BY WYF
extern u8   MemoryUploadFileName[];//�洢��ý�������ϴ��������ļ���
extern u8   CurrentWavFolder;//��ǰ¼���ļ���
extern CAMERA_STRUCT    Camera[];//����ͷ�ṹ��
extern u16  RecorderSerialNum;//��ʻ��¼��Ӧ����ˮ��
extern u32 TestBlindSaveCount;
extern u8      RecordSendFlag;//¼���ļ��ϴ���־,1Ϊ�ϴ�
extern u8 OneHourDelayTimeCountEnableFlag;
extern u32 OneHourDelayTimeCount;
extern u8  UpdataResultNoteAckFlag;//�������֪ͨӦ���־,1Ϊ�յ�ƽ̨��ͨ��Ӧ����
extern u8   TerminalAuthorizationFlag;//�ն˼�Ȩ��־,bit0:����1;bit1:����2;bit2~bit7����;
extern u8 CarControlFlag;//�������Ʊ�־
//ÿ���ն˼�Ȩ�ɹ������Ӧλ����1,����Ͽ�����Ӧλ����0
u8  ResetEnableFlag = 0;//����ʹ�ܱ�־
u8  Link1LoginCount = 0;//ע������ʹ�����ÿ��30�룬3�β��ɹ�����Ͽ����磬10���Ӻ�����
u8  Link1AuthorizationCount = 0;//��Ȩ����ʹ�����ÿ��30�룬10�β��ɹ���������Ȩ�룬����ע������
u8  Link1LoginAuthorizationState = 0;//��ȡ��Ȩ�룬����ע��������ͼ�Ȩ����Ͽ����磬��ʱ���������硢�ر����񼸸�״̬
u8  Link1LoginAuthorizationLastState = 0;//��һ��״̬
u16 Link1LoginAuthorizationDelayTime = 0;//ע���Ȩ��ʱʱ��
u8  Link2LoginCount = 0;//ע������ʹ�����ÿ��30�룬3�β��ɹ�����Ͽ����磬10���Ӻ�����
u8  Link2AuthorizationCount = 0;//��Ȩ����ʹ�����ÿ��30�룬10�β��ɹ���������Ȩ�룬����ע������
u8  Link2LoginAuthorizationState = 0;//��ȡ��Ȩ�룬����ע��������ͼ�Ȩ����Ͽ����磬��ʱ���������硢�ر����񼸸�״̬
u8  Link2LoginAuthorizationLastState = 0;//��һ��״̬
u16 Link2LoginAuthorizationDelayTime = 0;//ע���Ȩ��ʱʱ��
static u16  Link1LoginAuthorizationDelayCount = 0;//��ʱ������1���Ӽ���1
static u16  Link2LoginAuthorizationDelayCount = 0;//��ʱ������1���Ӽ���1
u8   Link1ManualCloseFlag = 0;//����1�ֶ��رձ�־,1Ϊ�ر�,0Ϊ����
u8   Link2ManualCloseFlag = 0;//����2�ֶ��رձ�־,1Ϊ�ر�,0Ϊ����
u8  GBTestFlag;//0:������������ģʽ;1:������ģʽ
u8  BBXYTestFlag = 0;//0Ϊ����ģʽ��1Ϊ����Э����ģʽ
u8  BBGNTestFlag = 0;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
u8  HYTestFlag = 0;//0Ϊ����ģʽ��1Ϊ����ƽ̨���ģʽ
u8  XYTestMachine1Flag = 0;//0Ϊ����ģʽ��1ΪЭ����1�Ż�ģʽ������+GPS�ٶ�
u8  XYTestMachine2Flag = 0;//0Ϊ����ģʽ��1ΪЭ����2�Ż�ģʽ����GPS�ٶ�
u8  XYTestMachine3Flag = 0;//0Ϊ����ģʽ��1ΪЭ����3�Ż�ģʽ���������ٶ� 
//***************���ر���******************
static u8   BeepOnEv = 0;//���������е��¼�,bit0,�յ�������Ϣ;bit1,����;bit2,ƣ�ͼ�ʻ;bit3,δ��¼
static u8   CostDownVersionFlag = 0;//dxl,2014.4.29,���ɱ���־��1Ϊ���ɱ��汾��0Ϊ���ɱ�֮ǰ�İ汾���ñ�־�漰��ʾ������ӡ����USB����˷����ؿ���
//****************ȫ�ֱ���*****************
u8      Ftp_Updata_BusyFlg;//ftp ����æ��־   0������   1�������� myh 130808
u8  SpeedFlag = 0x03;//�ٶ�����,0Ϊ����,1ΪGPS,2�Զ�����,3�Զ�GPS
u8  AccOffGpsControlFlag = 0;//ACC OFFʱGPSģ����Ʊ�־,0Ϊ��,1Ϊ��
u8  EvaluatorControlFlag = 0;//���������ܿ���,0Ϊ��,1Ϊ��
u8  TaximeterControlFlag = 0;//�Ƽ������ܿ���,0Ϊ��,1Ϊ��
u8  ToplightControlFlag = 0;//���ƹ��ܿ���,0Ϊ��,1Ϊ��
u8  LedlightControlFlag = 0;//LED��湦�ܿ���,0Ϊ��,1Ϊ��
u8      MultiCenterLinkFlag = 0;//���������ӱ�׼,1Ϊ������Ҫ�����ӱ��ݷ�����
u8  DelayBuffer[100];//��ʱ���壬�ɰ�Ҫ��ʱ���͵����ݿ��������ע����󳤶�����
u8  DelayBufferLen;//��ʱ�������ݳ���
u8  GanSuOilBuffer[100];//���ո�����������ͺĴ����������ݻ���
u8  GanSuOilBufferLen = 0;
u8  GanSuOilBufferBusyFlag = 0;//æ��־,1Ϊæ,��ʾ��ֹ����д������,0Ϊ��,��ʾ����д������
u32 ResetTimeCount = 0;//��λ�����������յ�ƽ̨Ӧ�����0������1���ۼ�1���ﵽ6Сʱ��λ����
//RadioProtocol_MultiMediaEvenReport(channel,PhotoID+1, MediaType, MediaFormat, MediaEvenCode, MediaChannelID);
u32 PhotoID_0800;//ר������0800��ý���¼��ϱ�
u8 MediaType_0800;
u8 MediaFormat_0800;
u8 MediaEvenCode_0800;
u8 MediaChannelID_0800;
u8 MediaEventSendChannel_0800;
u8 PhotoMediaEvUploadFlag = 0;
static u16 Peripheral1Type = 0;//����1���ͣ���Ӧ����ID E2_PRAM_BASE_CUSTOM_ID+0x80����E2_LED_PORT_ID
static u16 Peripheral2Type = 0;//����2���ͣ���Ӧ����ID E2_PRAM_BASE_CUSTOM_ID+0x81����E2_POS_PORT_ID
static u8 MultiCenterConnectRequestFlag = 0;//���������������־��1��ʾ��ǰ�ڲ��Զ���������
static u8 MultiCenterConnectFlag = 0;//���������ӱ�־��1��ʾ��ǰ�ڲ��Զ���������
static u8 AccelerationAckBuffer[20];
static u8 AccelerationAckBufferLen;
//****************�궨��******************
/*********************************************************************
//��������  :SetTerminalFirmwareVersion
//����      :�����ն˹̼��汾��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void SetTerminalFirmwareVersion(void)
{
	  u8 Buffer[30];
	  u8 BufferLen;
	
    if(1 == BBGNTestFlag)
		{
		    BufferLen = FRAM_BufferRead(Buffer, FRAM_PACKET_UPDATE_RESULT_LEN, FRAM_PACKET_UPDATE_RESULT_ADDR);
			  if((BufferLen == FRAM_PACKET_UPDATE_RESULT_LEN)&&(1 == Buffer[0]))
				{
				    EepromPram_WritePram(E2_FIRMWARE_VERSION_ID, (u8 *)BBTEST_FIRMWARE_VERSION, E2_FIRMWARE_VERSION_ID_LEN);
				}
		}
		else
		{
		    BufferLen = EepromPram_ReadPram(E2_FIRMWARE_VERSION_ID, Buffer);
		    if(BufferLen > 0)
		    {
		        if(0 != strncmp((const char *)Buffer,(const char *)FIRMWARE_VERSION,5))
				    {
				        EepromPram_WritePram(E2_FIRMWARE_VERSION_ID, (u8 *)FIRMWARE_VERSION, E2_FIRMWARE_VERSION_ID_LEN);
				    }
		    }
		    else
		    {
            EepromPram_WritePram(E2_FIRMWARE_VERSION_ID, (u8 *)FIRMWARE_VERSION, E2_FIRMWARE_VERSION_ID_LEN);
		    }
		}
}
/*********************************************************************
//��������  :UpdateRunMode
//����      :��������ģʽ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :bit7:Ϊ1�ǹ�����ģʽ
//��ע      :bit6:Ϊ1�ǲ���Э����ģʽ����ΪЭ����ʱ���������Ǽ��Ż�
//��ע      :bit5:Ϊ1�ǲ��깦�ܼ��ģʽ
//��ע      :bit4:Ϊ1��ȫ������ƽ̨���ģʽ
//��ע      :bit3:Ϊ1�ǲ���Э����3�Ż����ģʽ
//��ע      :bit2:Ϊ1�ǲ���Э����2�Ż����ģʽ
//��ע      :bit1:Ϊ1�ǲ���Э����1�Ż����ģʽ
//��ע      :bit0:Ϊ1�ǿ�����ʻʱ��ʻԱδǩ������
*********************************************************************/
void UpdateRunMode(void)
{
      u8  Buffer[5];
      u8  BufferLen;
    BufferLen = EepromPram_ReadPram(E2_UNLOGIN_TTS_PLAY_ID, Buffer);//��ʻԱδ��¼��ʾ����ʹ�ܿ���,bit7:1������ģʽ;bit6:1������ģʽ
    if(E2_UNLOGIN_TTS_PLAY_ID_LEN == BufferLen)
    {
        if(0x80 == (Buffer[0]&0x80))
          {
              GBTestFlag = 1;
          }
          else
          {
              GBTestFlag = 0;
          } 
                
                if(0x40 == (Buffer[0]&0x40))
          {
              BBXYTestFlag = 1;
          }
          else
          {
              BBXYTestFlag = 0;
          }
                
                if(0x20 == (Buffer[0]&0x20))
          {
              BBGNTestFlag = 1;
          }
          else
          {
              BBGNTestFlag = 0;
          }
                
          if(0x10 == (Buffer[0]&0x10))
          {
              HYTestFlag = 1;
          }
          else
          {
              HYTestFlag = 0;
          }
					
					if(0x08 == (Buffer[0]&0x08))
          {
              XYTestMachine3Flag = 1;
          }
          else
          {
              XYTestMachine3Flag = 0;
          }
					
					if(0x04 == (Buffer[0]&0x04))
          {
              XYTestMachine2Flag = 1;
          }
          else
          {
              XYTestMachine2Flag = 0;
          }
					
					if(0x02 == (Buffer[0]&0x02))
          {
              XYTestMachine1Flag = 1;
          }
          else
          {
              XYTestMachine1Flag = 0;
          }
					
    }
    else
    {
        GBTestFlag = 0;
              BBXYTestFlag = 0;
              BBGNTestFlag = 0;
              HYTestFlag = 0;
    }
        
     
}
/*********************************************************************
//��������  :UpdatePeripheralType
//����      :���±���Peripheral1Type,Peripheral2Type
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :1.����������1���ͻ�����2���Ͳ���ʱ��Ҫ���øýӿ�
//              :2.�ϵ�������ʼ��ʱ����øýӿڣ�����
*********************************************************************/
void UpdatePeripheralType(void)
{
        u8  Buffer[3];
    u8  BufferLen;
        
    BufferLen = EepromPram_ReadPram(E2_LED_PORT_ID, Buffer);
        if(E2_LED_PORT_ID_LEN == BufferLen)
        {
                Peripheral1Type = Public_ConvertBufferToShort(Buffer); 
        }
        else
        {
                Peripheral1Type = 0;
        }
        
        BufferLen = EepromPram_ReadPram(E2_POS_PORT_ID, Buffer);
        if(E2_POS_PORT_ID_LEN == BufferLen)
        {
                Peripheral2Type = Public_ConvertBufferToShort(Buffer); 
        }
        else
        {
                Peripheral2Type = 0;
        }     
}
/*********************************************************************
//��������  :ReadPeripheral1TypeBit
//����      :������1���͵�ĳһλ
//����      :�ڼ�λ��ȡֵΪ0~15
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :1:��ʾ�����豻������;0:��ʾ������δ������
//��ע      :����ֵ����15����ֵΪ2����ʾ���� 
*********************************************************************/
u8 ReadPeripheral1TypeBit(u8 Bit)
{
        if(Bit > 15)
        {
                return 2;
        }
        
        return PUBLIC_CHECKBIT_EX(Peripheral1Type,Bit);
}
/*********************************************************************
//��������  :ReadPeripheral2TypeBit
//����      :������1���͵�ĳһλ
//����      :�ڼ�λ��ȡֵΪ0~15
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :1:��ʾ�����豻������;0:��ʾ������δ������
//��ע      :����ֵ����15����ֵΪ2����ʾ���� 
*********************************************************************/
u8 ReadPeripheral2TypeBit(u8 Bit)
{
        if(Bit > 15)
        {
                return 2;
        }
        
        return PUBLIC_CHECKBIT_EX(Peripheral2Type,Bit);
}
/*********************************************************************
//��������  :UpdataAccOffGpsControlFlag(void)
//����      :���±���AccOffGpsControlFlag
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void UpdataAccOffGpsControlFlag(void)
{
        u8  Buffer[4];
        u8  PramLen;
        PramLen = EepromPram_ReadPram(E2_GPS_CONTROL_ID, Buffer);
        if(E2_GPS_CONTROL_LEN == PramLen)
        {
            if(0xff == Buffer[0])//û�����ù�����,Ĭ��Ϊ��
            {
                Buffer[0] = 0;
            }
            AccOffGpsControlFlag = Buffer[0];
        }
        else
        {
            AccOffGpsControlFlag = 0;
        }
}
/*********************************************************************
//��������  :UpdataEvaluatorControlFlag(void)
//����      :���±���EvaluatorControlFlag
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void UpdataEvaluatorControlFlag(void)
{
    u8  Buffer[4];
    u8  PramLen;
    PramLen = EepromPram_ReadPram(E2_USB_UPDATA_FLAG_ID, Buffer);
        if(E2_USB_UPDATA_FLAG_ID_LEN == PramLen)
        {
            EvaluatorControlFlag = Buffer[0];
        }
}
/*********************************************************************
//��������  :UpdataTaximeterControlFlag(void)
//����      :���±���TaximeterControlFlag
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void UpdataTaximeterControlFlag(void)
{
    u8  Buffer[4];
    u8  PramLen;
    PramLen = EepromPram_ReadPram(E2_TAXIMETER_CONTROL_ID, Buffer);
        if(E2_TAXIMETER_CONTROL_LEN == PramLen)
        {
            TaximeterControlFlag = Buffer[0];
        }
}
/*********************************************************************
//��������  :UpdataToplightControlFlag(void)
//����      :���±���ToplightControlFlag
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void UpdataToplightControlFlag(void)
{
    u8  Buffer[4];
    u8  PramLen;
    PramLen = EepromPram_ReadPram(E2_TOPLIGHT_CONTROL_ID, Buffer);
        if(E2_TOPLIGHT_CONTROL_ID_LEN == PramLen)
        {
            ToplightControlFlag = Buffer[0];
        }
}
/*********************************************************************
//��������  :UpdataLedlightControlFlag(void)
//����      :���±���LedlightControlFlag
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void UpdataLedlightControlFlag(void)
{
    u8  Buffer[4];
    u8  PramLen;
    PramLen = EepromPram_ReadPram(E2_LEDLIGHT_CONTROL_ID, Buffer);
        if(E2_LEDLIGHT_CONTROL_ID_LEN == PramLen)
        {
            LedlightControlFlag = Buffer[0];
        }
}
//**************��������*******************
/*********************************************************************
//��������  :EvRing
//����      :�����绰����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void EvRing(void)
{
  /*dxl,2014.8.8�����蹤�ύ�޸�
#ifdef TTS_M12
        M12TTSPlayStr("����������!",M12_TTS_RE_NO);
#else
    PlayMulTTSVoiceStr("����������!");
#endif
  */
  Public_PlayTTSVoiceStr("����������!");
}
  
  
/*********************************************************************
//��������  :EvRingOver
//����      :�رյ绰����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void EvRingOver(void)
{
#ifdef TTS_M12
        //M12TTSPlayStr(" ",M12_TTS_RE_NO);
#else
    //PlayMulTTSVoiceStr(" "); 
#endif
}
/*********************************************************************
//��������  :SpeedFlagUpdatePram(void)
//����      :����SpeedFlag����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void SpeedFlagUpdatePram(void)
{
    u8  PramLen;
    //u8    Buffer[1] = {1};
    u8  i;
    
    for(i=0; i<100; i++)
    {
        //��ʱһ���
    }
    //��ȡ�ٶ�ѡ��ֵ
    PramLen = EepromPram_ReadPram(E2_SPEED_SELECT_ID, &SpeedFlag);
        if(1 == PramLen)
        {
    
          //�����������
          if(0 == (SpeedFlag&0x01))//dxl,2015.5.11
          {
                    VDRPulse_UpdateParameter();
          }
              else
              {
                      //ΪGPS�ٶ�
                      GpsMile_UpdatePram();
                  MileMeter_ClearOldPoint();
              }
        }
        else
        {
        //����ΪGPS�ٶ�
                SpeedFlag = 0x03;//ΪGPS�ٶ�//dxl,2015.5.11
                GpsMile_UpdatePram();
            MileMeter_ClearOldPoint();
        }
//        SpeedMonitor_UpdateSpeedType(); dxl,2015.9,
}
/*********************************************************************
//��������  :SetBeepOnEvBit(u8 bit)
//����      :���ñ���BeepOnEv�е�ĳһλ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void SetBeepOnEvBit(u8 bit)
{
    u8  i;
    u8  temp;
    u8  count;
    
    temp = bit;
    count = 0;
    for(i=0; i<8; i++)
    {
        if(0x01 == ((temp >> i)&0x01))
        {
            count++;
        }
    }
    
    if(1 == count)
    {
        BeepOnEv |= bit;
    }
}
/*******************************************************************************
* Function Name  : SetLink1LoginAuthorizationState
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetLink1LoginAuthorizationState(u8 State)
{
    Link1LoginCount = 0;
    Link1AuthorizationCount = 0;
    Link1LoginAuthorizationLastState = 0;
    Link1LoginAuthorizationState = State;
        
}
/*******************************************************************************
* Function Name  : SetLink2LoginAuthorizationState
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetLink2LoginAuthorizationState(u8 State)
{
    Link2LoginCount = 0;
    Link2AuthorizationCount = 0;
    Link2LoginAuthorizationLastState = 0;
    Link2LoginAuthorizationState = State;
        
}
/*******************************************************************************
* Function Name  : EvGetAkeyTask
* Description    : ��ȡ��Ȩ��--��SMS����ͨ�� .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GetAkey_EvTask(void)
{   
    u8 channel = CHANNEL_DATA_1;//Ϊ����ͨ�������
    //���ͻ�ȡ��Ȩ�����
    RadioProtocol_TerminalRegister(channel);
}
/*******************************************************************************
* Function Name  : EvChangeServerTask
* Description    : �յ�ָ����л������ƽ̨
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ChangeServer_EvTask(void)
{
    /*
    //�ж�ƽ̨�������Ƿ���ȷ
    if (MOD_GetNetPara(&gNet, THI_IP) == true){
        gNet.lnkParam.bits.lnkIndex = THI_IP;  //���ƽ̨
        gNet.thirdLnkCnt = 3; //�������3�� 
        MOD_StateSwitch(MOD_DOWNGPRS);
    }
    */
}
/*******************************************************************************
* Function Name  : EvFtpUpdataTask
* Description    : ����FTP�����ļ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FtpUpdata_EvTask(void)
{

}
/*******************************************************************************
* Function Name  : EvDiallingTask
* Description    : һ��ͨ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Dialling_EvTask(void)
{

}
/*******************************************************************************
* Function Name  : EvSevDialTask
* Description    : �绰�ز�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SevDial_EvTask(void)
{
    #ifdef USE_PHONE
    Phone_SetDialEvTask();
    #else
    u8 ucTmp[30] = {0}; //����洢
    u8 ucFlg = 0;           //ͨ����־
    u8 len = 0;
    
    len = EepromPram_ReadPram(E2_PHONE_CALLBACK_ID, ucTmp);
    if (len != 0) {
        ucTmp[len] = '\0';
        EepromPram_ReadPram(E2_CALLBACK_FLAG_ID, &ucFlg);
        if (0 == ucFlg) {
            if (0 == communication_CallPhone(ucTmp, PHONE_NORMAL))
                //��ǰ״̬������ͨ������ʱ�ȴ�
                SetEvTask(EV_SEV_CALL);//communication_CallPhone(ucTmp, PHONE_NORMAL);
        } else {
            if (0 == communication_CallPhone(ucTmp, PHONE_MONITOR))
                //��ǰ״̬������ͨ������ʱ�ȴ�
                SetEvTask(EV_SEV_CALL);//communication_CallPhone(ucTmp, PHONE_NORMAL);
        }
    }
    #endif
}
/*********************************************************************
//��������  :Updata_SendResultNoteTimeTask
//����      :�����������֪ͨ��ƽ̨
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :Ĭ���ǿ�����,1�����1��
*********************************************************************/
FunctionalState  Updata_SendResultNoteTimeTask(void)
{
    static u8 State = 0;
    static u8 AckResult = 0;
    static u8 UpdataType = 0;
    static u8 count1 = 0;
    static u8 count2 = 0;
    
    u8  channel = CHANNEL_DATA_1;
    u8  Buffer[3] = {0};
    u8  BufferLen = 0;
    
    if(0 == State)//��ȡ�����Ϣ
    {
        //���������
        BufferLen = FRAM_BufferRead(Buffer, FRAM_FIRMWARE_UPDATA_FLAG_LEN, FRAM_FIRMWARE_UPDATA_FLAG_ADDR);
        if(0 == BufferLen)
        {
            return DISABLE;
        }
        else if((1 == Buffer[0])||(3 == Buffer[0]))//1:��ʾƽ̨�·���Զ������ָ��,2:�����ɹ�,3:����ʧ��
        {
            AckResult = 1;
        }
        else if(2 == Buffer[0])
        {
            AckResult = 0;
        } 
        else
        {
            return DISABLE;
        }
        //��ȡ��������
        BufferLen = FRAM_BufferRead(Buffer, FRAM_FIRMWARE_UPDATA_TYPE_LEN, FRAM_FIRMWARE_UPDATA_TYPE_ADDR);
        if(0 == BufferLen)
        {
            return DISABLE;
        }
        else if((0 == Buffer[0])||(0x09 == Buffer[0]))
        {
                  State = 1;//������һ��
                  count1 = 0;
                  count2 = 0;
                  UpdataType = Buffer[0];
        }
        else
        {
            return DISABLE;
        }
  
    }
    else if(1 == State)//��������ʧ�ܽ��֪ͨ��ƽ̨
    {
         //��ȡͨ����,��ǰĬ��ΪCHANNEL_DATA_1
         RadioProtocol_UpdataResultNote(channel,UpdataType,AckResult);
         State = 2;
    }
    else if(2 == State)//���Ӧ��
    {
        if(1 == UpdataResultNoteAckFlag)
        {
            UpdataResultNoteAckFlag = 0;
            State = 0;
            Buffer[0] = 0;
            FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_FLAG_ADDR,Buffer,1);//д�������´ξͲ��ᷢ�ͽ��֪ͨ��
            return DISABLE;
        }
        else
        {
            count1++;
            if(count1 >= 10)//��������3��û��Ӧ��Ļ��Ͳ�������
            {
                count1 = 0;
                State = 1;
                count2++;
                if(count2 > 3)
                {
                    count2 = 0;
                    Buffer[0] = 0;
                    FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_FLAG_ADDR,Buffer,1);//д�������´ξͲ��ᷢ�ͽ��֪ͨ��
                    return DISABLE;
                }
            }
                       
        }
    }
    return ENABLE;
}
/*********************************************************************
//��������  :Reset_TimeTask
//����      :ϵͳ��ʱ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
/*
FunctionalState Reset_TimeTask(void)
{
    u8 ACC;
    
    if(1 == ResetEnableFlag)
    {
        NVIC_SystemReset();
    }
    //��ACC״̬
    ACC = Io_ReadStatusBit(STATUS_BIT_ACC);
    if(0 == ACC)    //�ر�
    {
        NVIC_SystemReset();
        return DISABLE;
    }
    else    //����
    {
        SetTimerTask(TIME_SYSRESET, 60*SECOND);
        return ENABLE;
    }
}
*/
FunctionalState Reset_TimeTask(void)
{
       // u8      Acc = 0;
    //u8 flag;
        //u8 speed;
    //static u32 count = 0;
        /*
        count++;//1���1
        if(count >= (24*3600))//��24Сʱ
        {
               // flag = Report_GetBlindStatusFlag();//��ä����־,1Ϊä��,0Ϊ����,�ж�ä���ķ����ǣ��ɹ�����1��λ����Ϣ��ʼ��ʱ��
                                                  //��3�����ϱ�ʱ������û���յ��κ�ƽ̨�·���ָ����Ϊ��ä��,һ����ȡ��ä����־��Ϊ1λ�û㱨��Ϣֱ�Ӵ���flash,���ϱ���
                 flag = communicatio_GetMudulState(COMMUNICATE_STATE);                              //�費��Ҫ�޶��ϱ���ʱ�����������ĳ��ֵ,��3������
                if(1 == flag)
                {
                        speed = Gps_ReadSpeed();//��gps�ٶ�,��gps�ٶ�ͨ��һЩ
                        if(0 == speed)
                        {
                                NVIC_SystemReset();//ֻ������ʱ����24Сʱ,����ä��״̬,�ٶ�Ϊ0��������ʱ������
                        }
                }
                else
                {
                        count = 0;
                }
        }
        */
        //Acc = Io_ReadStatusBit(STATUS_BIT_ACC);//����ACC�����ƣ��ڹ�����ʱֻҪһֱ��ACC�����������ģ��Ͳ�����ּ������и�λ
        ResetTimeCount++;
        if(ResetTimeCount >= 3600*50)
        {
                ResetTimeCount = 0;
                NVIC_SystemReset();
        }
    if(1 == ResetEnableFlag)//��ĳЩ��������»�Ҫ����������,����ͨ����λResetEnableFlag��ʵ��
    {
        NVIC_SystemReset();
    }
        
    return ENABLE;
}
/*******************************************************************************
* Function Name  : DelayTrigTimeTask(void)
* Description    : ��ʱ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState DelayTrigTimeTask(void)
{
        u8      flag;
        u16     length;
        u8      Buffer[FLASH_BLIND_STEP_LEN+1];
        u8      Len;
        u8  Dir[15];
        UINT      Byte;
        u8  FileName[_MAX_LFN];
        u32     MediaID;
        FIL file;
        MESSAGE_HEAD head;
        u16 SerialNum;
        u8 i;
        u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;
	   
        
        if(0 == DelayAckCommand)
        {
                return DISABLE;
        }

        flag = 0;
        switch(DelayAckCommand)
        {
                case 0:
                {
                        break;
                }
                case 0x8300:
                {
                        RadioProtocol_TerminalGeneralAck(channel,0);
                        break;
                }
                case 0x0800://�Ϸ���ý���¼�
                {
										    RadioProtocol_MultiMediaEvenReport(MediaEventSendChannel_0800,PhotoID_0800, MediaType_0800, MediaFormat_0800, MediaEvenCode_0800, MediaChannelID_0800);
											  break;
                }
								case 0x0900:
								{
												 RadioProtocol_OriginalDataUpTrans(channel,0xFD,AccelerationAckBuffer,AccelerationAckBufferLen);
												 break;
								}
                case 0x0500://0x0500��������
                {
									      if(0x01 == (CarControlFlag & 0x01))
                        {
                             //�������ż�������
                             Io_WriteStatusBit(STATUS_BIT_DOOR_LOCK,SET);     
                        }
                        else
                        {
                             //�������Ž�������
                             Io_WriteStatusBit(STATUS_BIT_DOOR_LOCK,RESET);
                        }
                        //Ӧ����ˮ��
                        length = 0;
                        Buffer[0] = (CarControlAckSerialNum & 0xff00) >> 8;
                        Buffer[1] = CarControlAckSerialNum & 0xff;
                        length += 2;
                        //��ȡλ����Ϣ,������Buffer��
                        Len = Report_GetPositionInfo(Buffer+2);
                        //if(PramLen > 28)
                        //{
                               //PramLen = 28;
                        //}
                        length += Len;
                        //����λ�û㱨��Ϣ
                    //��ϢID
                        head.MessageID = 0x0500;
                        head.Attribute = length;
                        //����
                        if(0 != RadioProtocol_SendCmd(DelaySendChannel,&head, Buffer, length, DelaySmsPhone))
                        {
                            flag = 0;
                        }
                        break;
                }
        case 0x0003://0x0003,�ն�ע��
        {
            //TerminalAuthorizationFlag = 0;dxl,2014.11.26�յ�ע��Ӧ���ŰѼ�Ȩ��־��0
            break;
        }
				case 0x0200://0x0200,λ����Ϣ�ϱ�
        {
            Report_UploadPositionInfo(CHANNEL_DATA_1);
            break;
        }
        case 2:
        {
					   if((1 == BBGNTestFlag)||(1 == BBXYTestFlag))
						 {
						 
						 }
						 else
						 {
                 Blind_Erase(0xff);//����ǰ����һ��ä��flash myh 130808
							   ClrTimerTask(TIME_POSITION);//���������йر�λ����Ϣ�ϱ���ä�� myh 130808��ԭ���Ƿ�ֹ��ͬһ��flash����д
                 ClrTimerTask(TIME_BLIND);
						 }
             
             Ftp_Updata_BusyFlg = 1;//������æ��־ myh 130808
             // ����������������
             SetEvTask(EV_FTP_UPDATA);
             break;
        }
        case 4:
        {
            //�л�Ϊ�ϵ�״̬��֮��ͨѶģ���ڲ�����������,����Ĭ�ϵ���������
            break;
        }
        case 5:
        {
            //����
             NVIC_SystemReset();
            break;
        }
        case 6:
                {
            // �������Ӽ��ƽ̨����
            SetEvTask(EV_LINK_GOV_SERVER);
                        break;
                }
                case 7:
                {
                        MOD_CloseMainIp();//ģ�������Ͽ�����
                        DelayAckCommand = 5;//��һ������
                        flag = 1;
                        break;
                }
                case 8:
                {
                         //��Communication_TimeTask����
                        //ClrTimerTask(TIME_COMMUNICATION);ʵ��Ӧ��ʱ���ر�����
                        //�ȹر�ģ���Դ
                        GSM_PWR_OFF(); 
                        break;
                }
                case 9:
                {
                        MOD_CloseMainIp();//ģ�������Ͽ�����
                        DelayAckCommand = 10;//��һ���ǹر�ͨ��ģ������
                        //������ʱ����
                        SetTimerTask(TIME_DELAY_TRIG, 5*SECOND);
                        TerminalAuthorizationFlag = 0;
                        flag = 1;
                        break;
                }
                case 10:
                {
                         //��Communication_TimeTask����
                        //ClrTimerTask(TIME_COMMUNICATION);ʵ��Ӧ��ʱ���ر�����
                        //�ȹر�ģ���Դ
                        GSM_PWR_OFF(); 
#ifdef HUOYUN_DEBUG_OPEN
                        OneHourDelayTimeCountEnableFlag = 1;//ʱ�����ʹ��,����������������������ﵽ1Сʱ������
                        OneHourDelayTimeCount = 0;
#else
                          //10���Ӻ�����
                        ResetEnableFlag = 1;
                        SetTimerTask(TIME_SYSRESET, SYSTICK_10MINUTE);
#endif
                      
                        
                     
                        break;
                }
                case 11:
                {
                        MOD_CloseMainIp();//ģ�������Ͽ�����
                        DelayAckCommand = 12;//��һ���ǹػ�
                        //������ʱ����
                        SetTimerTask(TIME_DELAY_TRIG, 5*SECOND);
                        TerminalAuthorizationFlag = 0;
                        flag = 1;
                        break;
                }
                case 12:
                {
                        SleepDeep_DisposeTerminalShutdown();//ACC ON����
                        break;
                }
                case 13:
                {
                          if(ACK_OK == RadioProtocol_OriginalDataUpTrans(channel,0x41, DelayBuffer, DelayBufferLen))
                          {
                          
                          }
                          else
                          {
                                flag = 1;
                          }
                          break;
                }
								case 15:
                {
                        Area_CloseCollectGNSS();
                        break;
                }
                case 0x8801://���յ���������ָ���������Ӧ��ָ��,ֻӦ��1��ͼƬ
                {
									     
                        //Ӧ����ˮ��
                        Buffer[0] = (RecorderSerialNum&0xff00)>>8;
                        Buffer[1] = RecorderSerialNum;
                        channel = RadioProtocol_GetDownloadCmdChannel(0x8801, &SerialNum);
                        if(0 == MediaChannelID)
                        {
                            for(i=1; i<=CAMERA_ID_MAX; i++)
                            {
                                if(1 == Camera[i-1].OnOffFlag)
                                {
                                    break;
                                }
                            }
                            if(i == (CAMERA_ID_MAX+1))
                            {
                                Buffer[2] = 2;//ͨ����֧��
                                //��ý��ID����
                                Buffer[3] = 0;//Ĭ��Ϊ1
                                Buffer[4] = 1;
                                //��ý��ID
                                Buffer[5] = ((PhotoID+1)&0xff000000) >> 24;//ע��PhotoID+1������ʾPhotoID++������ʹ��PhotoID++
                                Buffer[6] = ((PhotoID+1)&0xff0000) >> 16;//����Ӧ��Ķ�ý��ID��Ҫ���ϴ���ID����һ�£������Ҫ++,dxl,2016.5.13
                                Buffer[7] = ((PhotoID+1)&0xff00) >> 8;
                                Buffer[8] = (PhotoID+1)&0xff;
                                RadioProtocol_PhotoAck(channel,Buffer,9);
                            }
                            else
                            {
                                Buffer[2] = 0;//�ɹ�
                                //��ý��ID����
                                Buffer[3] = 0;//Ĭ��Ϊ1
                                Buffer[4] = 1;
                                //��ý��ID
                                Buffer[5] = ((PhotoID+1)&0xff000000) >> 24;
                                Buffer[6] = ((PhotoID+1)&0xff0000) >> 16;
                                Buffer[7] = ((PhotoID+1)&0xff00) >> 8;
                                Buffer[8] = (PhotoID+1)&0xff;
                                RadioProtocol_PhotoAck(channel,Buffer,9);
                            }
                              
                        }
                        else
                        {
                            if(1 == Camera[MediaChannelID-1].OnOffFlag)
                            {
                                Buffer[2] = 0;//�ɹ�
                                //��ý��ID����
                                Buffer[3] = 0;//Ĭ��Ϊ1
                                Buffer[4] = 1;
                                //��ý��ID
                                Buffer[5] = ((PhotoID+1)&0xff000000) >> 24;
                                Buffer[6] = ((PhotoID+1)&0xff0000) >> 16;
                                Buffer[7] = ((PhotoID+1)&0xff00) >> 8;
                                Buffer[8] = (PhotoID+1)&0xff;
                                RadioProtocol_PhotoAck(channel,Buffer,9);   
                            }
                            else
                            {
                                Buffer[2] = 2;//ͨ����֧��
                                //��ý��ID����
                                Buffer[3] = 0;//Ĭ��Ϊ1
                                Buffer[4] = 1;
                                //��ý��ID
                                Buffer[5] = ((PhotoID+1)&0xff000000) >> 24;
                                Buffer[6] = ((PhotoID+1)&0xff0000) >> 16;
                                Buffer[7] = ((PhotoID+1)&0xff00) >> 8;
                                Buffer[8] = (PhotoID+1)&0xff;
                                RadioProtocol_PhotoAck(channel,Buffer,9);
                            }
                        }
												if(1 == PhotoMediaEvUploadFlag)//ͼ��ʵʱ�ϴ�ʱ����Ҫ�����Ϸ���ý���¼���ͼ�񱣴�ʱ�ڼ���ʱ�Ϸ���ý���¼�
												{
												    flag = 1;//dxl,2016.5.16���Ҫ����һ�����Ͷ�ý���¼��ϴ�
												    DelayAckCommand = 0x0800;
												   SetTimerTask(TIME_DELAY_TRIG, 10);
                        }
												
                     break;
                }
                case 0x8804://¼����ɺ�������һ��¼��(1������)�ϴ����������,�ù��ܽ���Ϊ�б걱���������
                {
                        //����Ƿ��������ϴ�״̬
                    if((0 == MultiMediaSendFlag)&&(0 == strlen((char const *)MemoryUploadFileName)))
                    {
                        strcpy((char *)Dir,"1:/WAV");
                            if(CurrentWavFolder < 10)
                            {
                                Dir[6] = CurrentWavFolder+0x30;
                                Dir[7] = 0;
                            }
                            else
                            {
                                Dir[6] = CurrentWavFolder/10+0x30;
                                Dir[7] = CurrentWavFolder%10+0x30;
                                Dir[8] = 0;
                            }
                            //д��־�ĵ�log.bin
                            strcpy((char *)FileName,(char const *)Dir);
                            length = strlen((char const *)FileName);
                            memcpy(FileName+length,"/log.bin",8);
                            FileName[length+8] = 0;
                                Buffer[0] = 0;
                                MediaID = 0;
                            if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ))
                            {
                                        if(file.fsize >= 9)
                                        {
                                        f_lseek(&file,file.fsize-9);
                                        if(FR_OK == f_read (&file, Buffer, 9, &Byte))
                                                {
                                                        MediaID = 0;
                                                        MediaID |= Buffer[4] << 24;
                                                        MediaID |= Buffer[5] << 16;
                                                        MediaID |= Buffer[6] << 8;
                                                        MediaID |= Buffer[7];
                                                }
                                        }
                                f_close(&file);
                            }
                                if(0 != MediaID)
                                {
                                        Buffer[0] = 0;
                                        MediaSearch_GetMediaIDFileName(Buffer,MEDIA_TYPE_WAV, MediaID);
                                        if(strlen((char const *)Buffer))
                                        {
                                                strcpy((char *)MemoryUploadFileName,(char const *)Buffer);
                                                //������ý�������ϴ�����
                                SetTimerTask(TIME_SEND_MEDIA, 10*SYSTICK_0p1SECOND);
                                        }
                                }
                        }
                }
                default :break;
        }

        if(0 == flag)
        {
                DelayAckCommand  = 0;
                return DISABLE;
        }
        else
        {
                return ENABLE;
        }
}
/*********************************************************************
//��������  :GetTerminalAuthorizationFlag(void)
//����      :��ȡ��Ȩ��־��ֵ
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :bit0:����;bit1:CHANNEL_DATA_1;bit2:CHANNEL_DATA_2;bit3~bit7:����,
/           :��ӦλΪ1��ʾ��Ӧͨ����Ȩ�ɹ�,����Ϊʧ��
*********************************************************************/
u8 GetTerminalAuthorizationFlag(void)
{
    return  TerminalAuthorizationFlag;
}
/*********************************************************************
//��������  :SetTerminalAuthorizationFlag(void)
//����      :��λĳ��ͨ���ļ�Ȩ��־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :bit0:��ȱ��Ԥ��;bit1:��ʾCHANNEL_DATA_1��bit2:CHANNEL_DATA_2
//          :channel��ֵ����Ϊ��������ͨ��ֵ���������ǵ����
*********************************************************************/
void SetTerminalAuthorizationFlag(u8 channel)
{
    if(CHANNEL_DATA_1 == (channel&CHANNEL_DATA_1))
    {
        TerminalAuthorizationFlag = TerminalAuthorizationFlag | CHANNEL_DATA_1;
    }
    
    if(CHANNEL_DATA_2 == (channel&CHANNEL_DATA_2))
    {
        TerminalAuthorizationFlag = TerminalAuthorizationFlag | CHANNEL_DATA_2;
    }
    
}
/*********************************************************************
//��������  :ClearTerminalAuthorizationFlag(u8 channel)
//����      :���ĳ��ͨ���ļ�Ȩ��־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :bit0:��ȱ��Ԥ��;bit1:��ʾCHANNEL_DATA_1��bit2:CHANNEL_DATA_2
//          :channel��ֵ����Ϊ��������ͨ��ֵ���������ǵ����
*********************************************************************/
void ClearTerminalAuthorizationFlag(u8 channel)
{
    if(CHANNEL_DATA_1 == (channel&CHANNEL_DATA_1))
    {
        TerminalAuthorizationFlag = TerminalAuthorizationFlag & 0xfd;
    }
    if(CHANNEL_DATA_2 == (channel&CHANNEL_DATA_2))
    {
        TerminalAuthorizationFlag = TerminalAuthorizationFlag & 0xfb;
    }
}
/*********************************************************************
//��������  :ClearTerminalAuthorizationCode(u8 channel)
//����      :���ĳ��ͨ���ļ�Ȩ��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :bit0:��ȱ��Ԥ��;bit1:��ʾCHANNEL_DATA_1��bit2:CHANNEL_DATA_2
//          :channel��ֵ����Ϊ��������ͨ��ֵ���������ǵ����
*********************************************************************/
void ClearTerminalAuthorizationCode(u8 channel)
{
    u8  i;
    
    if(CHANNEL_DATA_1 == (channel&CHANNEL_DATA_1))
    {
         for(i=0; i<20; i++)
         {
            E2prom_WriteByte(E2_LOGINGPRS_PASSWORD_ID_ADDR+i,0xff);
         }
    }
   if(CHANNEL_DATA_2 == (channel&CHANNEL_DATA_2))
    {
         for(i=0; i<20; i++)
         {
            E2prom_WriteByte(E2_SECOND_LOGINGPRS_PASSWORD_ID_ADDR+i,0xff);
         }
    }
}
/*******************************************************************************
* Function Name  : MultiCenterConnect_TimeTask(void)
* Description    : ���������Ӷ�ʱ����,�յ����ñ��÷���������ʱ����,1���ӵ���1��
* Input          : None
* Output         : None
* Return         : ����Ϊ�б걱���������
*******************************************************************************/
FunctionalState MultiCenterConnect_TimeTask(void)
{
    static u8 state = 0;
	  static u32 count = 0;
	  static u32 TcpResendNum = 0;//TCP��Ϣ�ش���������Ӧ����ID 0x0003
	  static u32 TcpAckOverTime = 0;//TCP��ϢӦ��ʱʱ�䣬��Ӧ����ID 0x0002
	  static u32 TcpResendCount = 0;//��������
	  static u32 TcpAckTimeCount = 0;//ʱ�����
	  
	  u8 Buffer[30];
	  u8 BufferLen;
	  u8  tab[50];
    u8  len;
	//  u8  flag;
//	  u32 temp;
//	  SOC_IPVAL ip_value;
	
	  if(0 == MultiCenterConnect_GetConnectFlag())
		{
		   return DISABLE;
		}
		
	  switch(state)
		{
			case 0://��ʼ��
			{
				  BufferLen = EepromPram_ReadPram(E2_TCP_ACK_OVERTIME_ID, Buffer);
          if(E2_TCP_ACK_OVERTIME_LEN == BufferLen)
          {
              TcpAckOverTime = Public_ConvertBufferToLong(Buffer);
          }
					else
					{
					    TcpAckOverTime = 20;
					}
					BufferLen = EepromPram_ReadPram(E2_TCP_RESEND_TIMES_ID, Buffer);
          if(E2_TCP_RESEND_TIMES_LEN == BufferLen)
          {
              TcpResendNum = Public_ConvertBufferToLong(Buffer);
          }
					else
					{
					    TcpResendNum = 2;
					}
					TcpResendCount = 0;
					TcpAckTimeCount = 0;
					count = 0;
				  state = 1;
					Public_ShowTextInfo("���������ӹ��ܿ���",100);
			    break;
			}
			case 1://�ȴ��Ͽ�����
			{ 
				  if(1 == Modem_State_SocSta(0))
					{
					    count++;
						  if(1 == count)
							{
							   Public_ShowTextInfo("�ȴ��Ͽ�����",100); 
							}
					}
					else
					{
						  state = 2;
						  count = 0;
						  MultiCenterConnect_SetRequestFlag();
						  ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);
						  Public_ShowTextInfo("�Ͽ�����",100);
					}
			    break;
			}
			case 2://�ȴ���������
			{
				  if(1 == Modem_State_SocSta(0))
					{
					     Public_ShowTextInfo("����������",100);
						   state = 3;
					}
			    break;
			}
			case 3://����3�μ�Ȩ
			{
				  if(TcpResendCount <= TcpResendNum)
					{
						  count = 0;
					    TcpAckTimeCount++;
						  if(TcpAckTimeCount >= (TcpAckOverTime*TcpResendCount*SECOND))
							{
								  if(ACK_OK == RadioProtocol_TerminalQualify(CHANNEL_DATA_1))
									{
										  TcpAckTimeCount = 0;
									    TcpResendCount++;
										  Public_ShowTextInfo("���ͼ�Ȩ",100);
									}
							}
					}
					else
					{
						  count++;
						  if(count >= (2*SECOND))
							{
								  count = 0;
							    state = 4;
							}
					    
					}
			    break;
			}
			case 4://����ָ���ķ����������ݷ�������
			{
				  if(1 == BBGNTestFlag)
					{
					    len = EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID, tab);
	            if((len > 0)&&(len < E2_BACKUP_SERVER_IP_LEN))
							{
							    EepromPram_WritePram(E2_MAIN_SERVER_IP_ID, tab, len); 
								  BufferLen = EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID,Buffer);
								  if(BufferLen == len)
									{
									    if(0 == strncmp((char *)Buffer,(char *)tab,BufferLen))
											{
												  count = 0;
												  state = 5;
												  MultiCenterConnectRequestFlag = 0;
												  Communication_Init();
												  Public_ShowTextInfo("���ӱ��ݷ�����...",100);
											}
											else
											{
											    Public_ShowTextInfo("��������IPд�����",100);
											}
									}
									else
									{
									    Public_ShowTextInfo("��������IPд�����",100);
									}
							}
					}
					/*
					else
					{
				      flag = 0;
              memset(tab,0,50);
              len = EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID, tab);
	            if((len > 0)&&(len < E2_BACKUP_SERVER_IP_LEN))
		          {
                  strcpy((char *)ip_value.ip,(char*)tab);
		          }
		          else
		          {
					        flag = 1;
			            Public_ShowTextInfo("���ݷ�����IP����",100);
		          }
	            memset(tab,0,50);
              len = EepromPram_ReadPram(E2_MAIN_SERVER_TCP_PORT_ID, tab);
              if(4 == len)
              {
                  temp = Public_ConvertBufferToLong(tab);
                  sprintf((char*)ip_value.port,"%d",temp);
              }
		          else
		          {
					        flag = 1;
			            Public_ShowTextInfo("��������TCP����",100);
		          }
              strcpy((char *)&ip_value.mode,"TCP");
		          if(0 == flag)
				      {
						      count = 0;
						      state = 5;
                  Net_First_ConTo(ip_value, 20);//ǿ������ʱ��Ϊ20����
						      MultiCenterConnectRequestFlag = 0;
				      }
				  }
					*/
			    break;
			}
			default:
			{
				  count++;
				  if(count >= (2*SECOND))
					{
					    count = 0;
						  state = 0;
						  MultiCenterConnectRequestFlag = 0;
						  MultiCenterConnectFlag = 0;
						  Public_ShowTextInfo("�˳�����������",100);
						  return DISABLE;
					}
			    break;
			}
		}
		
		return ENABLE;
}
/*******************************************************************************
* Function Name  : MultiCenterConnect_GetRequestFlag(void)
* Description    : ��ȡ���������������־
* Input          : None
* Output         : None
* Return         : ����1��ʾ��ǰΪ���������ӣ�����0��ʾ��ǰΪ����Ӧ�õ�����
*******************************************************************************/
u8 MultiCenterConnect_GetRequestFlag(void)
{
    return MultiCenterConnectRequestFlag;
}
/*******************************************************************************
* Function Name  : MultiCenterConnect_SetRequestFlag(void)
* Description    :��λ���������������־
* Input          : None
* Output         : None
* Return         : ��
*******************************************************************************/
void MultiCenterConnect_SetRequestFlag(void)
{
    MultiCenterConnectRequestFlag = 1;
}
/*******************************************************************************
* Function Name  : MultiCenterConnect_GetConnectFlag(void)
* Description    : ��ȡ���������ӱ�־
* Input          : None
* Output         : None
* Return         : ����1��ʾ��ǰΪ���������ӣ�����0��ʾ��ǰΪ����Ӧ�õ�����
*******************************************************************************/
u8 MultiCenterConnect_GetConnectFlag(void)
{
    return MultiCenterConnectFlag;
}
/*******************************************************************************
* Function Name  : MultiCenterConnect_SetFlag(void)
* Description    :��λ���������ӱ�־
* Input          : None
* Output         : None
* Return         : ��
*******************************************************************************/
void MultiCenterConnect_SetFlag(void)
{
    MultiCenterConnectFlag = 1;
}
/*******************************************************************************
* Function Name  : MultiCenterConnect_ResetFlag(void)
* Description    :������������ӱ�־
* Input          : None
* Output         : None
* Return         : ��
*******************************************************************************/
void MultiCenterConnect_ResetFlag(void)
{
    MultiCenterConnectFlag = 0;
}
/*******************************************************************************
* Function Name  : MultiCenterConnect_ResetRequestFlag(void)
* Description    :������������������־
* Input          : None
* Output         : None
* Return         : ��
*******************************************************************************/
void MultiCenterConnect_ResetRequestFlag(void)
{
    MultiCenterConnectRequestFlag = 0;
}
//Ϊ���벻���������ӣ���ʵ�ʸ�ģ��ʹ��Ϊ׼����ʵ��ʹ��ʱ�ö������ֱ��ȥ�� dxl
void TexiService_PlayTTSVoiceStr(u8 *StrPtr)
{

}
/********************************************************************
* ���� : Print_RTC
* ���� : ��ȡ��ǰ��RTCʱ��,��ӡ���
********************************************************************/
void Print_RTC( void )
{
  
  TIME_T nowTime;
  RTC_GetCurTime(&nowTime);
  LOG_PR( "%02d:%02d:%02d%t ;", nowTime.hour,nowTime.min,nowTime.sec );
 
}
/********************************************************************
* ���� : SetCostDownVersionFlag
* ���� : ����CostDown�汾��־λ����λΪ1��ʾPCBΪ���ɱ��汾��Ϊ0��ʾ֮ǰ�İ汾
********************************************************************/
void SetCostDownVersionFlag( void )
{
   CostDownVersionFlag = 1; 
}
/********************************************************************
* ���� : ClearCostDownVersionFlag
* ���� : ���CostDown�汾��־λ����λΪ1��ʾPCBΪ���ɱ��汾��Ϊ0��ʾ֮ǰ�İ汾
********************************************************************/
void ClearCostDownVersionFlag( void )
{
   CostDownVersionFlag = 0; 
}
/********************************************************************
* ���� : GetCostDownVersionFlag
* ���� : ��ȡCostDown�汾��־λ����λΪ1��ʾPCBΪ���ɱ��汾��Ϊ0��ʾ֮ǰ�İ汾
********************************************************************/
u8 GetCostDownVersionFlag( void )
{
   return CostDownVersionFlag; 
}
/********************************************************************
* ���� : GansuOiL_OilProtocolCheck
* ���� : ��������ͺ�Э����,�ͺĴ�����30�뷢��һ�����ݸ�GPS�ն�
********************************************************************/
u8 GanSuOil_OilProtocolCheck(u8 *pBuffer, u8 length)
{
   
    if(((length > 70)||(1 == GanSuOilBufferBusyFlag))&&(length > 2))
    {
        return 0;
    }
    if(('S'==*pBuffer)
       &&('t' == *(pBuffer+1))
         &&('E' == *(pBuffer+length-2))
           &&('d' == *(pBuffer+length-1)))
    {
        memcpy(GanSuOilBuffer,pBuffer,length);  
        GanSuOilBufferLen = length;
    }
    return length;
}
/********************************************************************
* ���� : GanSuOiL_GetOnePacketData
* ���� : ��ȡһ���ͺ����ݣ�λ�û㱨ʱ���øú���
********************************************************************/
u8 GanSuOil_GetOnePacketData(u8 *pBuffer)
{
    if((GanSuOilBufferLen > 70)&&(0 == GanSuOilBufferLen))//���ȳ���
    {
        return 0;
    }
  
    memcpy(pBuffer,GanSuOilBuffer,GanSuOilBufferLen);  
    return GanSuOilBufferLen;
}
/********************************************************************
* ���� : GanSuOiL_GetCurPacketLen
* ���� : ��ȡ��ǰ�ͺ����ݰ�����
********************************************************************/
u8 GanSuOil_GetCurPacketLen(void)
{
    return GanSuOilBufferLen;
}

/********************************************************************
* ?? : Acceleration_DisposeRadioProtocol
* ?? : ??????????
********************************************************************/
void Acceleration_DisposeRadioProtocol(u8 cmd, u8 *pBuffer, u16 length)
{
    u8 *p = NULL;
    
    if(0x02 == cmd)//������
    {
        p = pBuffer;
        if((7 == length)&&(1 == *(p+2)))//д
        {
            AccelerationAckBuffer[0] = 0x01;
            AccelerationAckBuffer[1] = 0x01;
            AccelerationAckBuffer[2] = 0xF7;
            AccelerationAckBuffer[3] = 0x02;
            AccelerationAckBuffer[4] = 0x00;					
            AccelerationAckBuffer[5] = 0x03;					
            memcpy(AccelerationAckBuffer+6,p,3);
            AccelerationAckBufferLen = 9;
            FRAM_BufferWrite(FRAM_ACCELERATION_ADDR,p+3,4);
            Set_Emer_Threshold(p+3,0);//zengliang add 2016-11-21
					
					CleanInitFlag();
        }
        else if((3 == length)&&(0 == *(p+2)))//��
        {
            AccelerationAckBuffer[0] = 0x01;
            AccelerationAckBuffer[1] = 0x01;
            AccelerationAckBuffer[2] = 0xF7;
            AccelerationAckBuffer[3] = 0x02;
            AccelerationAckBuffer[4] = 0x00;
            AccelerationAckBuffer[5] = 0x07;	
            memcpy(AccelerationAckBuffer+6,p,3);    
            FRAM_BufferRead(AccelerationAckBuffer+9,4,FRAM_ACCELERATION_ADDR);
            AccelerationAckBufferLen = 13;
        }
        DelayAckCommand = 0x0900;
        SetTimerTask(TIME_DELAY_TRIG,SECOND);
    }
    else if(0x03 == cmd)//������
    {
        p = pBuffer;
        if((5 == length)&&(1 == *(p+2)))//д
        {
            AccelerationAckBuffer[0] = 0x01;
            AccelerationAckBuffer[1] = 0x01;
            AccelerationAckBuffer[2] = 0xF7;
            AccelerationAckBuffer[3] = 0x03;
            AccelerationAckBuffer[4] = 0;
            AccelerationAckBuffer[5] = 0x03;	
            memcpy(AccelerationAckBuffer+6,p,3);
            AccelerationAckBufferLen = 9;
            FRAM_BufferWrite(FRAM_DECELERATION_ADDR,p+3,2);
            Set_Emer_Threshold(p+3,1);//zengliang add 2016-11-21
					
					CleanInitFlag();
        }
        else if((3 == length)&&(0 == *(p+2)))//��
        {
            AccelerationAckBuffer[0] = 0x01;
            AccelerationAckBuffer[1] = 0x01;
            AccelerationAckBuffer[2] = 0xF7;
            AccelerationAckBuffer[3] = 0x03;
            AccelerationAckBuffer[4] = 0;
            AccelerationAckBuffer[5] = 0x05;	
            memcpy(AccelerationAckBuffer+6,p,3);    
            FRAM_BufferRead(AccelerationAckBuffer+9,2,FRAM_DECELERATION_ADDR);
            AccelerationAckBufferLen = 11;
        }
        DelayAckCommand = 0x0900;
        SetTimerTask(TIME_DELAY_TRIG,SECOND);
    }
}
void Acceleration_Turn_DisposeRadioProtocol(u8 cmd, u8 *pBuffer, u16 length)
{
    u8 *p = NULL;
    u16 TurnSerilialNum=0;
    if(0x04 == cmd)//��ת����ز���
    {
        p = pBuffer;
			  TurnSerilialNum |= *(p+1) << 8;
        TurnSerilialNum |= *(p+2);
			
			  AccelerationAckBuffer[0]=0x01;
				AccelerationAckBuffer[1]=0x01;
				AccelerationAckBuffer[2]=0xF5;
				AccelerationAckBuffer[3]=0x04;  
				AccelerationAckBuffer[4]=0;
				AccelerationAckBuffer[5]=6; 
			  AccelerationAckBuffer[6] = 0x14;
			//��ˮ��
        AccelerationAckBuffer[7] = *(p+1) << 8;
        AccelerationAckBuffer[8] = *(p+2);
      //��д��־  
			AccelerationAckBuffer[9] = *(p+3);
			if(0x14 == *p)//��ת��
        {
            if((0 == *(p+3))&&(4 == length))//����ת��Ƕ�
            {
              FRAM_BufferRead(AccelerationAckBuffer+10,2,FRAM_TURN_ANGLE_ADDR);
            }
            else if((1 == *(p+3))&&(6 == length))//д
            {
							FRAM_BufferWrite(FRAM_TURN_ANGLE_ADDR,p+4,2);
							memcpy(AccelerationAckBuffer+10,p+4,2);
							Set_Emer_Threshold(p+4,2);
							
							CleanInitFlag();
            }
        }
				AccelerationAckBufferLen = 12;
        DelayAckCommand = 0x0900;
        SetTimerTask(TIME_DELAY_TRIG,SECOND);
    }
}
/*******************************************************************************
* Function Name  : Link1Login_TimeTask
* Description    : ����1��¼
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState  Link1Login_TimeTask(void)
{
    u8  ucTmp[30];
        static u8   errorcount = 0;
        
        Modem_Printf("Link1ManualCloseFlag = %d\r\n",Link1ManualCloseFlag);
        Modem_Printf("Link1LoginAuthorizationState = %d\r\n",Link1LoginAuthorizationState);
    
    if(CHANNEL_DATA_1 == (CHANNEL_DATA_1&GetTerminalAuthorizationFlag()))
    {
        Link1LoginAuthorizationState = LOGIN_CLOSE_TASK;
    }
    
    switch(Link1LoginAuthorizationState)
    {
        case LOGIN_READ_AUTHORIZATION_CODE://��ȡ��Ȩ��
            {
                Link1LoginCount = 0;
                errorcount = 0;
                Link1AuthorizationCount = 0;
                Link1LoginAuthorizationLastState = 0;
                Link1LoginAuthorizationDelayCount = 0;

                if(0 == EepromPram_ReadPram(E2_LOGINGPRS_PASSWORD_ID, ucTmp))//ʧ��
                {
                    Link1LoginAuthorizationState = LOGIN_SEND_LOGIN_CMD;
                }
                else//�ɹ�
                {
                    Link1LoginAuthorizationState = LOGIN_SEND_AUTHORIZATION_CMD;
                }
                break;
            }
        case LOGIN_SEND_LOGIN_CMD://����ע������
            {				
                if(ACK_OK == RadioProtocol_TerminalRegister(CHANNEL_DATA_1))
                {
                    Net_Other_Printf(PRTF_REGISTER);
                    Link1LoginAuthorizationDelayCount = 0;
                    Link1LoginAuthorizationDelayTime = 30;//ע���Ȩ��ʱʱ��
                    Link1LoginAuthorizationLastState = LOGIN_SEND_LOGIN_CMD;//��һ��״̬
                    Link1LoginCount++;
                    if(Link1LoginCount >= 3)
                    {
                        Link1LoginAuthorizationState = LOGIN_CLOSE_GPRS;
                    }
                    else
                    {
                        Link1LoginAuthorizationState = LOGIN_DELAY;
                    }
                    errorcount = 0;
                }
                else
                {
                    errorcount++;
                    if(errorcount >= 200)
                    {
                        Link1LoginAuthorizationState = LOGIN_CLOSE_TASK;
                    }
                }
                break;
            }
        case LOGIN_SEND_AUTHORIZATION_CMD://���ͼ�Ȩ����
            {
                if(ACK_OK == RadioProtocol_TerminalQualify(CHANNEL_DATA_1))
                {
                    Net_Other_Printf(PRTF_AUTHORIZATION);
                    Link1LoginAuthorizationDelayCount = 0;
                    Link1LoginAuthorizationDelayTime = 30;//ע���Ȩ��ʱʱ��,��1����������ʱ
                    Link1LoginAuthorizationLastState = LOGIN_SEND_AUTHORIZATION_CMD;//��һ��״̬
                    Link1AuthorizationCount++;
                    if(Link1AuthorizationCount >= 10)
                    {
                        Link1LoginAuthorizationState = LOGIN_SEND_LOGIN_CMD;
                    }
                    else
                    {
                        Link1LoginAuthorizationState = LOGIN_DELAY;
                    }
                    errorcount = 0;
                }
                else
                {
                    errorcount++;
                    if(errorcount >= 200)
                    {
                         Link1LoginAuthorizationState = LOGIN_CLOSE_TASK;
                    }
                }
                break;
            }
        case LOGIN_CLOSE_GPRS://�Ͽ�����
            {
                Communication_Close();
                Link1ManualCloseFlag = 1;
                ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);//��Ȩ��־��0
                ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
                Link1LoginAuthorizationDelayCount = 0;
                Link1LoginAuthorizationDelayTime = 10*60;//ע���Ȩ��ʱʱ�䣬���������ʱ10����
                Link1LoginAuthorizationLastState = LOGIN_RECONNECT_GPRS;
                Link1LoginAuthorizationState = LOGIN_DELAY; 
                break;
                
            }
        case LOGIN_DELAY://��ʱ
            {
                Link1LoginAuthorizationDelayCount++;
                if(Link1LoginAuthorizationDelayCount >= Link1LoginAuthorizationDelayTime)
                {
                    Link1LoginAuthorizationDelayCount = 0;
                    Link1LoginAuthorizationState = Link1LoginAuthorizationLastState;
                }
                break;
            }
        case LOGIN_RECONNECT_GPRS://��������
            {
                Link1LoginCount = 0;
                errorcount = 0;
                Link1AuthorizationCount = 0;
                Link1LoginAuthorizationLastState = 0;
                Link1LoginAuthorizationState = 0;
                Link1LoginAuthorizationDelayCount = 0; 
                Communication_Open();
                Link1ManualCloseFlag = 0;
                Link1LoginAuthorizationState = LOGIN_CLOSE_TASK;
                break;
            }
        case LOGIN_CLOSE_TASK://�ر�����
            {
                if(1 == Link1ManualCloseFlag)
                {
                    Communication_Open();
                    Link1ManualCloseFlag = 0;
                }
                Link1LoginCount = 0;
                errorcount = 0;
                Link1AuthorizationCount = 0;
                Link1LoginAuthorizationLastState = 0;
                Link1LoginAuthorizationState = 0;
                Link1LoginAuthorizationDelayCount = 0; 
                return DISABLE;
            }
        default:
            {
                Link1LoginAuthorizationState = LOGIN_CLOSE_TASK;
            }
        break;
    }
        
       
        
    return ENABLE;
    
}
/*******************************************************************************
* Function Name  : Link2Login_TimeTask
* Description    : ����2��¼
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState  Link2Login_TimeTask(void)
{
    u8  ucTmp[30];

        static u8   errorcount = 0;
        
        Modem_Printf("Link2ManualCloseFlag = %d\r\n",Link2ManualCloseFlag);
        Modem_Printf("Link2LoginAuthorizationState = %d\r\n",Link2LoginAuthorizationState);
    
    if(CHANNEL_DATA_2 == (CHANNEL_DATA_2&GetTerminalAuthorizationFlag()))
    {
        Link2LoginAuthorizationState = LOGIN_CLOSE_TASK;
    }
    
    switch(Link2LoginAuthorizationState)
    {
        case LOGIN_READ_AUTHORIZATION_CODE://��ȡ��Ȩ��
            {
                Link2LoginCount = 0;
                errorcount = 0;
                Link2AuthorizationCount = 0;
                Link2LoginAuthorizationLastState = 0;
                Link2LoginAuthorizationDelayCount = 0;

                if(0 == EepromPram_ReadPram(E2_SECOND_LOGINGPRS_PASSWORD_ID, ucTmp))//��Ҫ�޸�,��2�����ӵļ�Ȩ�뻹û����
                {
                    Link2LoginAuthorizationState = LOGIN_SEND_LOGIN_CMD;
                }
                else//�ɹ�
                {
                    Link2LoginAuthorizationState = LOGIN_SEND_AUTHORIZATION_CMD;
                }
								
                    break;
                }
        case LOGIN_SEND_LOGIN_CMD://����ע������
        {
            if(ACK_OK == RadioProtocol_TerminalRegister(CHANNEL_DATA_2))
                        {
                                Net_Other_Printf(PRTF_REGISTER);
                    Link2LoginAuthorizationDelayCount = 0;
                    Link2LoginAuthorizationDelayTime = 30;//ע���Ȩ��ʱʱ��
                    Link2LoginAuthorizationLastState = LOGIN_SEND_LOGIN_CMD;//��һ��״̬
                    Link2LoginCount++;
                    if(Link2LoginCount >= 3)
                    {
                        Link2LoginAuthorizationState = LOGIN_CLOSE_GPRS;
                    }
                    else
                    {
                        Link2LoginAuthorizationState = LOGIN_DELAY;
                    }
                                errorcount = 0;
                        }
                        else
                        {
                                errorcount++;
                                if(errorcount >= 200)
                                {
                                        Link2LoginAuthorizationState = LOGIN_CLOSE_TASK;
                                }
                        }
            break;
            }
        case LOGIN_SEND_AUTHORIZATION_CMD://���ͼ�Ȩ����
            {
                    if(ACK_OK == RadioProtocol_TerminalQualify(CHANNEL_DATA_2))
                        {
                                Net_Other_Printf(PRTF_AUTHORIZATION);
                    Link2LoginAuthorizationDelayCount = 0;
                                Link2LoginAuthorizationDelayTime = 30;//ע���Ȩ��ʱʱ��,��1����������ʱ
                    Link2LoginAuthorizationLastState = LOGIN_SEND_AUTHORIZATION_CMD;//��һ��״̬
                    Link2AuthorizationCount++;
                                if(Link2AuthorizationCount >= 10)
                    {
                        Link2LoginAuthorizationState = LOGIN_SEND_LOGIN_CMD;
                    }
                    else
                    {
                        Link2LoginAuthorizationState = LOGIN_DELAY;
                    }
                                errorcount = 0;
                        }
                        else
                        {
                                errorcount++;
                                if(errorcount >= 200)
                                {
                                        Link2LoginAuthorizationState = LOGIN_CLOSE_TASK;
                                }
                        }
            break;
            }
        case LOGIN_CLOSE_GPRS://�Ͽ�����
            {
                            Net_Second_Close();
                            Link2ManualCloseFlag = 1;
                            ClearTerminalAuthorizationFlag(CHANNEL_DATA_2);//��Ȩ��־��0
                            ClearTerminalAuthorizationCode(CHANNEL_DATA_2);
                            Link2LoginAuthorizationDelayCount = 0;
                            Link2LoginAuthorizationDelayTime = 10*60;//ע���Ȩ��ʱʱ�䣬���������ʱ10����
                            Link2LoginAuthorizationLastState = LOGIN_RECONNECT_GPRS;
                            Link2LoginAuthorizationState = LOGIN_DELAY; 
                break;
                
            }
        case LOGIN_DELAY://��ʱ
            {
                Link2LoginAuthorizationDelayCount++;
                if(Link2LoginAuthorizationDelayCount >= Link2LoginAuthorizationDelayTime)
                {
                    Link2LoginAuthorizationDelayCount = 0;
                    Link2LoginAuthorizationState = Link2LoginAuthorizationLastState;
                }
                break;
            }
        case LOGIN_RECONNECT_GPRS://��������
            {
                Link2LoginCount = 0;
                errorcount = 0;
                Link2AuthorizationCount = 0;
                Link2LoginAuthorizationLastState = 0;
                Link2LoginAuthorizationState = 0;
                Link2LoginAuthorizationDelayCount = 0; 
                Net_Second_Open();
                Link2ManualCloseFlag = 0;
                Link2LoginAuthorizationState = LOGIN_CLOSE_TASK;
                break;
            }
        case LOGIN_CLOSE_TASK://�ر�����
            {
                        if(1 == Link2ManualCloseFlag)
                        {
                                Net_Second_Open();//�˳�����֮ǰ�����ֶ��ر����ӣ���Ҫ���´�
                                Link2ManualCloseFlag = 0;
                        }
                        Link2LoginCount = 0;
                        errorcount = 0;
                        Link2AuthorizationCount = 0;
                        Link2LoginAuthorizationLastState = 0;
                        Link2LoginAuthorizationState = 0;
                        Link2LoginAuthorizationDelayCount = 0; 
                        return DISABLE;
            }
        default:
            {
                Link2LoginAuthorizationState = LOGIN_CLOSE_TASK;
            }
        break;
    }
    return ENABLE;
    
}

#if	TACHOGRAPHS_19056_TEST
static u8 simulationSpeed = 100;
static const  u8	simulationSpeedRate = 1;
static const  u8	simulationSpeedThreshold = 40;

void Tachographs_Test_Speed_Reduce(void)
{	
	static u8 flag = 0;
	if(!flag)
	{
		simulationSpeed -= simulationSpeedRate;
		if(simulationSpeedThreshold == simulationSpeed)flag = !flag;
	}
	else
	{
		simulationSpeed += simulationSpeedRate;
		if(simulationSpeed >= 100)flag = !flag;
	}
}
u8 Tachographs_Test_Get_Speed(void)
{	
	return simulationSpeed;
}
#endif

