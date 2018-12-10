/********************************************************************
//��Ȩ˵��	:Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����	:MediaSearch.c		
//����		:ʵ��ͼ���¼���ļ���
//�汾��	:
//������	:dxl 
//����ʱ��	:2012.11
//�޸���	:
//�޸�ʱ��	:
//�޸ļ�Ҫ˵��	:
//��ע		:
***********************************************************************/
//***************�����ļ�*****************
#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"
#include "MediaSearch.h"
#include "ff.h"
#include "Media.h"
#include "Public.h"
#include "usbh_usr.h"
#include <string.h>

//****************�궨��****************

//***************���ر���******************
static u8	MediaFindFolder1 = 0;//��ý�������õ��ļ���1
static u8	MediaFindFolder2 = 0;//��ý�������õ��ļ���2

//****************ȫ�ֱ���*****************

//****************�ⲿ����*****************

/*********************************************************************
//��������	:MediaSearch_Find
//����		:���ҷ��������ļ��к��ļ���
//����		:���ҵ���ʼʱ�䣬����ʱ�䣬��ý������
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//��ע		:�����������ļ����ڵ��ļ��к��ļ���
//		:�ӿ�ʼʱ���������ֻ�ܼ���30��������ƣ�
//		:*Folder1==0��*Folder2==0��ʾû�������������ļ��У���0ֵ��ʾ�ҵ�����Ӧ���ļ�
//		:�ҵ�����Ӧ���ļ��к���ȥ����Ӧbuffer����*Foler1��Ӧ*pBuffer����ֵ��4���ֽ�һ�飬��λ��ǰ��
//		:����������4���ֽ�0ʱ��ʾ������ÿ4���ֽڱ�ʾһ��ʱ�䣬��Ӧһ���ļ�����Ҫ��ȡ������ʱ����
//		:�����Gmtime(TIME_T *tt, u32 counter)������
//		:Ŀǰֻ��������ʱ��Ҫ����ļ��������¼����룬ͨ���ż���
//		:Typeֻ��ΪMEDIA_TYPE_JPG��MEDIA_TYPE_WAV
*********************************************************************/
void MediaSearch_Find(u8 *Folder1, u8 *Folder2, u8 *pBuffer1, u8 *pBuffer2, u8 MediaType, TIME_T *StartTime, TIME_T *EndTime)
{
	u32	StartTimeCount;
	u32	EndTimeCount;
	u32	TimeBuffer[64];
	u32	temp;
	u8	Buffer[10];
	u8	Folder[3];
	u8	Dir[15];
	u8	FileName[_MAX_LFN];
	u8	i;
	//u32	j;
	u8	length;
	u32	Byte;
	u8	*p = NULL;
	u16	count1;
	u16	count3;
	DIR dir;
	FIL file1,file2;
	u16	Count;
	u16	CountMax;

	//���ʱ���ʽ����ķ�Χ��00-99
	if(ERROR == Public_CheckTimeStruct(StartTime))
	{
		*Folder1 = 0;//������0
		*Folder2 = 0;
		return ;
	}
	if(ERROR == Public_CheckTimeStruct(EndTime))
	{
		*Folder1 = 0;
		*Folder2 = 0;
		return ;
	}
	//��ʼ��Ŀ¼
	if(MEDIA_TYPE_JPG == MediaType)
	{
		strcpy((char *)Dir,"1:/JPG");
	}
	else if(MEDIA_TYPE_WAV == MediaType)
	{
		strcpy((char *)Dir,"1:/WAV");
	}
	else
	{
		*Folder1 = 0;
		*Folder2 = 0;
		return ;
	}
	
	//�ѽṹ��ʱ��ת����32λ������ʾ��ʱ��
	StartTimeCount = ConverseGmtime(StartTime);
	EndTimeCount = ConverseGmtime(EndTime);
	if(StartTimeCount > EndTimeCount)
	{
		*Folder1 = 0;
		*Folder2 = 0;
		return ;
	}

	//��ȡ32���ļ���log.bin�ļ�����ʼʱ��ͽ���ʱ�䣬û�и��ļ��л���delete.bin�ļ������0
	length = strlen((char const *)Dir);
	for(i=1; i<=32; i++)
	{
                IWDG_ReloadCounter();//ι��
		//����ļ�������
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
		if(FR_OK == f_opendir(&dir,ff_NameConver(Dir)))//���ļ���
		{
			//����ļ���
			strcpy((char *)FileName,(char const *)Dir);
			length = strlen((char const *)FileName);
			memcpy(FileName+length,"/log.bin",8);
			length += 8;
			FileName[length] = 0;
			if(FR_OK == f_open(&file1,ff_NameConver(FileName),FA_READ))
			{
				//�ж��Ƿ�����ɾ��
				strcpy((char *)FileName,(char const *)Dir);
				length = strlen((char const *)FileName);
				memcpy(FileName+length,"/delete.bin",11);
				length += 11;
				FileName[length] = 0;
				if(FR_OK == f_open(&file2,ff_NameConver(FileName),FA_READ))
				{
					//�������
					TimeBuffer[2*(i-1)] = 0;
					TimeBuffer[2*(i-1)+1] = 0;
					
				}
				else
				{
					//�����ļ���������ʱ��
					f_lseek(&file1,0);
					temp = 0;
                IWDG_ReloadCounter();//ι��
					if(FR_OK == f_read (&file1, Buffer, 4, &Byte))
					{
						temp |= Buffer[0] << 24;
						temp |= Buffer[1] << 16;
						temp |= Buffer[2] << 8;
						temp |= Buffer[3];
					}
					else
					{
					
					}
					TimeBuffer[2*(i-1)] = temp;
					//�����ļ���������ʱ��
					f_lseek(&file1,file1.fsize-9);
					temp = 0;
					if(FR_OK == f_read (&file1, Buffer, 4, &Byte))
					{
						temp |= Buffer[0] << 24;
						temp |= Buffer[1] << 16;
						temp |= Buffer[2] << 8;
						temp |= Buffer[3];
					}
					else
					{
					
					}
					TimeBuffer[2*(i-1)+1] = temp;
				}
				f_close(&file2);
			}
			else
			{
				TimeBuffer[2*(i-1)] = 0;
				TimeBuffer[2*(i-1)+1] = 0;
			}
			f_close(&file1);
		}
		else
		{
			TimeBuffer[2*(i-1)] = 0;
			TimeBuffer[2*(i-1)+1] = 0;
		}
		
	}
	//ȷ����������ʼʱ��ͽ���ʱ�������ĸ��ļ��У����ֻ���������ļ���
	*Folder1 = 0;
	*Folder2 = 0;
	for(i=0; i<4; i++)
	{
		*(pBuffer1+i) = 0;
	}
	for(i=0; i<4; i++)
	{
		*(pBuffer2+i) = 0;
	}
    temp = 0xffffffff;
	for(i=1; i<=32; i++)
	{
		//dxl,2014.3.26����
		//if((EndTimeCount>=TimeBuffer[2*(i-1)])&&(EndTimeCount<=TimeBuffer[2*(i-1)+1]))
		//{
			//if(0 == *Folder2)
			//{
				//*Folder2 = i;
                //break;
			//}	
		//}
        //if((StartTimeCount>=TimeBuffer[2*(i-1)])&&(StartTimeCount<=TimeBuffer[2*(i-1)+1]))
        //if(TimeBuffer[2*(i-1)+1] <= EndTimeCount)
		//{
			//if(0 == *Folder1)
			//{
				//*Folder1 = i;
                //break;
			//}
		//}
        if(TimeBuffer[2*(i-1)] >= StartTimeCount)//���뿪ʼʱ��������Ǹ�ʱ��Ϊ����
        {
            if(TimeBuffer[2*(i-1)] < temp)
            {
                temp = TimeBuffer[2*(i-1)];
                *Folder1 = i;
            }
        }
        else 
        {
            if(TimeBuffer[2*(i-1)+1] >= StartTimeCount)
            {
                if(TimeBuffer[2*(i-1)+1] < temp)
                {
                    temp = TimeBuffer[2*(i-1)+1];
                    *Folder1 = i;
                }
            }
        }
	}
    i++;
    if(i > 32)
    {
        i = 1;
    }
    if((TimeBuffer[2*(i-1)] > StartTimeCount)&&(TimeBuffer[2*(i-2)+1] < EndTimeCount))
    {
        *Folder2 = i;
    }
	//��ȡ����������ʱ���б��ļ����б�
	if((0 == *Folder1)&&(0 == *Folder2))//û���ҵ�
	{
		return ;
	}
	else if(((0 != *Folder1)&&(0 != *Folder2))&&(*Folder1 != *Folder2))//�ҵ������ļ���
	{
		Folder[0] = *Folder1;
		Folder[1] = *Folder2;
		MediaFindFolder1 = *Folder1;
		MediaFindFolder2 = *Folder2;
		count3 = 2;
	}
	else if((0 != *Folder1)||(0 != *Folder2))//�ҵ�һ���ļ���
	{
		if(0 != *Folder1)
		{
			Folder[0] = *Folder1;
			*Folder2 = 0;
		}
		else
		{
			*Folder1 = *Folder2;
			Folder[0] = *Folder1;
			*Folder2 = 0;
		}
		MediaFindFolder1 = *Folder1;
		MediaFindFolder2 = *Folder2;
		count3 = 1;
	}
	//�ļ���������
	count1 = 0;
	//���Ŀ¼
	for(i=0; i<count3; i++)
	{
		if(0 == i)
		{
			p = pBuffer1;
		}
		else
		{
			p = pBuffer2;
		}

		if(Folder[i] < 10)
		{
			Dir[6] = Folder[i]+0x30;
			Dir[7] = 0;
		}
		else
		{
			Dir[6] = Folder[i]/10+0x30;
			Dir[7] = Folder[i]%10+0x30;
			Dir[8] = 0;
		}
		length = strlen((char const *)Dir);
		//����ļ���
		strcpy((char *)FileName,(char const *)Dir);
		length = strlen((char const *)FileName);
		memcpy(FileName+length,"/log.bin",8);
		length += 8;
		FileName[length] = 0;
		//��log�ļ�
                IWDG_ReloadCounter();//ι��
		if(FR_OK == f_open(&file1,ff_NameConver(FileName),FA_READ))
		{
			Count = 0;
			CountMax = file1.fsize/9;
			f_lseek(&file1,0);
			for(;;)
			{
				IWDG_ReloadCounter();
				Count++;
				if(Count > CountMax)
				{
					break;
				}
				//for(j=0; j<10000; j++)
				//{
				
				//}
				if(FR_OK == f_read (&file1, Buffer, 9, &Byte))
				{
					temp = 0;
					temp |= Buffer[0] << 24;
					temp |= Buffer[1] << 16;
					temp |= Buffer[2] << 8;
					temp |= Buffer[3];
					if((temp >= StartTimeCount)&&(temp <= EndTimeCount))
					{
						*p++ = Buffer[0];
						*p++ = Buffer[1];
						*p++ = Buffer[2];
						*p++ = Buffer[3];
						count1++;
						if(count1 >= MEDIA_SEARCH_MAX_FILE_NUM)
						{
							break;
						}
					}
					else if(temp > EndTimeCount)
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			*p++ = 0;
			*p++ = 0;
			*p++ = 0;
			*p++ = 0;
			
		}
		f_close(&file1);
		if(count1 >= MEDIA_SEARCH_MAX_FILE_NUM)
		{
			break ;
		}
	}
	
}
/*********************************************************************
//��������	:MediaSearch_GetMediaIDFileName
//����		:��ȡ��ý��ID��Ӧ���ļ���
//����		:��ý��ID��
//���		:�������ļ���Ŀ¼�������̷���Ŀ¼
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//��ע		:
*********************************************************************/
void MediaSearch_GetMediaIDFileName(u8 *pBuffer,u8 MediaType, u32 MediaID)
{
	u8	Dir[20];
	u8	FileName[_MAX_LFN];
	u8	Buffer[10];
	UINT	Byte;
	u32	temp;
	u8	length;
	u8	Folder[2];
	u8	i;
	///u32	j;
	FIL	file;
	TIME_T	tt;
	u16	CountMax;
	u16	Count;
	*pBuffer = 0;
	if(MEDIA_TYPE_JPG == MediaType)
	{
		strcpy((char *)Dir,"1:/JPG");
	}
	else if(MEDIA_TYPE_WAV == MediaType)
	{
		strcpy((char *)Dir,"1:/WAV");
	}
	else
	{
		return ;
	}
	Folder[0] = MediaFindFolder1;
	Folder[1] = MediaFindFolder2;
	//�ȴ���һ�μ�����Ŀ¼Folder1����
	for(i=0; i<2; i++)
	{
		if(0 == Folder[i])
		{
			continue;
		}
		if((Folder[i] >= 1)&&(Folder[i] <= 32))
		{
			//��ȡ��־�ļ���
			if(Folder[i] < 10)
			{
				Dir[6] = Folder[i]+0x30;
				Dir[7] = '/';
				Dir[8] = 0;
			}
			else 
			{
				Dir[6] = Folder[i]/10+0x30;
				Dir[7] = Folder[i]%10+0x30;
				Dir[8] = '/';
				Dir[9] = 0;
			}
			strcpy((char *)FileName,(char const *)Dir);
			strcat((char *)FileName,"log.bin");
			//����־�ļ�
			if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ))
			{
				Count = 0;
				CountMax = file.fsize/9;
				f_lseek(&file,0);
				for(;;)
				{
					Count++;
					if(Count >= CountMax)
					{
						break;
					}
                IWDG_ReloadCounter();//ι��
					
					//for(j = 0; j<10000; j++)
					//{
				
					//}
					if(FR_OK == f_read (&file, Buffer, 9, &Byte))
					{
						temp = 0;
						temp |= Buffer[4] << 24;
						temp |= Buffer[5] << 16;
						temp |= Buffer[6] << 8;
						temp |= Buffer[7];
						if(temp == MediaID)
						{
							temp = 0;
							temp |= Buffer[0] << 24;
							temp |= Buffer[1] << 16;
							temp |= Buffer[2] << 8;
							temp |= Buffer[3];
							Gmtime(&tt, temp);
							strcpy((char *)FileName,(char const *)Dir);
							length = strlen((char const *)FileName);
							if(MEDIA_TYPE_JPG == MediaType)
							{
								FileName[length++] =  tt.day/10+0x30;
								FileName[length++] =  tt.day%10+0x30;
								FileName[length++] =  tt.hour/10+0x30;
								FileName[length++] =  tt.hour%10+0x30;
								FileName[length++] =  tt.min/10+0x30;
								FileName[length++] =  tt.min%10+0x30;
								FileName[length++] =  tt.sec/10+0x30;
								FileName[length++] =  tt.sec%10+0x30;
								FileName[length++] =  '.';
								FileName[length++] =  'j';
								FileName[length++] =  'p';
								FileName[length++] =  'g';
								FileName[length++] =  0;
							}
							else if(MEDIA_TYPE_WAV == MediaType)
							{
								FileName[length++] =  tt.month/10+0x30;
								FileName[length++] =  tt.month%10+0x30;
								FileName[length++] =  tt.day/10+0x30;
								FileName[length++] =  tt.day%10+0x30;
								FileName[length++] =  tt.hour/10+0x30;
								FileName[length++] =  tt.hour%10+0x30;
								FileName[length++] =  tt.min/10+0x30;
								FileName[length++] =  tt.min%10+0x30;
								FileName[length++] =  '.';
								FileName[length++] =  'w';
								FileName[length++] =  'a';
								FileName[length++] =  'v';
								FileName[length++] =  0;
							}
							strcpy((char  *)pBuffer,(char const *)FileName);
							return ;
						}
						else if(temp > MediaID)
						{
							break;
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	//�ٴ����е�Ŀ¼����
	for(i=1; i<32; i++)
	{
		//��ȡ��־�ļ���
		if(i < 10)
		{
			Dir[6] = i+0x30;
			Dir[7] = '/';
			Dir[8] = 0;
		}
		else 
		{
			Dir[6] = i/10+0x30;
			Dir[7] = i%10+0x30;
			Dir[8] = '/';
			Dir[9] = 0;
		}
		strcpy((char  *)FileName,(char const *)Dir);
		strcat((char  *)FileName,"log.bin");
		//����־�ļ�
		if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ))
		{
			Count = 0;
			CountMax = file.fsize/9;
			f_lseek(&file,0);
			for(;;)
			{
				//for(j = 0; j<10000; j++)
				//{
				
				//}
                IWDG_ReloadCounter();//ι��
				Count++;
				//if(Count >= CountMax) dxl,2013.5.29
                                if(Count > CountMax)
				{
					break;
				}
				if(FR_OK == f_read (&file, Buffer, 9, &Byte))
				{
					temp = 0;
					temp |= Buffer[4] << 24;
					temp |= Buffer[5] << 16;
					temp |= Buffer[6] << 8;
					temp |= Buffer[7];
					if(temp == MediaID)
					{
						temp = 0;
						temp |= Buffer[0] << 24;
						temp |= Buffer[1] << 16;
						temp |= Buffer[2] << 8;
						temp |= Buffer[3];
						Gmtime(&tt, temp);
						strcpy((char *)FileName,(char const *)Dir);
						length = strlen((char const *)FileName);
						if(MEDIA_TYPE_JPG == MediaType)
						{
							FileName[length++] =  tt.day/10+0x30;
							FileName[length++] =  tt.day%10+0x30;
							FileName[length++] =  tt.hour/10+0x30;
							FileName[length++] =  tt.hour%10+0x30;
							FileName[length++] =  tt.min/10+0x30;
							FileName[length++] =  tt.min%10+0x30;
							FileName[length++] =  tt.sec/10+0x30;
							FileName[length++] =  tt.sec%10+0x30;
							FileName[length++] =  '.';
							FileName[length++] =  'j';
							FileName[length++] =  'p';
							FileName[length++] =  'g';
							FileName[length++] =  0;
						}
						else if(MEDIA_TYPE_WAV == MediaType)
						{
							FileName[length++] =  tt.month/10+0x30;
							FileName[length++] =  tt.month%10+0x30;
							FileName[length++] =  tt.day/10+0x30;
							FileName[length++] =  tt.day%10+0x30;
							FileName[length++] =  tt.hour/10+0x30;
							FileName[length++] =  tt.hour%10+0x30;
							FileName[length++] =  tt.min/10+0x30;
							FileName[length++] =  tt.min%10+0x30;
							FileName[length++] =  '.';
							FileName[length++] =  'w';
							FileName[length++] =  'a';
							FileName[length++] =  'v';
							FileName[length++] =  0;
						}
						strcpy((char *)pBuffer,(char const *)FileName);
						return ;
					}
				}
				else
				{
					break;
				}
			}
		}
	}
}
/*********************************************************************
//��������	:MediaSearch_GetMediaIndexInformation
//����		:��ȡ��ý�������Ϣ
//����		:���ҵ���ʼʱ�䣬����ʱ�䣬��ý������
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//��ע		:Ŀǰֻ֧�ְ�ʱ������������¼����ͣ�ͨ��ID
*********************************************************************/
u16 MediaSearch_GetMediaIndexInformation(u8 *pBuffer, u8 MediaType, TIME_T *StartTime, TIME_T *EndTime)
{

	u8	Buffer1[4*MEDIA_SEARCH_MAX_FILE_NUM];
	u8	Buffer2[4*MEDIA_SEARCH_MAX_FILE_NUM];
	u8	Buffer[_MAX_LFN];
	u8	FileName[_MAX_LFN];
	u8	Folder1;
	u8	Folder2;
	u8	Folder[2];
	u8	count;
	u8	Dir[15];
	UINT	Byte;
	u32	temp;
	u8	i;
	u8	*p1 = NULL;
	static u8	*p = NULL;
	u16	length;
	TIME_T  tt;
	FIL	file;

	//���һ�ȡ�ļ��м��ļ���
	MediaSearch_Find(&Folder1, &Folder2, Buffer1, Buffer2, MediaType, StartTime, EndTime);
	//��ʼ��Ŀ¼
	if(MEDIA_TYPE_JPG == MediaType)
	{
		strcpy((char *)Dir,"1:/JPG");
	}
	else if(MEDIA_TYPE_WAV == MediaType)
	{
		strcpy((char *)Dir,"1:/WAV");
	}
	//���ദ��
	if((0 == Folder1)&&(0 == Folder2))//û���ҵ�
	{
		return 0;
	}
	else if((0 != Folder1)&&(0 != Folder2)&&(Folder1 != Folder2))
	{
		count = 2;
		Folder[0] = Folder1;
		Folder[1] = Folder2;
	}
	else if(Folder1 != 0)//�ҵ���һ���ļ���
	{
		count = 1;
		Folder[0] = Folder1;
	}
	p = pBuffer;
	length = 0;
	for(i=0; i<count; i++)
	{
		if(Folder[i] < 10)
		{
			Dir[6] = Folder[i]+0x30;
			Dir[7] = '/';
			Dir[8] = 0;
		}
		else
		{
			Dir[6] = Folder[i]/10+0x30;
			Dir[7] = Folder[i]%10+0x30;
			Dir[8] = '/';
			Dir[9] = 0;
		}
		if(0 == i)
		{
			p1 = Buffer1;
		}
		else
		{
			p1 = Buffer2;
		}
		for(;;)
		{
                IWDG_ReloadCounter();//ι��
			temp = 0;
			temp |= *p1 << 24;
			p1++;
			temp |= *p1 << 16;
			p1++;
			temp |= *p1 << 8;
			p1++;
			temp |= *p1;
			p1++;
			if(0 == temp)
			{
				break;
			}
			else
			{
				//ת��ʱ��
				Gmtime(&tt, temp);
				//����ļ���
				if(MEDIA_TYPE_JPG == MediaType)
				{
					Buffer[0] = tt.day/10+0x30;
					Buffer[1] = tt.day%10+0x30;
					Buffer[2] = tt.hour/10+0x30;
					Buffer[3] = tt.hour%10+0x30;
					Buffer[4] = tt.min/10+0x30;
					Buffer[5] = tt.min%10+0x30;
					Buffer[6] = tt.sec/10+0x30;
					Buffer[7] = tt.sec%10+0x30;
					Buffer[8] = '.';
					Buffer[9] = 'j';
					Buffer[10] = 'p';
					Buffer[11] = 'g';
					Buffer[12] = 0;
				}
				else if(MEDIA_TYPE_WAV == MediaType)
				{
					Buffer[0] = tt.month/10+0x30;
					Buffer[1] = tt.month%10+0x30;
					Buffer[2] = tt.day/10+0x30;
					Buffer[3] = tt.day%10+0x30;
					Buffer[4] = tt.hour/10+0x30;
					Buffer[5] = tt.hour%10+0x30;
					Buffer[6] = tt.min/10+0x30;
					Buffer[7] = tt.min%10+0x30;
					Buffer[8] = '.';
					Buffer[9] = 'w';
					Buffer[10] = 'a';
					Buffer[11] = 'v';
					Buffer[12] = 0;
				}
				strcpy((char *)FileName,(char const *)Dir);
				strcat((char *)FileName,(char const *)Buffer);
				//���ļ�
				if(FR_OK == f_open(&file,ff_NameConver(FileName),FA_READ))
				{
					//ƫ��ָ��
					f_lseek(&file,file.fsize-36);
					//��ȡ��ý������
					if(FR_OK == f_read (&file, Buffer, 36, &Byte))
					{
						//��ý��ID
						*p++ = Buffer[0];
						*p++ = Buffer[1];
						*p++ = Buffer[2];
						*p++ = Buffer[3];
						//��ý������
						*p++ = Buffer[4];
						//ͨ��ID
						*p++ = Buffer[7];
						//�¼�����
						*p++ = Buffer[6];
						//λ����Ϣ
						memcpy(p,Buffer+8,28);
						p += 28;
						length += 35;
					}
				}
				f_close(&file);
			}
		}
	}
	p = NULL;
	return length;
}

