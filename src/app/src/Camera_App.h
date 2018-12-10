
#include "stm32f10x.h"

#ifndef __CAMERA_APP_H
#define __CAMERA_APP_H


#define CAMERA_STATE_SCHEDULE 	0//����״̬
#define CAMERA_STATE_CHECK 	1//�Լ�״̬
#define CAMERA_STATE_PHOTO 	2//����״̬
#define CAMERA_STATE_FETCH 	3//ȡͼ״̬
#define CAMERA_STATE_STORE 	4//�洢״̬
#define CAMERA_STATE_SEND 	5//����״̬
#define CAMERA_STATE_RESEND 	6//����״̬
#define CAMERA_STATE_TIMING 	7//Уʱ״̬ 
#define CAMERA_STATE_MAX	7

#define CAMERA_STATE_CHECK_OVERTIME 	(20*10)//�Լ�״̬��ʱʱ�䣬50msΪ��λ��10�볬ʱ
#define CAMERA_STATE_PHOTO_OVERTIME 	(20*3)//����״̬��ʱʱ�䣬50msΪ��λ��5�볬ʱ
#define CAMERA_STATE_FETCH_OVERTIME 	(20*3)//ȡͼ״̬��ʱʱ��
#define CAMERA_STATE_STORE_OVERTIME 	(20*3)//�洢״̬��ʱʱ��
#define CAMERA_STATE_SEND_OVERTIME 	(20*3)//����״̬��ʱʱ��
#define CAMERA_STATE_RESEND_OVERTIME 	(20*10)//����״̬��ʱʱ��
#define CAMERA_STATE_TIMING_OVERTIME 	(20*10)//Уʱ״̬��ʱʱ��

#define CAMERA_ID_MAX	4//���֧��4·��ID��1��ʼ

#define CAMERA_DATA_BUFFER_SIZE 560
#define CAMERA_CMD_BUFFER_SIZE  20
#define CAMERA_RESEND_LIST_SIZE 20

typedef struct
{
	u8 OnOffFlag;//���߱�־
	u8 FetchFlag;//ȡͼ�����־

	s32 TimePhotoNum;//������������������1������1�Ĺ��ൽ�¼����գ�
	u16 TimePhotoSpace;//����ʱ����
	u8 TimePhotoStoreFlag;//�洢��־:bit0:1��ʾ��Ҫ�ϴ�,0��ʾ����Ҫ�ϴ�;bit1:1��ʾ��Ҫ����,0��ʾ����Ҫ����
	u8 TimePhotoResolution;//���շֱ���
	u8 TimePhotoEventType;//�����¼����ͣ�һ��̶�Ϊ0
	u8 TimePhotoChannel;//�ϴ�ͨ��
	u32 TimePhotoTime;//����ʱ��
	
	s32 EventPhotoNum;//�¼����������������3�ţ�����3�Ű�3����
	u16 EventPhotoSpace;//�¼�����ʱ����
	u8 EventPhotoStoreFlag;//�洢��־
	u8 EventPhotoResolution;//���շֱ���
	u8 EventPhotoEventType;//�����¼�����
	u8 EventPhotoChannel;//�ϴ�ͨ��
	u32 EventPhotoTime;//����ʱ��
	

}CAMERA_STRUCT;

/**************************************************************************
//��������Camera_TimeTask
//���ܣ�ʵ�����չ���
//���룺��
//�������
//����ֵ����
//��ע�����ն�ʱ����50ms����1�Σ������������Ҫ���ô˺�����
***************************************************************************/
FunctionalState Camera_TimeTask(void);
/**************************************************************************
//��������Camera_Photo
//���ܣ�����ͷ����
//���룺Channel:�ϴ�ͨ��;ֻ����CHANNEL_DATA_1����CHANNEL_DATA_2
//	CameraID:����ͷID��;ֻ����0~CAMERA_ID_MAX��0��ʾȫ�ģ�������ʾ������
//	PhotoNum:��������;��Ϊ0xfffe,0xffff��ʾһֱ���գ���Ϊ0ʱ��ʾֹͣ����
//	PhotoSpace:���ռ��;0ֵ��Ч
//	Resolution:���շֱ���;0Ϊ320*240�ֱ��ʣ�1Ϊ640*480�ֱ��ʣ�����ֵ��Ч
//	StoreFlag:�洢��־;bit0��1��ʾ��Ҫ�ϴ���bit1��1��ʾ��Ҫ���棬��������1��ʾ��Ҫ�ϴ�ҲҪ���棬����λ����
//	PhotoType:��������;0��ʾ��ʱ���գ�1��ʾ�¼����գ�����ֵ��Ч
//	EventType:�¼�����;0��ʾƽ̨�·�ָ�1��ʾ��ʱ������2��ʾ����������3��ʾ��ײ��෭�������������⼸��ʱ����ʹ��0
//�������
//����ֵ��0��ʾ�ɹ���1��ʾʧ��
//��ע���������ϴ�ͼƬʱ���յ����µ�����ָ��������ϴ���ͼƬ�ᶪʧ������ȥִ���µ�����ָ��
***************************************************************************/
u8 Camera_Photo(u8 Channel, u8 CameraID, u16 PhotoNum, u16 PhotoSpace, u8 Resolution, u8 StoreFlag, u8 PhotoType, u8 EventType);
/**************************************************************************
//��������Camera_MediaUploadAck
//���ܣ���ý���ϴ�Ӧ��
//���룺pBuffer�׵�ַָ���ش���������Ȼ���ǰ��б�
//�������
//����ֵ����
//��ע�����ն�ȫ���ϴ������а�ʱ��ƽ̨���·���ý���ϴ�Ӧ������0x8800��
//�յ������������ô˺���
***************************************************************************/
void Camera_MediaUploadAck(u8 *pBuffer, u16 BufferLen);
/**************************************************************************
//��������Camera_GetOnlineFlag
//���ܣ���ȡ����ͷ���߱�־
//���룺��
//�������
//����ֵ�����߱�־
//��ע��bit7~bit4������bit3~bit0:�ֱ��ʾ1~4������ͷ���߱�־��1���ߣ�0������
***************************************************************************/
u8 Camera_GetOnlineFlag(void);
/**************************************************************************
//��������Camera_GetTotalNum
//���ܣ���ȡ�ܵ���������ͷ����
//���룺��
//�������
//����ֵ���ܵ���������ͷ����
//��ע�����ΪCAMERA_ID_MAX��
***************************************************************************/
u8 Camera_GetTotalNum(void);
#endif