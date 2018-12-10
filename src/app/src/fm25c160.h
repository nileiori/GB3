#ifndef __FRAM_H
#define __FRAM_H

//*********************头文件********************************
#include "stm32f10x.h"
#include "spi_flash.h"

//*********************自定义数据类型************************


//**********************宏定义********************************
//******************铁电空间分配******************
#define   FRAM_PHONE_MONTH_PARAM_ADDR                      0//每月通话参数
#define   FRAM_PHONE_MONTH_PARAM_LEN                       12//每月通话参数长度

#define     FRAM_LAST_LOCATION_ADDR                          13 //上一个有效定位点
#define     FRAM_LAST_LOCATION_LEN                           11 //上一个有效定位点


#define     FRAM_MEDIA_ID_ADDR                               25 //多媒体ID
#define     FRAM_MEDIA_ID_LEN                                4

#define   FRAM_PARAMETER_TIME_PHOTO_CHANNEL_ADDR           58//参数定时拍照通道
#define   FRAM_PARAMETER_TIME_PHOTO_CHANNEL_LEN            1

#define   FRAM_PARAMETER_DISTANCE_PHOTO_CHANNEL_ADDR       60//参数定距拍照通道
#define   FRAM_PARAMETER_DISTANCE_PHOTO_CHANNEL_LEN        1

#define   FRAM_PHOTO_ID_ADDR                               62
#define   FRAM_PHOTO_ID_LEN                                4

#define     FRAM_TEMP_TRACK_NUM_ADDR                             67 //临时跟踪剩余次数
#define     FRAM_TEMP_TRACK_NUM_LEN                              4

#define     FRAM_TEMP_TRACK_SPACE_ADDR                         72   //临时跟踪间隔
#define     FRAM_TEMP_TRACK_SPACE_LEN                            2  

#define     FRAM_EMERGENCY_FLAG_ADDR                             75 //紧急报警上一次状态
#define     FRAM_EMERGENCY_FLAG_LEN                              1

#define     FRAM_RECORD_CONTROL_ADDR                             77 //录音控制标志
#define     FRAM_RECORD_CONTROL_LEN                              1

#define     FRAM_RECORD_CLOSE_ADDR                               79 //关录音标志
#define     FRAM_RECORD_CLOSE_LEN                                  1

#define     FRAM_RECORD_OPEN_ADDR                                  81   //开录音标志
#define     FRAM_RECORD_OPEN_LEN                                   1

#define     FRAM_TTS_VOLUME_ADDR                                   83   //TTS音量
#define     FRAM_TTS_VOLUME_LEN                              1

#define     FRAM_TEL_VOLUME_ADDR                             85 //电话音量
#define     FRAM_TEL_VOLUME__LEN                             1

#define   FRAM_OIL_CTRL_ADDR                               87 //断油路
#define   FRAM_OIL_CTRL_ADDR_LEN                           1

#define   FRAM_CAR_OWNER_PHONE_ADDR                        89//车主手机号,字符串ASCII码,dxl
#define   FRAM_CAR_OWNER_PHONE_LEN                         12//写入或读取该项参数时长度需填写12,位数不足时后补0x00

#define   FRAM_MAIN_DOMAIN_NAME_ADDR                       101//主监控中心域名,字符串,ASCII码,为行标送检而添加,dxl
#define   FRAM_MAIN_DOMAIN_NAME_LEN                        30//写入或读取该项参数时长度需填写30,位数不足时后补0x00

#define   FRAM_BACKUP_DOMAIN_NAME_ADDR                     132//备份监控中心域名,字符串,ASCII码,为行标送检而添加,dxl
#define   FRAM_BACKUP_DOMAIN_NAME_LEN                      30//写入或读取该项参数时长度需填写30,位数不足时后补0x00

#define   FRAM_CAR_CARRY_STATUS_ADDR                       163//车辆载货状态,1字节,dxl
#define   FRAM_CAR_CARRY_STATUS_LEN                        1//写入或读取该参数时长度需为1

#define   FRAM_QUICK_OPEN_ACCOUNT_ADDR                     165//快速开户锁定状态存储地址
#define   FRAM_QUICK_OPEN_ACCOUNT_LEN                      1//写入或读取该参数时长度需为1

#define   FRAM_MULTI_CENTER_LINK_ADDR                      167//多中心连接标志
#define   FRAM_MULTI_CENTER_LINK_LEN                       1//多中心连接标志长度

#define   FRAM_FIRMWARE_UPDATA_FLAG_ADDR                   169//升级任务触发标志，等于1表示触发了远程升级，等于2表示升级成功，
                                                            //等于3表示升级失败，等于4表示已发送过0x0108命令了，每次上电鉴权完成后需要读取该值，
#define   FRAM_FIRMWARE_UPDATA_FLAG_LEN                    1

#define   FRAM_GNSS_SAVE_STEP_ADDR                         171//Gnss盲区补报的步数
#define   FRAM_GNSS_SAVE_STEP_LEN                          2

#define   FRAM_FIRMWARE_UPDATA_TYPE_ADDR                   174//远程升级类型,0为终端,9为载重控制器,目前只支持这两个
#define   FRAM_FIRMWARE_UPDATA_TYPE_LEN                    1

#define   FRAM_ROLL_OVER_INIT_ANGLE_ADDR                   176//侧翻初始角度
#define   FRAM_ROLL_OVER_INIT_ANGLE_LEN                    4

#define     FRAM_CAM_TIME_PHOTO_NUM_ADDR                    185 //平台定时拍照剩余张数
#define     FRAM_CAM_TIME_PHOTO_NUM_LEN                      2//4路摄像头共占用12字节,每路3字节,从1到4,顺序排列

#define     FRAM_CAM_TIME_PHOTO_SPACE_ADDR                  197 //平台定时拍照时间间隔，单位秒
#define     FRAM_CAM_TIME_PHOTO_SPACE_LEN                    2//4路摄像头共占用12字节,每路3字节,从1到4,顺序排列

#define     FRAM_CAM_TIME_PHOTO_STORE_FLAG_ADDR             209 //平台定时拍照保存标志,bit0置1表示需要上传，bit1置1表示需要保存至SD卡
#define     FRAM_CAM_TIME_PHOTO_STORE_FLAG_LEN               1//4路摄像头共占用8字节,每路2字节,从1到4,顺序排列

#define     FRAM_CAM_TIME_PHOTO_RESOLUTION_ADDR             217 //平台定时拍照分辨率,0为320*240,1为640*480
#define     FRAM_CAM_TIME_PHOTO_RESOLUTION_LEN               1//4路摄像头共占用8字节,每路2字节,从1到4,顺序排列

#define     FRAM_CAM_TIME_PHOTO_EVENT_TYPE_ADDR             225 //平台定时拍照事件类型
#define     FRAM_CAM_TIME_PHOTO_EVENT_TYPE_LEN               1//4路摄像头共占用8字节,每路2字节,从1到4,顺序排列

#define     FRAM_CAM_TIME_PHOTO_CHANNEL_ADDR                233 //平台定时拍照上传通道，只能为CHANNEL_DATA_1或者是CHANNEL_DATA_2
#define     FRAM_CAM_TIME_PHOTO_CHANNEL_LEN                  1//4路摄像头共占用8字节,每路2字节,从1到4,顺序排列

#define     FRAM_CAM_EVENT_PHOTO_NUM_ADDR                   241 //事件拍照剩余张数
#define     FRAM_CAM_EVENT_PHOTO_NUM_LEN                     2//4路摄像头共占用12字节,每路3字节,从1到4,顺序排列

#define     FRAM_CAM_EVENT_PHOTO_SPACE_ADDR                 253 //事件拍照时间间隔，单位秒
#define     FRAM_CAM_EVENT_PHOTO_SPACE_LEN                   2//4路摄像头共占用12字节,每路3字节,从1到4,顺序排列 

#define     FRAM_CAM_EVENT_PHOTO_STORE_FLAG_ADDR            265 //事件拍照保存标志,bit0置1表示需要上传，bit1置1表示需要保存至SD卡
#define     FRAM_CAM_EVENT_PHOTO_STORE_FLAG_LEN              1//4路摄像头共占用8字节,每路2字节,从1到4,顺序排列

#define     FRAM_CAM_EVENT_PHOTO_RESOLUTION_ADDR            273 //事件拍照分辨率,0为320*240,1为640*480
#define     FRAM_CAM_EVENT_PHOTO_RESOLUTION_LEN              1//4路摄像头共占用8字节,每路2字节,从1到4,顺序排列

#define     FRAM_CAM_EVENT_PHOTO_EVENT_TYPE_ADDR            281 //事件拍照事件类型
#define     FRAM_CAM_EVENT_PHOTO_EVENT_TYPE_LEN              1//4路摄像头共占用8字节,每路2字节,从1到4,顺序排列 

#define     FRAM_CAM_EVENT_PHOTO_CHANNEL_ADDR               289 //事件拍照上传通道，只能为CHANNEL_DATA_1或者是CHANNEL_DATA_2
#define     FRAM_CAM_EVENT_PHOTO_CHANNEL_LEN                 1//4路摄像头共占用8字节,每路2字节,从1到4,顺序排列


#define   FRAM_VDR_DOUBT_DATA_ADDR                           298//150个点
#define   FRAM_VDR_DOUBT_DATA_LEN                                    2

#define   FRAM_VDR_DOUBT_TIME_ADDR                           756
#define   FRAM_VDR_DOUBT_TIME_LEN                                  4

#define   FRAM_VDR_DOUBT_COUNT_ADDR                          861
#define   FRAM_VDR_DOUBT_COUNT_LEN                             2

#define   FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR                  864
#define   FRAM_VDR_SPEED_15_MINUTE_SPEED_TOTAL_LEN           96//总长度96,每分钟占6字节（4字节时间+1字节速度+1字节校验）

#define   FRAM_VDR_SPEED_15_MINUTE_SPEED_COUNT_ADDR        969
#define   FRAM_VDR_SPEED_15_MINUTE_SPEED_COUNT_LEN             4

#define   FRAM_VDR_SPEED_STOP_TIME_ADDR                        974
#define   FRAM_VDR_SPEED_STOP_TIME_LEN                         4

#define   FRAM_VDR_MILEAGE_BACKUP_ADDR                         979
#define   FRAM_VDR_MILEAGE_BACKUP_LEN                            4

#define   FRAM_VDR_MILEAGE_ADDR                                  984    
#define   FRAM_VDR_MILEAGE_LEN                                   4  

#define   FRAM_VDR_OVERTIME_LOGOUT_TIME_ADDR                   989
#define   FRAM_VDR_OVERTIME_LOGOUT_TIME_LEN                  4

#define   FRAM_VDR_OVERTIME_LOGIN_TIME_ADDR                  994
#define   FRAM_VDR_OVERTIME_LOGIN_TIME_LEN                   4

#define   FRAM_VDR_OVERTIME_CURRENT_NUM_ADDR                   999
#define   FRAM_VDR_OVERTIME_CURRENT_NUM_LEN                  1

#define   FRAM_VDR_OVERTIME_LICENSE_ADDR                         1001
#define   FRAM_VDR_OVERTIME_LICENSE_LEN                        18

#define   FRAM_VDR_OVERTIME_START_TIME_ADDR                  1020
#define   FRAM_VDR_OVERTIME_START_TIME_LEN                   4

#define   FRAM_VDR_OVERTIME_END_TIME_ADDR                        1025
#define   FRAM_VDR_OVERTIME_END_TIME_LEN                         4

#define   FRAM_VDR_OVERTIME_START_POSITION_ADDR              1030
#define   FRAM_VDR_OVERTIME_START_POSITION_LEN               10

#define   FRAM_VDR_OVERTIME_END_POSITION_ADDR                  1041
#define   FRAM_VDR_OVERTIME_END_POSITION_LEN                   10

#define   FRAM_VDR_OVERTIME_NIGHT_START_TIME_ADDR              1052
#define   FRAM_VDR_OVERTIME_NIGHT_START_TIME_LEN               4

#define   FRAM_VDR_OVERTIME_NIGHT_END_TIME_ADDR              1057
#define   FRAM_VDR_OVERTIME_NIGHT_END_TIME_LEN               4

#define   FRAM_VDR_OVERTIME_NIGHT_START_POSITION_ADDR        1062
#define   FRAM_VDR_OVERTIME_NIGHT_START_POSITION_LEN         10

#define   FRAM_VDR_OVERTIME_NIGHT_END_POSITION_ADDR        1073
#define   FRAM_VDR_OVERTIME_NIGHT_END_POSITION_LEN         10

#define   FRAM_VDR_OVERTIME_ONE_DAY_DRIVE_TIME_ADDR          1084
#define   FRAM_VDR_OVERTIME_ONE_DAY_DRIVE_TIME_LEN           4

#define   FRAM_RTC_TIME_ADDR                                    1104
#define   FRAM_RTC_TIME_LEN                                     4
#define   FRAM_RTC_TIME_BACKUP_ADDR                             1109
#define   FRAM_RTC_TIME_BACKUP_LEN                              4

#define   FRAM_VDR_LOG_COUNT_ADDR                          1114
#define   FRAM_VDR_LOG_COUNT_LEN                           4
#define   FRAM_VDR_LOG_COUNT_BACKUP_ADDR                   1119
#define   FRAM_VDR_LOG_COUNT_BACKUP_LEN                    4

#define FRAM_VDR_SPEED_STATUS_ENABLE_ADDR                                   1124
#define FRAM_VDR_SPEED_STATUS_ENABLE_LEN                                    1

#define FRAM_VDR_SPEED_STATUS_ADDR                                        1126
#define FRAM_VDR_SPEED_STATUS_LEN                                           1

#define  FRAM_QUICK_ACCOUNT_ADDR         1128      
#define  FRAM_QUICK_ACCOUNT_LEN      1

#define  FRAM_PACKET_UPDATE_RESULT_ADDR     1130   
#define  FRAM_PACKET_UPDATE_RESULT_LEN      1

#define FRAM_INTERFACE_SWOTCH_ADDR          1132
#define FRAM_INTERFACE_SWOTCH_LEN           2

#define FRAM_TURNSPEED_PULSE_NUM_ADDR       1135
#define FRAM_TURNSPEED_PULSE_NUM_LEN        2

#define FRAM_ACCELERATION_ADDR              1138
#define FRAM_ACCELERATION_LEN               4//0-30,30-60,60-90,>90

#define FRAM_DECELERATION_ADDR              1143
#define FRAM_DECELERATION_LEN               2//0-30,>30

#define FRAM_TURN_ANGLE_ADDR                1146
#define FRAM_TURN_ANGLE_LEN                 2//急转弯角度阀


//下一个地址从1138开始

#define     FRAM_LAST_ADDR              0x07fc  //2043,FRAM最后两个字节用于芯片自检

//*******************铁电操作相关*****************
//#define FRAM_CS           GPIOC
//#define FRAM_Pin_CS       GPIO_Pin_4

#define FRAM_CS_HIGH()  GpioOutOn(FR_CS);spi_Delay_uS(60)
#define FRAM_CS_LOW()   GpioOutOff(FR_CS);spi_Delay_uS(60)

//***********************函数声明********************************
/*********************************************************************
//函数名称  :FRAM_Init()
//功能      :铁电初始化
//备注      :
*********************************************************************/
void FRAM_Init(void);
/*********************************************************************
//函数名称  :CheckFramChip(void)
//功能      :检查铁电芯片工作是否正常
//输入      :铁电地址的最后两个字节用于自检
//备注      :
*********************************************************************/
ErrorStatus CheckFramChip(void);
/*********************************************************************
//函数名称  :FRAM_BufferWrite2(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite)
//功能      :往铁电里写数据,在数据某尾加入校验字节
//输入      :WriteAddr，铁电地址
//      :pBuffer，数据缓冲
//      :NumBytesToWrite，写入的字节数
//备注      :
*********************************************************************/
void FRAM_BufferWrite(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite);
/*********************************************************************
//函数名称  :FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr)
//功能      :从铁电里读数据
//输入      :ReadAddr，铁电地址
//      :pBuffer，目标缓冲
//      :NumBytesToRead，读出的字节数 
//返回      :实际读出的字节数
//备注      :
*********************************************************************/
u8 FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr);
/*********************************************************************
//函数名称  :FRAM_WriteEnable(void)
//功能      :铁电写使能
//备注      :
*********************************************************************/
void FRAM_WriteEnable(void);
/*********************************************************************
//函数名称  :FRAM_WriteDisable(void)
//功能      :铁电写禁止
//备注      :
*********************************************************************/
void FRAM_WriteDisable(void);
/*********************************************************************
//函数名称  :FRAM_WriteStatusRegister(u8 Byte)
//功能      :写铁电状态寄存器
//备注      :/WP必须是高电平或无效电平,/WP仅仅是保护写铁电状态寄存器
*********************************************************************/
void FRAM_WriteStatusRegister(u8 Byte);
/*********************************************************************
//函数名称  :FRAM_ReadStatusRegister(void)
//功能      :读铁电状态寄存器 
//返回      :状态寄存器的值
//备注      :
*********************************************************************/
u8   FRAM_ReadStatusRegister(void);
/*********************************************************************
//函数名称  :FRAM_EraseChip(void)
//功能      :擦除整个铁电
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :状态寄存器的值
//备注      :
*********************************************************************/
void  FRAM_EraseChip(void);
/*********************************************************************
//函数名称  :FRAM_EraseAllDvrArea(void)
//功能      :擦除整个DVR用到的存储区
//输入      :
//输出      :
//使用资源  :
//全局变量  :   
//调用函数  :
//中断资源  :  
//返回      :状态寄存器的值
//备注      :
*********************************************************************/
void  FRAM_EraseAllDvrArea(void);
#endif
