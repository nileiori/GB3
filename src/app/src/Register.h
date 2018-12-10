#ifndef __REGISTER_H
#define __REGISTER_H

//************�ļ�����***************
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "rtc.h"

//*************�궨��****************
#define REGISTER_TYPE_SPEED			0//��¼�Ǵ洢���ͣ�48Сʱ�ٶȼ�¼
#define REGISTER_TYPE_POSITION			1//360Сʱλ����Ϣ��¼
#define REGISTER_TYPE_DOUBT			2//�¹��ɵ����ݼ�¼
#define REGISTER_TYPE_OVER_TIME			3//��ʱ��ʻ��¼
#define REGISTER_TYPE_DRIVER			4//��ʻԱǩ��/ǩ�˼�¼
#define REGISTER_TYPE_POWER			5//��¼�ǹ���/�ϵ��¼
#define REGISTER_TYPE_PRAMATER			6//���ò�����¼
#define REGISTER_TYPE_SPEED_STATUS		7//�ٶ��쳣��¼
#define REGISTER_TYPE_MAX			8//���Ͳ��ܵ��ڻ���ڸ�ֵ

#define REGISTER_SPEED_STEP_LEN			144//5+126+13���洢�Ĳ�����4�ֽ�ʱ��+1�ֽ�У��+�������ݣ��ɼ�����+�������ݣ�
#define REGISTER_POSITION_STEP_LEN		681//5+666+10
#define REGISTER_DOUBT_STEP_LEN			251//5+234+12
#define REGISTER_OVER_TIME_STEP_LEN		67//5+50+12
#define REGISTER_DRIVER_STEP_LEN		36//5+25+6
#define REGISTER_POWER_STEP_LEN			18//5+7+6
#define REGISTER_PRAMATER_STEP_LEN		18//5+7+6
#define REGISTER_SPEED_STATUS_STEP_LEN		144//5+133+6
#define REGISTER_MAX_STEP_LEN			682////�������ܵ��ڻ���ڸ�ֵ

#define REGISTER_SPEED_COLLECT_LEN		126//�ɼ��ĳ��ȣ���ÿ�����ݳ���
#define REGISTER_POSITION_COLLECT_LEN		666
#define REGISTER_DOUBT_COLLECT_LEN		234
#define REGISTER_OVER_TIME_COLLECT_LEN		50
#define REGISTER_DRIVER_COLLECT_LEN		25
#define REGISTER_POWER_COLLECT_LEN		7
#define REGISTER_PRAMATER_COLLECT_LEN		7
#define REGISTER_SPEED_STATUS_COLLECT_LEN	133
//#define REGISTER_MAX_COLLECT_LEN		667

#define REGISTER_SPEED_PACKET_LIST		7//�������ʱ��ÿ�����Ĵ������
#define REGISTER_POSITION_PACKET_LIST		1
#define REGISTER_DOUBT_PACKET_LIST		4
#define REGISTER_OVER_TIME_PACKET_LIST		20
#define REGISTER_DRIVER_PACKET_LIST		40
#define REGISTER_POWER_PACKET_LIST		142
#define REGISTER_PRAMATER_PACKET_LIST		142
#define REGISTER_SPEED_STATUS_PACKET_LIST	7
//#define REGISTER_MAX_PACKET_LIST		143

#define REGISTER_BUFFER_SIZE			750

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
}REGISTER_STRUCT;

//************��������**************** 
/**************************************************************************
//��������Register_TimeTask(void)
//���ܣ���¼�����ݴ洢��ʱ����
//���룺��
//�������
//����ֵ��ʼ��ΪENABLE
//��ע��50ms����1�Σ���¼�����ݴ洢�ɸ�Ӧ�ó������Register_Write����ʵ�֣�������ֻ�Ƕ�
//�洢�ı�������ʼ����ʵʱ��⣬����������о�����
***************************************************************************/
FunctionalState Register_TimeTask(void);
/**************************************************************************
//��������Register_Write
//���ܣ�дһ����ʻ��¼������
//���룺Type:���ͣ���ѡֵΪREGISTER_TYPE_SPEED��REGISTER_TYPE_POSITION��
//REGISTER_TYPE_DOUBT��REGISTER_TYPE_OVER_TIME��REGISTER_TYPE_DRIVER��
//REGISTER_TYPE_POWER��REGISTER_TYPE_PRAMATER��REGISTER_TYPE_SPEED_STATUS��
//pBuffer:ָ�������׵�ַ��length:���ݳ��ȣ�Time:32λhex��ʾ��RTCʱ��
//�������
//����ֵ���ɹ�SUCCESS,ʧ��ERROR
//��ע����ʻ��¼�����ݼ�¼��flash�ĸ�ʽΪ��ʱ�䣨4�ֽڣ�+�������ݣ��̶����ȣ�+У��ͣ�1�ֽڣ���ǰ�������ֽ����ݵ�У��ͣ�
//REGISTER_TYPE_SPEED�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.17�涨�ĸ�ʽ��126�ֽڣ�+13�ֽ�Ԥ���ռ�
//REGISTER_TYPE_POSITION�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.19�涨�ĸ�ʽ��666�ֽڣ�+10�ֽ�Ԥ���ռ�
//REGISTER_TYPE_DOUBT�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.21�涨�ĸ�ʽ��234�ֽڣ�+12�ֽ�Ԥ���ռ�
//REGISTER_TYPE_OVER_TIME�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.24�涨�ĸ�ʽ��50�ֽڣ�+12�ֽ�Ԥ���ռ�
//REGISTER_TYPE_DRIVER�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.26�涨�ĸ�ʽ��25�ֽڣ�+6�ֽ�Ԥ���ռ�
//REGISTER_TYPE_POWER�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.28�涨�ĸ�ʽ��7�ֽڣ�+6�ֽ�Ԥ���ռ�
//REGISTER_TYPE_PRAMATER�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.30�涨�ĸ�ʽ��7�ֽڣ�+6�ֽ�Ԥ���ռ�
//REGISTER_TYPE_SPEED_STATUS�������ݸ�ʽΪ����ʻ��¼�Ǳ�׼��A.32�涨�ĸ�ʽ��133�ֽڣ�+6�ֽ�Ԥ���ռ�
***************************************************************************/
ErrorStatus Register_Write(u8 Type, u8 *pBuffer, u16 length, u32 Time);
/**************************************************************************
//��������Register_Read
//���ܣ���ȡָ������ʱ��ǰ��N����ʻ��¼������
//���룺Type:���ͣ���ѡֵΪREGISTER_TYPE_SPEED��REGISTER_TYPE_POSITION��
//REGISTER_TYPE_DOUBT��REGISTER_TYPE_OVER_TIME��REGISTER_TYPE_DRIVER��
//REGISTER_TYPE_POWER��REGISTER_TYPE_PRAMATER��REGISTER_TYPE_SPEED_STATUS��
//pBuffer:ָ�������׵�ַ��EndTime:����ʱ�䣻block:����
//�������
//����ֵ����ȡ�����ݳ���
//��ע�����ñ�����ķ�ʽ���������һ����¼��ʱ��պõ��ڽ���ʱ�䣬�ü�¼�������
***************************************************************************/
u16 Register_Read(u8 Type,u8 *pBuffer,TIME_T StartTime,TIME_T EndTime, u8 block); 
/**************************************************************************
//��������Register_GetNewestTime(u8 Type)
//���ܣ���ȡ�洢�����¼�¼ʱ��
//���룺Type:���ͣ���ѡֵΪREGISTER_TYPE_SPEED��REGISTER_TYPE_POSITION��
//REGISTER_TYPE_DOUBT��REGISTER_TYPE_OVER_TIME��REGISTER_TYPE_DRIVER��
//REGISTER_TYPE_POWER��REGISTER_TYPE_PRAMATER��REGISTER_TYPE_SPEED_STATUS
//�������
//����ֵ��32λhexֵ��ʾ��ʱ��
//��ע������ֵΪ0��ʾ�ô洢����û�м�¼�κ�����,����ֵΪ0xffffffff��ʾ��������
***************************************************************************/
u32 Register_GetNewestTime(u8 Type);
/**************************************************************************
//��������Register_GetOldestTime(u8 Type)
//���ܣ���ȡ�洢�����ϼ�¼ʱ��
//���룺Type:���ͣ���ѡֵΪREGISTER_TYPE_SPEED��REGISTER_TYPE_POSITION��
//REGISTER_TYPE_DOUBT��REGISTER_TYPE_OVER_TIME��REGISTER_TYPE_DRIVER��
//REGISTER_TYPE_POWER��REGISTER_TYPE_PRAMATER��REGISTER_TYPE_SPEED_STATUS
//�������
//����ֵ��32λhexֵ��ʾ��ʱ��
//��ע������ֵΪ0��ʾ��û�м�¼����
***************************************************************************/
u32 Register_GetOldestTime(u8 Type);
/**************************************************************************
//��������Register_EraseRecorderData(void)
//���ܣ�������¼�����д洢������
//���룺��
//�������
//����ֵ����
//��ע��������¼��flash�洢�����ݣ���������������ط��洢�ļ�¼������û��ȥ����
***************************************************************************/
void Register_EraseRecorderData(void);
/**************************************************************************
//��������Register_GetPacketData(u8 *pBuffer, u8 Type, u16 PacketNum)
//���ܣ���ȡ�ְ�����
//���룺Type:���ͣ���ѡֵΪREGISTER_TYPE_SPEED��REGISTER_TYPE_POSITION��
//REGISTER_TYPE_DOUBT��REGISTER_TYPE_OVER_TIME��REGISTER_TYPE_DRIVER��
//REGISTER_TYPE_POWER��REGISTER_TYPE_PRAMATER��REGISTER_TYPE_SPEED_STATUS
//PacketNum:����ţ���1��ʼ����ֵ�������ܰ�����ʱ�����µ���һ��Ϊ��1��
//������ְ�����
//����ֵ����ȡ�����ݳ���
//��ע�����ݳ������Ϊ1000�ֽڣ�������������£���ֵΪ�̶�ֵ��
//REGISTER_TYPE_SPEED_PACKET_LIST		7//�������ʱ��ÿ�����Ĵ������
//REGISTER_TYPE_POSITION_PACKET_LIST		1
//REGISTER_TYPE_DOUBT_PACKET_LIST		4
//REGISTER_TYPE_OVER_TIME_PACKET_LIST		20
//REGISTER_TYPE_DRIVER_PACKET_LIST		40
//REGISTER_TYPE_POWER_PACKET_LIST		142
//REGISTER_TYPE_PRAMATER_PACKET_LIST		142
//REGISTER_TYPE_SPEED_STATUS_PACKET_LIST	7
//--------------------------------�ر�ע��-----------------------------------
//�ر�ע�⣺�ڵ���Register_GetPacketData֮ǰ�����1��Register_GetTotalPacketNum������
//�ڷ�������Register_GetPacketData�ڼ䲻���ٵ���Register_GetTotalPacketNum������������ݳ���
//Ҫ�ְ���ȡ��ʻ��¼�����ݵĲ��������ǣ�
//1.����Register_GetTotalPacketNum
//2.����Register_GetPacketData
***************************************************************************/
u16 Register_GetPacketData(u8 *pBuffer, u8 Type, u16 PacketNum);
/**************************************************************************
//��������Register_GetTotalPacketNum(u8 Type)
//���ܣ���ȡ�ܰ���
//���룺Type:���ͣ���ѡֵΪREGISTER_TYPE_SPEED��REGISTER_TYPE_POSITION��
//REGISTER_TYPE_DOUBT��REGISTER_TYPE_OVER_TIME��REGISTER_TYPE_DRIVER��
//REGISTER_TYPE_POWER��REGISTER_TYPE_PRAMATER��REGISTER_TYPE_SPEED_STATUS
//�������
//����ֵ���ܰ��� 
//��ע�����ݳ������Ϊ1000�ֽڣ�������������£���ֵΪ�̶�ֵ��
//REGISTER_TYPE_SPEED_PACKET_LIST		7//�������ʱ��ÿ�����Ĵ������
//REGISTER_TYPE_POSITION_PACKET_LIST		1
//REGISTER_TYPE_DOUBT_PACKET_LIST		4
//REGISTER_TYPE_OVER_TIME_PACKET_LIST		20
//REGISTER_TYPE_DRIVER_PACKET_LIST		40
//REGISTER_TYPE_POWER_PACKET_LIST		142
//REGISTER_TYPE_PRAMATER_PACKET_LIST		142
//REGISTER_TYPE_SPEED_STATUS_PACKET_LIST	7
***************************************************************************/
u16 Register_GetTotalPacket(u8 Type);
/**************************************************************************
//��������Register_GetInitFlag(u8 Type)
//���ܣ���ȡ��ʼ����־
//���룺Type:���ͣ���ѡֵΪREGISTER_TYPE_SPEED��REGISTER_TYPE_POSITION��
//REGISTER_TYPE_DOUBT��REGISTER_TYPE_OVER_TIME��REGISTER_TYPE_DRIVER��
//REGISTER_TYPE_POWER��REGISTER_TYPE_PRAMATER��REGISTER_TYPE_SPEED_STATUS
//�������
//����ֵ����ʼ����־��1��ʾ�ô洢����ĳ�ʼ������ɣ�0��ʾδ��ɡ�
//��ע��ֻ�иô洢����ĳ�ʼ����־Ϊ1ʱ����������д���ݲŻ���Ч������дʧ��
***************************************************************************/
u8 Register_GetInitFlag(u8 Type);
#endif