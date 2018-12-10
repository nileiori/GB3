#ifndef __VDR_DATA_H
#define __VDR_DATA_H

//************�ļ�����***************
#include "stm32f10x.h"
#include "Rtc.h"

//*************�궨��****************
#define VDR_DATA_TYPE_SPEED			0//��¼�Ǵ洢���ͣ�48Сʱ�ٶȼ�¼
#define VDR_DATA_TYPE_POSITION			1//360Сʱλ����Ϣ��¼
#define VDR_DATA_TYPE_DOUBT			2//�¹��ɵ����ݼ�¼
#define VDR_DATA_TYPE_OVER_TIME			3//��ʱ��ʻ��¼
#define VDR_DATA_TYPE_DRIVER			4//��ʻ�����(ǩ��/ǩ��)��¼
#define VDR_DATA_TYPE_POWER			5//��¼�ǹ���/�ϵ��¼
#define VDR_DATA_TYPE_PARAMETER			6//���ò�����¼
#define VDR_DATA_TYPE_SPEED_STATUS		7//�ٶ��쳣��¼
#define VDR_DATA_TYPE_MAX			8//���Ͳ��ܵ��ڻ���ڸ�ֵ

#define VDR_DATA_SPEED_STEP_LEN			144//5+126+13���洢�Ĳ�����4�ֽ�ʱ��+�������ݣ��ɼ�����+�������ݣ�+1�ֽ�У��
#define VDR_DATA_POSITION_STEP_LEN		681//5+666+10
#define VDR_DATA_DOUBT_STEP_LEN			251//5+234+12
#define VDR_DATA_OVER_TIME_STEP_LEN		67//5+50+12
#define VDR_DATA_DRIVER_STEP_LEN		36//5+25+6
#define VDR_DATA_POWER_STEP_LEN			18//5+7+6
#define VDR_DATA_PARAMETER_STEP_LEN		18//5+7+6
#define VDR_DATA_SPEED_STATUS_STEP_LEN		144//5+133+6
#define VDR_DATA_MAX_STEP_LEN			682////�������ܵ��ڻ���ڸ�ֵ

#define VDR_DATA_SPEED_COLLECT_LEN		126//�ɼ��ĳ��ȣ���ÿ�����ݳ���
#define VDR_DATA_POSITION_COLLECT_LEN		666
#define VDR_DATA_DOUBT_COLLECT_LEN		234
#define VDR_DATA_OVER_TIME_COLLECT_LEN		50
#define VDR_DATA_DRIVER_COLLECT_LEN		25
#define VDR_DATA_POWER_COLLECT_LEN		7
#define VDR_DATA_PARAMETER_COLLECT_LEN		7
#define VDR_DATA_SPEED_STATUS_COLLECT_LEN	133
//#define VDR_DATA_MAX_COLLECT_LEN		667

#define VDR_DATA_SPEED_PACKET_LIST		7//�������ʱ��ÿ�����Ĵ������
#define VDR_DATA_POSITION_PACKET_LIST		1
#define VDR_DATA_DOUBT_PACKET_LIST		4
#define VDR_DATA_OVER_TIME_PACKET_LIST		20
#define VDR_DATA_DRIVER_PACKET_LIST		40
#define VDR_DATA_POWER_PACKET_LIST		142
#define VDR_DATA_PARAMETER_PACKET_LIST		142
#define VDR_DATA_SPEED_STATUS_PACKET_LIST	7
//#define VDR_DATA_MAX_PACKET_LIST		143



#define VDR_DATA_BUFFER_SIZE			VDR_DATA_MAX_STEP_LEN

//*************���ݽṹ����***********
typedef struct
{
	s16 StartSector;
	s16 EndSector;
	s16 StepLen;
	s16 SectorStep;
	s16 OldestSector;
	s16 CurrentSector;
	s16 CurrentStep;
	s8  LoopFlag;
	s8  InitFlag;
	u8	WriteEnableFlag;
}VDR_DATA_STRUCT;

//*************��������***********
/**************************************************************************
//��������VDRData_TimeTask(void)
//���ܣ���¼�����ݶ�ʱ����
//���룺��
//�������
//����ֵ��ʼ��ΪENABLE
//��ע��50ms����1�Σ�������ֻ�ǶԴ洢�ı�������ʼ����ʵʱ��⣬����������о�����
***************************************************************************/
FunctionalState VDRData_TimeTask(void);
/**************************************************************************
//��������VDRData_Init
//���ܣ�����ģ���ʼ��
//���룺��
//�������
//����ֵ��ʼ��ΪENABLE
//��ע��50ms����1�Σ�������ֻ�ǶԴ洢�ı�������ʼ����ʵʱ��⣬����������о�����
***************************************************************************/
void VDRData_Init(void);
/**************************************************************************
//��������VDRData_Write
//���ܣ�дһ����ʻ��¼������
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVER_TIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//pBuffer:ָ�������׵�ַ��length:���ݳ��ȣ�Time:32λhex��ʾ��RTCʱ��
//�������
//����ֵ���ɹ�SUCCESS,ʧ��ERROR
//��ע����ʻ��¼�����ݼ�¼��flash�ĸ�ʽΪ��ʱ�䣨4�ֽڣ�+�������ݣ��̶����ȣ�+У��ͣ�1�ֽڣ���ǰ�������ֽ����ݵ�У��ͣ�
//VDR_DATA_TYPE_SPEED�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.17�涨�ĸ�ʽ��126�ֽڣ�+13�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_POSITION�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.19�涨�ĸ�ʽ��666�ֽڣ�+10�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_DOUBT�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.21�涨�ĸ�ʽ��234�ֽڣ�+12�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_OVER_TIME�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.24�涨�ĸ�ʽ��50�ֽڣ�+12�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_DRIVER�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.26�涨�ĸ�ʽ��25�ֽڣ�+6�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_POWER�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.28�涨�ĸ�ʽ��7�ֽڣ�+6�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_PRAMATER�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.30�涨�ĸ�ʽ��7�ֽڣ�+6�ֽ�Ԥ���ռ�
//VDR_DATA_TYPE_SPEED_STATUS�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.32�涨�ĸ�ʽ��133�ֽڣ�+6�ֽ�Ԥ���ռ�
***************************************************************************/
ErrorStatus VDRData_Write(u8 Type, u8 *pBuffer, u16 length, u32 Time);
/**************************************************************************
//��������VDRData_Read
//���ܣ���ȡָ������ʱ��ǰ��N����ʻ��¼������
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVER_TIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//pBuffer:ָ�������׵�ַ��EndTime:����ʱ�䣻block:����
//�������
//����ֵ����ȡ�����ݳ���
//��ע�����ñ�����ķ�ʽ���������һ����¼��ʱ��պõ��ڽ���ʱ�䣬�ü�¼�������
***************************************************************************/
u16 VDRData_Read(u8 *pBuffer,u8 Type,TIME_T StartTime,TIME_T EndTime, u8 block);
/**************************************************************************
//��������VDRData_SetWriteEnableFlag
//���ܣ�����ĳ���������͵�дʹ�ܱ�־(��ֹ��ʹ��)
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVER_TIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//Value:0Ϊ��ֹ,1Ϊʹ��
//�������
//����ֵ���ɹ�����SUCCESS,ʧ�ܷ���ERROR
//��ע:!!!!!�ر�ע��˭��ֹ��˭���븺����,����Ӱ��������ݵļ�¼!!!!
//��Զ�̶�ȡ��ʻ��¼�Ƿְ�����ʱ�����Ƚ�ֹд,Ȼ������ɺ��ٿ���д,
//��������ԭ����:Զ�̶�ȡ���ݵĹ�������û�н�ֹд,��ܿ��ܻ��������ݼ�¼,
//�Ӷ����·ְ���Ϣ��̬�仯,��ȡ�ķְ����ݾͲ�׼ȷ��
***************************************************************************/
ErrorStatus VDRData_SetWriteEnableFlag(u8 Type, u8 Value);
/**************************************************************************
//��������VDRData_ReadWriteEnableFlag
//���ܣ���ĳ���������͵�дʹ�ܱ�־
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVER_TIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ��//Value:0Ϊ��ֹ,1Ϊʹ��,������0xFF��ʾ��������
//��ע:!!!!!�ر�ע��˭��ֹ��˭���븺����,����Ӱ��������ݵļ�¼!!!!
//��Զ�̶�ȡ��ʻ��¼�Ƿְ�����ʱ�����Ƚ�ֹд,Ȼ������ɺ��ٿ���д,
//��������ԭ����:Զ�̶�ȡ���ݵĹ�������û�н�ֹд,��ܿ��ܻ��������ݼ�¼,
//�Ӷ����·ְ���Ϣ��̬�仯,��ȡ�ķְ����ݾͲ�׼ȷ��
***************************************************************************/
u8 VDRData_ReadWriteEnableFlag(u8 Type);
/**************************************************************************
//��������VDRData_ReadPacket
//���ܣ���ȡָ����ŵ��ǰ�����
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVER_TIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//pBuffer:ָ�������׵�ַ��PacketNum:�����,��1��ʼ,ʱ�����ϵ�Ϊ��1��
//Direction:0:˳�����(��ʱ������ǰ��,��ʱ�����ں���);1:�������(��ʱ������ǰ��,��ʱ�����ں���)
//�������
//����ֵ����ȡ�����ݳ���
//��ע����ȡ����������Ϊ���������ݿ����ϣ�����������£�
//VDR_DATA_SPEED_PACKET_LIST		7//�������ʱ��ÿ�����������
//VDR_DATA_POSITION_PACKET_LIST		1
//VDR_DATA_DOUBT_PACKET_LIST		4
//VDR_DATA_OVER_TIME_PACKET_LIST	20
//VDR_DATA_DRIVER_PACKET_LIST		40
//VDR_DATA_POWER_PACKET_LIST		142
//VDR_DATA_PARAMETER_PACKET_LIST		142
//VDR_DATA_SPEED_STATUS_PACKET_LIST	7
***************************************************************************/
u16 VDRData_ReadPacket(u8 *pBuffer,u8 Type,u16 PacketNum,u8 Direction);
/**************************************************************************
//��������VDRData_ReadList
//���ܣ���ȡָ����ŵ���������
//���룺Type:���ͣ���ѡֵΪDATA_TYPE_SPEED��DATA_TYPE_POSITION��
//DATA_TYPE_DOUBT��DATA_TYPE_OVER_TIME��DATA_TYPE_DRIVER��
//DATA_TYPE_POWER��DATA_TYPE_PRAMATER��DATA_TYPE_SPEED_STATUS��
//pBuffer:ָ�������׵�ַ��ListNum:�����,��1��ʼ,ʱ�����ϵ�����Ϊ��1��
//�������
//����ֵ����ȡ�����ݳ���
//��ע����ȡ���������ݸ�ʽΪ����������ݿ��ʽ,������ʻ�ٶ����ݿ��ʽΪ��A.17
***************************************************************************/
u16 VDRData_ReadList(u8 *pBuffer,u8 Type,u16 ListNum);
/**************************************************************************
//��������VDRData_ReadTotalPacket(u8 Type)
//���ܣ���ĳ����¼���ʹ洢���ܰ��� 
//���룺Type:���ͣ���ѡֵΪDATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVER_TIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ���ܰ��� 
//��ע��
//VDR_DATA_SPEED_PACKET_LIST		7//�������ʱ��ÿ�����Ĵ������
//VDR_DATA_POSITION_PACKET_LIST		1
//VDR_DATA_DOUBT_PACKET_LIST		4
//VDR_DATA_OVER_TIME_PACKET_LIST	20
//VDR_DATA_DRIVER_PACKET_LIST		40
//VDR_DATA_POWER_PACKET_LIST		142
//VDR_DATA_PARAMETER_PACKET_LIST		142
//VDR_DATA_SPEED_STATUS_PACKET_LIST	7
***************************************************************************/
u16 VDRData_ReadTotalPacket(u8 Type);
/**************************************************************************
//��������VDRData_ReadTotalList(u8 Type)
//���ܣ���ĳ����¼���ʹ洢��������
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVER_TIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ��������
//��ע��
***************************************************************************/
u16 VDRData_ReadTotalList(u8 Type);

/**************************************************************************
//��������VDRData_ReadNewestTime(u8 Type)
//���ܣ���ȡ�洢�����¼�¼ʱ��
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVER_TIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ��32λhexֵ��ʾ��ʱ��
//��ע������ֵΪ0��ʾ�ô洢����û�м�¼�κ�����,����ֵΪ0xffffffff��ʾ��������
***************************************************************************/
u32 VDRData_ReadNewestTime(u8 Type);
/**************************************************************************
//��������VDRData_ReadOldestTime(u8 Type)
//���ܣ���ȡ�洢�����ϼ�¼ʱ��
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVER_TIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ��32λhexֵ��ʾ��ʱ��
//��ע������ֵΪ0��ʾ��û�м�¼����
***************************************************************************/
u32 VDRData_ReadOldestTime(u8 Type);

/**************************************************************************
//��������VDRData_ReadInitFlag
//���ܣ���ȡ��ʼ����־
//���룺��
//�������
//����ֵ����ʼ����־
//��ע��1��ʾ����ɳ�ʼ����0��ʾδ��ɳ�ʼ����0xFF��ʾ��������ֻ������ɳ�ʼ��д����Ч��
***************************************************************************/
u8 VDRData_ReadInitFlag(u8 Type);
/**************************************************************************
//��������VDRData_Erase(u8 Type)
//���ܣ�����ĳ���洢��������
//���룺Type:���ͣ���ѡֵΪVDR_DATA_TYPE_SPEED��VDR_DATA_TYPE_POSITION��
//VDR_DATA_TYPE_DOUBT��VDR_DATA_TYPE_OVER_TIME��VDR_DATA_TYPE_DRIVER��
//VDR_DATA_TYPE_POWER��VDR_DATA_TYPE_PRAMATER��VDR_DATA_TYPE_SPEED_STATUS��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void VDRData_Erase(u8 Type);

#endif

