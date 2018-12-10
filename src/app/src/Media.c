/********************************************************************
//��Ȩ˵��  :Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����  :Media.c        
//����      :ʵ��ͼ���¼���Ĵ洢
//�汾��    : 
//������    :dxl
//����ʱ��  :2012.9
//�޸���    :
//�޸�ʱ��  :
//�޸ļ�Ҫ˵��  :
//��ע      :
***********************************************************************/
//***************�����ļ�*****************
#include "stm32f10x.h"
#include "stm32f10x_lib.h"
#include "include.h"
//****************�궨��****************

//***************���ر���******************
u8  CurrentWavFolder = 0;//��ǰ¼���ļ���
u8  CurrentJpgFolder = 0;//��ǰͼ���ļ���
static u16  CurrentWavFile = 0;//��ǰ¼���ļ����µĵ�ǰ�ļ�
static u16  CurrentJpgFile = 0;//��ǰͼ���ļ����µĵ�ǰ�ļ�
static u8   MediaDelEnableFlag = 0;//��ý��ǿ��ɾ��ʹ�ܱ�־
static u8   EnableFlag = 1;//����ɾ�����̵�ʹ�ܱ�־
u8  SdOnOffFlag = 0;//SD�����߱�־��1���ߣ�0������,��sd����ʼ��ʧ��ʱ�ñ�־Ҳ����0
u32 JpgCreatTime;//jpg�ļ�������ʱ��
u32 WavCreatTime;//wav�ļ�������ʱ��
u8      MediaType = 0;//��ý�����ͣ�0ͼ��1��Ƶ��2��Ƶ
u8      MediaFormat = 0;//��ý���ʽ����,0JPEG,1TIF,2MP3,3WAV,4WMV
u8  MediaRatio = 1;//��ý��ֱ���
u8      MediaChannelID = 0;//��ý��ͨ��ID
u8      MediaEvenCode = 0;//��ý���¼�����
u16     MediaCommand = 0;//��ý������  0:ֹͣ  N:����  0XFFFF:¼�� 0XFFFE:һֱ����  BY WYF
u16     MediaPhotoSpace = 0; //���ռ��ʱ�� BY WYF
u8      MemoryUploadFileName[40];//�洢��ý�������ϴ��������ļ���
u8      MultiMediaSendFlag = 0;//��ý�巢�ͱ�־��0Ϊ���ڷ��ͣ�1Ϊ�ڷ���
u16 MemoryUploadSendCount = 0;//���ʹ�������
//****************ȫ�ֱ���*****************

//****************�ⲿ����*****************
//extern RecordDef RECORD_DEF;            //�ýṹ�������¼����ý��ID
extern u8      MultiMediaIndexSendFlag;//��ý������ϴ���־,1Ϊ��Ҫ�����ϴ�,0Ϊ����Ҫ
extern u8      RecordSendFlag;//¼���ļ��ϴ���־,1Ϊ�ϴ�
extern u32     PhotoID;//ͼ���ý��ID
extern u16     DelayAckCommand;//��ʱӦ������
/*********************************************************************
//��������  :Media_GetSdOnLineFlag
//����      :���SD�����߱�־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :����1���ߣ�����0������
*********************************************************************/
u8 Media_GetSdOnLineFlag(void)
{
    return SdOnOffFlag;
}
/*********************************************************************
//��������  :Media_FindCurFolder
//����      :����JPG��WAV�ĵ�ǰλ�ã���ǰ�ļ��У���ǰ�ļ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :Typeֻ��ΪMEDIA_TYPE_JPG��MEDIA_TYPE_WAV
//      :�ļ������涼����һ���ļ���־�ļ�log.bin
*********************************************************************/
void Media_FindCurFolder(u8 *Folder, u16 *File, u8 Type)
{
    u8  Dir[15];
    u8  FileName[_MAX_LFN];
    u8  i;
    u8  flag;
    u8  length;
    FIL     file;
    DIR dir;
    u8  CurFolder;
    u16 CurFile;
    u32 NewestTime;
    u32 Time;
    u8  Buffer[5];
    u32 Byte;
    //FRESULT res;
    
    if(0 == SdOnOffFlag)
    {
        return ;
    }

    if(MEDIA_TYPE_JPG == Type)
    {
        strcpy((char *)Dir,"1:/JPG");
    }
    else if(MEDIA_TYPE_WAV == Type)
    {
        strcpy((char *)Dir,"1:/WAV");
    }
    else
    {
        return ;
    }
    CurFolder = 0;
    CurFile = 0;
    flag = 0;
    NewestTime = 0;
    //���������Ѵ�����Ŀ¼
    for(i=1; i<=FOLDER_NUM_MAX; i++)//�ļ���JPG1~JPG32
    {
        IWDG_ReloadCounter();//ι��
        if(i < 10)
        {
            Dir[6] = i+0x30;
            Dir[7] = 0;
        }
        else
        {
            Dir[6] = i/10+0x30;
            Dir[7] = i%10+0x30;
            Dir[8] = 0;
        }
        if(FR_OK == f_opendir(&dir, ff_NameConver(Dir)))
        {
            strcpy((char *)FileName,(char const *)Dir);
            length = strlen((char const *)FileName);
            memcpy(FileName+length,"/log.bin",8);
            length += 8;
            FileName[length] = 0;
            if(f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE) == FR_OK)
            {
                flag = 1;
                //if(0 == file.fsize%9)
                if((0 == file.fsize%9)&&(file.fsize >= 9))//dxl,2014.9.9�޸�Ϊ���
                {
                    if((file.fsize/9) < FOLDER_FILE_NUM_MAX)
                    {
                        f_close(&file);
                        strcpy((char *)FileName,(char const *)Dir);
                        length = strlen((char const *)FileName);
                        strcpy((char *)FileName+length,"/delete.bin");
                        if(f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE) == FR_OK)
                        {
                        
                        }
                        else
                        {
                            //if(0 == file.fsize)
                            //{
                                f_close(&file);
                                CurFolder = i;
                                CurFile = file.fsize/9;
                                break;
                            //}
                                /*
                            res = f_lseek(&file,file.fsize-9);
                            if(FR_OK == res)
                            {
                                if(FR_OK == f_read (&file, Buffer, 4, &Byte))
                                {
                                    Time = 0;
                                    Time |= Buffer[0] << 24;
                                    Time |= Buffer[1] << 16;
                                    Time |= Buffer[2] << 8;
                                    Time |= Buffer[3];
                                    if(Time > NewestTime)
                                    {
                                        NewestTime = Time;
                                        CurFolder = i;
                                        CurFile = file.fsize/9;
                                    }   
                                }
                            }
                                */
                            
                            
                        }
                    }
                    else
                    {
                        f_close(&file);
                    }
                }
                else//log.bin�ļ�����
                {
                                        if(file.fsize > 0)//dxl,2015.5.26����
                                        {
                            f_close(&file);
                            strcpy((char *)FileName,(char const *)Dir);
                            length = strlen((char const *)FileName);
                            strcpy((char *)FileName+length,"/delete.bin");
                            if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_OPEN_ALWAYS|FA_READ|FA_WRITE))
                            {
                
                            }
                                        }
                    f_close(&file);
                    
                }
            }
            else//log.bin�����ڻ�򿪳���
            {   
                strcpy((char *)FileName,(char const *)Dir);
                length = strlen((char const *)FileName);
                strcpy((char *)FileName+length,"/delete.bin");
                if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_OPEN_ALWAYS|FA_READ|FA_WRITE))
                {
                
                }
                f_close(&file);
                
            }
            f_close(&file);
        }
        
    }
    if(0 == CurFolder)
    {
        if(0 == flag)//�տ�ʱû���κ��ļ��У�����JPG1
        {
            Dir[6] = '1';
            Dir[7] = 0;
            //�����µ�Ŀ¼
            f_mkdir(ff_NameConver(Dir));
            //������־�ļ�
            strcpy((char *)FileName,(char const *)Dir);
            length = strlen((char const *)FileName);
            memcpy(FileName+length,"/log.bin",8);
            length += 8;
            FileName[length] = 0;
            //�����ļ�
            if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_OPEN_ALWAYS|FA_READ|FA_WRITE))
            {
          
            }
            else
            {
                    //��ʾTF���ӿ��Լ�ɹ�,dxl,2014.1.6,V1.01.06�Ժ�ž��иù���
                    LcdClearScreen(); 
                    LcdShowCaptionEx((char *)"TF���Լ�ʧ��",2); 
            }
            f_close(&file);
            CurFolder = 1;
            CurFile = 0;
        }
        else
        {
                        
            NewestTime = 0;
            //���µ�ʱ��Ϊ��ǰ�ļ���
            for(i=1; i<=FOLDER_NUM_MAX; i++)
            {
                IWDG_ReloadCounter();//ι��
                if(i < 10)
                {
                    Dir[6] = i+0x30;
                    Dir[7] = 0;
                }
                else
                {
                    Dir[6] = i/10+0x30;
                    Dir[7] = i%10+0x30;
                    Dir[8] = 0;
                }
                strcpy((char *)FileName,(char const *)Dir);
                length = strlen((char const *)FileName);
                memcpy(FileName+length,"/log.bin",8);
                FileName[length+8] = 0;
                if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE))
                {
                    if((0 == file.fsize%9)&&(file.fsize >=9))//����Ҫ�洢һ�����ϵ��ļ�
                    {
                        f_lseek(&file,file.fsize-9);
                        if(FR_OK == f_read (&file, Buffer, 4, &Byte))
                        {
                            Time = 0;
                            Time |= Buffer[0] << 24;
                            Time |= Buffer[1] << 16;
                            Time |= Buffer[2] << 8;
                            Time |= Buffer[3];
                            if(Time > NewestTime)
                            {
                                NewestTime = Time;
                                CurFolder = i;
                                CurFile = file.fsize/9;
                            }
                        }
                    }
                }
                f_close(&file);
            }
            if(0 == NewestTime)//û���ҵ�,Ĭ��ΪWAV1
            {
                Dir[6] = '1';
                Dir[7] = 0;
                //�����µ�Ŀ¼
                f_mkdir(ff_NameConver(Dir));
                //������־�ļ�
                strcpy((char *)FileName,(char const *)Dir);
                length = strlen((char const *)FileName);
                memcpy(FileName+length,"/log.bin",8);
                length += 8;
                FileName[length] = 0;
                //�����ļ�
                if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_OPEN_ALWAYS|FA_READ|FA_WRITE))
                {
                                
                }
                else
                {
                        //��ʾTF���ӿ��Լ�ɹ�,dxl,2014.1.6,V1.01.06�Ժ�ž��иù���
                        LcdClearScreen(); 
                        LcdShowCaptionEx((char *)"TF���Լ�ʧ��",2); 
                }
                CurFolder = 1;
                CurFile = file.fsize/9;
                f_close(&file);
            }
        }
    }
        else
        {
          
        }
    *Folder = CurFolder;
    *File = CurFile;
}
/*********************************************************************
//��������  :Media_CheckSdOnOff
//����      :���SD�����벻��
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :��:SdOnOffFlag = 1;����SdOnOffFlag = 0,dxl,2014.6.18
//      :
*********************************************************************/
void Media_CheckSdOnOff(void)
{
    u8  Status;
    
    Status  = GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13);
    if(0 == Status)
    {
        SdOnOffFlag = 1;
    }
    else
    {
        SdOnOffFlag = 0;
    }
}

/*********************************************************************
//��������  :Media_FolderCheck(void)
//����      :�ļ����Լ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :ȷ����ǰʹ�õ�¼���ļ��к������ļ���
//      :�ļ������涼����һ���ļ���־�ļ�log.bin
*********************************************************************/
void Media_FolderCheck(void)
{
    if(1 == SdOnOffFlag)
    {
			  LcdClearScreen(); 
        LcdShowCaptionEx((char *)"TF���ļ����",2); 
			
        //��λ��ǰͼ���ļ���
        Media_FindCurFolder(&CurrentJpgFolder, &CurrentJpgFile, MEDIA_TYPE_JPG);
        //��λ��ǰ¼���ļ���
        Media_FindCurFolder(&CurrentWavFolder, &CurrentWavFile, MEDIA_TYPE_WAV);
        //�����Ҫɾ�����ļ�
        Media_SignDelFlag(MEDIA_TYPE_JPG);
        //�����Ҫɾ�����ļ�
        Media_SignDelFlag(MEDIA_TYPE_WAV);  
    }
}
/*********************************************************************
//��������  :Media_EnableDeleteFlag(void)
//����      :ʹ��ɾ����־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :�������ǳ�Сʱ��ǿ��ɾ���ļ�
*********************************************************************/
void Media_EnableDeleteFlag(void)
{

    if(0 == SdOnOffFlag)
    {
        return;
    }
    EnableFlag = 1;
    MediaDelEnableFlag = 1;
}
/*********************************************************************
//��������  :Media_GetSignNum
//����      :��ȡ�ѱ���ļ�������
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :Typeֻ��ΪMEDIA_TYPE_JPG��MEDIA_TYPE_WAV
*********************************************************************/
u8 Media_GetSignNum(u8 Type)
{
    u8  i;
    u8  length;
    u8  FileName[_MAX_LFN];
    FIL file;
    u8  count;
    
    if(0 == SdOnOffFlag)
    {
        return 0;
    }

    if(MEDIA_TYPE_JPG == Type)
    {
        strcpy((char *)FileName,"1:/JPG");
    }
    else if(MEDIA_TYPE_WAV == Type)
    {
        strcpy((char *)FileName,"1:/WAV");
    }
    length = strlen((char *)FileName);
    count = 0;
    for(i=1; i<= FOLDER_NUM_MAX; i++)
    {
        IWDG_ReloadCounter();//ι��
        if(i<10)
        {
            FileName[length++] = i+0x30;
            strcpy((char *)FileName+length,"/delete.bin");
            if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE))
            {
                count++;
                f_close(&file);
            }
            else
            {
                f_close(&file);
                length = 6;
                FileName[length++] = i+0x30;
                strcpy((char *)FileName+length,"/DELETE.BIN");
                if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE))
                {
                    count++;
                }
                f_close(&file);
            }
        }
        else 
        {
            FileName[length++] = i/10+0x30;
            FileName[length++] = i%10+0x30;
            strcpy((char *)FileName+length,"/delete.bin");
            if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE))
            {
                count++;
                f_close(&file);
            }
            else
            {
                f_close(&file);
                length = 6;
                FileName[length++] = i/10+0x30;
                FileName[length++] = i%10+0x30;
                strcpy((char *)FileName+length,"/DELETE.BIN");
                if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE))
                {
                    count++;
                }
                f_close(&file);
            }
        }
        
    }
    return count;
    
}
/*********************************************************************
//��������  :Media_GetFileName
//����      :��ȡ�ļ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :Typeֻ��ΪMEDIA_TYPE_JPG��MEDIA_TYPE_WAV
//      :¼���ļ�������+��+ʱ+�֣���ѭ8.3�ļ�����ʽ
//      :ͼ���ļ�������+ʱ+��+�룬��ѭ8.3�ļ�����ʽ
*********************************************************************/
void Media_GetFileName(u8 *pBuffer, u8 Type)
{
    TIME_T tt;
    u8  *p;
    u8  length;

    if(0 == SdOnOffFlag)
    {
        return ;
    }
    if(MEDIA_TYPE_JPG == Type)
    {
        RTC_GetCurTime(&tt);
        JpgCreatTime = RTC_GetCounter();
        p=pBuffer;
        strcpy((char *)p,"1:/JPG");
        length = strlen((char const *)p);
        p += length;
        if(CurrentJpgFolder < 10)
        {
            *p++ = CurrentJpgFolder+0x30;
            *p++ = '/';
        } 
        else
        {
            *p++ = CurrentJpgFolder/10+0x30;
            *p++ = CurrentJpgFolder%10+0x30;
            *p++ = '/';
        }
        *p++ = tt.day/10+0x30;
        *p++ = tt.day%10+0x30;
        *p++ = tt.hour/10+0x30;
        *p++ = tt.hour%10+0x30;
        *p++ = tt.min/10+0x30;
        *p++ = tt.min%10+0x30;
        *p++ = tt.sec/10+0x30;
        *p++ = tt.sec%10+0x30;
        *p++ = '.';
        *p++ = 'j';
        *p++ = 'p';
        *p++ = 'g';
        *p++ = 0;//��β�ַ�
    }
    else if(MEDIA_TYPE_WAV == Type)
    {
        RTC_GetCurTime(&tt);
        WavCreatTime = RTC_GetCounter();
        p=pBuffer;
        strcpy((char *)p,"1:/WAV");
        length = strlen((char const *)p);
        p += length;
        if(CurrentWavFolder < 10)
        {
            *p++ = CurrentWavFolder+0x30;
            *p++ = '/';
        } 
        else
        {
            *p++ = CurrentWavFolder/10+0x30;
            *p++ = CurrentWavFolder%10+0x30;
            *p++ = '/';
        }
        *p++ = tt.month/10+0x30;
        *p++ = tt.month%10+0x30;
        *p++ = tt.day/10+0x30;
        *p++ = tt.day%10+0x30;
        *p++ = tt.hour/10+0x30;
        *p++ = tt.hour%10+0x30;
        *p++ = tt.min/10+0x30;
        *p++ = tt.min%10+0x30;
        *p++ = '.';
        *p++ = 'w';
        *p++ = 'a';
        *p++ = 'v';
        *p++ = 0;//��β�ַ�
    }
}
/*********************************************************************
//��������  :Media_SaveLog(void)
//����      :����log.bin�ļ�
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :Typeֻ��ΪMEDIA_TYPE_JPG��MEDIA_TYPE_WAV
//��ע      :�ڱ�����һ��ͼ���һ��¼������Ҫ���øú������������Ϣ�����������ļ�log.bin��
*********************************************************************/
void Media_SaveLog(u32 MediaID, u8 Type)
{
    FIL file;
    u8  Dir[15];
    u8  FileName[_MAX_LFN];
    u8  CurrentFolder;
    u16 CurrentFile;
    u8  i;
    u8  sum;
    u8  length;
    u8  Buffer[12];
    u32 temp;
    u32 Bw;
    u32 CreatTime;
    FRESULT  res;
    
    if(0 == SdOnOffFlag)
    {
        return ;
    }

    if(MEDIA_TYPE_JPG == Type)
    {
        CurrentJpgFile++;
        CurrentFolder = CurrentJpgFolder;
        if(0 == CurrentFolder)
        {
            return ;
        }
        strcpy((char *)Dir,"1:/JPG");
        CreatTime = JpgCreatTime;
        CurrentFile = CurrentJpgFile;
    }
    else if(MEDIA_TYPE_WAV == Type)
    {
        CurrentWavFile++;
        CurrentFolder = CurrentWavFolder;
        if(0 == CurrentFolder)
        {
            return ;
        }
        strcpy((char *)Dir,"1:/WAV");
        CreatTime = WavCreatTime;
        CurrentFile = CurrentWavFile;
    }
    else
    {
        return ;
    }
    if(CurrentFolder < 10)
    {
        Dir[6] = CurrentFolder+0x30;
        Dir[7] = 0;
    }
    else
    {
        Dir[6] = CurrentFolder/10+0x30;
        Dir[7] = CurrentFolder%10+0x30;
        Dir[8] = 0;
    }
    //д��־�ĵ�log.bin
    strcpy((char *)FileName,(char const *)Dir);
    length = strlen((char const *)FileName);
    memcpy(FileName+length,"/log.bin",8);
    FileName[length+8] = 0;
    if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_OPEN_ALWAYS|FA_READ|FA_WRITE))
    {
        f_lseek(&file,file.fsize);
        //д����ļ�������ʱ��
        temp = CreatTime;
        Buffer[0] = temp >> 24;
        Buffer[1] = temp >> 16;
        Buffer[2] = temp >> 8;
        Buffer[3] = temp;
        //д���ý��ID
        temp = MediaID;
        Buffer[4] = temp >> 24;
        Buffer[5] = temp >> 16;
        Buffer[6] = temp >> 8;
        Buffer[7] = temp;
        sum = 0;
        for(i=0; i<8; i++)
        {
            sum += Buffer[i];
        }
        Buffer[8] = sum;
        res = f_write(&file,Buffer,9,&Bw);
        f_close(&file);
    }
    if(CurrentFile >= FOLDER_FILE_NUM_MAX)//����������ǰ�ļ�������
    {
        CurrentFile = 0;
        //������һ���ļ��д��ڷ�
        CurrentFolder++;
        if(CurrentFolder > FOLDER_NUM_MAX)
        {
            CurrentFolder = 1;
        }
        if(MEDIA_TYPE_JPG == Type)
        {
            CurrentJpgFolder = CurrentFolder;
            CurrentJpgFile = 0;
        }
        else if(MEDIA_TYPE_WAV == Type)
        {
            CurrentWavFolder = CurrentFolder;
            CurrentWavFile = 0;
        }
        if(CurrentFolder < 10)
        {
            Dir[6] = CurrentFolder+0x30;
            Dir[7] = 0;
        }
        else
        {
            Dir[6] = CurrentFolder/10+0x30;
            Dir[7] = CurrentFolder%10+0x30;
            Dir[8] = 0;
        }
        //�����µ��ļ���
        res = f_mkdir(ff_NameConver(Dir));
        //������־�ĵ�
        strcpy((char *)FileName,(char const *)Dir);
        length = strlen((char const *)FileName);
        memcpy(FileName+length,"/log.bin",8);
        FileName[length+8] = 0;
        res = f_open(&file,ff_NameConver(FileName),FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
        f_close(&file);
        //�����Ҫɾ�����ļ�
        Media_SignDelFlag(Type);
    }
    if(FR_OK == res)
    {
    
    }
}
/*********************************************************************
//��������  :Media_SignDelFlag(void)
//����      :���ɾ����־
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :��һ���ļ��д���һ��delete.bin�ļ������ڱ�ʶ���ļ�����Ҫɾ��,����һ��ֻ�ܱ��һ���ļ���
*********************************************************************/
void Media_SignDelFlag(u8 Type)
{
    u32 ReservedByte;
    DIR dir;
    FIL file;
    u8  Dir[15];
    u8  Folder;
    u8  FileName[_MAX_LFN];
    u8  i;
    u8  j =0;
    u8  count;
    u32 OldestTime;
    u32 Time;
    u8  Buffer[5];
    u32 Byte;
    u8  length;
    u8  flag;
    u8  SignNum;
    
    if(0 == SdOnOffFlag)
    {
        return ;
    }

    if(MEDIA_TYPE_JPG == Type)
    {
        strcpy((char *)Dir,"1:/JPG");
        Folder = CurrentJpgFolder;
        if(0 == Folder)
        {
            return ;
        }
    }
    else
    {
        strcpy((char *)Dir,"1:/WAV");
        Folder = CurrentWavFolder;
        if(0 == Folder)
        {
            return ;
        }
    }
    flag = 0;
    //�ж��Ƿ����ѭ���洢
    for(i=0; i<2; i++)
    {
        Folder++;
        if(Folder > FOLDER_NUM_MAX)
        {
            Folder = 1;
        }
        if(Folder < 10)
        {
            Dir[6] = Folder+0x30;
            Dir[7] = 0;
        }
        else
        {
            Dir[6] = Folder/10+0x30;
            Dir[7] = Folder%10+0x30;
            Dir[8] = 0;
        }
        IWDG_ReloadCounter();//ι��,dxl,2014.6.18
        if(FR_OK == f_opendir(&dir,ff_NameConver(Dir)))
        {
            strcpy((char *)FileName,(char const *)Dir);
            length = strlen((char const *)FileName);
            memcpy(FileName+length,"/log.bin",8);
            FileName[length+8] = 0;
            if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE))
            {
                if(file.fsize >= 9)
                {
                    f_close(&file);
                    flag = 1;
                    strcpy((char *)FileName,(char const *)Dir);
                    length = strlen((char const *)FileName);
                    strcpy((char *)FileName+length,"/delete.bin");
                    if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_OPEN_ALWAYS|FA_READ|FA_WRITE))
                    {
                
                    }
                    f_close(&file);
                    Media_EnableDeleteFlag();//������ACC ON����ACC OFF����ɾ���ļ�
                }
            }
            f_close(&file);
        }
    }
    if(1 == flag)
    {
        return ;//���ȱ����Ҫɾ����
    }
        
    //��ʣ��ռ�,С��500Mʱ��ʼ���
    SignNum = Media_GetSignNum(Type);
    if(SignNum >= 3)//�ﵽ3�����ʱǿ��ɾ��
    {
        Media_EnableDeleteFlag();//������ACC ON����ACC OFF����ɾ���ļ�
        return ;//���ֻ�ܱ������
    }
    ReservedByte = GetRestKByte();
    if(0 == ReservedByte)//��ʣ��ռ����
    {
        return ;
    }
    if(ReservedByte <= MEDIA_START_SIGN_SIZE)//С��500Mʱ����Ҫɾ�����ļ�
    {
        OldestTime = 0xffffffff;
        count = 0;
        for(i=1; i<=FOLDER_NUM_MAX; i++)
        {
                IWDG_ReloadCounter();//ι��
            if(i < 10)
            {
                Dir[6] = i+0x30;
                Dir[7] = 0;
            }
            else
            {
                Dir[6] = i/10+0x30;
                Dir[7] = i%10+0x30;
                Dir[8] = 0;
            }
            strcpy((char *)FileName,(char const *)Dir);
            length = strlen((char const *)FileName);
            memcpy(FileName+length,"/log.bin",8);
            FileName[length+8] = 0;
            IWDG_ReloadCounter();//ι��,dxl,2014.6.18
            if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE))
            {
                count++;
                if(file.fsize >= 9)
                {
                    f_lseek(&file,file.fsize-9);
                    if(FR_OK == f_read (&file, Buffer, 4, &Byte))
                    {
                        Time = 0;
                        Time |= Buffer[0] << 24;
                        Time |= Buffer[1] << 16;
                        Time |= Buffer[2] << 8;
                        Time |= Buffer[3];
                        f_close(&file);
                        if(Time < OldestTime)
                        {
                            strcpy((char *)FileName,(char const *)Dir);
                            length = strlen((char const *)FileName);
                            strcpy((char *)FileName+length,"/delete.bin");
                            if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE))
                            {
                            
                            }
                            else
                            {
                                OldestTime = Time;
                                j = i;
                            }
                        }
                    }
                }
            }
            f_close(&file);
        }
        if((OldestTime != 0xffffffff)&&((count-SignNum) > 3))//���ٱ���3��û�б���ǵ��ļ���
        {
            if(j < 10)
            {
                Dir[6] = j+0x30;
                Dir[7] = 0;
            }
            else
            {
                Dir[6] = j/10+0x30;
                Dir[7] = j%10+0x30;
                Dir[8] = 0;
            }
            strcpy((char *)FileName,(char const *)Dir);
            length = strlen((char const *)FileName);
            strcpy((char *)FileName+length,"/delete.bin");
            if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_OPEN_ALWAYS|FA_READ|FA_WRITE))
            {
                
            }
            f_close(&file);
        }
        SignNum = Media_GetSignNum(Type);
        //���ͼ�����Ƶ
        if((SignNum >= 3)||(ReservedByte <= MEDIA_UNLIMITED_SIGN_SIZE))//С��100M��ﵽ3�����ʱǿ��ɾ��
        {
            Media_EnableDeleteFlag();//������ACC ON����ACC OFF����ɾ���ļ�
            return ;//���ֻ�ܱ������
        }
    }
    
    
    
    
}
/*********************************************************************
//��������  :Media_TimeTask
//����      :��������ɾ��SD���б���˵��ļ���
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :   
//���ú���  :
//�ж���Դ  :  
//����      :
//��ע      :��һ���ļ��д���һ��delete.bin�ļ������ڱ�ʶ���ļ�����Ҫɾ��,����һ��ֻ�ܱ��һ���ļ���
*********************************************************************/
FunctionalState   Media_TimeTask(void)
{
    static u8   DelFolder = 0;
    static u8   DelType = 0;
    static u8   DelState = 0;
    static u16  Count = 0;
    static u8   DelDir[15];
    static u16  Count2 = 0;
    
    u8  i;
    u8  ACC;
    u8  FileName[_MAX_LFN];
    u8  flag;
    u8  length;
    u8  Dir[15];
    u8  SignNum;
    u32 ReservedByte;
    DIR dir;
    FRESULT res;
    FILINFO FileInfo;
    FIL file;
    u8      FileName_1[_MAX_LFN];
        
    if(0 == SdOnOffFlag)
    {
        return ENABLE;
    }
        
    ACC = Io_ReadStatusBit(STATUS_BIT_ACC);
    if(0 == ACC)
    {
        SetTimerTask(TIME_MEDIA, SYSTICK_1SECOND);//1�����һ��
        
    }
    else
    {
        SetTimerTask(TIME_MEDIA, 5*SYSTICK_0p1SECOND);//0.5�����һ��
    }
    Count2++;
    if(Count2 >= 7200)//ACC ONʱ,ÿСʱ���һ��ʣ��ռ�,ACC OFFʱÿ2Сʱ���һ��
    {
        Count2 = 0;
        ReservedByte = GetRestKByte();
        if(ReservedByte <= MEDIA_UNLIMITED_SIGN_SIZE)//С��500Mʱ����Ҫɾ�����ļ�
        {
            //����ǰ��ǵ�����
            SignNum = Media_GetSignNum(MEDIA_TYPE_JPG);
            if(0 == SignNum)
            {
                Media_SignDelFlag(MEDIA_TYPE_JPG);
            }
            //����ǰ��ǵ�����
            SignNum = Media_GetSignNum(MEDIA_TYPE_WAV);
            if(0 == SignNum)
            {
                Media_SignDelFlag(MEDIA_TYPE_WAV);
            }
            Media_EnableDeleteFlag();
        }
    }
    if((0 == ACC)||(1 == MediaDelEnableFlag))//ACC OFF
    {
        if(0 == DelState)//����
        {
            if(1 == EnableFlag)
            {
                DelState = 1;
                Count = 0;
            }
            else
            {
                Count++;
                if(Count > 1800)//���а�Сʱ���1�Σ����Ƿ����ļ���Ҫɾ��
                {
                    EnableFlag = 1;
                }
            }
        }
        else if(1 == DelState)//ȷ��������ļ���
        {
            DelType = DelType%2;
            if(0 == DelType)
            {
                strcpy((char *)Dir,"1:/WAV");//¼������
            }
            else
            {
                strcpy((char *)Dir,"1:/JPG");
            }
            DelFolder = 0;
            for(i=1; i<=FOLDER_NUM_MAX; i++)
            {
                if(i < 10)
                {
                    Dir[6] = i+0x30;
                    Dir[7] = 0;
                }
                else
                {
                    Dir[6] = i/10+0x30;
                    Dir[7] = i%10+0x30;
                    Dir[8] = 0;
                }
                strcpy((char *)FileName,(char const *)Dir);
                length = strlen((char const *)FileName);
                strcpy((char *)FileName+length,"/delete.bin");
                IWDG_ReloadCounter();//ι��,dxl,2014.6.18
                if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE))
                {
                    DelFolder = i;
                    strcpy((char *)DelDir,(char const*)Dir);
                    f_close(&file);
                    break;
                }
                f_close(&file);
                
            }
            DelType++;//��һ��ɾ��������Ҫ�ı�
            if(0 != DelFolder)//�ҵ����ļ���
            {
                DelState = 2;
            }
            else
            {
                DelType = DelType%2;
                if(0 == DelType)
                {
                    strcpy((char *)Dir,"1:/WAV");//¼������
                }
                else
                {
                    strcpy((char *)Dir,"1:/JPG");
                }
                DelFolder = 0;
                for(i=1; i<=FOLDER_NUM_MAX; i++)
                {
                    if(i < 10)
                    {
                        Dir[6] = i+0x30;
                        Dir[7] = 0;
                    }
                    else
                    {
                        Dir[6] = i/10+0x30;
                        Dir[7] = i%10+0x30;
                        Dir[8] = 0;
                    }
                    strcpy((char *)FileName,(char const *)Dir);
                    length = strlen((char const *)FileName);
                    strcpy((char *)FileName+length,"/delete.bin");
                    IWDG_ReloadCounter();//ι��,dxl,2014.6.18
                    if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ|FA_WRITE))
                    {
                        DelFolder = i;
                        strcpy((char *)DelDir,(char const*)Dir);
                        f_close(&file);
                        break;
                    }
                    f_close(&file);
                
                }
                DelType++;//��һ��ɾ��������Ҫ�ı�
                if(0 != DelFolder)//�ҵ����ļ���
                {
                    DelState = 2;
                }
                else
                {
                    EnableFlag = 0;//�ر�ɾ������
                    DelState = 0;
                    MediaDelEnableFlag = 0;
                }
            }
            
        }
        else if(2 == DelState)//ɾ���ļ�
        {
            flag = 0;
            if(f_opendir(&dir, ff_NameConver(DelDir)) == FR_OK)   //��Ŀ¼�ɹ�
            {
                for(;;)
                {
                    IWDG_ReloadCounter();//ι��,dxl,2014.6.18
                    res = f_readdir(&dir,&FileInfo);
                    if(res != FR_OK || FileInfo.fname[0]==0)break;
                    if(FileInfo.fname[0]=='.')continue  ;
                    strcpy((char *)FileName,(char const*)DelDir);
                    ff_NameForward(FileName_1,FileInfo.fname);
                    if((0 == strncmp((char const *)FileName_1,"delete",6))||(0 == strncmp((char const *)FileName_1,"DELETE",6)))
                    {
                        //�����ɾ��delete.bin�ļ�
                    }
                    else
                    {
                        flag = 1;
                        length = strlen((char const *)FileName);
                        FileName[length] = '/';
                        length++;
                        FileName[length] = 0;
                        ff_NameForward(FileName_1,FileInfo.fname);
                        strcat((char *)FileName,(char const *)FileName_1);
                        res = f_unlink(ff_NameConver(FileName));       //ɾ���ļ�
                        break; 
                    }
                }
            }
            //�Ƿ�ֻʣ���һ���ļ���?
            if(0 == flag)
            {
                //ɾ��delete.bin�ļ�
                strcpy((char *)FileName,(char const *)DelDir);
                length = strlen((char const *)FileName);
                strcpy((char *)FileName+length,"/delete.bin");
                res = f_unlink(ff_NameConver(FileName));  //ɾ���ļ�
                //ɾ��Ŀ¼
                res = f_unlink(ff_NameConver(DelDir)); //ɾ���ļ�
                DelState = 0;
            }
            
            
        }
        
    }
    else
    {
        DelState = 0;
        DelType = 0;
        Count = 0;
        EnableFlag = 1;
    }
    return ENABLE;
}
/*********************************************************************
//��������  :Media_UploadTimeTask(void)
//����      :�洢��ý�������ϴ���ʱ����
//����      :
//���      :
//ʹ����Դ  :
//ȫ�ֱ���  :
//���ú���  :
//�ж���Դ  :
//����      :
//��ע      :
*********************************************************************/
FunctionalState Media_UploadTimeTask(void)
{
    static  u16 MemoryUploadSendCountMax = 0;//���ʹ����������ֵ
    static  u8  State = 0;//״̬
    static  u8  count = 0;
    static  u8  MemoryUploadBuffer[600];
    static  u8  Buffer[37];
    static  FIL file;
    u32 Byte;
    u16 length;
    u8  flag = 0;
    u8      Buffer2[5];
    u32     temp;
    u8 channel = CHANNEL_DATA_1;//Ϊ����ͨ�������
        
    if(0 == strlen((char const *)MemoryUploadFileName))
    {
        MultiMediaSendFlag = 0;
        State = 0;
        count = 0;
        MemoryUploadSendCount = 0;
        MemoryUploadSendCountMax = 0;
        memset(MemoryUploadFileName,0,33);
        RecordSendFlag = 0;
        return DISABLE;
    }
    if(0 == State)//���ļ�
    {
        if(FR_OK == f_open(&file,ff_NameConver(MemoryUploadFileName),FA_READ))
        {
					  DelayAckCommand = 0x0800;
            SetTimerTask(TIME_DELAY_TRIG, 1);//dxl,2016.5.16���Ҫ���ϴ�ͼ��ʱ�ȷ��Ͷ�ý���¼��ϴ�
            count = 0;
            State++;
            MemoryUploadSendCount = 1;//���ʹ�������
            MemoryUploadSendCountMax = (file.fsize-36)/512;
            if(0 != (file.fsize-36)%512)
            {
                MemoryUploadSendCountMax++;
            }
//                        Subpackage_Media_File_Name_Set((char const *)MemoryUploadFileName); dxl,2015.9,
                        
        }
        else
        {
            count++;
            if(count >= 3)
            {
                f_close(&file);
                flag = 1;
            }
        }
    }
    else if(1 == State)//��ȡ������Ϣ
    {
        f_lseek(&file, file.fsize-36);
        if(FR_OK == f_read (&file, Buffer, 36, &Byte))
        {
            count = 0;
            State++;
            f_lseek(&file, 0);
        }
        else
        {
            count++;
            if(count >= 3)
            {
                f_close(&file);
                flag = 1;
            }
        }
    }
    else if(2 == State)//��ȡһ������
    {
        if(FR_OK == f_read (&file, MemoryUploadBuffer, 512, &Byte))
        {
            count = 0;
            State++;
        }
        else
        {
            count++;
            if(count >= 3)
            {
                f_close(&file);
                flag = 1;
            }
        }
    }
    else if(3 == State)//����
    {
        if(1 == MemoryUploadSendCount)//��1��
        {
            memmove(MemoryUploadBuffer+36,MemoryUploadBuffer,512);
            memcpy(MemoryUploadBuffer,Buffer,36);
            length = 36+512;
                        //����������ʱ����Ϊ10����
                        temp = 600;
                    Buffer2[0] = (temp & 0xff000000) >> 24;
                    Buffer2[1] = (temp & 0xff0000) >> 16;
                    Buffer2[2] = (temp & 0xff00) >> 8;
                    Buffer2[3] = temp;
                    EepromPram_WritePram(E2_TERMINAL_HEARTBEAT_ID, Buffer2, E2_TERMINAL_HEARTBEAT_LEN);
        }
        else
        {
            length = 512;
        }
        if(ACK_OK == RadioProtocol_MultiMediaDataReport(channel,MemoryUploadBuffer, length, MemoryUploadSendCountMax, MemoryUploadSendCount))
        {
            MemoryUploadSendCount++;
            if(MemoryUploadSendCount > MemoryUploadSendCountMax)
            {
                State++;
            }
            else
            {
                State--;
            }
            count = 0;
        }
        else
        {
            count++;
            if(count >= 30)//�ش�30�β��ɹ����˳�
            {
                flag = 1;
                                f_close(&file);
            }
        }
    }
    else
    {
        f_close(&file);
        flag = 1;
    }
    if(0 == SdOnOffFlag)
    {
        flag = 1;//SD�����ڻ�����ͨ�Ź�����ֱ�ӹرո�����,dxl,2014.6.18
    }
    if(0 == flag)
    {
        MultiMediaSendFlag = 1;
        //��ͣ����,���ϴ����Ժ��ٿ���
        ClrTimerTask(TIME_CAMERA);

            return ENABLE;
    }
    else
    {
        MultiMediaSendFlag = 0;
        State = 0;
        count = 0;
        MemoryUploadSendCount = 0;
        MemoryUploadSendCountMax = 0;
        memset(MemoryUploadFileName,0,33);
                RecordSendFlag = 0;
        //��������
        SetTimerTask(TIME_CAMERA, 1);
                //SetTimerTask(TIME_CAMERA, 1);//dxl
        //SetTimerTask(TIME_RECORD,RECORD_TASK_TIME);//�������õ�������
                //����������
                SetTimerTask(TIME_HEART_BEAT, SYSTICK_1SECOND);
                //����λ�û㱨,�б���ʱʹ�ã�ʵ�ʳ���ʱ��ʹ��
                //SetTimerTask(TIME_POSITION, 1);
                //����������ʱ����Ϊ60��
                //temp = 180;//�б���ʱ���ó�Щ������Ӱ��������⹦��
                //temp = 60;
            //Buffer2[0] = (temp & 0xff000000) >> 24;
            //Buffer2[1] = (temp & 0xff0000) >> 16;
            //Buffer2[2] = (temp & 0xff00) >> 8;
            //Buffer2[3] = temp;
            //EepromPram_WritePram(E2_TERMINAL_HEARTBEAT_ID, Buffer2, E2_TERMINAL_HEARTBEAT_LEN);
        return DISABLE;
    }
}
