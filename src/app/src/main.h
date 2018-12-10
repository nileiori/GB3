
#ifndef __MAIN_H
#define __MAIN_H

//*********宏定义**************** 
//#define WATCHDOG_OPEN 1
//#define SLEEP_OPEN    2
//#define DEBUG_OPEN    3
//#define HUOYUN_DEBUG_OPEN  1//货运平台联调启用该宏，正常发货该宏要屏蔽，不开启
//#define HUOYUN 1//行驶记录仪分包固定900字节需要开启该宏，正常发货该宏要屏蔽，不开启

#define LOG_TYPE 0   //打印输出调试信息,正常出货时应为0:0-忽略,1-输出

//#define GPS_MODE_TYPE_UM330   //只能二选一
//#define GPS_MODE_TYPE_TD3017  
#define GPS_MODE_TYPE_ATGM332D


#define  BEEP_ON()          GpioOutOn(BUZZER);BeepOnOffFlag = 1;BeepOnTime=Timer_Val()//实际应用使用
//#define  BEEP_ON()        GpioOutOff(BUZZER);BeepOnOffFlag = 1;BeepOnTime=Timer_Val()//调试时静音

//#define GPRS_DATA_PRINT_TO_DB9  1//把GPRS交互指令（7E开头，7E结尾的指令）打印到DB9串口

#define BBTEST_FIRMWARE_VERSION     "30105"//提交给检测平台升级的固件版本
#define FIRMWARE_VERSION     "30104"//
#define FIRMWARE_TIME        "201811291155" //编译时间

#define Recorder_CCC_ID             "C000116"          /*7个字节*/
#define Recorder_Product_VER        "EGS701          " /*16个字节*/

extern const char ProductInfo[][17]; 
extern const char MANUFACTURER[];
extern const char PRODUCT_MODEL[];//含2个空格
extern const char PCB_VERSION[];//含1个空格
extern const char SOFTWARE_VERSION[];//与main.h中的FIRMWARE_VERSION保持一致，前面1位填充0,后5位一致。
//*********函数声明**************
#endif
