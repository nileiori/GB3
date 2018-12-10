/************************************************************************
//�������ƣ�Camera_App.c 
//���ܣ���ģ��ʵ�����չ���
//�汾�ţ�V0.3
//��Ȩ˵������Ȩ�����������������¼����������޹�˾
//�����ˣ�dxl
//����ʱ�䣺2014.11
//�ļ���������Ҫʵ�����չ��ܣ�����֧��4·����ͷ��320*240��640*480���ֱַ���
//���յķ�ʽ�У�ƽ̨�·�ָ�����գ���1��Ҳ�ɶ��Ż������ţ���ĳ1·����ͷ��Ҳ��ȫ��4·����ͷ����
//�¼��������գ�һ����ÿ·1�ţ�Ҳ�ɶ��ţ���಻����3�ţ���
//������ʱ���գ������������ա����ȼ��Ӹߵ��ͷֱ�Ϊ��ƽ̨�·�ָ�����գ��¼��������ա�
//��ʱ�������ա�����������գ�ֻ���ڴ���������ȼ������������Żᴦ������ȼ�����������
//��ʵ��˫��������ҵ����������������Ķ����Կ����ն����գ��ն��������յ����Ǹ������������ָ��//Ϊ׼��

//�汾��¼���汾��¼������汾���޸��ˡ��޸�ʱ�䡢�޸�ԭ���޸ļ�Ҫ˵��
//V0.1  dxl 2014.11  ������������ͷ���չ��ܣ�����ȶ��ԺͿɿ��ԣ��ṩ˫�������չ�������
//V0.2  dxl 2015.01  ��������ͷУʱ���ܣ��ն��ϵ絼���������ͷУʱ1�Σ�֮��ÿ��12СʱУʱ1��
//V0.3  dxl 2015.03  ��ֹ����ͷ����ʣ��������ȡ�����µ��ն��Զ��������󣬽���������£�
��1��ֻ�е�����ָ�������Ϊ0xFFFEʱ�ſ���һֱ���չ��ܣ�
��2��������ָ�����������255��ʱ����255�Ŵ���С��255��ʱ�����·�����������
��3���ն�������ÿ��2Сʱ�����eeprom��ȡ����ָ�����������ʣ�������������ڴ�eeprom��ȡ������ʱ�����������ϣ����������ֹͣ���ա�
��4����Ϊ������һֱ���չ���ʱ����ÿ��2Сʱ��ȡeeprom��������ֵ���Լ���Ӧ�ı���ֵ����2��ֵ����ʱֹͣ���ա�
*************************************************************************/

/********************�ļ�����*************************/
#include <stdio.h>
#include <string.h>
#include "modem_app_first.h"
#include "Camera_App.h"
#include "ff.h"
#include "modem_lib.h"
#include "modem_app_com.h"
#include "Public.h"
#include "fm25c160.h"
#include "GPIOControl.h"
#include "Io.h"
#include "RadioProtocol.h"
#include "Camera_Driver.h"
#include "Blind.h"
#include "rtc.h"
#include "EepromPram.h"
#include "SysTickApp.h"
#include "Media.h"
#include "E2prom_25LC320A.h"
#include "other.h"
#include "Report.h"
#include "usbh_usr.h"
/********************���ر���*************************/
static u8 CameraTaskInitFlag = 0;
static u8 CameraState = 0;//��ǰ����ͷ״̬
static u8 CameraSubState = 0;//��ǰ����ͷ��״̬
static u8 CameraPhotoID = 1;//��ǰ����ID����1~CAMERA_ID_MAX
static u8 CameraFetchID = 1;//��ǰȡͼID����1~CAMERA_ID_MAX
static u8 CameraCheckID = 1;//��ǰ�Լ�ID����1~CAMERA_ID_MAX
static u8 CameraTimingID = 1;//УʱID����1~CAMERA_ID_MAX
static u8 CameraPhotoInfo[CAMERA_ID_MAX*36] = {0};//8�ֽڵ�ͼƬ��Ϣ+28�ֽڵĻ���λ����Ϣ
static u8 CameraResolution[CAMERA_ID_MAX] = {0};//���շֱ��ʣ�0��ʾ320*240,1��ʾ640*480
static u8 CameraPhotoType[CAMERA_ID_MAX] = {0};//�������ͣ�0Ϊ��ʱ��ƽ̨����1Ϊ�¼�
static u8 CameraEventType[CAMERA_ID_MAX] = {0};//��ý���¼�����
static u8 CameraStoreFlag[CAMERA_ID_MAX] = {0};//�����־��0Ϊ�����棬1Ϊ����
static u16 CameraTimeSpace[CAMERA_ID_MAX] = {0};//����ʱ����
static u8 CameraChannel[CAMERA_ID_MAX] = {0};//�ϴ�ͨ��
static u16 CameraTotalPacket[CAMERA_ID_MAX] = {0};//ͼƬ�ܰ���
static u16 CameraStateTimeCount[CAMERA_STATE_MAX+1] = {0};//״̬��ʱ
static u16 CameraPacket = 0;//ȡͼ����ţ���0��ʼ
static FIL CameraFile;
static u8 CameraFileName[40];
static u32 StateCheckCount1 = 0;
static u32 StateCheckCount2 = 0;
static s32 CameraPhotoCmd = 0;//����ͷ��������

/********************ȫ�ֱ���*************************/
CAMERA_STRUCT Camera[CAMERA_ID_MAX];
u8 CameraPowerFlag = 0;//����ͷ�����־��1Ϊ���磬0Ϊ�ص�
u8 CameraResendEnableFlag = 0;//�ش���������ʹ�ܱ�־��ֻ�����ϴ������һ��ʱ��ʹ�ܣ���������ֹʹ�ܡ�
u8 CameraResendFlag = 0;//�ش���־��1��ʾ�ش���0��ʾ�����ش�
u8 CameraStateAck[CAMERA_STATE_MAX+1] = {0};//״̬��־��0Ϊ��ʼֵ��1ΪӦ����ȷ��2Ϊ��Ӧ�𣨳�ʱ��
u8 CameraStateErrorCount[CAMERA_STATE_MAX+1] = {0};//״̬�������
u8 CameraDataBuffer[CAMERA_DATA_BUFFER_SIZE] = {0};//ͼ�����ݻ��壬�洢��������ͷͼƬ���ݣ�����������ͷЭ���֡ͷ��֡β������
u16 CameraDataBufferLen;//ͼ�����ݳ���
//u8 CameraDataBufferBusyFlag = 0;//æ��־��1æ��0����
u8 CameraCmdAckBuffer[CAMERA_CMD_BUFFER_SIZE] = {0};//����ͷ����Ӧ�𻺳壬�洢����ͷӦ��ָ���������ȡͼ��Ӧ�𣬰���֡ͷ��֡β����
u8 CameraCmdAckBufferLen;//����Ӧ�𳤶�
u16 CameraResendPacketList[CAMERA_RESEND_LIST_SIZE] = {0};//�ش������б�
u16 CameraResendTotalPacket = 0;//�ش��ܰ���
u32 PhotoID = 1;//��ý��ID

/********************�ⲿ����*************************/

/********************���غ�������*********************/
static void Camera_StateSchedule(void);
static void Camera_StateCheck(void);
static void Camera_StatePhoto(void);
static void Camera_StateFetch(void);
static void Camera_StateStore(void);
static void Camera_StateSend(void);
static void Camera_StateResend(void);
static void Camera_StateTiming(void);
static void Camera_StateSwitch(u8 State);
static void Camera_TaskInit(void);
static void Camera_ErrorHandle(void);
static u8 Camera_ScheduleTiming(void);
static u8 Camera_SchedulePhoto(void);
static u8 Camera_ScheduleFetch(void);
static void Camera_ScheduleCheck(void);
static void Camera_SavePhotoNumToFram(void);
static void Camera_WriteParameter(u8 Channel, u8 CameraID, u16 PhotoNum, u16 PhotoSpace, u8 Resolution, u8 StoreFlag, u8 PhotoType, u8 EventType);

/********************��������*************************/
/**************************************************************************
//��������Camera_TimeTask
//���ܣ�ʵ�����չ���
//���룺��
//�������
//����ֵ����
//��ע�����ն�ʱ����50ms����1�Σ������������Ҫ���ô˺�����
***************************************************************************/
FunctionalState Camera_TimeTask(void)
{
    if(0 == CameraTaskInitFlag)
    {
        CameraTaskInitFlag = 1;
        Camera_TaskInit();
        Camera_StateSwitch(CAMERA_STATE_CHECK);
    }
    
    switch(CameraState)
    {
        case CAMERA_STATE_SCHEDULE:
        {
            Camera_StateSchedule();
            break;
        }
        case CAMERA_STATE_CHECK:
        {
            Camera_StateCheck();
            break;
        }
        case CAMERA_STATE_PHOTO:
        {
            Camera_StatePhoto();
            break;
        }
        case CAMERA_STATE_FETCH:
        {
            Camera_StateFetch();
            break;
        }
        case CAMERA_STATE_STORE:
        {
            Camera_StateStore();
            break;
        }
        case CAMERA_STATE_SEND:
        {
            Camera_StateSend();
            break;
        }
        case CAMERA_STATE_RESEND:
        {
            Camera_StateResend();
            break;
        }
        case CAMERA_STATE_TIMING:
        {
            Camera_StateTiming();
            break;
        }
        default:
        {
            break;
        }
    }

    Camera_ErrorHandle();

    return ENABLE;
}
/**************************************************************************
//��������Camera_Photo
//���ܣ�����ͷ����
//���룺Channel:�ϴ�ͨ��;ֻ����CHANNEL_DATA_1����CHANNEL_DATA_2
//  CameraID:����ͷID��;ֻ����0~CAMERA_ID_MAX��0��ʾȫ�ģ�������ʾ������
//  PhotoNum:��������;��Ϊ0xfffe��ʾһֱ���գ���Ϊ0ʱ��ʾֹͣ����
//  PhotoSpace:���ռ��;0ֵ��Ч
//  Resolution:���շֱ���;1Ϊ320*240�ֱ��ʣ�2Ϊ640*480�ֱ��ʣ�����ֵ��Ч
//  StoreFlag:�洢��־;bit0��1��ʾ��Ҫ�ϴ���bit1��1��ʾ��Ҫ���棬��������1��ʾ��Ҫ�ϴ�ҲҪ���棬����λ����
//  PhotoType:��������;0��ʾ��ʱ���գ�1��ʾ�¼����գ�����ֵ��Ч
//  EventType:�¼�����;0��ʾƽ̨�·�ָ�1��ʾ��ʱ������2��ʾ����������3��ʾ��ײ��෭�������������⼸��ʱ����ʹ��0
//�������
//����ֵ��0��ʾ�ɹ���1��ʾʧ��
//��ע���������ϴ�ͼƬʱ���յ����µ�����ָ��������ϴ���ͼƬ�ᶪʧ������ȥִ���µ�����ָ��
***************************************************************************/
u8 Camera_Photo(u8 Channel, u8 CameraID, u16 PhotoNum, u16 PhotoSpace, u8 Resolution, u8 StoreFlag, u8 PhotoType, u8 EventType)
{
    u8 i;
    u8 Buffer[5];

    if(CameraID > CAMERA_ID_MAX)
    {
        return 1;
    }
    if(PhotoType > 1)
    {
        return 1;
    }

    if((Channel&CHANNEL_DATA_2) == CHANNEL_DATA_2)//��Ҫ�Ե�2����������
    {
        if(1 == Blind_GetLink2OpenFlag())
        {

        }
        else
        {
            Channel = Channel & (~CHANNEL_DATA_2);
        }
    }
    

    if(((Channel&CHANNEL_DATA_1) == CHANNEL_DATA_1)||((Channel&CHANNEL_DATA_2) == CHANNEL_DATA_2))
    {
        if(0 == PhotoNum)
        {
            for(i=1; i<=CAMERA_ID_MAX; i++)
            {
                Camera[i-1].FetchFlag = 0;
                Camera[i-1].TimePhotoNum = 0;
                Camera[i-1].EventPhotoNum = 0;
                Public_ConvertLongToBuffer(Camera[i-1].TimePhotoNum,Buffer);
                FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_NUM_ADDR+(i-1)*(FRAM_CAM_TIME_PHOTO_NUM_LEN+1),Buffer+2,FRAM_CAM_TIME_PHOTO_NUM_LEN);//�洢�ĺ�2�ֽ�
                Public_ConvertLongToBuffer(Camera[i-1].EventPhotoNum,Buffer);
                FRAM_BufferWrite(FRAM_CAM_EVENT_PHOTO_NUM_ADDR+(i-1)*(FRAM_CAM_EVENT_PHOTO_NUM_LEN+1),Buffer+2,FRAM_CAM_EVENT_PHOTO_NUM_LEN);//�洢�ĺ�2�ֽ�
            }   
            for(i=0; i<=CAMERA_STATE_MAX; i++)
            {
                CameraStateAck[i] = 0;
                CameraStateErrorCount[i] = 0;
            }
            CameraState = 0;
            CameraSubState = 0;
            CameraPhotoID = 1;
            CameraFetchID = 1;
            CameraCheckID = 1;
            CameraPacket = 0;
            StateCheckCount1 = 0;
            StateCheckCount2 = 0;
            CameraResendEnableFlag = 0;
            CameraResendFlag = 0;
            CameraResendTotalPacket = 0;
            Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        
        }
        else
        {
            if(0 == CameraID)
            {
                
                for(i=1; i<=CAMERA_ID_MAX; i++)
                {
                    Camera_WriteParameter(Channel,i,PhotoNum,PhotoSpace,Resolution,StoreFlag,PhotoType,EventType);
                }

            }
            else
            {
                Camera_WriteParameter(Channel,CameraID,PhotoNum,PhotoSpace,Resolution,StoreFlag,PhotoType,EventType);
            }
        }
    }
    else
    {
        return 1;
    }

    return 0;
}
/**************************************************************************
//��������Camera_MediaUploadAck
//���ܣ���ý���ϴ�Ӧ��
//���룺pBuffer�׵�ַָ���ش���������Ȼ���ǰ��б�
//�������
//����ֵ����
//��ע�����ն�ȫ���ϴ������а�ʱ��ƽ̨���·���ý���ϴ�Ӧ������0x8800��
//�յ������������ô˺����������������1��ͼ��ʱ���ն˻�ȴ�10�룬�յ���Ӧ��Ჹ����10��û�յ���Ӧ�𲻲���
***************************************************************************/
void Camera_MediaUploadAck(u8 *pBuffer, u16 BufferLen)
{
    u8 i;
    u8 *p = NULL;
    u16 length;
    u16 temp;
    u32 ResendPhotoID;

	
    if((BufferLen > (2*CAMERA_RESEND_LIST_SIZE+1))||(CAMERA_STATE_RESEND != CameraState))
    {
        return ;
    }
		if(p != NULL)
		{
		    return ;
		}
		if(0 == BufferLen%2)
		{
		    return ;
		}
		

		p=pBuffer;
		length = BufferLen;

    ResendPhotoID = 0;
		ResendPhotoID |= *p<<24;
		p++;
		ResendPhotoID |= *p<<16;
		p++;
		ResendPhotoID |= *p<<8;
		p++;
		ResendPhotoID |= *p;
		p++;
		
		if(ResendPhotoID != PhotoID)
		{
		    return ;
		}
		
	  CameraResendTotalPacket = *p++;
		
    length -= 5;

    for(i=0; i<(length/2); i++)
    {
         temp = 0;
         temp |= *p++ << 8;
         temp |= *p++;
         CameraResendPacketList[i] = temp-1;//����ͷȡͼ������Ǵ�0��ʼ���ϴ��İ�����Ǵ�1��ʼ
    }

		
}
/**************************************************************************
//��������Camera_GetOnlineFlag
//���ܣ���ȡ����ͷ���߱�־
//���룺��
//�������
//����ֵ�����߱�־
//��ע��bit7~bit4������bit3~bit0:�ֱ��ʾ1~4������ͷ���߱�־��1���ߣ�0������
***************************************************************************/
u8 Camera_GetOnlineFlag(void)
{
    u8 i;
    u8 temp;

    temp = 0;
    for(i=1; i<=CAMERA_ID_MAX; i++)
    {
        if(1 == Camera[i-1].OnOffFlag)
        {
            temp |= 1 << (i-1);
        }
    }
    
    return temp;    
}
/**************************************************************************
//��������Camera_GetTotalNum
//���ܣ���ȡ�ܵ���������ͷ����
//���룺��
//�������
//����ֵ���ܵ���������ͷ����
//��ע�����ΪCAMERA_ID_MAX��
***************************************************************************/
u8 Camera_GetTotalNum(void)
{
    u8 i;
    u8 temp;

    temp = 0;
    for(i=1; i<=CAMERA_ID_MAX; i++)
    {
        if(1 == Camera[i-1].OnOffFlag)
        {
            temp++;
        }
    }
    
    return temp;
}
/**************************************************************************
//��������Camera_TaskInit
//���ܣ���ʼ������ͷ��ر���
//���룺��
//�������
//����ֵ����
//��ע��Camera_TimeTask��Ҫ���ô˺���
***************************************************************************/
static void Camera_TaskInit(void)
{
    u8 i,j;
    u8 Buffer[5];
    u8 BufferLen;
    u16 Address;

    for(i=1; i<=CAMERA_ID_MAX; i++)
    {

        CameraResolution[i-1] = 0;
        CameraPhotoType[i-1] = 0;
        CameraEventType[i-1] = 0;
        CameraStoreFlag[i-1] = 0;
        CameraTimeSpace[i-1] = 0;
        CameraChannel[i-1] = 0;
        CameraTotalPacket[i-1] = 0;
        CameraStateTimeCount[i-1] = 0;

        Camera[i-1].OnOffFlag = 0;
        Camera[i-1].FetchFlag = 0;

        Camera[i-1].TimePhotoNum = 0;
        Camera[i-1].TimePhotoSpace = 0;
        Camera[i-1].TimePhotoStoreFlag = 0;
        Camera[i-1].TimePhotoResolution = 0;
        Camera[i-1].TimePhotoEventType = 0;
        Camera[i-1].TimePhotoChannel = 0;
        Camera[i-1].TimePhotoTime = 0;

        Camera[i-1].EventPhotoNum = 0;
        Camera[i-1].EventPhotoSpace = 0;
        Camera[i-1].EventPhotoStoreFlag = 0;
        Camera[i-1].EventPhotoResolution = 0;
        Camera[i-1].EventPhotoEventType = 0;
        Camera[i-1].EventPhotoChannel = 0;
        Camera[i-1].EventPhotoTime = 0;

        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_TIME_PHOTO_NUM_LEN,FRAM_CAM_TIME_PHOTO_NUM_ADDR+(i-1)*(FRAM_CAM_TIME_PHOTO_NUM_LEN+1));
        if(FRAM_CAM_TIME_PHOTO_NUM_LEN == BufferLen)
        {
            Camera[i-1].TimePhotoNum = Public_ConvertBufferToShort(Buffer);
            if((Camera[i-1].TimePhotoNum > 255)&&(Camera[i-1].TimePhotoNum != 0xFFFE))
            {
                Camera[i-1].TimePhotoNum = 255;
            }
            if(Camera[i-1].TimePhotoNum < 0)
            {
                Camera[i-1].TimePhotoNum = 0;
            }
        }

        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_TIME_PHOTO_SPACE_LEN,FRAM_CAM_TIME_PHOTO_SPACE_ADDR+(i-1)*(FRAM_CAM_TIME_PHOTO_SPACE_LEN+1));
        if(FRAM_CAM_TIME_PHOTO_SPACE_LEN == BufferLen)
        {
            Camera[i-1].TimePhotoSpace = Public_ConvertBufferToShort(Buffer);
        }

        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_TIME_PHOTO_STORE_FLAG_LEN,FRAM_CAM_TIME_PHOTO_STORE_FLAG_ADDR+(i-1)*(FRAM_CAM_TIME_PHOTO_STORE_FLAG_LEN+1));
        if(FRAM_CAM_TIME_PHOTO_STORE_FLAG_LEN == BufferLen)
        {
            Camera[i-1].TimePhotoStoreFlag = Buffer[0];
        }

        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_TIME_PHOTO_RESOLUTION_LEN,FRAM_CAM_TIME_PHOTO_RESOLUTION_ADDR+(i-1)*(FRAM_CAM_TIME_PHOTO_RESOLUTION_LEN+1));
        if(FRAM_CAM_TIME_PHOTO_RESOLUTION_LEN == BufferLen)
        {
            Camera[i-1].TimePhotoResolution = Buffer[0];
        }

        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_TIME_PHOTO_EVENT_TYPE_LEN,FRAM_CAM_TIME_PHOTO_EVENT_TYPE_ADDR+(i-1)*(FRAM_CAM_TIME_PHOTO_EVENT_TYPE_LEN+1));
        if(FRAM_CAM_TIME_PHOTO_EVENT_TYPE_LEN == BufferLen)
        {
            Camera[i-1].TimePhotoEventType = Buffer[0];
        }

        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_TIME_PHOTO_CHANNEL_LEN,FRAM_CAM_TIME_PHOTO_CHANNEL_ADDR+(i-1)*(FRAM_CAM_TIME_PHOTO_CHANNEL_LEN+1));
        if(FRAM_CAM_TIME_PHOTO_CHANNEL_LEN == BufferLen)
        {
            Camera[i-1].TimePhotoChannel = Buffer[0];
        }

        //////////////////////////////////////////
        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_EVENT_PHOTO_NUM_LEN,FRAM_CAM_EVENT_PHOTO_NUM_ADDR+(i-1)*(FRAM_CAM_EVENT_PHOTO_NUM_LEN+1));
        if(FRAM_CAM_EVENT_PHOTO_NUM_LEN == BufferLen)
        {
            Camera[i-1].EventPhotoNum = Public_ConvertBufferToShort(Buffer);
            if(Camera[i-1].EventPhotoNum > 5)
            {
                Camera[i-1].EventPhotoNum = 5;
            }
            if(Camera[i-1].EventPhotoNum < 0)
            {
                Camera[i-1].EventPhotoNum = 0;
            }
        }

        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_EVENT_PHOTO_SPACE_LEN,FRAM_CAM_EVENT_PHOTO_SPACE_ADDR+(i-1)*(FRAM_CAM_EVENT_PHOTO_SPACE_LEN+1));
        if(FRAM_CAM_EVENT_PHOTO_SPACE_LEN == BufferLen)
        {
            Camera[i-1].EventPhotoSpace = Public_ConvertBufferToShort(Buffer);
        }

        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_EVENT_PHOTO_STORE_FLAG_LEN,FRAM_CAM_EVENT_PHOTO_STORE_FLAG_ADDR+(i-1)*(FRAM_CAM_EVENT_PHOTO_STORE_FLAG_LEN+1));
        if(FRAM_CAM_EVENT_PHOTO_STORE_FLAG_LEN == BufferLen)
        {
            Camera[i-1].EventPhotoStoreFlag = Buffer[0];
        }

        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_EVENT_PHOTO_RESOLUTION_LEN,FRAM_CAM_EVENT_PHOTO_RESOLUTION_ADDR+(i-1)*(FRAM_CAM_EVENT_PHOTO_RESOLUTION_LEN+1));
        if(FRAM_CAM_EVENT_PHOTO_RESOLUTION_LEN == BufferLen)
        {
            Camera[i-1].EventPhotoResolution = Buffer[0];
        }

        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_EVENT_PHOTO_EVENT_TYPE_LEN,FRAM_CAM_EVENT_PHOTO_EVENT_TYPE_ADDR+(i-1)*(FRAM_CAM_EVENT_PHOTO_EVENT_TYPE_LEN+1));
        if(FRAM_CAM_EVENT_PHOTO_EVENT_TYPE_LEN == BufferLen)
        {
            Camera[i-1].EventPhotoEventType = Buffer[0];
        }

        BufferLen = FRAM_BufferRead(Buffer,FRAM_CAM_EVENT_PHOTO_CHANNEL_LEN,FRAM_CAM_EVENT_PHOTO_CHANNEL_ADDR+(i-1)*(FRAM_CAM_EVENT_PHOTO_CHANNEL_LEN+1));
        if(FRAM_CAM_EVENT_PHOTO_CHANNEL_LEN == BufferLen)
        {
            Camera[i-1].EventPhotoChannel = Buffer[0];
        }
    }

    for(i=0; i<=CAMERA_STATE_MAX; i++)
    {
        CameraStateAck[i] = 0;
        CameraStateErrorCount[i] = 0;
    }

    BufferLen = FRAM_BufferRead(Buffer,FRAM_PHOTO_ID_LEN,FRAM_PHOTO_ID_ADDR);
    if(FRAM_PHOTO_ID_LEN == BufferLen)
    {
        PhotoID = Public_ConvertBufferToLong(Buffer);
    }

    for(i=0; i<3; i++)
    {
        Buffer[i] = 0;
    }
    Address = E2_CAMERA_PHOTO_CMD_ADDR;
    E2prom_ReadByte(Address,Buffer,3);
    if(Buffer[2] == (Buffer[0]+Buffer[1]))
    {
        CameraPhotoCmd = 0;
        CameraPhotoCmd |= Buffer[0] << 8;
        CameraPhotoCmd |= Buffer[1];    
    }
    else
    {
        CameraPhotoCmd = 0;
    }
    for(i=1; i<=CAMERA_ID_MAX; i++)
    {
        if(Camera[i-1].TimePhotoNum > CameraPhotoCmd)
        {
            Camera[i-1].TimePhotoNum = 0;
            for(j=0; j<4; j++)
            {
                Buffer[j] = 0;
            }
            FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_NUM_ADDR+(i-1)*(FRAM_CAM_TIME_PHOTO_NUM_LEN+1),Buffer,FRAM_CAM_TIME_PHOTO_NUM_LEN);
        }
    }

    CameraState = 0;
    CameraSubState = 0;
    CameraPhotoID = 1;
    CameraFetchID = 1;
    CameraCheckID = 1;
    CameraPacket = 0;
    StateCheckCount1 = 0;
    StateCheckCount2 = 0;
    CameraResendEnableFlag = 0;
    CameraResendFlag = 0;
    CameraResendTotalPacket = 0;
}

/**************************************************************************
//��������Camera_StateSwitch
//���ܣ�ʵ������ͷ״̬�л�����
//���룺��
//�������
//����ֵ����
//��ע��Camera_StateSchedule��Ҫ���ô˺�������ʱʱCamera_StateCheck
//״̬�л�����ֻ�ܴ�����״̬�л�������״̬����ӵ���״̬�л�������״̬
***************************************************************************/
static void Camera_StateSwitch(u8 State)
{
    
    if((State > CAMERA_STATE_MAX)||(CameraState > CAMERA_STATE_MAX))
    {
        return ;
    }

    if(CAMERA_STATE_SCHEDULE == State)
    {
        if(CameraStateAck[CameraState] > 1)//����
        {
            CameraStateErrorCount[CameraState]++;
            if(CameraStateErrorCount[CameraState] > 254)
            {
                CameraStateErrorCount[CameraState] = 254;
            }
        }
        else
        {
            CameraStateErrorCount[CameraState] = 0;
        }
        CameraState = State;
        CameraSubState = 0;
    }
    else
    {
        if(CAMERA_STATE_SCHEDULE == CameraState)
        {
            CameraState = State;
            CameraSubState = 0;
            CameraStateTimeCount[State] = 0;
            CameraStateAck[State] = 0;
        }
    }
}
/**************************************************************************
//��������Camera_ErrorHandle
//���ܣ�����ͷ������
//���룺��
//�������
//����ֵ����
//��ע������ͷ��������Camera_TimeTask��Ҫ���ô˺���
***************************************************************************/
static void Camera_ErrorHandle(void)
{
    u8 flag;
    u8 i,j;
    u8 Buffer[5] = {0};
    u16 Address;

    static u32 count = 0;

    flag = 0;

    if((CameraPhotoID > CAMERA_ID_MAX)||(0 == CameraPhotoID))//���ID�Ƿ�����
    {
        flag = 1;
    }
    if((CameraFetchID > CAMERA_ID_MAX)||(0 == CameraFetchID))
    {
        flag = 1;
    }
    if((CameraCheckID > CAMERA_ID_MAX)||(0 == CameraCheckID))
    {
        flag = 1;
    }

    if( 0 == CameraResendFlag)//�������
    {
        if(CameraPacket > CameraTotalPacket[CameraFetchID-1])
        {
            flag = 1;
        }
    }
    else
    {
        if(CameraPacket > CameraResendTotalPacket)
        {
            flag = 1;
        }
    }

    for(i=1; i<=CAMERA_ID_MAX; i++)
    {
        if(Camera[i-1].TimePhotoNum < 0)
        {
            Camera[i-1].TimePhotoNum = 0;
            for(j=0; j<4; j++)
            {
                Buffer[j] = 0;
            }
            FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_NUM_ADDR+(i-1)*(FRAM_CAM_TIME_PHOTO_NUM_LEN+1),Buffer,FRAM_CAM_TIME_PHOTO_NUM_LEN);
        }
        if((Camera[i-1].EventPhotoNum < 0)||(Camera[i-1].EventPhotoNum > 5))
        {
            Camera[i-1].EventPhotoNum = 0;
            for(j=0; j<4; j++)
            {
                Buffer[j] = 0;
            }
            FRAM_BufferWrite(FRAM_CAM_EVENT_PHOTO_NUM_ADDR+(i-1)*(FRAM_CAM_EVENT_PHOTO_NUM_LEN+1),Buffer,FRAM_CAM_EVENT_PHOTO_NUM_LEN); 
        }
    }

    if((CameraStateErrorCount[CAMERA_STATE_PHOTO] > 3)||(CameraStateErrorCount[CAMERA_STATE_FETCH] > 3))//����״̬�Ƿ�����
    {
        flag = 1;
    }

    if(1 == flag)
    {
        CameraTaskInitFlag = 1;
    }

    count++;
    if(count >= 2*3600*20)//2Сʱ��ȡһ��eepromֵ���бȽ�
    {
        count = 0;
        for(i=0; i<3; i++)
        {
            Buffer[i] = 0;
        }
        Address = E2_CAMERA_PHOTO_CMD_ADDR;
        E2prom_ReadByte(Address,Buffer,3);
        if(Buffer[2] == (Buffer[0]+Buffer[1]))
        {
            CameraPhotoCmd = 0;
            CameraPhotoCmd |= Buffer[0] << 8;
            CameraPhotoCmd |= Buffer[1];    
        }
        else
        {
            CameraPhotoCmd = 0;
        }
        for(i=1; i<=CAMERA_ID_MAX; i++)
        {
            if(Camera[i-1].TimePhotoNum > CameraPhotoCmd)
            {
                Camera[i-1].TimePhotoNum = 0;
                for(j=0; j<4; j++)
                {
                    Buffer[j] = 0;
                }
                FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_NUM_ADDR+(i-1)*(FRAM_CAM_TIME_PHOTO_NUM_LEN+1),Buffer,FRAM_CAM_TIME_PHOTO_NUM_LEN);
            }
        }
    }

    
}
/**************************************************************************
//��������Camera_StateSchedule
//���ܣ�ʵ������ͷ���ո�״̬�ĵ��ȣ�״̬�л���
//���룺��
//�������
//����ֵ����
//��ע������ͷ��������Camera_TimeTask��Ҫ���ô˺���
***************************************************************************/
static void Camera_StateSchedule(void)
{

    if(1 == Camera_ScheduleTiming())//����Уʱ��Уʱ���ȼ��ϸ�
    {
        StateCheckCount1 = 0;
        StateCheckCount2 = 0;
        return ;
    }

    if( 1 == Camera_SchedulePhoto())//�������գ�ֻ��������״̬
    {
        StateCheckCount1 = 0;
        StateCheckCount2 = 0;
        return;
    }

    if(1 == Camera_ScheduleFetch())//����ȡͼ������ȡͼ���洢�����͡���������״̬
    {
        StateCheckCount1 = 0;
        StateCheckCount2 = 0;
        return;
    }

    

    Camera_ScheduleCheck();//�Լ���ƣ�ʡ�����
}
/**************************************************************************
//��������Camera_ScheduleTiming
//���ܣ�����Ƿ���Уʱ����Уʱ��������ͷÿ���ϵ�1�Σ��ն˵�1�ε���Уʱ1�Σ�ÿ��12СʱУʱ1��
//���룺��
//�������
//����ֵ��1��ʾ��Уʱ����0��ʾû��
//��ע������Camera_StateSchedule��Ҫ���ô˺���
***************************************************************************/
static u8 Camera_ScheduleTiming(void)
{
    static u8 NavigationTimingFlag = 0;//�ϵ絼���������ͷУʱ
    static u8 PowerOnTimingFlag = 0;//�ϵ������ͷУʱ
    static u32 StateTimingCount = 0;

    u8 NavigationFlag = 0;

    

    NavigationFlag = Io_ReadStatusBit(STATUS_BIT_NAVIGATION);

    if((0 == PowerOnTimingFlag)&&(1 == CameraTaskInitFlag))
    {
        PowerOnTimingFlag = 1;
        Camera_StateSwitch(CAMERA_STATE_TIMING);
        return 1;
    }

    if((1 == NavigationFlag)&&(0 == NavigationTimingFlag)&&(1 == CameraTaskInitFlag))
    {
        NavigationTimingFlag = 1;
        Camera_StateSwitch(CAMERA_STATE_TIMING);
        return 1;
    }

    StateTimingCount++;
    if((StateTimingCount >= SECOND*43200)&&(1 == NavigationFlag))//12��Сʱ
    {
        StateTimingCount = 0;
        Camera_StateSwitch(CAMERA_STATE_TIMING);
        return 1;
    }   

    return 0;
}
/**************************************************************************
//��������Camera_SchedulePhoto
//���ܣ�����Ƿ�����������
//���룺��
//�������
//����ֵ��1��ʾ����������0��ʾû��
//��ע������Camera_StateSchedule��Ҫ���ô˺���
***************************************************************************/
static u8 Camera_SchedulePhoto(void)
{
    u8 i;
    u32 time;

    time = RTC_GetCounter();

    for(i=1; i<=CAMERA_ID_MAX; i++)//��������¼�����������
    {
        if((1 == Camera[i-1].OnOffFlag)&&(0 == Camera[i-1].FetchFlag)&&(Camera[i-1].EventPhotoNum > 0))
        {
            if((0x01 == (0x01&Camera[i-1].EventPhotoStoreFlag))&&(0 != (Camera[i-1].EventPhotoChannel&GetTerminalAuthorizationFlag())))//��Ҫ�ϴ����ж�ͨ���Ƿ�����
            {
                if(time >= Camera[i-1].EventPhotoTime)
                {
                    CameraPhotoID = i;

                    CameraResolution[i-1] = Camera[i-1].EventPhotoResolution;
                    CameraEventType[i-1] = Camera[i-1].EventPhotoEventType;
                    CameraTimeSpace[i-1] = Camera[i-1].EventPhotoSpace;
                    CameraChannel[i-1] = Camera[i-1].EventPhotoChannel;
                    CameraStoreFlag[i-1] = Camera[i-1].EventPhotoStoreFlag;
                    CameraPhotoType[i-1] = 1;

                    Camera_StateSwitch(CAMERA_STATE_PHOTO);

                    return 1;
                }
                
            }
            if((0x02 == (0x02&Camera[i-1].EventPhotoStoreFlag))&&(1 == Media_GetSdOnLineFlag()))//��Ҫ������ж�SD���Ƿ�����
            {
                if(time >= Camera[i-1].EventPhotoTime)
                {
                    CameraPhotoID = i;

                    CameraResolution[i-1] = Camera[i-1].EventPhotoResolution;
                    CameraEventType[i-1] = Camera[i-1].EventPhotoEventType;
                    CameraTimeSpace[i-1] = Camera[i-1].EventPhotoSpace;
                    CameraChannel[i-1] = Camera[i-1].EventPhotoChannel;
                    CameraStoreFlag[i-1] = Camera[i-1].EventPhotoStoreFlag;
                    CameraPhotoType[i-1] = 1;

                    Camera_StateSwitch(CAMERA_STATE_PHOTO);

                    return 1;
                }
            }   
        }
    }

    for(i=1; i<=CAMERA_ID_MAX; i++)//�������ƽ̨��ʱ��������
    {
        if((1 == Camera[i-1].OnOffFlag)&&(0 == Camera[i-1].FetchFlag)&&(Camera[i-1].TimePhotoNum > 0))
        {
            if((0x01 == (0x01&Camera[i-1].TimePhotoStoreFlag))&&(0 != (Camera[i-1].TimePhotoChannel&GetTerminalAuthorizationFlag())))//��Ҫ�ϴ����ж�ͨ���Ƿ�����
            {
                if(time >= Camera[i-1].TimePhotoTime)
                {
                    CameraPhotoID = i;

                    CameraResolution[i-1] = Camera[i-1].TimePhotoResolution;
                    CameraEventType[i-1] = Camera[i-1].TimePhotoEventType;
                    CameraTimeSpace[i-1] = Camera[i-1].TimePhotoSpace;
                    CameraChannel[i-1] = Camera[i-1].TimePhotoChannel;
                    CameraStoreFlag[i-1] = Camera[i-1].TimePhotoStoreFlag;
                    CameraPhotoType[i-1] = 0;

                    Camera_StateSwitch(CAMERA_STATE_PHOTO);

                    return 1;
                }
            }

            if((0x02 == (0x02&Camera[i-1].TimePhotoStoreFlag))&&(1 == Media_GetSdOnLineFlag()))//��Ҫ������ж�SD���Ƿ�����
            {
                if(time >= Camera[i-1].TimePhotoTime)
                {
                    CameraPhotoID = i;

                    CameraResolution[i-1] = Camera[i-1].TimePhotoResolution;
                    CameraEventType[i-1] = Camera[i-1].TimePhotoEventType;
                    CameraTimeSpace[i-1] = Camera[i-1].TimePhotoSpace;
                    CameraChannel[i-1] = Camera[i-1].TimePhotoChannel;
                    CameraStoreFlag[i-1] = Camera[i-1].TimePhotoStoreFlag;
                    CameraPhotoType[i-1] = 0;

                    Camera_StateSwitch(CAMERA_STATE_PHOTO);

                    return 1;
                }
            }   
        }
    }


    return 0;
}
/**************************************************************************
//��������Camera_ScheduleFetch
//���ܣ�����Ƿ���ȡͼ����
//���룺��
//�������
//����ֵ��1��ʾ��ȡͼ����0��ʾû��
//��ע������Camera_StateSchedule��Ҫ���ô˺���
***************************************************************************/
static u8 Camera_ScheduleFetch(void)
{
    u8 i,j;

    if((1 == Camera[CameraFetchID-1].OnOffFlag)&&(Camera[CameraFetchID-1].FetchFlag >= CAMERA_STATE_FETCH))//����Ƿ���������ȡͼ��
    {
        if(Camera[CameraFetchID-1].FetchFlag >= CAMERA_STATE_RESEND)
        {
            CameraPacket = 0;
            CameraResendEnableFlag = 0;
            if(CameraResendTotalPacket > 0)
            {
                CameraResendFlag = 1;
                Camera_StateSwitch(CAMERA_STATE_FETCH);
                return 1;
            }
            else
            {
                CameraResendFlag = 0;
                Camera[CameraFetchID-1].FetchFlag = 0;
                Camera_SavePhotoNumToFram();
            }
        }
        else if(Camera[CameraFetchID-1].FetchFlag == CAMERA_STATE_SEND)
        {
            if(0 == CameraResendFlag)//�������ϴ�
            {
                if(CameraPacket >= (CameraTotalPacket[CameraFetchID-1]-1))//���һ��
                {
                    CameraResendTotalPacket = 0;
                    for(i=0; i<CAMERA_RESEND_LIST_SIZE; i++)
                    {
                        CameraResendPacketList[i] = 0;
                    }
                    CameraResendEnableFlag = 1;
                    Camera_StateSwitch(CAMERA_STATE_RESEND);
                    return 1;
                
                }
                else
                {
                    CameraPacket++;
                    Camera_StateSwitch(CAMERA_STATE_FETCH);
                    return 1;
                }
                    
            }
            else//�ش�
            {
                if(CameraPacket >= (CameraResendTotalPacket-1))//���һ��
                {
                    CameraResendFlag = 0;
                    Camera[CameraFetchID-1].FetchFlag = 0;
                    Camera_SavePhotoNumToFram();
                
                }
                else
                {
                    CameraPacket++;
                    Camera_StateSwitch(CAMERA_STATE_FETCH);
                    return 1;
                }   
            }
            
        }
        else
        {
            Camera_StateSwitch(Camera[CameraFetchID-1].FetchFlag+1);
            return 1;
        }

            
    }

    j = CameraFetchID;//��������ȡͼ�ı�־����Ҫ��0
    if((j > CAMERA_ID_MAX)||(0 == j))
    {
        j = 1;
    }
    for(i = 0; i<=CAMERA_ID_MAX; i++)//����Ƿ���ȡͼ����
    {
        if((1 == Camera[j-1].OnOffFlag)&&(Camera[j-1].FetchFlag >= CAMERA_STATE_FETCH))
        {
            Camera[j-1].FetchFlag = 0;  
        }
        j++;
        if(j > CAMERA_ID_MAX)
        {
            j = 1;
        }
    }


    j = CameraFetchID+1;
    if((j > CAMERA_ID_MAX)||(0 == j))
    {
        j = 1;
    }
    for(i = 0; i<=CAMERA_ID_MAX; i++)//����Ƿ���ȡͼ����
    {
        if((1 == Camera[j-1].OnOffFlag)&&(Camera[j-1].FetchFlag == CAMERA_STATE_PHOTO))
        {
            Camera_StateSwitch(CAMERA_STATE_FETCH);
            CameraFetchID = j;
            CameraPacket = 0;
            return 1;   
        }
        j++;
        if(j > CAMERA_ID_MAX)
        {
            j = 1;
        }
    }


    return 0;
}
/**************************************************************************
//��������Camera_ScheduleCheck
//���ܣ�����Ƿ����Լ�����
//���룺��
//�������
//����ֵ��1��ʾ���Լ�����0��ʾû��
//��ע������Camera_StateSchedule��Ҫ���ô˺���
***************************************************************************/
static void Camera_ScheduleCheck(void)
{
    u8 acc;

    acc = Io_ReadStatusBit(STATUS_BIT_ACC);

    if(1 == acc)//ACC��ʱ��û�����մ���ʱÿ��60����һ������ͷ������״̬
    {
        StateCheckCount1++;
        if(StateCheckCount1 >= (SECOND*60))
        {
            StateCheckCount1 = 0;
            CameraCheckID = 1;
            Camera_StateSwitch(CAMERA_STATE_CHECK);
        }
    }
    else//ACC OFFʱ�ڵ紦��:����ȡͼ�������10�뽫�ر�����ͷ��Դ��ACC OFFʱ���Լ�����ͷ
    {
        
        StateCheckCount2++;
        if(StateCheckCount2 >= (SECOND*10))
        {
            StateCheckCount2 = 0;
            CAMERA_POWER_OFF();//�ص�
        }       
    }

    return ;
}
/**************************************************************************
//��������Camera_SavePhotoNumToFram
//���ܣ�����ʣ����������������
//���룺��
//�������
//����ֵ����
//��ע������Camera_ScheduleFetch��Ҫ���ô˺�����ÿ����1��ʣ�������Զ���1
***************************************************************************/
static void Camera_SavePhotoNumToFram(void)
{
    u8 Buffer[5];

    if(0 == CameraPhotoType[CameraFetchID-1])
    {
        if(Camera[CameraFetchID-1].TimePhotoNum == 0xfffe)//һֱ��
        {

        }
        else if(Camera[CameraFetchID-1].TimePhotoNum > 0)
        {
            if(0x01 == (0x01&CameraStoreFlag[CameraFetchID-1]))
            {
                if(1 == CameraStateAck[CAMERA_STATE_SEND])
                {
                    Camera[CameraFetchID-1].TimePhotoNum--;
                }
            }
            else if(0x02 == (0x02&CameraStoreFlag[CameraFetchID-1]))
            {
                if(1 == CameraStateAck[CAMERA_STATE_STORE])
                {
                    Camera[CameraFetchID-1].TimePhotoNum--;
                }
            }
            if(Camera[CameraFetchID-1].TimePhotoNum > 255)
            {
                Camera[CameraFetchID-1].TimePhotoNum = 255;
            }
            Public_ConvertLongToBuffer(Camera[CameraFetchID-1].TimePhotoNum,Buffer);
            FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_NUM_ADDR+(CameraFetchID-1)*(FRAM_CAM_TIME_PHOTO_NUM_LEN+1),Buffer+2,FRAM_CAM_TIME_PHOTO_NUM_LEN);//�洢�ĺ�2�ֽ�       
        }
        else
        {
            Camera[CameraFetchID-1].TimePhotoNum = 0;
            Public_ConvertLongToBuffer(Camera[CameraFetchID-1].TimePhotoNum,Buffer);
            FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_NUM_ADDR+(CameraFetchID-1)*(FRAM_CAM_TIME_PHOTO_NUM_LEN+1),Buffer+2,FRAM_CAM_TIME_PHOTO_NUM_LEN);//�洢�ĺ�2�ֽ�
        }
        
    }
    else
    {
        if(Camera[CameraFetchID-1].EventPhotoNum > 0)
        {
            if(0x01 == (0x01&CameraStoreFlag[CameraFetchID-1]))
            {
                if(1 == CameraStateAck[CAMERA_STATE_SEND])
                {
                    Camera[CameraFetchID-1].EventPhotoNum--;
                }
            }
            else if(0x02 == (0x02&CameraStoreFlag[CameraFetchID-1]))
            {
                if(1 == CameraStateAck[CAMERA_STATE_STORE])
                {
                    Camera[CameraFetchID-1].EventPhotoNum--;
                }
            }
        }
        else
        {
            Camera[CameraFetchID-1].EventPhotoNum = 0;
        }
        Public_ConvertLongToBuffer(Camera[CameraFetchID-1].EventPhotoNum,Buffer);
        FRAM_BufferWrite(FRAM_CAM_EVENT_PHOTO_NUM_ADDR+(CameraFetchID-1)*(FRAM_CAM_EVENT_PHOTO_NUM_LEN+1),Buffer+2,FRAM_CAM_EVENT_PHOTO_NUM_LEN);//�洢�ĺ�2�ֽ�
    }
}
/**************************************************************************
//��������Camera_StateCheck
//���ܣ�ʵ������ͷ�Լ�״̬����
//���룺��
//�������
//����ֵ����
//��ע������ͷ��������Camera_TimeTask��Ҫ���ô˺���
//���η����Լ��������·����ͷ������Ӧ����OnOffFlagΪ0����Ӧ��OnOffFlagΪ1
***************************************************************************/
static void Camera_StateCheck(void)
{
    u8 Buffer[20] = {0x40,0x40,0x69,0xff,0xff,0x0d,0x0a};//����ͷ�Լ�����
    u8 length = 7;
    u8 i;
    //u8 Buffer[20] = {0x40,0x40,0x60,0x5a,0xff,0xff,0x01,0x00,0x0d,0x0a};//��������ͷ��IDΪ1
    //u8 Buffer[20] = {0x40,0x40,0x60,0x5a,0xff,0xff,0x02,0x00,0x0d,0x0a};//��������ͷ��IDΪ2
    //u8 Buffer[20] = {0x40,0x40,0x60,0x5a,0xff,0xff,0x03,0x00,0x0d,0x0a};//��������ͷ��IDΪ3
    //u8 Buffer[20] = {0x40,0x40,0x60,0x5a,0xff,0xff,0x04,0x00,0x0d,0x0a};//��������ͷ��IDΪ4
    //length = 10;

    static u16 TimeCount = 0;

    if(CameraCheckID > CAMERA_ID_MAX)
    {
        CameraStateAck[CAMERA_STATE_CHECK] = 4;
        CameraCheckID = 1;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }

    CameraStateTimeCount[CAMERA_STATE_CHECK]++;
    if(CameraStateTimeCount[CAMERA_STATE_CHECK] > CAMERA_STATE_CHECK_OVERTIME)//��ʱ
    {
        Camera[CameraCheckID].OnOffFlag = 0;
        CameraStateAck[CAMERA_STATE_CHECK] = 4;
        CameraCheckID = 1;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }

    if(0 == CameraSubState)//�ϵ�
    {
        if(0 == CameraPowerFlag)
        {
            CAMERA_POWER_ON();//�ϵ�
            CameraSubState = 1;//�ϵ����ȴ�һ��ʱ��
        }
        else
        {
            CameraSubState = 2;//�����ȴ�״̬
            TimeCount = 0;
        }
    }
    else if(1 == CameraSubState)//�ϵ���ʱ
    {
        if(CameraStateTimeCount[CAMERA_STATE_CHECK] > (CAMERA_STATE_CHECK_OVERTIME/2))
        {
            CameraSubState = 2;
            TimeCount = 0;
        }
    }
    else if(2 == CameraSubState)//����ָ��
    {
        Buffer[3] = CameraCheckID;
        Buffer[4] = 0;
        if(ACK_OK == Usart3_SendData(Buffer,length))
        {
            CameraSubState = 3;
        }
    }
    else if(3 == CameraSubState)//�ȴ�Ӧ��
    {
        TimeCount++;
        if(1 == CameraStateAck[CAMERA_STATE_CHECK])//��Ӧ��
        {
            Camera[CameraCheckID-1].OnOffFlag = 1;
            CameraCheckID++;//��һ������ͷ
            if(CameraCheckID > CAMERA_ID_MAX)
            {
                for(i=1; i<=CAMERA_ID_MAX; i++)
                {
                    if(1 == Camera[i-1].OnOffFlag)
                    {
                        break;
                    }
                }
                if(i > CAMERA_ID_MAX)
                {
                    Io_WriteAlarmBit(ALARM_BIT_CAMERA_FAULT,SET);
                }
                else
                {
                    Io_WriteAlarmBit(ALARM_BIT_CAMERA_FAULT,RESET);
                }
                CameraCheckID = 1;
                Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
            }
            CameraStateAck[CAMERA_STATE_CHECK] = 0;
            CameraSubState = 0;
            TimeCount = 0;
        }
        else if(2 == CameraStateAck[CAMERA_STATE_CHECK])//��Ӧ��
        {
            if(TimeCount > SECOND)
            {
                Camera[CameraCheckID-1].OnOffFlag = 0;
                CameraCheckID++;//��һ������ͷ
                if(CameraCheckID > CAMERA_ID_MAX)
                {
                    for(i=1; i<=CAMERA_ID_MAX; i++)
                    {
                        if(1 == Camera[i-1].OnOffFlag)
                        {
                            break;
                        }
                    }
                    if(i > CAMERA_ID_MAX)
                    {
                        Io_WriteAlarmBit(ALARM_BIT_CAMERA_FAULT,SET);
                    }
                    else
                    {
                        Io_WriteAlarmBit(ALARM_BIT_CAMERA_FAULT,RESET);
                    }
                    CameraCheckID = 1;
                    Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
                }
                CameraSubState = 0;
                TimeCount = 0;
            }
        }
        else//��ⳬʱ
        {
            if(TimeCount > SECOND)
            {
                CameraStateAck[CAMERA_STATE_CHECK] = 2;//��ʱ1�룬�ط�1��
                CameraSubState = 2;
                TimeCount = 0;
            }
        }
    }

    
}
/**************************************************************************
//��������Camera_StatePhoto
//���ܣ�ʵ������ͷ����״̬����
//���룺��
//�������
//����ֵ����
//��ע������ͷ��������Camera_TimeTask��Ҫ���ô˺���
***************************************************************************/
static void Camera_StatePhoto(void)
{
    u8 Buffer[20] = {0x40,0x40,0x61,0x81,0x00,0x00,0x00,0x02,0x0d,0x0a};//����ͷ�������� 
    u8 length = 10;
    u8 *p = NULL;
    u8 PhotoIDBuffer[5] = {0};

    static u16 TimeCount = 0;

    if(CameraPhotoID > CAMERA_ID_MAX)
    {
        CameraStateAck[CAMERA_STATE_PHOTO] = 4;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }
    
    CameraStateTimeCount[CAMERA_STATE_PHOTO]++;
    if(CameraStateTimeCount[CAMERA_STATE_PHOTO] > CAMERA_STATE_PHOTO_OVERTIME)//��ʱ
    {
        Camera[CameraPhotoID-1].OnOffFlag = 0;
        Camera[CameraPhotoID-1].FetchFlag = 0;
        CameraStateAck[CAMERA_STATE_PHOTO] = 4;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }

    if(0 == CameraSubState)//�ϵ�
    {
        if(0 == CameraPowerFlag)
        {
            CAMERA_POWER_ON();//�ϵ�
            CameraSubState = 1;//�ϵ����ȴ�һ��ʱ��
        }
        else
        {
            CameraSubState++;//�����ȴ�״̬
            CameraSubState++;
            TimeCount = 0;
        }
    }
    else if(1 == CameraSubState)//�ϵ���ʱ
    {
        if(CameraStateTimeCount[CAMERA_STATE_PHOTO] > (CAMERA_STATE_CHECK_OVERTIME/2))
        {
            CameraSubState++;
            TimeCount = 0;
        }
    }
    else if(2 == CameraSubState)//����ָ��
    {
        if(1 == CameraResolution[CameraPhotoID-1])
        {
            Buffer[3] = 0x81;
        }
        else if(2 == CameraResolution[CameraPhotoID-1])
        {
            Buffer[3] = 0x82;
        }
        else
        {
            Buffer[3] = 0x81;
        }
        Buffer[4] = CameraPhotoID;
        Buffer[5] = 0;
        if(ACK_OK == Usart3_SendData(Buffer,length))
        {
            CameraStateAck[CAMERA_STATE_PHOTO] = 0;
            CameraSubState++;
        }
    }
    else if(3 == CameraSubState)//�ȴ�Ӧ��
    {
        TimeCount++;
        if(1 == CameraStateAck[CAMERA_STATE_PHOTO])//��Ӧ��
        {
            CameraTotalPacket[CameraPhotoID-1] = (CameraCmdAckBuffer[6] << 8)|CameraCmdAckBuffer[5];
            PhotoID++;//ID��1
            Public_ConvertLongToBuffer(PhotoID,PhotoIDBuffer);
            FRAM_BufferWrite(FRAM_PHOTO_ID_ADDR,PhotoIDBuffer,FRAM_PHOTO_ID_LEN);
            p = CameraPhotoInfo+(CameraPhotoID-1)*36;
            *p++ = (PhotoID&0xff000000) >> 24;//��ý��ID
            *p++ = (PhotoID&0xff0000) >> 16;
            *p++ = (PhotoID&0xff00) >> 8;
            *p++ = PhotoID&0xff;
            *p++ = 0;//��ý�����ͣ�0Ϊͼ��
            *p++ = 0;//��ý���ʽ��0ΪJPEG
            *p++ = CameraEventType[CameraPhotoID-1];//��ý���¼�
            *p++ = CameraPhotoID;//��ý��ͨ��
            Report_GetPositionBasicInfo(p);

            Camera[CameraPhotoID-1].FetchFlag = CAMERA_STATE_PHOTO;
            if(0 == CameraPhotoType[CameraPhotoID-1])
            {
                Camera[CameraPhotoID-1].TimePhotoTime = RTC_GetCounter()+CameraTimeSpace[CameraPhotoID-1];
            }
            else
            {
                Camera[CameraPhotoID-1].EventPhotoTime = RTC_GetCounter()+CameraTimeSpace[CameraPhotoID-1];
            }

            TimeCount = 0;

            Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        }
        else if(3 == CameraStateAck[CAMERA_STATE_PHOTO])//Ӧ�����
        {
            if(TimeCount > SECOND)
            {
                CameraSubState--;
                TimeCount = 0;
            }
        }
        else//��ʱ���
        {
            if(TimeCount > SECOND)
            {
                CameraSubState--;
                TimeCount = 0;
            }
        }
    }
}
/**************************************************************************
//��������Camera_StateFetch
//���ܣ�ʵ������ͷȡͼ״̬����
//���룺��
//�������
//����ֵ����
//��ע������ͷ��������Camera_TimeTask��Ҫ���ô˺���
***************************************************************************/
static void Camera_StateFetch(void)
{
    u8 Buffer[20] = {0x40,0x40,0x62,0x81,0x00,0x00,0x00,0x00,0x0d,0x0a};//����ͷȡͼ����
    u8 length = 10;
    static u16 TimeCount = 0;
    
    Camera[CameraFetchID-1].FetchFlag = CAMERA_STATE_FETCH;

    if(CameraFetchID > CAMERA_ID_MAX)
    {
        CameraStateAck[CAMERA_STATE_FETCH] = 4;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }
    
    CameraStateTimeCount[CAMERA_STATE_FETCH]++;
    if(CameraStateTimeCount[CAMERA_STATE_FETCH] > CAMERA_STATE_FETCH_OVERTIME)//��ʱ
    {
        Camera[CameraFetchID-1].OnOffFlag = 0;
        Camera[CameraFetchID-1].FetchFlag = 0;
        CameraStateAck[CAMERA_STATE_FETCH] = 4;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }

    if(0 == CameraSubState)//����ָ��
    {
        
        if(0 == CameraResolution[CameraFetchID-1])
        {
            Buffer[3] = 0x81;
        }
        else if(1 == CameraResolution[CameraFetchID-1])
        {
            Buffer[3] = 0x82;
        }
        else
        {
            Buffer[3] = 0x81;
        }
        Buffer[4] = CameraFetchID;
        Buffer[5] = 0;
        if(0 == CameraResendFlag)
        {
            Buffer[6] = CameraPacket&0x00ff;
            Buffer[7] = (CameraPacket&0xff00) >> 8;
        }
        else
        {
            Buffer[6] = CameraResendPacketList[CameraPacket]&0x00ff;
            Buffer[7] = (CameraResendPacketList[CameraPacket]&0xff00) >> 8;
        }
        if(ACK_OK == Usart3_SendData(Buffer,length))
        {
            CameraStateAck[CAMERA_STATE_FETCH] = 0;
            CameraSubState++;
        }
    }
    else if(1 == CameraSubState)//�ȴ�Ӧ��
    {
        TimeCount++;
        if(1 == CameraStateAck[CAMERA_STATE_FETCH])//��Ӧ��
        {
            Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
            TimeCount = 0;
        }
        else if(3 == CameraStateAck[CAMERA_STATE_FETCH])
        {
            CameraSubState--;
            TimeCount = 0;
        }
        else//��ʱ���
        {
            if(TimeCount > SECOND)
            {
                CameraSubState--;
                TimeCount = 0;
            }
        }
    }
}
/**************************************************************************
//��������Camera_StateStore
//���ܣ�ʵ������ͷ�洢״̬����
//���룺��
//�������
//����ֵ����
//��ע������ͷ��������Camera_TimeTask��Ҫ���ô˺���
***************************************************************************/
static void Camera_StateStore(void)
{
    static u8 WriteEnableFlag = 0;
	  u32 Bw;
    Camera[CameraFetchID-1].FetchFlag = CAMERA_STATE_STORE;

    if(CameraFetchID > CAMERA_ID_MAX)
    {
        CameraStateAck[CAMERA_STATE_STORE] = 4;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }

    CameraStateTimeCount[CAMERA_STATE_STORE]++;
    if(CameraStateTimeCount[CAMERA_STATE_STORE] > CAMERA_STATE_STORE_OVERTIME)//��ʱ
    {
        CameraStateAck[CAMERA_STATE_STORE] = 4;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }

    if(1 == CameraResendFlag)
    {
        CameraStateAck[CAMERA_STATE_STORE] = 0;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }

    if((1 == Media_GetSdOnLineFlag())&&(1 == Camera[CameraFetchID-1].OnOffFlag)&&(0x02 == (0x02&CameraStoreFlag[CameraFetchID-1])))
    {
        if(0 == CameraPacket)//��1��
        {
            WriteEnableFlag = 0;                
            Media_GetFileName(CameraFileName,MEDIA_TYPE_JPG);
            if(FR_OK == f_open(&CameraFile,ff_NameConver(CameraFileName),FA_OPEN_ALWAYS|FA_READ|FA_WRITE))
            {
                WriteEnableFlag = 1;
            }
            else
            {
                Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
            }   
            
        }
      
        if(1 == WriteEnableFlag)
        {
            if(FR_OK == f_write(&CameraFile,CameraDataBuffer,CameraDataBufferLen,&Bw))
            {
                if(CameraPacket >= (CameraTotalPacket[CameraFetchID-1]-1))//���1��
                {
                    if(FR_OK == f_write(&CameraFile,CameraPhotoInfo+(CameraFetchID-1)*36,36,&Bw))
                    {
                
                    }
                    else
                    {
                        
                    }
                    f_close(&CameraFile);
                    Media_SaveLog(PhotoID,MEDIA_TYPE_JPG);
                }

                CameraStateAck[CAMERA_STATE_STORE] = 1;
                Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
            }
            else
            {
                WriteEnableFlag = 0;
                CameraStateAck[CAMERA_STATE_STORE] = 2;
                Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
            }

        }
        else
        {
            Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        }   
        
    }
    else
    {
        CameraStateAck[CAMERA_STATE_STORE] = 0;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
    }
}
/**************************************************************************
//��������Camera_StateSend
//���ܣ�ʵ������ͷ����״̬����
//���룺��
//�������
//����ֵ����
//��ע������ͷ��������Camera_TimeTask��Ҫ���ô˺���
***************************************************************************/
static void Camera_StateSend(void)
{
    static u16 TimeCount = 0;
    static u8 WaitFlag = 0;
    static u16 WaitCount = 0;

    u16 length;
	  u8  OnlineFlag[3] = {0,0,0};
    
    if(1 == WaitFlag)
    {
        WaitCount++;
        if(WaitCount >= 5*SYSTICK_0p1SECOND)
        {
            WaitCount = 0;
            WaitFlag = 0;
        }
        return ;
    }
    
    Camera[CameraFetchID-1].FetchFlag = CAMERA_STATE_SEND;

    if(CameraFetchID > CAMERA_ID_MAX)
    {
        CameraStateAck[CAMERA_STATE_SEND] = 4;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }
    
    CameraStateTimeCount[CAMERA_STATE_SEND]++;
    if(CameraStateTimeCount[CAMERA_STATE_SEND] > CAMERA_STATE_SEND_OVERTIME)//��ʱ
    {
        CameraStateAck[CAMERA_STATE_SEND] = 4;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }

		OnlineFlag[1] = (GetTerminalAuthorizationFlag()&0x02) >> 1;
		OnlineFlag[2] = (GetTerminalAuthorizationFlag()&0x04) >> 2;
    if((CHANNEL_DATA_1 == (CameraChannel[CameraFetchID-1]&CHANNEL_DATA_1))&&(1 == OnlineFlag[1]))
		{
		
		}
		else if((CHANNEL_DATA_2 == (CameraChannel[CameraFetchID-1]&CHANNEL_DATA_2))&&(1 == OnlineFlag[2]))
		{
		
		}
		else
    {
        CameraStateAck[CAMERA_STATE_SEND] = 2;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }


    if(0x01 != (0x01&CameraStoreFlag[CameraFetchID-1]))
    {
        CameraStateAck[CAMERA_STATE_SEND] = 2;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }

    if(0 == CameraSubState)//����
    {
        if(0 == CameraResendFlag)
        {
            length = CameraDataBufferLen;
            if(0 == CameraPacket)//��1��
            {
                memmove(CameraDataBuffer+36,CameraDataBuffer,CameraDataBufferLen);
                memcpy(CameraDataBuffer,CameraPhotoInfo+(CameraFetchID-1)*36,36);
                length += 36;
            }
            if(ACK_OK == RadioProtocol_MultiMediaDataReport(CameraChannel[CameraFetchID-1],CameraDataBuffer,length,CameraTotalPacket[CameraFetchID-1],CameraPacket+1))
            {
                CameraStateAck[CAMERA_STATE_SEND] = 1;
                CameraSubState++;
                TimeCount = 0;
            }
            else
            {
                WaitFlag = 1;
            }
        }
        else
        {
            length = CameraDataBufferLen;
            if(0 == CameraResendPacketList[CameraPacket])//��1��
            {
                memmove(CameraDataBuffer+36,CameraDataBuffer,CameraDataBufferLen);
                memcpy(CameraDataBuffer,CameraPhotoInfo+(CameraFetchID-1)*36,36);
                length += 36;
            }
            if(ACK_OK == RadioProtocol_MultiMediaDataReport(CameraChannel[CameraFetchID-1],CameraDataBuffer,length,CameraTotalPacket[CameraFetchID-1],CameraResendPacketList[CameraPacket]+1))
            {
                CameraStateAck[CAMERA_STATE_SEND] = 1;
                CameraSubState++;
                TimeCount = 0;
            }
            else
            {
                WaitFlag = 1;
            }
        }
    }
    else if(1 == CameraSubState)//�ȴ�Ӧ��
    {
        TimeCount++;
        if((CHANNEL_DATA_1 == CameraChannel[CameraFetchID-1])||(CHANNEL_DATA_2 == CameraChannel[CameraFetchID-1]))
        {
            if(TimeCount >= 8*SYSTICK_0p1SECOND)
            {
                TimeCount = 0;
                Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
            }
        }
        else//˫ͨ��ͬʱ����ͼƬ�Ļ����ܹ���
        {
            if(TimeCount >= 16*SYSTICK_0p1SECOND)
            {
                TimeCount = 0;
                Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
            }
        }
    }
}
/**************************************************************************
//��������Camera_StateResend
//���ܣ�ʵ������ͷ����״̬����
//���룺��
//�������
//����ֵ����
//��ע������ͷ��������Camera_TimeTask��Ҫ���ô˺���
***************************************************************************/
static void Camera_StateResend(void)
{
    Camera[CameraFetchID-1].FetchFlag = CAMERA_STATE_RESEND;
    CameraStateTimeCount[CAMERA_STATE_RESEND]++;
    if(CameraStateTimeCount[CAMERA_STATE_RESEND] > CAMERA_STATE_RESEND_OVERTIME)//��ʱ
    {
        CameraStateAck[CAMERA_STATE_RESEND] = 4;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }

    if(CameraResendTotalPacket > 0)
    {
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
        return ;
    }
}
/**************************************************************************
//��������Camera_StateTiming
//���ܣ�ʵ������ͷУʱ״̬����
//���룺��
//�������
//����ֵ����
//��ע������ͷ��������Camera_TimeTask��Ҫ���ô˺���
***************************************************************************/
static void Camera_StateTiming(void)
{
    u8 Buffer[20] = {0x40,0x40,0x65,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0d,0x0a};//����ͷУʱ����
    u8 length = 13;
    TIME_T tt;

    static u16 TimeCount = 0;

    CameraStateTimeCount[CAMERA_STATE_TIMING]++;
    if(CameraStateTimeCount[CAMERA_STATE_TIMING] > CAMERA_STATE_TIMING_OVERTIME)//��ʱ
    {
        CameraStateAck[CAMERA_STATE_TIMING] = 4;
        CameraTimingID = 1;
        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
    }

    if(0 == CameraSubState)//�ϵ�
    {
        if(0 == CameraPowerFlag)
        {
            CAMERA_POWER_ON();//�ϵ�
            CameraSubState = 1;//�ϵ����ȴ�һ��ʱ��
        }
        else
        {
            CameraSubState = 2;//�����ȴ�״̬
            TimeCount = 0;
        }
    }
    else if(1 == CameraSubState)//�ϵ���ʱ
    {
        if(CameraStateTimeCount[CAMERA_STATE_TIMING] > (CAMERA_STATE_TIMING_OVERTIME/2))
        {
            CameraSubState = 2;
            TimeCount = 0;
        }
    }
    else if(2 == CameraSubState)//����ָ��
    {
        Buffer[3] = CameraTimingID;
        Buffer[4] = 0;
        RTC_GetCurTime(&tt);
        Public_ConvertTimeToBCDEx(tt,Buffer+5);
        if(ACK_OK == Usart3_SendData(Buffer,length))
        {
            CameraSubState = 3;
        }
    }
    else if(3 == CameraSubState)//�ȴ�Ӧ��
    {
        TimeCount++;
        if(1 == CameraStateAck[CAMERA_STATE_TIMING])//��Ӧ��
        {
            CameraTimingID++;//��һ������ͷ
            if(CameraTimingID > CAMERA_ID_MAX)
            {
                
                CameraTimingID = 1;
                Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
            }
            CameraStateAck[CAMERA_STATE_TIMING] = 0;
            CameraSubState = 0;
            TimeCount = 0;
        }
        else if(2 == CameraStateAck[CAMERA_STATE_TIMING])//��Ӧ��
        {
            if(TimeCount > SECOND)
            {
                CameraTimingID++;//��һ������ͷ
                if(CameraTimingID > CAMERA_ID_MAX)
                {
                    CameraTimingID = 1;
                    Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
                }
                CameraSubState = 0;
                TimeCount = 0;
            }
        }
        else//��ⳬʱ
        {
            if(TimeCount > SECOND)
            {
                CameraStateAck[CAMERA_STATE_TIMING] = 2;//��ʱ1�룬�ط�1��
                CameraSubState = 2;
                TimeCount = 0;
            }
        }
    }
}
/**************************************************************************
//��������Camera_WriteParameter
//���ܣ�д���ղ��������ձ�����������
//���룺Channel:�ϴ�ͨ��;ֻ����CHANNEL_DATA_1����CHANNEL_DATA_2
//  CameraID:����ͷID��;ֻ����1~CAMERA_ID_MAX
//  PhotoNum:��������;��Ϊ0xfffe,0xffff��ʾһֱ���գ���Ϊ0ʱ��ʾֹͣ����
//  PhotoSpace:���ռ��;0ֵ��Ч
//  Resolution:���շֱ���;0Ϊ320*240�ֱ��ʣ�1Ϊ640*480�ֱ��ʣ�����ֵ��Ч
//  StoreFlag:�洢��־;bit0��1��ʾ��Ҫ�ϴ���bit1��1��ʾ��Ҫ���棬��������1��ʾ��Ҫ�ϴ�ҲҪ���棬����λ����
//  PhotoType:��������;0��ʾ��ʱ���գ�1��ʾ�¼����գ�����ֵ��Ч
//  EventType:�¼�����;0��ʾƽ̨�·�ָ�1��ʾ��ʱ������2��ʾ����������3��ʾ��ײ��෭�������������⼸��ʱ����ʹ��0
//�������
//����ֵ����
//��ע��
***************************************************************************/
static void Camera_WriteParameter(u8 Channel, u8 CameraID, u16 PhotoNum, u16 PhotoSpace, u8 Resolution, u8 StoreFlag, u8 PhotoType, u8 EventType)
{
    u8 Buffer[5];
    u8 i;

    if((CameraID == 0)||(CameraID > CAMERA_ID_MAX))
    {
        return;
    }

    if(0 == PhotoType)//д��ʱ���ղ���
    {

        for(i=1; i<=CAMERA_ID_MAX; i++)
        {
            Camera[i-1].FetchFlag = 0;
        }   
        for(i=0; i<=CAMERA_STATE_MAX; i++)
        {
            CameraStateAck[i] = 0;
            CameraStateErrorCount[i] = 0;
        }
        CameraState = 0;
        CameraSubState = 0;
        CameraPhotoID = 1;
        CameraFetchID = 1;
        CameraCheckID = 1;
        CameraPacket = 0;
        StateCheckCount1 = 0;
        StateCheckCount2 = 0;
        CameraResendEnableFlag = 0;
        CameraResendFlag = 0;
        CameraResendTotalPacket = 0;

        Camera[CameraID-1].TimePhotoTime = RTC_GetCounter();

        if((PhotoNum != 0xfffe)&&(PhotoNum > 255))
        {
            PhotoNum = 255;
        }
        Camera[CameraID-1].TimePhotoNum = PhotoNum;
        Public_ConvertShortToBuffer(PhotoNum,Buffer);
        FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_NUM_ADDR+(CameraID-1)*(FRAM_CAM_TIME_PHOTO_NUM_LEN+1),Buffer,FRAM_CAM_TIME_PHOTO_NUM_LEN);
                        
        Camera[CameraID-1].TimePhotoSpace = PhotoSpace;
        Public_ConvertShortToBuffer(PhotoSpace,Buffer);
        FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_SPACE_ADDR+(CameraID-1)*(FRAM_CAM_TIME_PHOTO_SPACE_LEN+1),Buffer,FRAM_CAM_TIME_PHOTO_SPACE_LEN);

        Camera[CameraID-1].TimePhotoStoreFlag = StoreFlag;
        Buffer[0] = StoreFlag;
        FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_STORE_FLAG_ADDR+(CameraID-1)*(FRAM_CAM_TIME_PHOTO_STORE_FLAG_LEN+1),Buffer,FRAM_CAM_TIME_PHOTO_STORE_FLAG_LEN);

        Camera[CameraID-1].TimePhotoResolution = Resolution;
        Buffer[0] = Resolution;
        FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_RESOLUTION_ADDR+(CameraID-1)*(FRAM_CAM_TIME_PHOTO_RESOLUTION_LEN+1),Buffer,FRAM_CAM_TIME_PHOTO_RESOLUTION_LEN);

        Camera[CameraID-1].TimePhotoEventType = EventType;
        Buffer[0] = EventType;
        FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_EVENT_TYPE_ADDR+(CameraID-1)*(FRAM_CAM_TIME_PHOTO_EVENT_TYPE_LEN+1),Buffer,FRAM_CAM_TIME_PHOTO_EVENT_TYPE_LEN);

        Camera[CameraID-1].TimePhotoChannel = Channel;
        Buffer[0] = Channel;
        FRAM_BufferWrite(FRAM_CAM_TIME_PHOTO_CHANNEL_ADDR+(CameraID-1)*(FRAM_CAM_TIME_PHOTO_CHANNEL_LEN+1),Buffer,FRAM_CAM_TIME_PHOTO_CHANNEL_LEN);

        Camera_StateSwitch(CAMERA_STATE_SCHEDULE);
    }
    else if(1 == PhotoType)//д�¼����ղ���
    {
        if((1 == Camera[CameraID-1].OnOffFlag)&&(Camera[CameraID-1].FetchFlag < CAMERA_STATE_FETCH))//����
        {
            Camera[CameraID-1].FetchFlag = 0;

            if(PhotoNum > 3)
            {
                PhotoNum = 3;
            }
            Camera[CameraID-1].EventPhotoNum = PhotoNum;
            Public_ConvertShortToBuffer(PhotoNum,Buffer);
            FRAM_BufferWrite(FRAM_CAM_EVENT_PHOTO_NUM_ADDR+(CameraID-1)*(FRAM_CAM_EVENT_PHOTO_NUM_LEN+1),Buffer,FRAM_CAM_EVENT_PHOTO_NUM_LEN);

            Camera[CameraID-1].EventPhotoTime = RTC_GetCounter();
                        
            Camera[CameraID-1].EventPhotoSpace = PhotoSpace;
            Public_ConvertShortToBuffer(PhotoSpace,Buffer);
            FRAM_BufferWrite(FRAM_CAM_EVENT_PHOTO_SPACE_ADDR+(CameraID-1)*(FRAM_CAM_EVENT_PHOTO_SPACE_LEN+1),Buffer,FRAM_CAM_EVENT_PHOTO_SPACE_LEN);

            Camera[CameraID-1].EventPhotoStoreFlag = StoreFlag;
            Buffer[0] = StoreFlag;
            FRAM_BufferWrite(FRAM_CAM_EVENT_PHOTO_STORE_FLAG_ADDR+(CameraID-1)*(FRAM_CAM_EVENT_PHOTO_STORE_FLAG_LEN+1),Buffer,FRAM_CAM_EVENT_PHOTO_STORE_FLAG_LEN);

            Camera[CameraID-1].EventPhotoResolution = Resolution;
            Buffer[0] = Resolution;
            FRAM_BufferWrite(FRAM_CAM_EVENT_PHOTO_RESOLUTION_ADDR+(CameraID-1)*(FRAM_CAM_EVENT_PHOTO_RESOLUTION_LEN+1),Buffer,FRAM_CAM_EVENT_PHOTO_RESOLUTION_LEN);

            Camera[CameraID-1].EventPhotoEventType = EventType;
            Buffer[0] = EventType;
            FRAM_BufferWrite(FRAM_CAM_EVENT_PHOTO_EVENT_TYPE_ADDR+(CameraID-1)*(FRAM_CAM_EVENT_PHOTO_EVENT_TYPE_LEN+1),Buffer,FRAM_CAM_EVENT_PHOTO_EVENT_TYPE_LEN);

            Camera[CameraID-1].EventPhotoChannel = Channel;
            Buffer[0] = Channel;
            FRAM_BufferWrite(FRAM_CAM_EVENT_PHOTO_CHANNEL_ADDR+(CameraID-1)*(FRAM_CAM_EVENT_PHOTO_CHANNEL_LEN+1),Buffer,FRAM_CAM_EVENT_PHOTO_CHANNEL_LEN);
        }
        else if((1 == Camera[CameraID-1].OnOffFlag)&&(Camera[CameraID-1].FetchFlag >= CAMERA_STATE_FETCH))//����ȡͼ
        {
            Camera[CameraID-1].EventPhotoNum = Camera[CameraID-1].EventPhotoNum+PhotoNum;
            if(Camera[CameraID-1].EventPhotoNum > 3)
            {
                Camera[CameraID-1].EventPhotoNum = 3;
            }
            Public_ConvertShortToBuffer(PhotoNum,Buffer);
            FRAM_BufferWrite(FRAM_CAM_EVENT_PHOTO_NUM_ADDR+(CameraID-1)*(FRAM_CAM_EVENT_PHOTO_NUM_LEN+1),Buffer,FRAM_CAM_EVENT_PHOTO_NUM_LEN);  
        }
    }
}





























