/********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS 
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
#include "VDR.h"
#include "Can_App.h"
#include "DeepPlow.h"
#include "turnspeed.h"
/* extern function -----------------------------------------------------------*/
void EvNopHandleTask(void);
FunctionalState TimerNopHandleTask(void);

static u32	TimeTaskResumeSta[MAX_TIMETASK/32+1] = {0,};
static u32	EvTaskResumeSta			 = 0;

#define			BIT_SET(Val,bit)			(Val |= 0x1<<bit)
#define			BIT_CLR(Val,bit)			(Val &= ~(0x1<<bit))
#define			BIT_CHECK(Val,bit)		(Val & 0x1<<bit)

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIME_TASK MyTimerTask[MAX_TIMETASK] = {  
    /*
        {ENABLE, 0, 1, Communication_TimeTask},                            // 0:����ͨ�ţ�Ĭ�Ͽ���
        {ENABLE, 0, SYSTICK_0p1SECOND, RadioProtocolParse_TimeTask},       // 1:����Э�������Ĭ�Ͽ���   
        {ENABLE, 0, 1,Io_TimeTask},                                        // 2:IO ״̬��⣬Ĭ�Ͽ��� 
        {ENABLE, 0, 1*SYSTICK_1SECOND, Gps_TimeTask},                      // 3:GPS���ݴ���Ĭ�Ͽ���
        {ENABLE,0,5*SYSTICK_0p1SECOND,GpsMile_TimeTask},                   // 4:GPS��̼��㣬Ĭ�Ͽ���
        {ENABLE,0,SYSTICK_1SECOND,Report_TimeTask},                        // 5:λ�û㱨��Ĭ�Ͽ���
        {ENABLE,1,1,Camera_TimeTask},                                      // 6:����ͷ���գ�Ĭ�Ͽ���         
        {DISABLE, 0, 1*SYSTICK_1SECOND, Media_UploadTimeTask},             // 7:�����ý�����ݵ�ƽ̨��Ĭ�Ϲر�
        {ENABLE,0,SYSTICK_1SECOND,HeartBeat_TimeTask},                     // 8:��������Ĭ�Ͽ���
        {ENABLE,0,SYSTICK_0p1SECOND,Usart3_TimeTask},                      // 9:USART3����3��ʱ��������,����ͷ�õĴ˴��ڣ�Ĭ�Ͽ���
        {DISABLE, 0, 1*SYSTICK_1SECOND, TimerNopHandleTask},               // 10: �޴˹���    
        {DISABLE, 0, 1, DelayTrigTimeTask},                                // 11:��ʱ������Ĭ�Ϲر�
        {ENABLE, 0, 1*SYSTICK_1SECOND, Reset_TimeTask},                    // 12:50Сʱ��ϵͳ������Ĭ�Ͽ���
        //{ENABLE,0,1,BMA250EApp_TimeTask},                                // 13:��ײ������⣬Ĭ�Ͽ���
        {DISABLE,0,5,BMA250_TimeTask},                                      // 13:��ײ������⣬Ĭ�Ͽ���
        //{DISABLE,0,1,BMA220App_TimeTask},                                
        {DISABLE,0,SYSTICK_1SECOND,Link1Login_TimeTask},                   // 14:����1��¼��Ȩ��Ĭ�Ϲر� 
        {ENABLE,0,1,Usb_AppTimeTask},                                      // 15:USB��Ĭ�Ͽ���
        {ENABLE,0,1,Adc_CollectApp_TimeTask},                              // 16:AD�ɼ���Ĭ�Ͽ���
        {ENABLE,0,SYSTICK_1SECOND,Sleep_TimeTask},                         // 17:���� ��Ĭ�Ͽ��� 
        {ENABLE,1,1,KeyBoardAdc_TimeTask},                                 // 18:����ɨ�裬Ĭ�Ͽ���
        {ENABLE,0,1,TaskMenuMessage_TimeTask},                             // 19:LCD������ʾ����ʾ��Ĭ�Ͽ���
        {ENABLE,0, SYSTICK_1SECOND,AreaManage_TimeTask},                   // 20:���� ��Ĭ�Ͽ���
        {DISABLE,0, SYSTICK_1SECOND,Media_TimeTask},                        // 21:ɾ����ý���ļ���Ĭ�Ͽ���
        {ENABLE,0,3*SYSTICK_0p1SECOND,Route_TimeTask},                     // 22:��·��Ĭ�Ͽ���
        {DISABLE,0,1*SECOND,ControlOil_TimeTask},                          // 23:����·��Ĭ�Ϲر�
        {DISABLE, 0, ICAUC_TIME_TICK, ICAUC_TimeTask},                     // 24:IC����֤TCP��������,Ĭ��ʹ���ǿ�����,��1���汾���������Ӳ����ƣ���˹ر�,dxl,2014.7.15
        {DISABLE,0,SECOND,NaviLcd_TimerTask},                              // 25: ������ ��Ĭ�Ͽ���
        {DISABLE, 0, SECOND, MultiCenterConnect_TimeTask},                 // 26:����������,�������ܲ���ʱʹ�� ��Ĭ�Ϲر�
        {DISABLE, 0, 1, GnssRealData_TimeTask},                             // 27������ʵʱ����͸����Ĭ�Ͽ���
        {DISABLE, 0, 5, TimerNopHandleTask},                                 // 28:�޴˹���
        {DISABLE, 0, 1, OpenAccount_TimeTask},                             // 29:���ٿ��� ��Ĭ�Ϲر�
        {DISABLE, 0, SECOND, Communication_CloseGprsTimeTask},             // 30:�ر�GPRS������ʹ��ǰ����һ ��Ĭ�Ͽ���
        {ENABLE,0,SECOND,Photo_TimeTask},                                  // 31:���������ƵĶ�ʱ������������,��Ĭ�Ͽ���
        {DISABLE,0,2,SleepDeep_TimeTask},                                  // 32:������ߣ�Ĭ�Ͽ���
        {DISABLE,0,2,Polygon_TimeTask},                                    // 33:����Σ�Ĭ�Ͽ���
        {DISABLE,0,1*SECOND,Updata_SendResultNoteTimeTask},                 // 34:�����������֪ͨ��Ĭ�Ͽ���
        {DISABLE,0,1*SECOND,Corner_TimeTask},                               // 35:�յ㲹����Ĭ�Ͽ���
        {DISABLE,0,2,LZM_PublicTimerTask},                                 // 36:����
        {DISABLE,0,SYSTICK_1SECOND,Link2Login_TimeTask},                   // 37:����2��¼��Ȩ��Ĭ�Ͽ���
        {ENABLE,0,1,Blind_TimeTask},                                       // 38:ä��������Ĭ�Ͽ���
        {ENABLE,0,1,StatusAlarm_TimeTask},                                 // 39:״̬�仯��������������ϱ����ģ�Ĭ�Ͽ���
                {ENABLE,    0, 1, VDRUsart_TimeTask},                                // 40:VDR ���ڽ���
                {ENABLE,    0, 1, VDRData_TimeTask},                                 // 41:VDR ���ݼ�¼�Ͷ�ȡ
                {ENABLE,    0, 1, VDRPulse_TimeTask},                                // 42:VDR �����ٶȺ��������
                {ENABLE,    0, 1, VDRPosition_TimeTask},                             // 43:VDR λ����Ϣ 
                {ENABLE,    0, 1, VDRSpeed_TimeTask},                                // 44:VDR ��ʻ�ٶȼ�¼
                {ENABLE,    0, 1, VDRDoubt_TimeTask},                                // 45:VDR �ɵ�����
                {ENABLE,    0, SECOND, VDROvertime_TimeTask},                        // 46:VDR ��ʱ��ʻ��¼
                {ENABLE, 0, SYSTICK_0p1SECOND, VDRCard_TimeTask},                  // 47:VDR ��ʻ����ݼ�¼
                {ENABLE,    0, 1, VDRPower_TimeTask},                                // 48:VDR �ⲿ�����¼
                {ENABLE,    0, SECOND, VDRSpeedStatus_TimeTask},                     // 49:VDR �ٶ�״̬
                {ENABLE,    0, SECOND, VDROverspeed_TimeTask},                       // 50:VDR ���ٱ���
                {DISABLE, 0, SECOND, VDRCoefficient_TimeTask},                      // 51:VDR ����ϵ��У׼  
                {DISABLE, 0, SECOND, VDRGprs_TimeTask},                            // 52:VDR ���߲ɼ���ʻ��¼������  
                */
        {ENABLE, 0, 1, Communication_TimeTask},                            // 0:����ͨ�ţ�Ĭ�Ͽ���
        {ENABLE, 0, SYSTICK_0p1SECOND, RadioProtocolParse_TimeTask},       // 1:����Э�������Ĭ�Ͽ���   
        {ENABLE, 0, 1,Io_TimeTask},                                        // 2:IO ״̬��⣬Ĭ�Ͽ��� 
        {ENABLE, 0, 1*SYSTICK_1SECOND, Gps_TimeTask},                      // 3:GPS���ݴ���Ĭ�Ͽ���
        {ENABLE,0,5*SYSTICK_0p1SECOND,GpsMile_TimeTask},                   // 4:GPS��̼��㣬Ĭ�Ͽ���
        {ENABLE,0,1,Report_TimeTask},                                      // 5:λ�û㱨��Ĭ�Ͽ���
        {ENABLE,1,1,Camera_TimeTask},                                      // 6:����ͷ���գ�Ĭ�Ͽ���         
        {DISABLE, 0, 1*SYSTICK_1SECOND, Media_UploadTimeTask},             // 7:�����ý�����ݵ�ƽ̨��Ĭ�Ϲر�
        {ENABLE,0,SYSTICK_1SECOND,HeartBeat_TimeTask},                     // 8:��������Ĭ�Ͽ���
        {ENABLE,0,SYSTICK_0p1SECOND,Usart3_TimeTask},                      // 9:USART3����3��ʱ��������,����ͷ�õĴ˴��ڣ�Ĭ�Ͽ���
        {DISABLE, 0, 1*SYSTICK_1SECOND, TimerNopHandleTask},               // 10: �޴˹���    
        {DISABLE, 0, 1, DelayTrigTimeTask},                                // 11:��ʱ������Ĭ�Ϲر�
        {DISABLE, 0, 1*SYSTICK_1SECOND, Reset_TimeTask},                    // 12:50Сʱ��ϵͳ������Ĭ�Ͽ���
        {ENABLE,0,3,BMA250E_TimeTask},                                      // 13:��ײ������⣬Ĭ�Ͽ���
        {DISABLE,0,SYSTICK_1SECOND,Link1Login_TimeTask},                   // 14:����1��¼��Ȩ��Ĭ�Ϲر� 
        {ENABLE,0,1,Usb_AppTimeTask},                                      // 15:USB��Ĭ�Ͽ���
        {ENABLE,0,1,Adc_CollectApp_TimeTask},                              // 16:AD�ɼ���Ĭ�Ͽ���
        {ENABLE,0,SYSTICK_1SECOND,Sleep_TimeTask},                         // 17:���� ��Ĭ�Ͽ��� 
        {ENABLE,1,1,KeyBoardAdc_TimeTask},                                 // 18:����ɨ�裬Ĭ�Ͽ���
        {ENABLE,0,1,TaskMenuMessage_TimeTask},                             // 19:LCD������ʾ����ʾ��Ĭ�Ͽ���
        {ENABLE,0, SYSTICK_1SECOND,AreaManage_TimeTask},                   // 20:���� ��Ĭ�Ͽ���
        {ENABLE,0, SYSTICK_1SECOND,Media_TimeTask},                        // 21:ɾ����ý���ļ���Ĭ�Ͽ���
        {ENABLE,0,3*SYSTICK_0p1SECOND,Route_TimeTask},                     // 22:��·��Ĭ�Ͽ���
        {DISABLE,0,1*SECOND,ControlOil_TimeTask},                          // 23:����·��Ĭ�Ϲر�
        {ENABLE, 0, 1, Card_TimeTask},                                     // 24:IC����֤TCP��������
        {ENABLE,0,SYSTICK_50ms,NaviLcd_TimerTask},                              // 25: ������ ��Ĭ�Ͽ���
        {DISABLE, 0, SECOND, MultiCenterConnect_TimeTask},                 // 26:����������,�������ܲ���ʱʹ�� ��Ĭ�Ϲر�
        {ENABLE, 0, 1, GnssRealData_TimeTask},                             // 27������ʵʱ����͸����Ĭ�Ͽ���
        {ENABLE,0,1,TTS_TimeTask},                                          // 28:TTS����
        {DISABLE, 0, 1, TimerNopHandleTask},                             // 29:���ٿ��� ��Ĭ�Ϲر�
        {DISABLE, 0, SECOND, TimerNopHandleTask},                         // 30:�ر�GPRS������ʹ��ǰ����һ ��Ĭ�Ͽ���
        {ENABLE,0,SECOND,Photo_TimeTask},                                  // 31:���������ƵĶ�ʱ������������,Ĭ�Ͽ���
        {DISABLE,0,2,SleepDeep_TimeTask},                                  // 32:������ߣ�Ĭ�Ͽ���
        {ENABLE,0,2,Polygon_TimeTask},                                    // 33:����Σ�Ĭ�Ͽ���
        {ENABLE,0,1*SECOND,Updata_SendResultNoteTimeTask},                 // 34:�����������֪ͨ��Ĭ�Ͽ���
        {ENABLE,0,1*SECOND,Corner_TimeTask},                               // 35:�յ㲹����Ĭ�Ͽ���
        {ENABLE,0,2,LZM_PublicTimerTask},                                 // 36:����
        {DISABLE,0,SYSTICK_1SECOND,Link2Login_TimeTask},                   // 37:����2��¼��Ȩ��Ĭ�Ϲر�
        {ENABLE,0,1,Blind_TimeTask},                                       // 38:ä��������Ĭ�Ͽ���
        {ENABLE,0,1,StatusAlarm_TimeTask},                                 // 39:״̬�仯��������������ϱ����ģ�Ĭ�Ͽ���

        {ENABLE,    0, 1, VDRUsart_TimeTask},                              // 40:VDR ���ڽ���
        {ENABLE,    0, 1, VDRData_TimeTask},                                 // 41:VDR ���ݼ�¼�Ͷ�ȡ
        {ENABLE,    0, 1, VDRPulse_TimeTask},                                // 42:VDR �����ٶȺ��������
        {ENABLE,    0, 1, VDRPosition_TimeTask},                             // 43:VDR λ����Ϣ 
        {ENABLE,    0, 1, VDRSpeed_TimeTask},                                // 44:VDR ��ʻ�ٶȼ�¼
        {ENABLE,    0, 1, VDRDoubt_TimeTask},                                // 45:VDR �ɵ�����
        {ENABLE,    0, SECOND, VDROvertime_TimeTask},                        // 46:VDR ��ʱ��ʻ��¼
        {ENABLE, 0, SYSTICK_0p1SECOND, VDRCard_TimeTask},                    // 47:VDR ��ʻ����ݼ�¼
        {ENABLE,    0, 1, VDRPower_TimeTask},                                // 48:VDR �ⲿ�����¼
        {ENABLE,    0, 1, VDRSpeedStatus_TimeTask},                          // 49:VDR �ٶ�״̬
        {ENABLE,    0, SECOND, VDROverspeed_TimeTask},                       // 50:VDR ���ٱ���
        {ENABLE, 0, SECOND, VDRCoefficient_TimeTask},                      // 51:VDR ����ϵ��У׼  
        {DISABLE, 0, 2*SECOND, VDRGprs_TimeTask},                            // 52:VDR ���߲ɼ���ʻ��¼������ 
        {ENABLE, 0, SECOND, VDRLog_TimeTask},                               // 53:VDR ��ӡVDR��־��DB9����
        {ENABLE, 0, 1, Can_TimeTask},                                       // 54:can���ݲɼ�
        {ENABLE, 0, SECOND, Lock1_TimeTask},                                // 55:ʹ��ǰ����һ
        {ENABLE, 0, SECOND, Lock2_TimeTask},                               // 56:ʹ��ǰ������
				{DISABLE, 0, 2*SECOND, VDRGprs_ResendTimeTask},                    // 57:VDR���ݷְ�����
				{ENABLE, 0, SECOND, PacketUpdate_TimeTask},                       // 58:�ְ�����
				{ENABLE,0,1*SECOND,JointechOiLCost_TimeTask },                   //59:��ͨ��λ������
				{ENABLE,0,1,Temperature_TimeTask},                               //60:�¶Ȳɼ��� 
				{ENABLE,0,SECOND,DeepPlow_TimeTask},                              //61:����
				{ENABLE,0,SECOND,TurnSpeed_TimeTask},                             //62:ũ��ת��
				{ENABLE,0,SECOND,Vss_TimeTask},                             //63:VSS�����ж�
				{ENABLE,0,SECOND,A2OBD_TimeTask},                             //64:A2OBDģ������		
				{DISABLE,0,SECOND,Acceleration_TimeTask},       //65:�����١������١���ת�䣬����gps�ٶȺ�gps����

};
			
EVEN_TASK MyEvTask[MAX_EVENTASK] = {                                 
                   {DISABLE, EvNopHandleTask},          // 0:����CAN����SendCanData_EvTask
                   {DISABLE, EvNopHandleTask},          // 1:CAN�������ݴ��� ,DecodeCan_EvTask
                   {DISABLE, Gps_EvTask},               // 2:GPS���ݴ��� Gps_EvTask
                   {DISABLE, EvNopHandleTask},          // 3:�ϱ�һ��λ����Ϣ 
                   {DISABLE, GetAkey_EvTask},           // 4: ��ȡ��Ȩ������ 
                   {DISABLE, ChangeServer_EvTask},      // 5: �л����������ƽ̨ 
                   {DISABLE, FtpUpdata_EvTask},         // 6: FTP�������� 
                   {DISABLE, Dialling_EvTask},          // 7: һ��ͨ�� 
                   {DISABLE, SevDial_EvTask},           // 8: �������ذε绰���� 
                   {DISABLE, EvRing},                           // 9:�������� 
                   {DISABLE, EvRingOver},                       // 10:��ֹ���� 
                   {DISABLE, Lcd_ShowText_EvTask},              // 11:��ʾ�ı���Ϣ  
                   {DISABLE, Lcd_ShowText_EvTask},              // 11:��ʾ�ı���Ϣ  
                   };
                                               
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : InitTimerTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitTimerTask(void)
{
  //u8 i;
  /*
  for(i = 1; i < MAX_TIMETASK; i++)
  {
    MyTimerTask[i].TaskTimerState = DISABLE;
  }
  */
  //���ӳ�ʼ������,��������ִ�к���ʱ��  
}

/*******************************************************************************
* Function Name  : SetTimerTask
* Description    : Setup timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTimerTask(TIMERTASK task, u32 time)
{
  MyTimerTask[task].TaskTimerState = ENABLE;
  MyTimerTask[task].TaskExTimer = time;
  MyTimerTask[task].TaskTimerCount = time + Timer_Val();
}

/*******************************************************************************
* Function Name  : ClrTimerTask
* Description    : Cancel timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClrTimerTask(TIMERTASK task)
{
  	MyTimerTask[task].TaskTimerState = DISABLE;
}

/*******************************************************************************
* Function Name  : ResumeTimerTask
* Description    : resume timer task .
* Input          : task MAX_TIMETASK: resume all task otherwise resume appointed task
* Output         : None
* Return         : None
*******************************************************************************/
void ResumeTimerTask(TIMERTASK task)
{
	u8 i,j,k;
	if (task > MAX_TIMETASK)return;
	if (task == MAX_TIMETASK)//�ָ���������
	{
		for(i = 0; i < MAX_TIMETASK; i++)
		{
			j = i/32;
			k = i%32;
			if(BIT_CHECK(TimeTaskResumeSta[j],k))
			{
				BIT_CLR(TimeTaskResumeSta[j],k);
				MyTimerTask[i].TaskTimerState = ENABLE;
			}
		}
	}
	else
	{
  	MyTimerTask[task].TaskTimerState = ENABLE;
	}
}

/*******************************************************************************
* Function Name  : SuspendTimerTask
* Description    : suspend timer task .
* Input          : task MAX_TIMETASK: suspend all task otherwise suspend appointed task
* Output         : None
* Return         : None
*******************************************************************************/
void SuspendTimerTask(TIMERTASK task)
{
	u8 i,j,k;
	if (task > MAX_TIMETASK)return;
	if (task == MAX_TIMETASK)//������������
	{
		for(i = 0; i < MAX_TIMETASK; i++)
		{
			if(MyTimerTask[i].TaskTimerState == ENABLE)
			{
				j = i/32;
				k = i%32;
				BIT_SET(TimeTaskResumeSta[j],k);
				MyTimerTask[i].TaskTimerState = DISABLE;
			}
		}
	}
	else
	{
  	MyTimerTask[task].TaskTimerState = DISABLE;
	}
}

/*******************************************************************************
* Function Name  : TimerTaskScheduler
* Description    : Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TimerTaskScheduler(void)  
{
  u8 i;
  u32 nowtimer;
  FunctionalState validflag;
  nowtimer = Timer_Val();
  for(i = 0; i < MAX_TIMETASK; i++)
  {
     if(MyTimerTask[i].TaskTimerState != ENABLE) 
     {
         continue; 
     }

    if(nowtimer >= MyTimerTask[i].TaskTimerCount)
    {
        MyTimerTask[i].TaskTimerCount = nowtimer + MyTimerTask[i].TaskExTimer;
        validflag = MyTimerTask[i].operate();
        MyTimerTask[i].TaskTimerState = validflag;   
    }
  }
}


/*******************************************************************************
* Function Name  : InitEvTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitEvTask(void)
{
  u8 i;
  
  for(i = 0; i < MAX_EVENTASK; i++)
  {
    MyEvTask[i].evTaskOnState = DISABLE;
  }
  //���ӳ�ʼ������
}

/*******************************************************************************
* Function Name  : SetEvTask
* Description    : Setup even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetEvTask(u8 ev)
{
  MyEvTask[ev].evTaskOnState = ENABLE;
}
/*******************************************************************************
* Function Name  : ClrEvTask
* Description    : Cancel ev task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClrEvTask(EVENTTASK ev)
{
  	MyEvTask[ev].evTaskOnState = DISABLE;
}
/*******************************************************************************
* Function Name  : ResumeEvTask
* Description    : resume ev task .
* Input          : ev MAX_EVENTASK: resume all task otherwise resume appointed task
* Output         : None
* Return         : None
*******************************************************************************/
void ResumeEvTask(EVENTTASK ev)
{
	u8 i;
	if (ev > MAX_EVENTASK)return;
	if (ev == MAX_EVENTASK)//������������
	{
		for(i = 0; i < MAX_EVENTASK; i++)
		{
			if(BIT_CHECK(EvTaskResumeSta,i))
			{
				BIT_CLR(EvTaskResumeSta,i);
				MyEvTask[i].evTaskOnState = ENABLE;
			}
		}
	}
	else
	{
  	MyEvTask[ev].evTaskOnState = ENABLE;
	}
}

/*******************************************************************************
* Function Name  : SuspendEvTask
* Description    : suspend ev task .
* Input          : ev MAX_EVENTASK: suspend all task otherwise suspend appointed task
* Output         : None
* Return         : None
*******************************************************************************/
void SuspendEvTask(EVENTTASK ev)
{
	u8 i;
	if (ev > MAX_EVENTASK)return;
	if (ev == MAX_EVENTASK)//������������
	{
		for(i = 0; i < MAX_EVENTASK; i++)
		{
			if(MyEvTask[i].evTaskOnState == ENABLE)
			{
				BIT_SET(EvTaskResumeSta,i);
				MyEvTask[i].evTaskOnState = DISABLE;
			}
		}
	}
	else
	{
  	MyEvTask[ev].evTaskOnState = DISABLE;
	}
}

/*******************************************************************************
* Function Name  : EvTaskScheduler
* Description    : Schedule even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EvTaskScheduler(void) 
{
  u8 i;

  for(i = 0; i < MAX_EVENTASK; i++)
  {
    if(MyEvTask[i].evTaskOnState != ENABLE)
      continue;
    MyEvTask[i].evTaskOnState = DISABLE;
    MyEvTask[i].operate();
   
  }
} 
/*******************************************************************************
* Function Name  : EvNopHandleTask
* Description    : ���¼����� .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState TimerNopHandleTask()
{
  
  return DISABLE;
}

/*******************************************************************************
* Function Name  : EvNopHandleTask
* Description    : ���¼����� .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EvNopHandleTask()
{

}
/*******************************************************************************
* Function Name  : SendCanData_EvTask
* Description    : ����CAN����
* Input          : None
* Output         : None
* Return         : None
* Note           :��Ҫ���͵�CAN���ݷ���RadioProtocolMiddleBuffer,����ΪRadioProtocolMiddleLen
*******************************************************************************/
void SendCanData_EvTask(void)
{
        //���÷���CAN���ݵĺ���
        
}
