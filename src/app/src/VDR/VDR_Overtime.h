#ifndef __VDR_OVERTIME_H
#define __VDR_OVERTIME_H

//#include "stm32f2xx.h"
#include "stm32f10x.h"
#include "Rtc.h"

#define	DRIVER_NUM_MAX											6//��಻����6����ʻԱ�����Ϊ0-5��ϵͳĬ�ϼ�ʻԱΪ0
#define	DRIVER_INFO_TYPE_DRIVER_NUM					      0x0001
#define	DRIVER_INFO_TYPE_DRIVER_LICENSE			      0x0002
#define	DRIVER_INFO_TYPE_START_TIME					      0x0004
#define	DRIVER_INFO_TYPE_END_TIME						      0x0008
#define	DRIVER_INFO_TYPE_START_POSITION			      0x0010
#define	DRIVER_INFO_TYPE_END_POSITION				      0x0020
#define	DRIVER_INFO_TYPE_LOGIN_TIME					      0x0040
#define	DRIVER_INFO_TYPE_LOGOUT_TIME				      0x0080
#define	DRIVER_INFO_TYPE_NIGHT_START_TIME					0x0100
#define	DRIVER_INFO_TYPE_NIGHT_END_TIME						0x0200
#define	DRIVER_INFO_TYPE_NIGHT_START_POSITION			0x0400
#define	DRIVER_INFO_TYPE_NIGHT_END_POSITION				0x0800
#define	DRIVER_INFO_TYPE_ONE_DAY_DRIVE_TIME				0x1000

#define	DRIVER_INFO_TYPE_ALL								0xFFFF 

typedef struct
{
	u8	License[18];//��ʻ֤�ţ������ֽ�Ϊ0xff��ʾδ�Ǽ�
	u32	StartTimeCount;//��ʱ��ʻ��ʼʱ�䣬ֵΪ0��ʾδд���
	u32	EndTimeCount;//��ʱ��ʻ����ʱ�䣬�����ֽ�Ϊ0xff��ʾδд���
	u8	StartPosition[10];//��ʱ��ʻ��ʼλ�ã������ֽ�Ϊ0xff��0x00��ʾδд���
	u8	EndPosition[10];//��ʱ��ʻ��ʼλ��
	u32	LoginTimeCount;//ǩ��ʱ��
	u32	LogoutTimeCount;//ǩ��ʱ��
	u32	NightStartTimeCount;//ҹ�䳬ʱ��ʻ��ʼʱ�䣬ֵΪ0��ʾδд���
	u32	NightEndTimeCount;//ҹ�䳬ʱ��ʻ����ʱ�䣬�����ֽ�Ϊ0xff��ʾδд���
	u8	NightStartPosition[10];//ҹ�䳬ʱ��ʻ��ʼλ�ã������ֽ�Ϊ0xff��0x00��ʾδд���
	u8	NightEndPosition[10];//ҹ�䳬ʱ��ʻ��ʼλ��	
	u32 OneDayDriveTimeCount;//ÿ�ռ�ʻ��ʱ�䣬0��0ʱ0����0

}VDR_OVERTIME_DRIVER_STRUCT;

typedef struct
{
	u32	EarlyAlarmTime;//Ԥ��ʱ�䣬��λ�룬��Ӧ������ID 0x0057ֵ - ����ID 0x005cֵ����������ʱ0x0057����Ϊ14400��0x005c����Ϊ1800���ɡ�
	u32	EarlyAlarmKeeptime;//Ԥ������ʱ�䣬��λ�룬��Ӧ����ID 0x005c��������ʱ����Ϊ1800���ɡ�
		//Ԥ��������ʹ�þֲ���������Ӧ����ID 0xF26e��
	u32	EarlyAlarmVoiceGroupTime;//Ԥ������ÿ����ʾ�������λ���ӣ���Ӧ����ID 0xF26f��������ʱ����Ϊ5���ɡ�
	u32	EarlyAlarmVoiceNum;//Ԥ������ÿ����ʾ��������Ӧ����ID 0xF270��������ʱ����Ϊ3���ɡ�
	u32	EarlyAlarmVoiceNumTime;//Ԥ������ÿ����ʾ�������λ�룬��Ӧ����ID 0xF271

	u32	AlarmTime;//����ʱ�䣬��λ�룬��Ӧ����ID 0x0057
	u32	AlarmVoiceKeeptime;//������������ʱ�䣬��λ���ӣ���Ӧ����ID 0xF26D��������ʱ����Ϊ30���ɡ�
		//����������ʹ�þֲ���������Ӧ����ID 0xF272
	u32	AlarmVoiceGroupTime;//��������ÿ����ʾ�������λ���ӣ���Ӧ����ID 0xF273��������ʱ����Ϊ5���ɡ�
	u32	AlarmVoiceNum;//��������ÿ����ʾ��������Ӧ����ID 0xF274��������ʱ����Ϊ3���ɡ�
	u32	AlarmVoiceNumTime;//��������ÿ����ʾ�������λ�룬��Ӧ����ID 0xF275

	u32	MinRestTime;//��С��Ϣʱ�䣬��λ�룬��Ӧ����ID 0x0059��������ʱ����Ϊ1200���ɡ�
        
        u8      RunSpeed;//��ʻ�ж��ٶȣ���λΪkm/h��������ʱ����Ϊ0����
        u32     RunKeeptime;//��ʻ�ж�����ʱ�䣬��λΪ�룬������ʱ����Ϊ10����

	u32	NightEarlyAlarmTime;//ҹ��Ԥ��ʱ�䣬��λ�룬��Ӧ������ID 0xF26Bֵ - ����ID 0x005cֵ����������ʱ������ҹ�书�ܼ��ɡ�
	u32	NightAlarmTime;//ҹ�䱨��ʱ�䣬��λ�룬��Ӧ����ID 0xF26B
	u32	NightMinRestTime;//ҹ����С��Ϣʱ�䣬��λ�룬��Ӧ����ID 0xF26C��������ʱ������ҹ�书�ܼ��ɡ�
	//u8  NightTime[14];//ҹ��ʱ�䣬��14�ֽڣ�1-2�ֽ������ԣ�3-8�ֽ��ǿ�ʼʱ�䣬9-14�ֽ��ǽ���ʱ��
	TIME_T NightStartTime;//ҹ�俪ʼʱ��
	TIME_T NightEndTime;//ҹ�����ʱ��
	u16    NightTimeAttribute;//ҹ��ʱ������
	
	u32	NightVoiceKeeptime;//ҹ����������ʱ�䣬��λ���ӣ���Ӧ����ID 0xF25C
	u32	NightVoiceGroupTime;//ҹ������ÿ����ʾ�������λ���ӣ���Ӧ����ID 0xF25E
	
	u32 OneDayAlarmTime;//�����ۼƼ�ʻʱ�䣬��λ�룬��Ӧ����ID 0x0058

	VDR_OVERTIME_DRIVER_STRUCT Driver[DRIVER_NUM_MAX];//����Ĭ�ϼ�ʻԱ���ڣ����֧��6����ʻԱ����ŷֱ�Ϊ0-5��Ĭ�ϵļ�ʻԱ���Ϊ0
}VDR_OVERTIME_STRUCT;

/**************************************************************************
//��������VDROvertime_TimeTask
//���ܣ���ʱ��ʻ
//���룺��
//�������
//����ֵ��ʼ����ENABLE
//��ע����ʱ��ʻ��ʱ����1���ӽ���1�Σ������������Ҫ���ô˺���
***************************************************************************/
FunctionalState VDROvertime_TimeTask(void);
/**************************************************************************
//��������VDROvertime_UpdateParameter
//���ܣ����³�ʱ��ʻ��ز���
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void VDROvertime_UpdateParameter(void);
/**************************************************************************
//��������VDROvertime_DriverLogin
//���ܣ���ʻԱǩ������¼��
//���룺pBuffer:ָ���ʻ֤�����ָ��;BufferLen:���ݳ���
//�������
//����ֵ��0:��¼�ɹ�����0:��¼ʧ�ܣ�Ϊ1��ʾ���ȴ���Ϊ2��ʾ��ʻ֤�����ʽ����
//Ϊ3��ʾ��ʻ֤��ȫΪASCII 0������ֹϵͳĬ�ϼ�ʻԱǩ������Ϊ4��ʾ�ü�ʻԱ��ǩ��
//��ע����ʻ֤���볤��BufferLen����Ϊ18�ֽڣ������¼ʧ��
***************************************************************************/
u8 VDROvertime_DriverLogin(u8 *pBuffer, u8 BufferLen);
/**************************************************************************
//��������VDROvertime_DriverLogout
//���ܣ���ʻԱǩ�ˣ�δ��¼��
//���룺pBuffer:ָ���ʻ֤�����ָ��;BufferLen:���ݳ���
//�������
//����ֵ��0:��¼�ɹ�����0:��¼ʧ�ܣ�Ϊ1��ʾ���ȴ���Ϊ2��ʾ��ʻ֤�����ʽ����
//Ϊ3��ʾ��ʻ֤��ȫΪASCII 0������ֹϵͳĬ�ϼ�ʻԱǩ������Ϊ4��ʾ�ü�ʻԱ��ǩ��
//��ע����ʻ֤���볤��BufferLen����Ϊ18�ֽڣ������¼ʧ��
***************************************************************************/
void VDROvertime_DriverLogout(void);
/**************************************************************************
//��������VDROvertime_GetLoginFlag
//���ܣ���ȡ��¼��־
//���룺��
//�������¼��־
//����ֵ��1:��¼��0:δ��¼
//��ע����
***************************************************************************/
u8 VDROvertime_GetLoginFlag(void);
/**************************************************************************
//��������VDROvertime_Get2daysLog
//���ܣ���ȡ2��������ĳ�ʱ��ʻ��¼
//���룺��
//�����
//����ֵ�����ݳ���
//��ע��
***************************************************************************/
u16 VDROvertime_Get2daysLog(u8 *pBuffer);
/**************************************************************************
//��������VDROvertime_GetUnsaveLog
//���ܣ���ȡ��δ���浽flash�ĳ�ʱ��ʻ��¼
//���룺ָ��ʱ��
//�����
//����ֵ�����ݳ���
//��ע����RAM�������ж�ȡ��δд�뵽FLASH�еĳ�ʱ��ʻ��¼
***************************************************************************/
u16 VDROvertime_GetUnsaveLog(u8 *pBuffer, TIME_T Time, u8 DriverNum);
/**************************************************************************
//��������VDROvertime_GetInitFlag
//���ܣ����ģ���ʼ����־
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
u8 VDROvertime_GetInitFlag(void);
/**************************************************************************
//��������VDROvertime_GetLicense
//���ܣ���ȡ��ǰ��ʻ֤����
//���룺ָ��Ŀ�Ļ���ָ��
//�����
//����ֵ�����ݳ���
//��ע��
***************************************************************************/
u8 VDROvertime_GetLicense(u8 *pBuffer);
/**************************************************************************
//��������VDROvertime_GetDriveTime
//���ܣ���ȡ��ǰ��ʻԱ������ʻʱ��
//���룺��
//�����
//����ֵ��������ʻʱ�䣬��λ����
//��ע��
***************************************************************************/
u32 VDROvertime_GetDriveTime(void);
/**************************************************************************
//��������VDROvertime_ClearStartTime
//���ܣ������ʼʱ�䣬���»�ȡ��ʼʱ��
//���룺��
//�������
//����ֵ����
//��ע������Э����ƣ�ͼ�ʻʱ��������ƣ�ͼ�ʻ����ʱ��Ҫ���ô˺���
***************************************************************************/
void VDROvertime_ClearStartTime(void);
#endif
