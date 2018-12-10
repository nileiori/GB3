#ifndef __SPI_FLASHAPI_H
#define __SPI_FLASHAPI_H

/************************�ļ�����***********************/
#include "stm32f10x.h"
#include "stm32f10x_lib.h"

//**************************�궨��************************
#define FLASH_ONE_SECTOR_BYTES                    4096//ÿ������4096�ֽ�

//*****************FLASH��ַ����**********************
#define FLASH_GB2312_16_START_SECTOR                  0//16*16�����GB2312�ֿ�
#define FLASH_GB2312_16_END_SECTOR                  66//256K byte+2K byte ASCII�ַ�,
                    //���ڵ���FLASH_GB2312_16_END_SECTORʱ����Ϊ�ǳ���������Χ,��������
#define FLASH_UPDATE_START_SECTOR                     67//���������̼��洢����,ä���洢����,��������һ��洢����
#define FLASH_UPDATE_END_SECTOR                       223//640K byte

#define FLASH_BLIND_START_SECTOR                      67//ä��
#define FLASH_BLIND_MIDDLE_SECTOR                 327
#define FLASH_BLIND_END_SECTOR                        587
#define FLASH_BLIND_STEP_LEN                            150//dxl,2014.7.15ԭ��100�ֽڲ����ã�������
#define FLASH_BLIND_STEP_LEN_BBGN_TEST                  40//dxl,2016.5.13���깦�ܼ��ʱʹ�õĳ���,ä��ֻ��¼����λ����Ϣ

////////////////////ʣ��100����//////////////////////////
/*
#define FLASH_VDR_LOG_START_SECTOR                327//588,�Ա��¼�����е���־ GBTestFlag == 1
#define FLASH_VDR_LOG_END_SECTOR                  688
#define FLASH_VDR_LOG_START_ADDR                  (FLASH_VDR_LOG_START_SECTOR*FLASH_ONE_SECTOR_BYTES)
#define FLASH_VDR_LOG_END_ADDR                    (FLASH_VDR_LOG_END_SECTOR*FLASH_ONE_SECTOR_BYTES)
*/
//����Ϊ��ʹ����ͨ�����ָ���ʹ�������//
#define FLASH_VDR_LOG_START_SECTOR                689//588,�Ա��¼�����е���־ GBTestFlag == 1
#define FLASH_VDR_LOG_END_SECTOR                  690
#define FLASH_VDR_LOG_START_ADDR                  (FLASH_VDR_LOG_START_SECTOR*FLASH_ONE_SECTOR_BYTES)
#define FLASH_VDR_LOG_END_ADDR                    (FLASH_VDR_LOG_END_SECTOR*FLASH_ONE_SECTOR_BYTES)



#define FLASH_STORE_IMAGE_START_SECTOR                588//588,�Ա��¼�����е���־ GBTestFlag == 1
#define FLASH_STORE_IMAGE_END_SECTOR                  606   //ÿ��ͼƬ24K һ����18������
#define FLASH_STORE_IMAGE_START_ADDR                  (FLASH_STORE_IMAGE_START_SECTOR*FLASH_ONE_SECTOR_BYTES)
#define FLASH_STORE_IMAGE_END_ADDR                    (FLASH_STORE_IMAGE_END_SECTOR*FLASH_ONE_SECTOR_BYTES)

#define AUDIO_START_SECTOR                            607    //��Ƶ�����ַ��ռ��8K
#define AUDIO_END_SECTOR                              609

#define VIDEO_START_SECTOR                            610    //��Ƶ�����ַ��ռ��28K
#define VIDEO_END_SECTOR                              617

///////////////ʣ����������DVR��־��¼///////////////////

#define FLASH_VDR_DATA_SPEED_START_SECTOR            697
#define FLASH_VDR_DATA_SPEED_END_SECTOR                801//48Сʱ�ٶȼ�¼

#define FLASH_VDR_DATA_POSITION_START_SECTOR         802
#define FLASH_VDR_DATA_POSITION_END_SECTOR           864//360Сʱλ����Ϣ��¼

#define FLASH_VDR_DATA_DOUBT_START_SECTOR              865
#define FLASH_VDR_DATA_DOUBT_END_SECTOR                875//�¹��ɵ����ݼ�¼

#define FLASH_VDR_DATA_OVER_TIME_START_SECTOR        876
#define FLASH_VDR_DATA_OVER_TIME_END_SECTOR          880//��ʱ��ʻ��¼

#define FLASH_VDR_DATA_DRIVER_START_SECTOR           881
#define FLASH_VDR_DATA_DRIVER_END_SECTOR                 885//��ʻ�����(ǩ��/ǩ��)��¼

#define FLASH_VDR_DATA_POWER_START_SECTOR              886
#define FLASH_VDR_DATA_POWER_END_SECTOR              889//��¼�ǹ���/�ϵ��¼

#define FLASH_VDR_DATA_PARAMETER_START_SECTOR        890
#define FLASH_VDR_DATA_PARAMETER_END_SECTOR          893//���ò�����¼

#define FLASH_VDR_DATA_SPEED_STATUS_START_SECTOR        894
#define FLASH_VDR_DATA_SPEED_STATUS_END_SECTOR        897//�ٶ��쳣��¼

#define FLASH_VDR_STATUS_SET_SECTOR                     898//4K byte
#define FLASH_VDR_STATUS_SET_ADDR       (FLASH_VDR_STATUS_SET_SECTOR*FLASH_ONE_SECTOR_BYTES)

#define FLASH_RECTANGLE_AREA_START_SECTOR             899//��������
#define FLASH_RECTANGLE_AREA_END_SECTOR               904//16K
#define FLASH_RECTANGLE_AREA_START_ADDR     (FLASH_RECTANGLE_AREA_START_SECTOR*FLASH_ONE_SECTOR_BYTES)//���������ַ
#define FLASH_RECTANGLE_AREA_END_ADDR       (FLASH_RECTANGLE_AREA_END_SECTOR*FLASH_ONE_SECTOR_BYTES)//12K

#define FLASH_ROUND_AREA_START_SECTOR                   905//Բ������
#define FLASH_ROUND_AREA_END_SECTOR                   910//16K
#define FLASH_ROUND_AREA_START_ADDR     (FLASH_ROUND_AREA_START_SECTOR*FLASH_ONE_SECTOR_BYTES)//Բ�������ַ
#define FLASH_ROUND_AREA_END_ADDR       (FLASH_ROUND_AREA_END_SECTOR*FLASH_ONE_SECTOR_BYTES)//12K

#define FLASH_POLYGON_AREA_START_SECTOR               911//���������
#define FLASH_POLYGON_AREA_END_SECTOR                   921//36K

#define FLASH_ROUTE_START_SECTOR                        922//·��
#define FLASH_ROUTE_END_SECTOR                          951//112K byte

#define FLASH_ATTEMPE_START_SECTOR                    952//�ı���Ϣ
#define FLASH_ATTEMPE_END_SECTOR                        957//16K byte
#define FLASH_TEXT_START_ADDR           (FLASH_ATTEMPE_START_SECTOR*FLASH_ONE_SECTOR_BYTES)//Բ�������ַ
#define FLASH_TEXT_END_ADDR         (FLASH_ATTEMPE_END_SECTOR*FLASH_ONE_SECTOR_BYTES)//12K

#define FLASH_MESSAGE_START_SECTOR                    958//��������Ϣ�㲥,�绰��,�¼�����,��ʻԱ��Ϣ,����Ӧ��,��Ϣ����
#define FLASH_MESSAGE_END_SECTOR                        964//24K byte

#define FLASH_OVER_TIME_DRIVE_INFO_START_SECTOR       965//��ʱ��ʻ�ļ�ʻԱ��Ϣ
#define FLASH_OVER_TIME_DRIVE_INFO_END_SECTOR       966
#define FLASH_OVER_TIME_DRIVE_INFO_START_ADDR   (FLASH_OVER_TIME_DRIVE_INFO_START_SECTOR*FLASH_ONE_SECTOR_BYTES)

#define FLASH_CALL_RECORD_START_SECTOR                967//ͨ����¼
#define FLASH_CALL_RECORD_END_SECTOR                    968

#define FLASH_MUCK_DUMP_AREA_START_SECTOR             969//�����㵹����
#define FLASH_MUCK_DUMP_AREA_END_SECTOR                 970

#define FLASH_DRIVER_SIGN_INFO_START_SECTOR             971//��ʻԱǩ��ǩ���ϱ���Ϣ
#define FLASH_DRIVER_SIGN_INFO_END_SECTOR             972//��ʻԱǩ��ǩ���ϱ���Ϣ
/////////////////////////////////////////////////
#define FLASH_BMA250_SAVE_START_SECTOR              973//BMA250���ݱ��濪ʼ��ַ
#define FLASH_BMA250_SAVE_END_SECTOR                973//BMA250���ݱ��������ַ
/////////////////////////////////////////////////
#define FLASH_GNSS_SAVE_START_SECTOR                    974//GNSS���ر��濪ʼ��ַ
#define FLASH_GNSS_SAVE_END_SECTOR                    1020//GNSS���ر��������ַ

#define FLASH_VDR_POSITION_START_SECOTR               1021
#define FLASH_VDR_POSITION_END_SECOTR                   1022
//****************��ֹ����Ϊ1023���������벻Ҫ������ֵ**********************8


#endif
