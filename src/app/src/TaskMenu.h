/*******************************************************************************
 * File Name:			TaskMenu.h 
 * Function Describe:	
 * Relate Module:		
 * Writer:				joneming
 * Date:				2012-01-12
 * Rewriter:
 * Date:		
 *******************************************************************************/
#ifndef _TASKMENU_H_
#define _TASKMENU_H_
#include "include.h"

//#define SERVICE_MENU_JIUTONG  //������,�򰴾�ͨҪ��,��������������ͨ����ʾ������
///////////////////////////////////////////////////
#define SYS_DEFAULT_PASSWORD    701000 //ϵͳĬ������
/////(ע��:��������������ϵͳĬ������һ��///////////////
#define SUPER_PASSWORD          601703////(���6λ)��������,������ô��������붼���Խ���
//(ע��://ϵͳĬ������,���SHOW_INPUT_PASSWORDҲΪϵͳĬ������,��������ڰ���)////////
#define SHOW_INPUT_PASSWORD     0///(���6λ)�ڽ�����������ʱ,����ֻ���������ʾ������
//////////////////////////////////////////////////////////
extern u16 CommonShareBufferLen; 
extern u8 CommonShareBufferBusyFlag;
extern u8 RadioShareBuffer[];
extern unsigned char GB2312Buffer[];
/////////////////////////////////

typedef enum 
{
    MENU_TEST_REGISTER,//"�ն�ע��"
    MENU_TEST_QUALIFY,//"�ն˼�Ȩ"
    //MENU_TEST_LOGOUT,//"�ն�ע��",
    MENU_TEST_SEND_POST,//"λ����Ϣ�㱨"
    MENU_TEST_EVENT_REPORT,//"�¼�����",//4
    MENU_TEST_SEND_MESSAGE,//"��Ϣ�㲥"
    MENU_TEST_CANCEL_MESSAGE,// "��Ϣȡ��",//6
    MENU_TEST_SEND_E_WAYBILL,//"�����˵��ϱ�",//7
    MENU_TEST_DRIVER_INFO,//"��ʻԱ�����Ϣ"   
    MENU_TEST_MULIT_EVENT,//"�Ϸ���ý���¼�"
    MENU_TEST_SEND_DTU_DATA,//"��������͸��"
    MENU_TEST_MAX
}eSTMENUTEST;
//////////////////////////////////
///////////////////////////////
void ShowStartLogo(void);
void OpenLcdBackLight(void);
void FlashLcdBackLight(void);
void FlashLcdBackLightFinish(void);
void SysAutoRunCountReset(void);
void SysLcdBackLightCntReset(void);
void SysAutoRunToMainInfoTask(void);
unsigned char BrushAdministratorCardResponse(unsigned char *data);
///////////////////
void PublicSysParamterInit(void);
void ShowTextForMsgBusyFlag(unsigned char flag);
////////////////////////////
void ReflashToReturnMainInterface(unsigned long time);
void ShowMainInterface(void);
void SystemAutoShowText(unsigned short textlen,unsigned char *buffer);
void SystemAutoShowTextForTaxiLender(void);
////////////////////////////////////////////////////
void PublicConfirmAll(char *caption,void (*doit)(void),void (*Cancel)(void),unsigned long time,unsigned char select);
void PublicConfirm(char *caption,void (*doit)(void));
void PublicConfirmAutoExit(char *caption,void (*doit)(void),unsigned long time);
//////////////////////////////
LZM_RET TaskShowLogo(LZM_MESSAGE *Msg);
LZM_RET TaskMenuMain(LZM_MESSAGE *Msg);
LZM_RET TaskShowTextInfo(LZM_MESSAGE *Msg);
LZM_RET TaskShowMainInterface(LZM_MESSAGE *Msg);
LZM_RET TaskMenuPrint(LZM_MESSAGE *Msg);
/*************************************************************
** ��������: SystemAutoShowQuestionResponse
** ��������: 
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void SystemAutoShowQuestionResponse(unsigned short datalen,unsigned char *buffer,unsigned char flag);
/*************************************************************
** ��������: GetShowMainInterfaceFlag
** ��������: �Ƿ���������
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char GetShowMainInterfaceFlag(void);
/*************************************************************OK
** ��������: Question_DisposeRadioProtocol
** ��������: �����·�Э�����
** ��ڲ���: pBuffer:�����׵�ַ,BufferLen:���ݳ���			 
** ���ڲ���: 
** ���ز���: :
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
void Question_DisposeRadioProtocol(u8 *pBuffer, u16 BufferLen);
/*************************************************************
** ��������: InfoService_DisposeRadioProtocol
** ��������: �㲥��Ϣ����Э�����
** ��ڲ���: pBuffer:�����׵�ַ,BufferLen:���ݳ���
** ���ڲ���: ��
** ���ز���: ���ز������
** ȫ�ֱ���: ��
** ����ģ��: ��
*************************************************************/
unsigned char InfoService_DisposeRadioProtocol(unsigned char *pBuffer,unsigned short BufferLen);
/*************************************************************
** ��������: GetCurentProductModel
** ��������: ��õ�ǰ�ն��ͺ�
** ��ڲ���: ��
** ���ڲ���: name�׵�ַ
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
void GetCurentProductModel(char *name);
/*************************************************************
** ��������: Task_GetCurSystemIsDeepFlag
** ��������: �Ƿ�Ϊ�����־
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Task_GetCurSystemIsDeepFlag(void);
/*************************************************************
** ��������: Task_GetCurSystemIsTurnSpeedFlag
** ��������: �Ƿ�Ϊת�ٱ�־
** ��ڲ���: ��
** ���ڲ���: ��
** ���ز���: ��
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/
unsigned char Task_GetCurSystemIsTurnSpeedFlag(void);
////////////////////////////////
#endif//_TASKMENU_H_
/*******************************************************************************
 *                             end of module
 *******************************************************************************/
