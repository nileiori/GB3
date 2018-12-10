#ifndef	__OTHER_H
#define	__OTHER_H

//*************�ļ�����***************
#include "stm32f10x.h"
#include "Io.h"
//*************�궨��****************
#define BEEP_ON_EV_ATTEMP		0x01//�յ�������Ϣ	
#define BEEP_ON_EV_SPEED		0x02//����
#define BEEP_ON_EV_TIRED		0x04//ƣ�ͼ�ʻ
#define BEEP_ON_EV_LAND			0x08//δ��¼(��������ʱ������)
#define	BEEP_ON_EV_SPEED_TIRED		0x06//���ٺ�ƣ�ͼ�ʻ����
#define BEEP_GPIO_CC                  			GPIOD
#define BEEP_GPIO_Pin                    		GPIO_Pin_3 


#define   PWR_24V()		GPIO_WriteBit(GPIOF, GPIO_Pin_3, Bit_SET)
#define   PWR_12V()		GPIO_WriteBit(GPIOF, GPIO_Pin_3, Bit_RESET)
//************�ṹ������******************
//ʱ����ƽṹ��
typedef  struct
{
	u32  MenuTime;				//��¼���뵱ǰ�˵���ʱ��
    u32  BeepTime;				//��¼��������ʼ���е�ʱ��
    u32  KeyTime;				//��¼���յ���ǰ������ʱ��
    u8   BeepFlag;				//������״̬��־,0ΪOFF; 1ΪON
}TIME_CONTROL_STRUCT;
//ע���Ȩ����״̬
typedef enum {
  LOGIN_READ_AUTHORIZATION_CODE = 0,//��ȡ��Ȩ�룬ÿ��������������Ǵ�״̬
  LOGIN_SEND_LOGIN_CMD,//����ע������
  LOGIN_SEND_AUTHORIZATION_CMD,//���ͼ�Ȩ����
  LOGIN_CLOSE_GPRS,//�Ͽ�����
  LOGIN_DELAY,//��ʱ
  LOGIN_RECONNECT_GPRS,//��������
  LOGIN_CLOSE_TASK,//�ر�����
} LOGIN_STATE;  
//*************��������**************
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
void SetTerminalFirmwareVersion(void);
/*********************************************************************
//��������  :UpdateHbRunMode
//����      :���±���HbRunMode
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :
*********************************************************************/
void UpdateRunMode(void);
/*********************************************************************
//��������	:UpdatePeripheralType
//����		:���±���Peripheral1Type,Peripheral2Type
//��ע		:1.����������1���ͻ�����2���Ͳ���ʱ��Ҫ���øýӿ�
//              :2.�ϵ�������ʼ��ʱ����øýӿ� 
*********************************************************************/
void UpdatePeripheralType(void);
/*********************************************************************
//��������	:ReadPeripheral1TypeBit
//����		:������1���͵�ĳһλ
//����		:�ڼ�λ��ȡֵΪ0~15
//����		:1:��ʾ�����豻������;0:��ʾ������δ������
//��ע		:����ֵ����15����ֵΪ2����ʾ���� 
*********************************************************************/
u8 ReadPeripheral1TypeBit(u8 Bit);
/*********************************************************************
//��������	:ReadPeripheral2TypeBit
//����		:������1���͵�ĳһλ
//����		:�ڼ�λ��ȡֵΪ0~15 
//����		:1:��ʾ�����豻������;0:��ʾ������δ������
//��ע		:����ֵ����15����ֵΪ2����ʾ���� 
*********************************************************************/
u8 ReadPeripheral2TypeBit(u8 Bit);
/*********************************************************************
//��������	:UpdataAccOffGpsControlFlag(void)
//����		:���±���AccOffGpsControlFlag
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void UpdataAccOffGpsControlFlag(void);
/*********************************************************************
//��������	:UpdataEvaluatorControlFlag(void)
//����		:���±���EvaluatorControlFlag
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void UpdataEvaluatorControlFlag(void);
/*********************************************************************
//��������	:UpdataTaximeterControlFlag(void)
//����		:���±���TaximeterControlFlag
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void UpdataTaximeterControlFlag(void);
/*********************************************************************
//��������	:UpdataToplightControlFlag(void)
//����		:���±���ToplightControlFlag
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void UpdataToplightControlFlag(void);
/*********************************************************************
//��������	:UpdataLedlightControlFlag(void)
//����		:���±���LedlightControlFlag
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void UpdataLedlightControlFlag(void);
/*********************************************************************
//��������	:EvRing
//����		:�����绰����
//��ע		:
*********************************************************************/
void EvRing(void);
/*********************************************************************
//��������	:EvRingOver
//����		:�رյ绰����
//��ע		:
*********************************************************************/
void EvRingOver(void);
void SpeedFlagUpdatePram(void);
void SetBeepOnEvBit(u8 bit);
void SetCurCamID(u8 CurCamID);
FunctionalState Reset_TimeTask(void);  
/*******************************************************************************
* Function Name  : Link1Login_TimeTask
* Description    : ����1��¼����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState Link1Login_TimeTask(void);
/*******************************************************************************
* Function Name  : Link2Login_EvTask
* Description    : ����1��¼����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState Link2Login_TimeTask(void);
/*******************************************************************************
* Function Name  : SetLink1LoginAuthorizationState
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetLink1LoginAuthorizationState(u8 State);
/*******************************************************************************
* Function Name  : SetLink2LoginAuthorizationState
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetLink2LoginAuthorizationState(u8 State);
/*******************************************************************************
* Function Name  : EvGetAkeyTask
* Description    : ��ȡ��Ȩ��--��SMS����ͨ�� .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GetAkey_EvTask(void);
/*******************************************************************************
* Function Name  : EvChangeServerTask
* Description    : �յ�ָ����л������ƽ̨
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ChangeServer_EvTask(void);
/*******************************************************************************
* Function Name  : EvFtpUpdataTask
* Description    : ����FTP�����ļ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FtpUpdata_EvTask(void);
/*******************************************************************************
* Function Name  : EvDiallingTask
* Description    : һ��ͨ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Dialling_EvTask(void);
/*******************************************************************************
* Function Name  : EvSevDialTask
* Description    : �绰�ز�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SevDial_EvTask(void);
/*********************************************************************
//��������	:Updata_SendResultNoteTimeTask
//����		:�����������֪ͨ��ƽ̨
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:
//���ú���	:
//�ж���Դ	:
//����		:
//��ע		:Ĭ���ǿ�����,1�����1��
*********************************************************************/
FunctionalState  Updata_SendResultNoteTimeTask(void);
/*********************************************************************
//��������	:Reset_TimeTask
//����		:ϵͳ��ʱ����
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:
//���ú���	:
//�ж���Դ	:
//����		:
//��ע		:
*********************************************************************/
FunctionalState Reset_TimeTask(void);
/*******************************************************************************
* Function Name  : DelayTrigTimeTask(void)
* Description    : ��ʱ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState DelayTrigTimeTask(void);
/*********************************************************************
//��������	:GetTerminalAuthorizationFlag(void)
//����		:��ȡ��Ȩ��־��ֵ
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:
//���ú���	:
//�ж���Դ	:
//����		:
//��ע		:bit0:����1;bit1:����2;bit2~bit7:����,��ӦλΪ1��ʾ��Ȩ�ɹ�,����Ϊʧ��
*********************************************************************/
u8 GetTerminalAuthorizationFlag(void);
/*********************************************************************
//��������	:SetTerminalAuthorizationFlag(u8 channel)
//����		:��λĳ��ͨ���ļ�Ȩ��־
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:
//���ú���	:
//�ж���Դ	:
//����		:
//��ע		:bit0:��ȱ��Ԥ��;bit1:��ʾCHANNEL_DATA_1��bit2:CHANNEL_DATA_2
//          :channel��ֵ����Ϊ��������ͨ��ֵ���������ǵ����
*********************************************************************/
void SetTerminalAuthorizationFlag(u8 channel);
/*********************************************************************
//��������	:ClearTerminalAuthorizationFlag(u8 channel)
//����		:���ĳ��ͨ���ļ�Ȩ��־
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:
//���ú���	:
//�ж���Դ	:
//��ע		:bit0:��ȱ��Ԥ��;bit1:��ʾCHANNEL_DATA_1��bit2:CHANNEL_DATA_2
//          :channel��ֵ����Ϊ��������ͨ��ֵ���������ǵ����
*********************************************************************/
void ClearTerminalAuthorizationFlag(u8 channel);
/*********************************************************************
//��������	:ClearTerminalAuthorizationCode(u8 channel)
//����		:���ĳ��ͨ���ļ�Ȩ��
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:
//���ú���	:
//�ж���Դ	:
//����		:
//��ע		:bit0:��ȱ��Ԥ��;bit1:��ʾCHANNEL_DATA_1��bit2:CHANNEL_DATA_2
//          :channel��ֵ����Ϊ��������ͨ��ֵ���������ǵ����
*********************************************************************/
void ClearTerminalAuthorizationCode(u8 channel);
/*******************************************************************************
* Function Name  : MultiCenterConnect_TimeTask(void)
* Description    : ���������Ӷ�ʱ����,�յ����ñ��÷���������ʱ����,1���ӵ���1��
* Input          : None
* Output         : None
* Return         : ����Ϊ�б걱���������
*******************************************************************************/
FunctionalState MultiCenterConnect_TimeTask(void);
/*******************************************************************************
* Function Name  : MultiCenterConnect_GetRequestFlag(void)
* Description    : ��ȡ���������������־
* Input          : None
* Output         : None
* Return         : ����1��ʾ��ǰΪ���������ӣ�����0��ʾ��ǰΪ����Ӧ�õ�����
*******************************************************************************/
u8 MultiCenterConnect_GetRequestFlag(void);
/*******************************************************************************
* Function Name  : MultiCenterConnect_SetRequestFlag(void)
* Description    :��λ���������������־
* Input          : None
* Output         : None
* Return         : ��
*******************************************************************************/
void MultiCenterConnect_SetRequestFlag(void);
/*******************************************************************************
* Function Name  : MultiCenterConnect_ResetRequestFlag(void)
* Description    :������������������־
* Input          : None
* Output         : None
* Return         : ��
*******************************************************************************/
void MultiCenterConnect_ResetRequestFlag(void);
/*******************************************************************************
* Function Name  : MultiCenterConnect_GetConnectFlag(void)
* Description    : ��ȡ���������ӱ�־
* Input          : None
* Output         : None
* Return         : ����1��ʾ��ǰΪ���������ӣ�����0��ʾ��ǰΪ����Ӧ�õ�����
*******************************************************************************/
u8 MultiCenterConnect_GetConnectFlag(void);
/*******************************************************************************
* Function Name  : MultiCenterConnect_SetFlag(void)
* Description    :��λ���������ӱ�־
* Input          : None
* Output         : None
* Return         : ��
*******************************************************************************/
void MultiCenterConnect_SetFlag(void);
/*******************************************************************************
* Function Name  : MultiCenterConnect_ResetFlag(void)
* Description    :������������ӱ�־
* Input          : None
* Output         : None
* Return         : ��
*******************************************************************************/
void MultiCenterConnect_ResetFlag(void);
/*******************************************************************************
* Function Name  : Communication_Open
* Description    : ��������ͨ��
* Input          : None
* Output         : None
* Return         : �б걱������ä����������ʱ,���������Ҫ�����������
*******************************************************************************/
void Communication_Open(void);
/*******************************************************************************
* Function Name  : Communication_CloseGprsTimeTask(void)
* Description    : ����ʹ��ǰ����һ�������ر�GPRS
* Input          : None
* Output         : None
* Return         : ����Ϊ�б걱���������
*******************************************************************************/
FunctionalState Communication_CloseGprsTimeTask(void);
/*******************************************************************************
* Function Name  : Communication_Close
* Description    : �ر�����ͨ��
* Input          : None
* Output         : None
* Return         : �б걱������ä����������ʱ,���������Ҫ�����������
*******************************************************************************/
void Communication_Close(void);
void Communication_Open(void);
void Communication_Close2(void);
void NaviLcdShowTempText(u8 *pBuffer);//����ʾ����ʾһ���ı���Ϣ
/********************************************************************
* ���� : Print_RTC
* ���� : ��ȡ��ǰ��RTCʱ��,��ӡ���
********************************************************************/
void Print_RTC( void );
/********************************************************************
* ���� : SetCostDownFlag
* ���� : ����CostDown�汾��־λ����λΪ1��ʾPCBΪ���ɱ��汾��Ϊ0��ʾ֮ǰ�İ汾
********************************************************************/
void SetCostDownVersionFlag( void );
/********************************************************************
* ���� : ClearCostDownFlag
* ���� : ���CostDown�汾��־λ����λΪ1��ʾPCBΪ���ɱ��汾��Ϊ0��ʾ֮ǰ�İ汾
********************************************************************/
void ClearCostDownVersionFlag( void );
/********************************************************************
* ���� : GetCostDownFlag
* ���� : ��ȡCostDown�汾��־λ����λΪ1��ʾPCBΪ���ɱ��汾��Ϊ0��ʾ֮ǰ�İ汾
********************************************************************/
u8 GetCostDownVersionFlag( void );
/********************************************************************
* ���� : GansuOiL_OilProtocolCheck
* ���� : ��������ͺ�Э����,�ͺĴ�����30�뷢��һ�����ݸ�GPS�ն�
********************************************************************/
u8 GanSuOil_OilProtocolCheck(u8 *pBuffer, u8 length);
/********************************************************************
* ���� : GanSuOiL_GetOnePacketData
* ���� : ��ȡһ���ͺ����ݣ�λ�û㱨ʱ���øú���
********************************************************************/
u8 GanSuOil_GetOnePacketData(u8 *pBuffer);
/********************************************************************
* ���� : GanSuOiL_GetCurPacketLen
* ���� : ��ȡ��ǰ�ͺ����ݰ�����
********************************************************************/
u8 GanSuOil_GetCurPacketLen(void);
extern u8	ResetEnableFlag;//����ʹ�ܱ�־
extern u8      Ftp_Updata_BusyFlg;//ftp ����æ��־   0������   1��������
void Acceleration_DisposeRadioProtocol(u8 cmd, u8 *pBuffer, u16 length);
void Acceleration_Turn_DisposeRadioProtocol(u8 cmd, u8 *pBuffer, u16 length);

#define	TACHOGRAPHS_19056_TEST	0	//��ʻ��¼�ǲ���ʱ����Ϊ����0��ֵ������ʱ��0
#if	TACHOGRAPHS_19056_TEST
void Tachographs_Test_Speed_Reduce(void);
u8 Tachographs_Test_Get_Speed(void);
#endif

#endif
