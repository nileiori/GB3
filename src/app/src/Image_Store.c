
/*******************************************************************************
 * File Name:           Image_Store.c
 * Function Describe:   ʵ�ֶ�ý��ͼƬ����������ͼƬ�����ϴ�����
 * Relate Module:
 * Writer:              fanqinghai 
 * Date:                2016-03-15
 *******************************************************************************/


#include "include.h"


#define BUFFER_SIZE             550
#define ATTEMPE_STEP_LEN      6*FLASH_ONE_SECTOR_BYTES   //һ��ͼƬ������24k�ռ�
#define STORE_IMAGE_NMB         3    //�ɱ���ͼƬ����

static u32 SendMediaID;
u8 FlashBuffer[BUFFER_SIZE];    

/**************************************************************************
//��������delay
//���ܣ���ʱ����
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void delay()
{
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
__nop();
}

/**************************************************************************
//��������Store_Image_Date
//���ܣ�ʵ��ͼƬ�洢��������
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
u8 Store_Image_Date(u16 pacagenmb,u8 *pBuffer ,u16 length)
{

    static u32 Address;
    static u8 CurrentStep =0;
    static u16 LastPacagenmb ;
    u16 TotalPacage;
    static u16 TotalDataLong;
   // u8 bufer[4] = {0};
    u8 i =0;
    u32 RtcCount ;
    memset(FlashBuffer,0,BUFFER_SIZE);
    memcpy(FlashBuffer,pBuffer,length);
    //��ȡ��ǰ��ַ
    if(0 == pacagenmb)   //��һ��
    {
        
        CurrentStep ++;     //������
        if(CurrentStep>2)
        {
            CurrentStep =0;
        }
        Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
        for(i=0;i<6;i++)    //ÿ��ͼƬ����24K�ռ�
        {
            
            sFLASH_EraseSector(Address);
            Address += FLASH_ONE_SECTOR_BYTES;
            IWDG_ReloadCounter();
        }
        
        delay();     //���п���
        TotalDataLong = 0;
        LastPacagenmb = 0;
        Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN; //��ַ��λ
    }
    //д���� ����ѭ������д
    sFLASH_WriteBuffer(FlashBuffer, Address, length);
    Address += length; //
    TotalDataLong += length;
    
     if(0xAA == pacagenmb)
     {
         memset(FlashBuffer,0,2);
         TotalPacage = LastPacagenmb+1;
         TotalPacage += 1;              //���ӵ�0����ʼ�������ټ�1��
         FlashBuffer[0] = (TotalPacage&0xff00)>>8;
         FlashBuffer[1] = TotalPacage&0x00ff;
         Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -2;
         sFLASH_WriteBuffer(FlashBuffer, Address, 2);

         ////���������ܳ���//////////////
        memset(FlashBuffer,0,2);
        FlashBuffer[0] = (TotalDataLong&0xff00)>>8;
        FlashBuffer[1] = TotalDataLong&0x00ff;
        Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -4;
        sFLASH_WriteBuffer(FlashBuffer, Address, 2);

       ////��������ʱ��//////////////
        memset(FlashBuffer,0,4);
        RtcCount = RTC_GetCounter();
        FlashBuffer[0] = (RtcCount&0xff000000)>>24;
        FlashBuffer[1] = (RtcCount&0x00ff0000)>>16;
        FlashBuffer[2] = (RtcCount&0x0000ff00)>>8;
        FlashBuffer[3] = RtcCount&0x000000ff;
        
        Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -8;
        sFLASH_WriteBuffer(FlashBuffer, Address, 4);
     }
    LastPacagenmb =  pacagenmb ; 
    return 0;
}
/**************************************************************************
//��������Get_ID
//���ܣ����Ҫ������ͼƬid
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
void Get_ID(u32 MediaID)
{
    SendMediaID = MediaID;

}
/**************************************************************************
//��������MediaSearch_GetMediaIndex
//���ܣ���ѯָ��ʱ���ͼƬ��Ϣ
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
u16 MediaSearch_GetMediaIndex(u8 *pBuffer, u8 MediaType, TIME_T *StartTime, TIME_T *EndTime)
{
    u32 Address;
    u32 FindTime;
    u32 Start_Time;
    u32 End_Time;
    u16 Length;
    u8 CurrentStep =0;
    u8 i;
    u8 Time[4];
    static u8 *p=NULL;
        
    p= pBuffer;
    Length = 0;
    if(!(Public_CheckTimeStruct(StartTime))||(!Public_CheckTimeStruct(EndTime))) //ʱ��ṹ����
    {
        return 0;
    }
     //ֻ֧��ͼƬ����
	if(MediaType  ==1)   //��Ƶ
	{
        //��ý��ID
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x01;
        //��ý������
        *p++ = 0x01;
        //ͨ��ID
        *p++ = 0x01;
        //�¼�����
        *p++ = 0;
        //λ����Ϣ
        Report_GetPositionBasicInfo(p);
        Length += 35;
        p = NULL;
        return Length ;
    }
    else if(MediaType  ==2)  //��Ƶ
    {
        //��ý��ID
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0x01;
        //��ý������
        *p++ = 0x02;
        //ͨ��ID
        *p++ = 0x01;
        //�¼�����
        *p++ = 0;
        //λ����Ϣ
        Report_GetPositionBasicInfo(p);
        Length += 35;
        p = NULL;
        return Length ;
    }
    else if(MediaType  ==0)   //ͼ��
    {
        CurrentStep = 0;
      for(i=0;i<STORE_IMAGE_NMB;i++)

        {

            IWDG_ReloadCounter();
            CurrentStep ++;     //������
            if(CurrentStep>2)
            {
                CurrentStep =0;
            }
            
            Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -8;
            sFLASH_ReadBuffer(Time, Address, 4);
            FindTime = 0;
            FindTime |= Time[0]<<24;
            FindTime |= Time[1]<<16;
            FindTime |= Time[2]<<8;
            FindTime |= Time[3];
            
            //Public_ConvertBCDToTime(tt,Time);
            Start_Time = ConverseGmtime(StartTime);
            End_Time = ConverseGmtime(EndTime);
            if((FindTime>Start_Time)&&(FindTime<End_Time))
            {
                memset(FlashBuffer,0,50);
                Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
                sFLASH_ReadBuffer(FlashBuffer, Address, 36);
                //��ý��ID
                *p++ = FlashBuffer[0];
                *p++ = FlashBuffer[1];
                *p++ = FlashBuffer[2];
                *p++ = FlashBuffer[3];
                //��ý������
                *p++ = FlashBuffer[4];
                //ͨ��ID
                *p++ = FlashBuffer[7];
                //�¼�����
                *p++ = FlashBuffer[6];
                //λ����Ϣ
                memcpy(p,FlashBuffer+8,28);
                p += 28;
                //Report_GetPositionBasicInfo(p);
                Length += 35;
        
            }
        }
        p = NULL;
        return Length ;
    }
    return 0;
}
/**************************************************************************
//��������SendImage_TimeTask
//���ܣ�ʵ��ͼƬ��ID��������
//���룺��
//�������
//����ֵ����
//��ע��
***************************************************************************/
FunctionalState SendImage_TimeTask(void)
{
    static u32 Address;
    static u8 flag = 0;
    u32 ReadID;
    static u8 CurrentStep = 0;
    static u16 TotalPacage;
    static u16 SendPacage;
    static u16 TotalDataLong;
    
    u16  Length ;  
    u8 channel = CHANNEL_DATA_1;//Ϊ����ͨ�������
    u8 i =0;

    if(!flag)
    {
        CurrentStep = 0 ;
        for(i=0;i<STORE_IMAGE_NMB;i++)
        {
            IWDG_ReloadCounter();
            CurrentStep ++;     //������
            if(CurrentStep>2)
            {
                CurrentStep =0;
            }
            
            Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
            memset(FlashBuffer,0,4);
            sFLASH_ReadBuffer(FlashBuffer, Address, 100);
            ReadID = 0;
            ReadID |= FlashBuffer[0]<<24;
            ReadID |= FlashBuffer[1]<<16;
            ReadID |= FlashBuffer[2]<<8;
            ReadID |= FlashBuffer[3];
            
            if(SendMediaID == ReadID)
            {
                memset(FlashBuffer,0,2);
                TotalPacage = 0;
                TotalDataLong = 0;

                Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -2;
                sFLASH_ReadBuffer(FlashBuffer, Address, 2);
                TotalPacage |= FlashBuffer[0]<<8;
                TotalPacage |= FlashBuffer[1];

                memset(FlashBuffer,0,2);
                Address = FLASH_STORE_IMAGE_START_ADDR+(CurrentStep+1)*ATTEMPE_STEP_LEN -4;
                sFLASH_ReadBuffer(FlashBuffer, Address, 2);
                TotalDataLong |= FlashBuffer[0]<<8;
                TotalDataLong |= FlashBuffer[1];

                flag = 1;
                SendPacage = 1;    // �ӵ�һ����ʼ
                Address = FLASH_STORE_IMAGE_START_ADDR+CurrentStep*ATTEMPE_STEP_LEN;
        
                break;
            }
        
        }
        if(3==i)
        {
            flag = 0;
            return DISABLE;
        }
    }
    else
    {
        if(SendPacage<=TotalPacage)
        {

            if(SendPacage==TotalPacage)
            {
                Length = TotalDataLong%512;
            }
            else
            {
                Length = 512;
            }
            memset(FlashBuffer,0,BUFFER_SIZE);
            sFLASH_ReadBuffer(FlashBuffer, Address, Length);
            
            if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
            {
                SendPacage++;
                Address += 512;

              return ENABLE; 
            }
        }
        else               //�������
        {
            flag = 0;                   
            return DISABLE;
        }

    }
    return ENABLE;
}

/*********************************************************************
//��������  :Media_UploadTimeTask(void)
//����      :�洢��ý�������ϴ���ʱ���񣬷�����Ƶ�ļ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
FunctionalState MediaUploadTimeTask(void)
{
    static u32 Address;
    static u8 flag = 0;
    static u16 TotalPacage;
    static u16 SendPacage;
    static u32 TotalDataLong;
    
    u16  Length ;  
    u8 channel = CHANNEL_DATA_1;//Ϊ����ͨ�������
    u8 *p = NULL;
        
        if(! flag)
        {
            
            Address = VIDEO_START_SECTOR*FLASH_ONE_SECTOR_BYTES;
            
            memset(FlashBuffer,0,4);
            sFLASH_ReadBuffer(FlashBuffer, Address, 4);
            
            TotalDataLong |= FlashBuffer[0]<<24;
            TotalDataLong |= FlashBuffer[1]<<16;
            TotalDataLong |= FlashBuffer[2]<<8;
            TotalDataLong |= FlashBuffer[3];
            
            TotalPacage = TotalDataLong/512 + 1;
            memset(FlashBuffer,0,BUFFER_SIZE);
             p=FlashBuffer;
            *p++ = 0;//��ý��ID
            *p++ = 0;
            *p++ = 0;
            *p++ = 0x01;
            
            *p++ = 2;//��ý�����ͣ�2Ϊ��Ƶ
            *p++ = 0xaa;//��ý���ʽ� ��ʾ��������
            *p++ = 0x00;//��ý���¼�  0:ƽ̨�Զ��Ϸ�
            *p++ = 0x01;//��ý��ͨ��
            Report_GetPositionBasicInfo(p);
            Address = VIDEO_START_SECTOR*FLASH_ONE_SECTOR_BYTES+4;
            sFLASH_ReadBuffer(FlashBuffer+36, Address, 512);
            flag = 1;
            SendPacage = 1;
            Length =  512+36;
            if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
            {
                SendPacage++;
                Address += 512;
            
              return ENABLE; 
            }
            
        }

        else
        {

            if(SendPacage<=TotalPacage)
            {

                if(SendPacage==TotalPacage)
                {
                    Length = TotalDataLong%512;
                }
                else
                {
                    Length = 512;
                }
                memset(FlashBuffer,0,BUFFER_SIZE);
                sFLASH_ReadBuffer(FlashBuffer, Address, Length);
                
                if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
                {
                    SendPacage++;
                    Address += 512;

                  return ENABLE; 
                }
            }
            else               //�������
            {
                flag = 0;                   
                return DISABLE;
            }


        }
     return ENABLE;     
}
/*********************************************************************
//��������  :SendAudio_TimeTask(void)
//����      :����¼���ļ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
FunctionalState SendAudio_TimeTask(void)
{
    static u32 Address;
    static u8 flag = 0;
    static u16 TotalPacage;
    static u16 SendPacage;
    static u32 TotalDataLong;
    
    u16  Length ;  
    u8 channel = CHANNEL_DATA_1;//Ϊ����ͨ�������
    u8 *p = NULL;
        
        if(! flag)
        {
            
            Address = AUDIO_START_SECTOR*FLASH_ONE_SECTOR_BYTES;
            
            memset(FlashBuffer,0,4);
            sFLASH_ReadBuffer(FlashBuffer, Address, 4);
            
            TotalDataLong |= FlashBuffer[0]<<24;
            TotalDataLong |= FlashBuffer[1]<<16;
            TotalDataLong |= FlashBuffer[2]<<8;
            TotalDataLong |= FlashBuffer[3];            
            TotalPacage = TotalDataLong/512 + 1;
            memset(FlashBuffer,0,BUFFER_SIZE);
            p=FlashBuffer;
            *p++ = 0;//��ý��ID
            *p++ = 0;
            *p++ = 0;
            *p++ = 0x01;
            *p++ = 1;//��ý�����ͣ�1Ϊ��Ƶ
            *p++ = 0x02;//��ý���ʽ� 2ΪMP3
            *p++ = 0x00;//��ý���¼�  0:ƽ̨�Զ��Ϸ�
            *p++ = 0x01;//��ý��ͨ��
            Report_GetPositionBasicInfo(p);
            Address = AUDIO_START_SECTOR*FLASH_ONE_SECTOR_BYTES+4;
            sFLASH_ReadBuffer(FlashBuffer+36, Address, 512);
            flag = 1;
            SendPacage = 1;
            Length =  512+36;
            if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
            {
                SendPacage++;
                Address += 512;
            
              return ENABLE; 
            }
            
        }

        else
        {

            if(SendPacage<=TotalPacage)
            {

                if(SendPacage==TotalPacage)
                {
                    Length = TotalDataLong%512;
                }
                else
                {
                    Length = 512;
                }
                memset(FlashBuffer,0,BUFFER_SIZE);
                sFLASH_ReadBuffer(FlashBuffer, Address, Length);
                
                if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,FlashBuffer, Length, TotalPacage, SendPacage))
                {
                    SendPacage++;
                    Address += 512;

                  return ENABLE; 
                }
            }
            else               //�������
            {
                flag = 0;                   
                return DISABLE;
            }


        }
        
    return ENABLE; 
}









