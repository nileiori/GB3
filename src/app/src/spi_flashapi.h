#ifndef __SPI_FLASHAPI_H
#define __SPI_FLASHAPI_H

/************************文件包含***********************/
#include "stm32f10x.h"
#include "stm32f10x_lib.h"

//**************************宏定义************************
#define FLASH_ONE_SECTOR_BYTES                    4096//每个扇区4096字节

//*****************FLASH地址分配**********************
#define FLASH_GB2312_16_START_SECTOR                  0//16*16点阵的GB2312字库
#define FLASH_GB2312_16_END_SECTOR                  66//256K byte+2K byte ASCII字符,
                    //大于等于FLASH_GB2312_16_END_SECTOR时就认为是超出了区域范围,其它类似
#define FLASH_UPDATE_START_SECTOR                     67//程序升级固件存储区域,盲区存储区域,两个共用一块存储区域
#define FLASH_UPDATE_END_SECTOR                       223//640K byte

#define FLASH_BLIND_START_SECTOR                      67//盲区
#define FLASH_BLIND_MIDDLE_SECTOR                 327
#define FLASH_BLIND_END_SECTOR                        587
#define FLASH_BLIND_STEP_LEN                            150//dxl,2014.7.15原来100字节不够用，超出了
#define FLASH_BLIND_STEP_LEN_BBGN_TEST                  40//dxl,2016.5.13部标功能检测时使用的长度,盲区只记录基本位置信息

////////////////////剩余100扇区//////////////////////////
/*
#define FLASH_VDR_LOG_START_SECTOR                327//588,以便记录下所有的日志 GBTestFlag == 1
#define FLASH_VDR_LOG_END_SECTOR                  688
#define FLASH_VDR_LOG_START_ADDR                  (FLASH_VDR_LOG_START_SECTOR*FLASH_ONE_SECTOR_BYTES)
#define FLASH_VDR_LOG_END_ADDR                    (FLASH_VDR_LOG_END_SECTOR*FLASH_ONE_SECTOR_BYTES)
*/
//以下为了使编译通过，恢复可使用上面的//
#define FLASH_VDR_LOG_START_SECTOR                689//588,以便记录下所有的日志 GBTestFlag == 1
#define FLASH_VDR_LOG_END_SECTOR                  690
#define FLASH_VDR_LOG_START_ADDR                  (FLASH_VDR_LOG_START_SECTOR*FLASH_ONE_SECTOR_BYTES)
#define FLASH_VDR_LOG_END_ADDR                    (FLASH_VDR_LOG_END_SECTOR*FLASH_ONE_SECTOR_BYTES)



#define FLASH_STORE_IMAGE_START_SECTOR                588//588,以便记录下所有的日志 GBTestFlag == 1
#define FLASH_STORE_IMAGE_END_SECTOR                  606   //每副图片24K 一共用18个扇区
#define FLASH_STORE_IMAGE_START_ADDR                  (FLASH_STORE_IMAGE_START_SECTOR*FLASH_ONE_SECTOR_BYTES)
#define FLASH_STORE_IMAGE_END_ADDR                    (FLASH_STORE_IMAGE_END_SECTOR*FLASH_ONE_SECTOR_BYTES)

#define AUDIO_START_SECTOR                            607    //音频保存地址，占用8K
#define AUDIO_END_SECTOR                              609

#define VIDEO_START_SECTOR                            610    //视频保存地址，占用28K
#define VIDEO_END_SECTOR                              617

///////////////剩余扇区用作DVR日志记录///////////////////

#define FLASH_VDR_DATA_SPEED_START_SECTOR            697
#define FLASH_VDR_DATA_SPEED_END_SECTOR                801//48小时速度记录

#define FLASH_VDR_DATA_POSITION_START_SECTOR         802
#define FLASH_VDR_DATA_POSITION_END_SECTOR           864//360小时位置信息记录

#define FLASH_VDR_DATA_DOUBT_START_SECTOR              865
#define FLASH_VDR_DATA_DOUBT_END_SECTOR                875//事故疑点数据记录

#define FLASH_VDR_DATA_OVER_TIME_START_SECTOR        876
#define FLASH_VDR_DATA_OVER_TIME_END_SECTOR          880//超时驾驶记录

#define FLASH_VDR_DATA_DRIVER_START_SECTOR           881
#define FLASH_VDR_DATA_DRIVER_END_SECTOR                 885//驾驶人身份(签到/签退)记录

#define FLASH_VDR_DATA_POWER_START_SECTOR              886
#define FLASH_VDR_DATA_POWER_END_SECTOR              889//记录仪供电/断电记录

#define FLASH_VDR_DATA_PARAMETER_START_SECTOR        890
#define FLASH_VDR_DATA_PARAMETER_END_SECTOR          893//设置参数记录

#define FLASH_VDR_DATA_SPEED_STATUS_START_SECTOR        894
#define FLASH_VDR_DATA_SPEED_STATUS_END_SECTOR        897//速度异常记录

#define FLASH_VDR_STATUS_SET_SECTOR                     898//4K byte
#define FLASH_VDR_STATUS_SET_ADDR       (FLASH_VDR_STATUS_SET_SECTOR*FLASH_ONE_SECTOR_BYTES)

#define FLASH_RECTANGLE_AREA_START_SECTOR             899//矩形区域
#define FLASH_RECTANGLE_AREA_END_SECTOR               904//16K
#define FLASH_RECTANGLE_AREA_START_ADDR     (FLASH_RECTANGLE_AREA_START_SECTOR*FLASH_ONE_SECTOR_BYTES)//矩形区域地址
#define FLASH_RECTANGLE_AREA_END_ADDR       (FLASH_RECTANGLE_AREA_END_SECTOR*FLASH_ONE_SECTOR_BYTES)//12K

#define FLASH_ROUND_AREA_START_SECTOR                   905//圆形区域
#define FLASH_ROUND_AREA_END_SECTOR                   910//16K
#define FLASH_ROUND_AREA_START_ADDR     (FLASH_ROUND_AREA_START_SECTOR*FLASH_ONE_SECTOR_BYTES)//圆形区域地址
#define FLASH_ROUND_AREA_END_ADDR       (FLASH_ROUND_AREA_END_SECTOR*FLASH_ONE_SECTOR_BYTES)//12K

#define FLASH_POLYGON_AREA_START_SECTOR               911//多边形区域
#define FLASH_POLYGON_AREA_END_SECTOR                   921//36K

#define FLASH_ROUTE_START_SECTOR                        922//路线
#define FLASH_ROUTE_END_SECTOR                          951//112K byte

#define FLASH_ATTEMPE_START_SECTOR                    952//文本信息
#define FLASH_ATTEMPE_END_SECTOR                        957//16K byte
#define FLASH_TEXT_START_ADDR           (FLASH_ATTEMPE_START_SECTOR*FLASH_ONE_SECTOR_BYTES)//圆形区域地址
#define FLASH_TEXT_END_ADDR         (FLASH_ATTEMPE_END_SECTOR*FLASH_ONE_SECTOR_BYTES)//12K

#define FLASH_MESSAGE_START_SECTOR                    958//包含了信息点播,电话簿,事件报告,驾驶员信息,提问应答,信息服务
#define FLASH_MESSAGE_END_SECTOR                        964//24K byte

#define FLASH_OVER_TIME_DRIVE_INFO_START_SECTOR       965//超时驾驶的驾驶员信息
#define FLASH_OVER_TIME_DRIVE_INFO_END_SECTOR       966
#define FLASH_OVER_TIME_DRIVE_INFO_START_ADDR   (FLASH_OVER_TIME_DRIVE_INFO_START_SECTOR*FLASH_ONE_SECTOR_BYTES)

#define FLASH_CALL_RECORD_START_SECTOR                967//通话记录
#define FLASH_CALL_RECORD_END_SECTOR                    968

#define FLASH_MUCK_DUMP_AREA_START_SECTOR             969//渣土倾倒区域
#define FLASH_MUCK_DUMP_AREA_END_SECTOR                 970

#define FLASH_DRIVER_SIGN_INFO_START_SECTOR             971//驾驶员签到签退上报信息
#define FLASH_DRIVER_SIGN_INFO_END_SECTOR             972//驾驶员签到签退上报信息
/////////////////////////////////////////////////
#define FLASH_BMA250_SAVE_START_SECTOR              973//BMA250数据保存开始地址
#define FLASH_BMA250_SAVE_END_SECTOR                973//BMA250数据保存结束地址
/////////////////////////////////////////////////
#define FLASH_GNSS_SAVE_START_SECTOR                    974//GNSS本地保存开始地址
#define FLASH_GNSS_SAVE_END_SECTOR                    1020//GNSS本地保存结束地址

#define FLASH_VDR_POSITION_START_SECOTR               1021
#define FLASH_VDR_POSITION_END_SECOTR                   1022
//****************截止扇区为1023，扇区号请不要超过该值**********************8


#endif
