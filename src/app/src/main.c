/*
********************************************************************************
*
*版权说明   :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
*文件名称   :main.c     
*功能       :主函数 
*版本号     :V1.0
*创建时间   :20140818
*备注       :
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
    {"产品名称:       "},
    {"汽车行驶记录仪  "},
    {"产品型号:       "},
    {"EGS701          "},
    {"软件版本:V1.03  "},
    {"("FIRMWARE_VERSION")"},
    {"版本时间:       "},
    {FIRMWARE_TIME},
		{"终端ID:         "},
		{"                "},
		{"                "},
		{"                "},
    {"制造商:         "},
    {"深圳市伊爱高新技"},
    {"术开发有限公司  "},
    {"                "},
    {"地址:           "},
    {"深圳市龙华新区清"},
    {"祥路1号宝能科技 "},
		{"园9栋A座10楼    "},
};

//制造商ID Bytes:5,eeprom可设置该参数,当读取eeprom错误时取这个默认值
const u8 ProductID[5]         ={ '7','0', '1', '0', '8'}; 

//终端型号ID Bytes:20
const u8 TerminalTypeID[20]   ={'E', 'G', 'S','7', '0','1'} ;

//dxl,2014.6.5与固件升级相关的常量
const char MANUFACTURER[]     = "MANUFACTURER:EI";

//含2个空格
const char PRODUCT_MODEL[]    = "PRODUCT MODEL:EGS701  ";

//含1个空格
const char PCB_VERSION[]      = "PCB VERSION:A ";

//与main.h中的FIRMWARE_VERSION保持一致，前面1位填充0,后5位一致。
//const char SOFTWARE_VERSION[] = "SOFTWARE VERSION:010131";
const char SOFTWARE_VERSION[] = "SOFTWARE VERSION:0"FIRMWARE_VERSION;
/*
********************************************************************************
*                          EXTERN VARIABLES
********************************************************************************
*/
extern u8 FirmwareVersion[];//固件版本号,固定为5字节
extern u8  BBGNTestFlag;//0为正常模式，1为部标功能检测模式
extern u8      TerminalPhoneNumber[];//终端手机号
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
  * @brief  主函数
  * @param  None
  * @retval None
  */  
int main(void)      
{            
	             
    PeripheralInit();              //外设初始化 
    
    ShowStartLogo();               //上电显示LOG

    MemoryChipSelfCheck();         //存储芯片自检
    
    PowerOnUpdata();               //上电更新一些变量
    
    LZM_TaskMenuStart();           //菜单初始化
	
    //VDRData_EraseAllFlashArea();
	
	  VDRData_Init();                //行驶记录仪存储区初始化

    GPS_POWER_ON();                // 上面是关闭了GPS模块，在这打开
    
    Io_PowerOnDetectMainPower();   //检测是12V供电还是24V供电
	
	  RTC_PowerOnHandle();
    
		SetTerminalFirmwareVersion();
	 
#ifdef   WATCHDOG_OPEN  //只在进入while前开启，因为文件系统操作有些情况下有些函数会耗时很久,dxl,2014.9.9
       //开启看门狗
    IDWG_Configuration(); 
#endif

  //  Modem_Api_Tts_Clvl(1);
	 
	 //Addr = FRAM_VDR_SPEED_STATUS_ADDR;
	 //Buffer[0] = 0;
	 //FRAM_BufferWrite(Addr, Buffer, FRAM_VDR_SPEED_STATUS_LEN);
	
	//Area_ClearRectArea(); 
    ReadRtc();//更新RTC时间,dxl,2015.12.13
		
		
		
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



