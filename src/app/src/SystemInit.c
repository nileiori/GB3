/********************************************************************
//版权说明  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//文件名称  :SystemInit.c       
//功能      :系统初始化 
//版本号    :
//开发人    :dxl
//开发时间  :2012.4
//修改者    :
//修改时间  :
//修改简要说明  :
//备注      :
***********************************************************************/
//***************包含文件*****************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
#include "VDR.h"
#include "Can_Driver.h"
//#include "Polygon.h"
//***************本地变量******************
//***************外部变量*****************
extern u8      MultiCenterLinkFlag;//多中心连接标准,1为已设置要求连接备份服务器
//extern u8 SdOnOffLineFlag;//SD卡插入标志,0未插入,1插入
extern u8   RadioProtocolRx1Buffer[];//连接1接收缓冲
extern Queue  RadioProtocolRx1Q;//连接1接收队列
extern u8   RadioProtocolRx2Buffer[];//连接2接收缓冲
extern Queue  RadioProtocolRx2Q;//连接2接收队列
extern u8   RadioProtocolRxSmsBuffer[];//短信接收缓冲
extern Queue  RadioProtocolRxSmsQ;//短信接收队列
extern u8  VdrTxBuffer[];//VDR发送队列缓冲
extern Queue  VdrTxQ;//VDR发送队列
extern Queue CanRxQ;//CAN1接收队列
//****************全局变量*****************
u8 FirmwareVersion[] = FIRMWARE_VERSION;//固件版本号,固定为5字节
//u8    FirstRunFlag = 0;//第1次使用标志
u8      IC_Reader_flag=0;//0表示老的PCB版本,1表示降成本后的PCB版本
//************** 宏定义*******************
#define USER_PLLI2S_N   426
#define USER_PLLI2S_R   4
//**************函数定义*******************
/*********************************************************************
//函数名称  :PowerOnUpdata
//功能      :上电更新变量
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :更新的变量有：IO、GPS、位置汇报、摄像头
*********************************************************************/
void PowerOnUpdata(void)
{
        //读第一次使用标志
    u8  Buffer[6];
    
    //更新IO变量
    Io_UpdataPram();
    //更新GPS变量
        Gps_PowerOnUpdataPosition();
    //更新心跳包时间间隔
    HeartBeat_UpdatePram();
    //更新速度类型变量
    SpeedFlagUpdatePram();
    
    //更新GPS模块开关变量
    UpdataAccOffGpsControlFlag();
     //区域初始化
    Area_ParameterInitialize(); 
    
  
    

    Buffer[0] = 0;
    EepromPram_ReadPram(E2_FIRST_POWERON_FLAG_ID, Buffer);
    if((0xff == Buffer[0])||(0 == Buffer[0]))
    {
        EepromPram_DefaultSet();
        Buffer[0] = 1;
        EepromPram_WritePram(E2_FIRST_POWERON_FLAG_ID, Buffer, 1);
        
    }
    //更新手机号码
    RadioProtocol_UpdateTerminalPhoneNum();

        //调度屏初始化
        NaviLcd_ParameterInitialize();
        
    //录音任务初始化
//    RecordTask_Init(); 

        ///////快速开户初始化/////////////////
     //   OpenAccount_ParameterInitialize();
        
        //读多中心连接标志,行标检测时使用
        //FRAM_BufferRead(&MultiCenterLinkFlag, FRAM_MULTI_CENTER_LINK_LEN, FRAM_MULTI_CENTER_LINK_ADDR);

    //深度休眠初始化 dxl,2016.8.9深度休眠有些bug,关闭该功能
//    SleepDeep_ParameterInitialize();
        
        Polygon_ParameterInitialize();
        //断油路控制
        OilControl_ParameterInitialize();
        //更新拐点补传参数
        Corner_UpdataPram();
       
    
        //无线协议解析建立接收队列
        RadioProtocolRx1Q = QueueCreate1(RADIO_PROTOCOL_RX1_BUFFER_SIZE);
        RadioProtocolRx2Q = QueueCreate2(RADIO_PROTOCOL_RX2_BUFFER_SIZE);
        RadioProtocolRxSmsQ = QueueCreate3(RADIO_PROTOCOL_RX_SMS_BUFFER_SIZE);
				//DB9发送队列
				VdrTxQ = QueueCreate4(VDR_TX_Q_BUFFER_SIZE);
				//CAN1接收队列
				CanRxQ = QueueCreate5(CAN_RX_BUFFER_SIZE);
				
}
/*********************************************************************
//函数名称  :MemoryChipSelfCheck
//功能      :上电存储芯片自检
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :自检的存储芯片有：FRAM、FLASH、EEPROM
*********************************************************************/
void MemoryChipSelfCheck(void)
{

    u8  flag = 0;
    u16 i;  
    u32 Delay = 0;
        
    if((ERROR == CheckFlashChip())&&(0 == flag))//检查flash
    {
        for(i=0; i<10000; i++)
        {
            //延时一段时间,再次检查
        }
        if((ERROR == CheckFlashChip())&&(0 == flag))//检查flash
        {
            flag = 1;
        }
        
    }
    IWDG_ReloadCounter();//喂狗
    if((ERROR ==  CheckFramChip())&&(0 == flag))//检查fram
    {
        for(i=0; i<10000; i++)
        {
            //延时一段时间,再次检查
        }
        if((ERROR ==  CheckFramChip())&&(0 == flag))//检查fram
        {
            flag = 2;
        }
    }
        
    IWDG_ReloadCounter();//喂狗
    if((ERROR ==  CheckEepromChip())&&(0 == flag))//检查eeprom
    {
        for(i=0; i<10000; i++)
        {
            //延时一段时间,再次检查
        }
        if((ERROR ==  CheckEepromChip())&&(0 == flag))//检查eeprom
        {
            flag = 3;
        }
        
    }
    IWDG_ReloadCounter();//喂狗
    if(1 == flag)//FLASH错误
    {
        LcdClearScreen(); 
        LcdShowCaptionEx((char *)"flash自检错误",2); 
        Delay  = Timer_Val();
        while((Timer_Val() - Delay) < (20*600));//延时10分钟

        while(1)
        {
                    //发送错误信息给导航屏
            
        }
    }
    else if(2 == flag)//FRAM错误
    {
        LcdClearScreen(); 
        LcdShowCaptionEx((char *)"fram自检错误",2); 
        Delay  = Timer_Val();
        while((Timer_Val() - Delay) < (20*600));//延时10分钟

        while(1)
        {
                   //发送错误信息给导航屏 
            
        }
    }
    else if(3 == flag)//EEPROM错误
    {
        LcdClearScreen(); 
        LcdShowCaptionEx((char *)"eeprom自检错误",2); 
        Delay  = Timer_Val();
        while((Timer_Val() - Delay) < (20*600));//延时10分钟
        
        while(1)
        {
          //发送错误信息给导航屏  
            
        }
    }
}
/*********************************************************************
//函数名称  :PeripheralInit
//功能      :外设初始化
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :
//备注      :初始化的外设有：GPS、GPRS、FRAM、FLASH、EEPROM、摄像头
*********************************************************************/
#if DISK_MODE == DISK_MODE_USE_FLASH
FIL flash_file;
u8	flash_buff_in[16] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
u8	flash_buff_out[16];
#endif
FRESULT	res_flash;
void PeripheralInit(void)
{
	
	#if DISK_MODE == DISK_MODE_USE_FLASH
	UINT Bw;
	#endif
  u32 time;
  u32     Delay;
  RCC_ClocksTypeDef RCC_ClocksStatus;
    
  for(Delay=0; Delay<100; Delay++)
  {
        
  }
    
  RCC_Configuration();   
  RCC_GetClocksFreq(&RCC_ClocksStatus);
     
  time = RCC_ClocksStatus.SYSCLK_Frequency/20;
  SysTick_Config(time);//50ms one time

  NVIC_Configuration();
     
  Delay  = Timer_Val();
  while((Timer_Val() - Delay) < 50){;}//上电延时0.5s
   
  InitRtc();
        
  Ad_Init();
    
  Io_Init();
     
  SD_POWER_OFF();//先关闭SD卡电源供电
	time  = Timer_Val();
  while((Timer_Val() - time) < 5);//延时0.1s
        
  //硬件复位显示屏,使用的是USB电源控制引脚
  USB_SW_Init();
  USB_SW_0();
  Delay  = Timer_Val();
  while((Timer_Val() - Delay) < 5){}//上电延时0.25s
    USB_SW_1();
  Delay  = Timer_Val();
  while((Timer_Val() - Delay) < 5){}//上电延时0.25
  Lcd_Initial();
        
  //FLASH模块初始化 
  #if DISK_MODE != DISK_MODE_USE_FLASH
  sFLASH_DeInit();
  sFLASH_Init();
  sFLASH_AllUnprotect();
  #endif
  //EEPROM模块初始化    
  LcdClearScreen(); 
  LcdShowCaptionEx((char *)"eeprom初始化",2); 
  E2prom_InitIO();
      
  UpdatePeripheralType();//更新外设1和外设2类型变量,dxl,2014.12.26,考虑到其它模块会用到外设1类型或外设2类型,因此放在最前面
    
  //FRAM模块初始化
  LcdClearScreen(); 
  LcdShowCaptionEx((char *)"fram初始化",2); 
  FRAM_Init();
        
       
   //GPS模块初始化,使用串口4
  LcdClearScreen(); 
  LcdShowCaptionEx((char *)"gps初始化",2); 
  Gps_Init();
         
  //DB9串口初始化,使用串口1,标准要求115200波特率
  //COM1_Init(115200);
  //LOG_SetUsart( COM1 );
    VDRUsart_Init();
    
     
  //打印机初始化
  LcdClearScreen(); 
  LcdShowCaptionEx((char *)"printer初始化",2); 
  print_init();
 
  //USB初始化
  LcdClearScreen(); 
  LcdShowCaptionEx((char *)"usb初始化",2); 
  USB_AppInitialize();
    
    //串口6未使用
    
    //检测PD5引脚，以该引脚判断是降成本后的硬件PCB还是降成本前的硬件PCB,
    //dxl,注意该口为共用口，同时用作麦克风控制，需先配置为输入，然后配置为输出
    //while((Timer_Val() - Delay) < 2);//上电延时0.1s,mq,2014.5.10
  
    
  SD_POWER_ON();//给SD卡供电
  time  = Timer_Val();
  while((Timer_Val() - time) < 10);//延时0.1s


        
 /* LcdClearScreen(); 
  LcdShowCaptionEx((char *)"ICCard初始化",2); 
  ICCARD_M3_Init();
  */
    //GPRS模块初始化,使用串口5
  LcdClearScreen(); 
  LcdShowCaptionEx((char *)"gprs初始化",2); 
  Communication_Init();
     
  //休眠初始化,主要是备电充电控制
  LcdClearScreen(); 
  LcdShowCaptionEx((char *)"sleep初始化",2); 
    Sleep_Init();
         
    
    //摄像头模块初始化,使用串口3
   LcdClearScreen(); 
   LcdShowCaptionEx((char *)"camera初始化",2); 
   Camera_Init();

   //线路初始化
  LcdClearScreen(); 
  LcdShowCaptionEx((char *)"route初始化",2); 
  InitRoute();

  LcdClearScreen(); 
  LcdShowCaptionEx((char *)"Navilcd初始化",2); 
  NaviLcd_ParameterInitialize(); 
        
  //GNSS详细信息采集初始
  InitConfigGNSS();

  Public_ParameterInitialize();
   
   //为编译时不把MANUFACTURER常量数组优化掉（去掉了）而增加以下代码,dxl,2014.6.5
   (void)&MANUFACTURER;
   (void)&PRODUCT_MODEL;
   (void)&PCB_VERSION;
   (void)&SOFTWARE_VERSION;
         
        //CAN总线初始化
    LcdClearScreen(); 
    LcdShowCaptionEx((char *)"can初始化",2);                             
    //CAN_TimeTask_Init(); dxl,2016.5.20
        
         
    // SD SPI初始
         
    Media_CheckSdOnOff();//dxl,2014.6.18,检测SD卡在不在需要放在InitSDFile之前,因为在InitSDFile里面还会检测SD卡是否损坏，若损坏了等同于不在
    LcdClearScreen(); 
    LcdShowCaptionEx((char *)"sd初始化",2); 
    SPIx_Init(); 
    res_flash = f_mount(1,&fs);
		if (res_flash == FR_OK)
		#if DISK_MODE == DISK_MODE_USE_FLASH
		{
			res_flash = f_mkfs(1,1,4096);//格式化文件系统
			if (res_flash == FR_OK)
			{
				res_flash = f_mount(NULL,&fs);//取消挂载
				res_flash = f_mount(1,&fs);//重新挂载
				res_flash = f_open(&flash_file,ff_NameConver((u8 *)"1:hello.txt"),FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
				if(FR_OK == f_write(&flash_file,flash_buff_in,16,&Bw))
				{
					res_flash = f_lseek(&flash_file,0);
					f_read(&flash_file,flash_buff_out,10,&Bw);
					f_read(&flash_file,flash_buff_out+10,6,&Bw);
					f_close(&flash_file);
				}
			}
		}
		#else
		{
    	InitSDFile();//程序在此处计数次进入会启动不了，偶数次可以
    }
		#endif
    Media_FolderCheck();
		
		UpdateRunMode();
		
		BEEP_ON();
		time  = Timer_Val();
    while((Timer_Val() - time) < 10);//延时0.1s
		BEEP_OFF();
  
}
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
    SystemInit();
        
    /******************************************************************************/
    /*            I2S clock configuration (For devices Rev B and Y)               */
    /******************************************************************************/
    /* PLLI2S clock used as I2S clock source */
    //  RCC->CFGR &= ~RCC_CFGR_I2SSRC;

        /* Configure PLLI2S */
    //  RCC->PLLI2SCFGR = (USER_PLLI2S_N << 6) | (USER_PLLI2S_R << 28);

        /* Enable PLLI2S */
    //  RCC->CR |= ((uint32_t)RCC_CR_PLLI2SON);

        /* Wait till PLLI2S is ready */
    //  while((RCC->CR & RCC_CR_PLLI2SRDY) == 0)
    //  {
            
    //  }
}
/*******************************************************************************
* Function Name  : IDWG_Configuration
* Description    : Configures IDWG clock source and prescaler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IDWG_Configuration(void)
{
#ifdef WATCHDOG_OPEN
/* Enable the LSI oscillator ************************************************/
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
  //IWDG_SetPrescaler(IWDG_Prescaler_32);
    IWDG_SetPrescaler(IWDG_Prescaler_128);/*40KHz(LSI) / 128 = 320Hz */
    
  /* Set counter reload value to 2500 */
  IWDG_SetReload(0x9c4);//2秒
    //IWDG_SetReload(0x4E2);//1秒
  //IWDG_SetReload(0x271);//0.5秒
  //IWDG_SetReload(0xFA);//0.2秒


  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();  
#endif
}

/*******************************************************************************
* Function Name  : PWR_PVDInit
* Description    : 电源PVD初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PWR_PVDInit(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    
    PWR_PVDLevelConfig(PWR_PVDLevel_2V9);
    PWR_PVDCmd(ENABLE);
    EXTI_StructInit(&EXTI_InitStructure);
    EXTI_InitStructure.EXTI_Line = EXTI_Line16;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
}
/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration()
{
  NVIC_InitTypeDef NVIC_InitStructure;


  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);   

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
//#ifdef  VECT_TAB_RAM  
    /* Set the Vector Table base location at 0x20000000 */ 
 //   NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
//#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */ 
  //  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x1000);   
//#endif 
  

/* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the USART3 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
   /* Enable the UART4 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the UART5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the USART6 Interrupt */
 // NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //NVIC_Init(&NVIC_InitStructure);
  
  //Enable the RTC Interrupt 
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
 // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
 // NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
}
/*******************************************************************************
* Function Name  : USB_SW_Init
* Description    : USB供电引脚初始化，该引脚用于硬件复位LCD屏
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void   USB_SW_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
  
/******************************IO***********************************/    
    RCC_APB2PeriphClockCmd(  RCC_USB_SW , ENABLE);         /* 打开GPIO时钟 */

    GPIO_InitStructure.GPIO_Pin = PIN_USB_SW  ;
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        /* 快速模式 */ 
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;             /* 输出/输入 */
    
    //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           /* 开漏 */
    
  //  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             /* 上拉 */
    
    GPIO_Init( GPIO_USB_SW , &GPIO_InitStructure);
    
    USB_SW_1();
}
