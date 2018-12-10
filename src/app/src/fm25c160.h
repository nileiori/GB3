#ifndef __FRAM_H
#define __FRAM_H

//*********************ͷ�ļ�********************************
#include "stm32f10x.h"
#include "spi_flash.h"

//*********************�Զ�����������************************


//**********************�궨��********************************
//******************����ռ����******************
#define   FRAM_PHONE_MONTH_PARAM_ADDR                      0//ÿ��ͨ������
#define   FRAM_PHONE_MONTH_PARAM_LEN                       12//ÿ��ͨ����������

#define     FRAM_LAST_LOCATION_ADDR                          13 //��һ����Ч��λ��
#define     FRAM_LAST_LOCATION_LEN                           11 //��һ����Ч��λ��


#define     FRAM_MEDIA_ID_ADDR                               25 //��ý��ID
#define     FRAM_MEDIA_ID_LEN                                4

#define   FRAM_PARAMETER_TIME_PHOTO_CHANNEL_ADDR           58//������ʱ����ͨ��
#define   FRAM_PARAMETER_TIME_PHOTO_CHANNEL_LEN            1

#define   FRAM_PARAMETER_DISTANCE_PHOTO_CHANNEL_ADDR       60//������������ͨ��
#define   FRAM_PARAMETER_DISTANCE_PHOTO_CHANNEL_LEN        1

#define   FRAM_PHOTO_ID_ADDR                               62
#define   FRAM_PHOTO_ID_LEN                                4

#define     FRAM_TEMP_TRACK_NUM_ADDR                             67 //��ʱ����ʣ�����
#define     FRAM_TEMP_TRACK_NUM_LEN                              4

#define     FRAM_TEMP_TRACK_SPACE_ADDR                         72   //��ʱ���ټ��
#define     FRAM_TEMP_TRACK_SPACE_LEN                            2  

#define     FRAM_EMERGENCY_FLAG_ADDR                             75 //����������һ��״̬
#define     FRAM_EMERGENCY_FLAG_LEN                              1

#define     FRAM_RECORD_CONTROL_ADDR                             77 //¼�����Ʊ�־
#define     FRAM_RECORD_CONTROL_LEN                              1

#define     FRAM_RECORD_CLOSE_ADDR                               79 //��¼����־
#define     FRAM_RECORD_CLOSE_LEN                                  1

#define     FRAM_RECORD_OPEN_ADDR                                  81   //��¼����־
#define     FRAM_RECORD_OPEN_LEN                                   1

#define     FRAM_TTS_VOLUME_ADDR                                   83   //TTS����
#define     FRAM_TTS_VOLUME_LEN                              1

#define     FRAM_TEL_VOLUME_ADDR                             85 //�绰����
#define     FRAM_TEL_VOLUME__LEN                             1

#define   FRAM_OIL_CTRL_ADDR                               87 //����·
#define   FRAM_OIL_CTRL_ADDR_LEN                           1

#define   FRAM_CAR_OWNER_PHONE_ADDR                        89//�����ֻ���,�ַ���ASCII��,dxl
#define   FRAM_CAR_OWNER_PHONE_LEN                         12//д����ȡ�������ʱ��������д12,λ������ʱ��0x00

#define   FRAM_MAIN_DOMAIN_NAME_ADDR                       101//�������������,�ַ���,ASCII��,Ϊ�б��ͼ�����,dxl
#define   FRAM_MAIN_DOMAIN_NAME_LEN                        30//д����ȡ�������ʱ��������д30,λ������ʱ��0x00

#define   FRAM_BACKUP_DOMAIN_NAME_ADDR                     132//���ݼ����������,�ַ���,ASCII��,Ϊ�б��ͼ�����,dxl
#define   FRAM_BACKUP_DOMAIN_NAME_LEN                      30//д����ȡ�������ʱ��������д30,λ������ʱ��0x00

#define   FRAM_CAR_CARRY_STATUS_ADDR                       163//�����ػ�״̬,1�ֽ�,dxl
#define   FRAM_CAR_CARRY_STATUS_LEN                        1//д����ȡ�ò���ʱ������Ϊ1

#define   FRAM_QUICK_OPEN_ACCOUNT_ADDR                     165//���ٿ�������״̬�洢��ַ
#define   FRAM_QUICK_OPEN_ACCOUNT_LEN                      1//д����ȡ�ò���ʱ������Ϊ1

#define   FRAM_MULTI_CENTER_LINK_ADDR                      167//���������ӱ�־
#define   FRAM_MULTI_CENTER_LINK_LEN                       1//���������ӱ�־����

#define   FRAM_FIRMWARE_UPDATA_FLAG_ADDR                   169//�������񴥷���־������1��ʾ������Զ������������2��ʾ�����ɹ���
                                                            //����3��ʾ����ʧ�ܣ�����4��ʾ�ѷ��͹�0x0108�����ˣ�ÿ���ϵ��Ȩ��ɺ���Ҫ��ȡ��ֵ��
#define   FRAM_FIRMWARE_UPDATA_FLAG_LEN                    1

#define   FRAM_GNSS_SAVE_STEP_ADDR                         171//Gnssä�������Ĳ���
#define   FRAM_GNSS_SAVE_STEP_LEN                          2

#define   FRAM_FIRMWARE_UPDATA_TYPE_ADDR                   174//Զ����������,0Ϊ�ն�,9Ϊ���ؿ�����,Ŀǰֻ֧��������
#define   FRAM_FIRMWARE_UPDATA_TYPE_LEN                    1

#define   FRAM_ROLL_OVER_INIT_ANGLE_ADDR                   176//�෭��ʼ�Ƕ�
#define   FRAM_ROLL_OVER_INIT_ANGLE_LEN                    4

#define     FRAM_CAM_TIME_PHOTO_NUM_ADDR                    185 //ƽ̨��ʱ����ʣ������
#define     FRAM_CAM_TIME_PHOTO_NUM_LEN                      2//4·����ͷ��ռ��12�ֽ�,ÿ·3�ֽ�,��1��4,˳������

#define     FRAM_CAM_TIME_PHOTO_SPACE_ADDR                  197 //ƽ̨��ʱ����ʱ��������λ��
#define     FRAM_CAM_TIME_PHOTO_SPACE_LEN                    2//4·����ͷ��ռ��12�ֽ�,ÿ·3�ֽ�,��1��4,˳������

#define     FRAM_CAM_TIME_PHOTO_STORE_FLAG_ADDR             209 //ƽ̨��ʱ���ձ����־,bit0��1��ʾ��Ҫ�ϴ���bit1��1��ʾ��Ҫ������SD��
#define     FRAM_CAM_TIME_PHOTO_STORE_FLAG_LEN               1//4·����ͷ��ռ��8�ֽ�,ÿ·2�ֽ�,��1��4,˳������

#define     FRAM_CAM_TIME_PHOTO_RESOLUTION_ADDR             217 //ƽ̨��ʱ���շֱ���,0Ϊ320*240,1Ϊ640*480
#define     FRAM_CAM_TIME_PHOTO_RESOLUTION_LEN               1//4·����ͷ��ռ��8�ֽ�,ÿ·2�ֽ�,��1��4,˳������

#define     FRAM_CAM_TIME_PHOTO_EVENT_TYPE_ADDR             225 //ƽ̨��ʱ�����¼�����
#define     FRAM_CAM_TIME_PHOTO_EVENT_TYPE_LEN               1//4·����ͷ��ռ��8�ֽ�,ÿ·2�ֽ�,��1��4,˳������

#define     FRAM_CAM_TIME_PHOTO_CHANNEL_ADDR                233 //ƽ̨��ʱ�����ϴ�ͨ����ֻ��ΪCHANNEL_DATA_1������CHANNEL_DATA_2
#define     FRAM_CAM_TIME_PHOTO_CHANNEL_LEN                  1//4·����ͷ��ռ��8�ֽ�,ÿ·2�ֽ�,��1��4,˳������

#define     FRAM_CAM_EVENT_PHOTO_NUM_ADDR                   241 //�¼�����ʣ������
#define     FRAM_CAM_EVENT_PHOTO_NUM_LEN                     2//4·����ͷ��ռ��12�ֽ�,ÿ·3�ֽ�,��1��4,˳������

#define     FRAM_CAM_EVENT_PHOTO_SPACE_ADDR                 253 //�¼�����ʱ��������λ��
#define     FRAM_CAM_EVENT_PHOTO_SPACE_LEN                   2//4·����ͷ��ռ��12�ֽ�,ÿ·3�ֽ�,��1��4,˳������ 

#define     FRAM_CAM_EVENT_PHOTO_STORE_FLAG_ADDR            265 //�¼����ձ����־,bit0��1��ʾ��Ҫ�ϴ���bit1��1��ʾ��Ҫ������SD��
#define     FRAM_CAM_EVENT_PHOTO_STORE_FLAG_LEN              1//4·����ͷ��ռ��8�ֽ�,ÿ·2�ֽ�,��1��4,˳������

#define     FRAM_CAM_EVENT_PHOTO_RESOLUTION_ADDR            273 //�¼����շֱ���,0Ϊ320*240,1Ϊ640*480
#define     FRAM_CAM_EVENT_PHOTO_RESOLUTION_LEN              1//4·����ͷ��ռ��8�ֽ�,ÿ·2�ֽ�,��1��4,˳������

#define     FRAM_CAM_EVENT_PHOTO_EVENT_TYPE_ADDR            281 //�¼������¼�����
#define     FRAM_CAM_EVENT_PHOTO_EVENT_TYPE_LEN              1//4·����ͷ��ռ��8�ֽ�,ÿ·2�ֽ�,��1��4,˳������ 

#define     FRAM_CAM_EVENT_PHOTO_CHANNEL_ADDR               289 //�¼������ϴ�ͨ����ֻ��ΪCHANNEL_DATA_1������CHANNEL_DATA_2
#define     FRAM_CAM_EVENT_PHOTO_CHANNEL_LEN                 1//4·����ͷ��ռ��8�ֽ�,ÿ·2�ֽ�,��1��4,˳������


#define   FRAM_VDR_DOUBT_DATA_ADDR                           298//150����
#define   FRAM_VDR_DOUBT_DATA_LEN                                    2

#define   FRAM_VDR_DOUBT_TIME_ADDR                           756
#define   FRAM_VDR_DOUBT_TIME_LEN                                  4

#define   FRAM_VDR_DOUBT_COUNT_ADDR                          861
#define   FRAM_VDR_DOUBT_COUNT_LEN                             2

#define   FRAM_VDR_SPEED_15_MINUTE_SPEED_ADDR                  864
#define   FRAM_VDR_SPEED_15_MINUTE_SPEED_TOTAL_LEN           96//�ܳ���96,ÿ����ռ6�ֽڣ�4�ֽ�ʱ��+1�ֽ��ٶ�+1�ֽ�У�飩

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
#define FRAM_TURN_ANGLE_LEN                 2//��ת��Ƕȷ�


//��һ����ַ��1138��ʼ

#define     FRAM_LAST_ADDR              0x07fc  //2043,FRAM��������ֽ�����оƬ�Լ�

//*******************����������*****************
//#define FRAM_CS           GPIOC
//#define FRAM_Pin_CS       GPIO_Pin_4

#define FRAM_CS_HIGH()  GpioOutOn(FR_CS);spi_Delay_uS(60)
#define FRAM_CS_LOW()   GpioOutOff(FR_CS);spi_Delay_uS(60)

//***********************��������********************************
/*********************************************************************
//��������  :FRAM_Init()
//����      :�����ʼ��
//��ע      :
*********************************************************************/
void FRAM_Init(void);
/*********************************************************************
//��������  :CheckFramChip(void)
//����      :�������оƬ�����Ƿ�����
//����      :�����ַ����������ֽ������Լ�
//��ע      :
*********************************************************************/
ErrorStatus CheckFramChip(void);
/*********************************************************************
//��������  :FRAM_BufferWrite2(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite)
//����      :��������д����,������ĳβ����У���ֽ�
//����      :WriteAddr�������ַ
//      :pBuffer�����ݻ���
//      :NumBytesToWrite��д����ֽ���
//��ע      :
*********************************************************************/
void FRAM_BufferWrite(u16 WriteAddr, u8 *pBuffer, u16 NumBytesToWrite);
/*********************************************************************
//��������  :FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr)
//����      :�������������
//����      :ReadAddr�������ַ
//      :pBuffer��Ŀ�껺��
//      :NumBytesToRead���������ֽ��� 
//����      :ʵ�ʶ������ֽ���
//��ע      :
*********************************************************************/
u8 FRAM_BufferRead(u8 *pBuffer, u16 NumBytesToRead, u16 ReadAddr);
/*********************************************************************
//��������  :FRAM_WriteEnable(void)
//����      :����дʹ��
//��ע      :
*********************************************************************/
void FRAM_WriteEnable(void);
/*********************************************************************
//��������  :FRAM_WriteDisable(void)
//����      :����д��ֹ
//��ע      :
*********************************************************************/
void FRAM_WriteDisable(void);
/*********************************************************************
//��������  :FRAM_WriteStatusRegister(u8 Byte)
//����      :д����״̬�Ĵ���
//��ע      :/WP�����Ǹߵ�ƽ����Ч��ƽ,/WP�����Ǳ���д����״̬�Ĵ���
*********************************************************************/
void FRAM_WriteStatusRegister(u8 Byte);
/*********************************************************************
//��������  :FRAM_ReadStatusRegister(void)
//����      :������״̬�Ĵ��� 
//����      :״̬�Ĵ�����ֵ
//��ע      :
*********************************************************************/
u8   FRAM_ReadStatusRegister(void);
/*********************************************************************
//��������  :FRAM_EraseChip(void)
//����      :������������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :״̬�Ĵ�����ֵ
//��ע      :
*********************************************************************/
void  FRAM_EraseChip(void);
/*********************************************************************
//��������  :FRAM_EraseAllDvrArea(void)
//����      :��������DVR�õ��Ĵ洢��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :״̬�Ĵ�����ֵ
//��ע      :
*********************************************************************/
void  FRAM_EraseAllDvrArea(void);
#endif
