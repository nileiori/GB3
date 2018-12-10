/*
********************************************************************************
*
*��Ȩ˵��   :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
*�ļ�����   :main.c     
*����       :������ 
*�汾��     :V1.0
*����ʱ��   :20140818
*��ע       :
*
********************************************************************************
*/

/*
********************************************************************************
*                               INCLUDES
********************************************************************************
*/
#include "include.h"
#include "VDR.h"
#include "main.h"

/*
********************************************************************************
*                          LOCAL VARIABLES
********************************************************************************
*/
//static TIME_T Time;
//static u32 TimeCount = 0;

/*
********************************************************************************
*                          GLOBAL VARIABLES
********************************************************************************
*/
const char ProductInfo[][17] =
{
    {"��Ʒ����:       "},
    {"������ʻ��¼��  "},
    {"��Ʒ�ͺ�:       "},
    {"EGS701          "},
    {"����汾:V1.03  "},
    {"("FIRMWARE_VERSION")"},
    {"�汾ʱ��:       "},
    {FIRMWARE_TIME},
		{"�ն�ID:         "},
		{"                "},
		{"                "},
		{"                "},
    {"������:         "},
    {"�������������¼�"},
    {"���������޹�˾  "},
    {"                "},
    {"��ַ:           "},
    {"����������������"},
    {"��·1�ű��ܿƼ� "},
		{"԰9��A��10¥    "},
};

//������ID Bytes:5,eeprom�����øò���,����ȡeeprom����ʱȡ���Ĭ��ֵ
const u8 ProductID[5]         ={ '7','0', '1', '0', '8'}; 

//�ն��ͺ�ID Bytes:20
const u8 TerminalTypeID[20]   ={'E', 'G', 'S','7', '0','1'} ;

//dxl,2014.6.5��̼�������صĳ���
const char MANUFACTURER[]     = "MANUFACTURER:EI";

//��2���ո�
const char PRODUCT_MODEL[]    = "PRODUCT MODEL:EGS701  ";

//��1���ո�
const char PCB_VERSION[]      = "PCB VERSION:A ";

//��main.h�е�FIRMWARE_VERSION����һ�£�ǰ��1λ���0,��5λһ�¡�
//const char SOFTWARE_VERSION[] = "SOFTWARE VERSION:010131";
const char SOFTWARE_VERSION[] = "SOFTWARE VERSION:0"FIRMWARE_VERSION;
/*
********************************************************************************
*                          EXTERN VARIABLES
********************************************************************************
*/
extern u8 FirmwareVersion[];//�̼��汾��,�̶�Ϊ5�ֽ�
extern u8  BBGNTestFlag;//0Ϊ����ģʽ��1Ϊ���깦�ܼ��ģʽ
extern u8      TerminalPhoneNumber[];//�ն��ֻ���
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
/**
  * @brief  ������
  * @param  None
  * @retval None
  */  
int main(void)      
{            
	             
    PeripheralInit();              //�����ʼ�� 
    
    ShowStartLogo();               //�ϵ���ʾLOG

    MemoryChipSelfCheck();         //�洢оƬ�Լ�
    
    PowerOnUpdata();               //�ϵ����һЩ����
    
    LZM_TaskMenuStart();           //�˵���ʼ��
	
    //VDRData_EraseAllFlashArea();
	
	  VDRData_Init();                //��ʻ��¼�Ǵ洢����ʼ��

    GPS_POWER_ON();                // �����ǹر���GPSģ�飬�����
    
    Io_PowerOnDetectMainPower();   //�����12V���绹��24V����
	
	  RTC_PowerOnHandle();
    
		SetTerminalFirmwareVersion();
	 
#ifdef   WATCHDOG_OPEN  //ֻ�ڽ���whileǰ��������Ϊ�ļ�ϵͳ������Щ�������Щ�������ʱ�ܾ�,dxl,2014.9.9
       //�������Ź�
    IDWG_Configuration(); 
#endif

  //  Modem_Api_Tts_Clvl(1);
	 
	 //Addr = FRAM_VDR_SPEED_STATUS_ADDR;
	 //Buffer[0] = 0;
	 //FRAM_BufferWrite(Addr, Buffer, FRAM_VDR_SPEED_STATUS_LEN);
	
	//Area_ClearRectArea(); 
    ReadRtc();//����RTCʱ��,dxl,2015.12.13
		
		
		
while(1)
{   
    TimerTaskScheduler();
    EvTaskScheduler();
    print_ctrl_task();
#ifdef   WATCHDOG_OPEN
    IWDG_ReloadCounter();
#endif
#ifdef SLEEP_OPEN
    __WFI();
#endif
}
}



