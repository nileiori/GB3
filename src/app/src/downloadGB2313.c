/********************************************************************
//��Ȩ˵��	:Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//�ļ�����	:downloadGB2313.c
//����		:�洢���ѯ�·����꺺��
//�汾��	:V0.1
//������	:dxl
//����ʱ��	:2010.06
//�޸���	:
//�޸�ʱ��	:
//�޸ļ�Ҫ˵��	:
//��ע		:
***********************************************************************/

//********************************ͷ�ļ�************************************
#include "include.h"

//********************************�Զ�����������****************************

//********************************�궨��************************************

//********************************ȫ�ֱ���**********************************

//********************************�ⲿ����********************************** 
extern u8 s_ucMsgFlashBuffer[];

#define ATTEMPE_STEP_LEN	  320 // 1024*16/50  ��Ϣ�洢50�� ֮ǰ��1000   fanqinghai 2016.03.09

typedef struct
{
    s16 CurrentStep;	//��ǰ����
    s16 SectorStep;	//���������ܲ���
    s16 LoopCount;	//ѭ���洢��־
    s16 CurrentSector;	//��ǰʱ������
    s16 OldestSector;	//����ʱ������
    s16 StartSector;	//�洢��ַ��ʼ����
    s16 EndSector;	//�洢��ַ��������
}STFLASH_AREA_STATUS;
//********************************���ر���**********************************
static STFLASH_AREA_STATUS 	s_stTextAreaStatus;	//FLASH����״̬����

#define ONE_SECTORSTEP_NUM (unsigned char)(FLASH_ONE_SECTOR_BYTES/ATTEMPE_STEP_LEN)
#define MAX_DOWN_NUM (unsigned char)((FLASH_ATTEMPE_END_SECTOR-FLASH_ATTEMPE_START_SECTOR)*ONE_SECTORSTEP_NUM)
//********************************��������**********************************

//********************************��������***********************************
/*********************************************************************
//��������	:DownloadGB2313_CheckFlashArea(void)
//����		:����·����꺺����������
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:
//���ú���	:
//�ж���Դ	:  
//����		:��
//��ע		:
*********************************************************************/
ErrorStatus   DownloadGB2313_CheckFlashArea(void)
{
    s16 	j,k;
    u32 	Address;
    u8  	TimeChar[7];
    TIME_T  tt;
    u32 	MaxTimeCount;
    u32	MinTimeCount;
    u32 	TimeCount;

    //��ʼ����ر���
    s_stTextAreaStatus.StartSector = FLASH_ATTEMPE_START_SECTOR;
    s_stTextAreaStatus.OldestSector = FLASH_ATTEMPE_START_SECTOR;
    s_stTextAreaStatus.EndSector = FLASH_ATTEMPE_END_SECTOR;
    s_stTextAreaStatus.SectorStep = FLASH_ONE_SECTOR_BYTES/ATTEMPE_STEP_LEN;
    s_stTextAreaStatus.CurrentStep = 0;
    s_stTextAreaStatus.CurrentSector = FLASH_ATTEMPE_START_SECTOR;
    s_stTextAreaStatus.LoopCount = 0;
    //��ʼ�����ֵΪ0
    MaxTimeCount = 0;
    MinTimeCount = 0xFFFFFFFF;

    //ȷ����ǰ����,��ǰ����,��������
    //�ӿ�ʼ����--->��������
    for(k=FLASH_ATTEMPE_START_SECTOR; k<FLASH_ATTEMPE_END_SECTOR; k++)
    {
        #ifdef WATCHDOG_OPEN
        //ι��
        IWDG_ReloadCounter();
        #endif
        //��������0��--->����ĩβ
        for(j=0; j<s_stTextAreaStatus.SectorStep; j++)
        {
            //��ȡʱ����Ϣ
            Address = k*FLASH_ONE_SECTOR_BYTES+j*ATTEMPE_STEP_LEN;
            sFLASH_ReadBuffer(TimeChar, Address, 5);

            //С������
            TimeCount = 0;
            TimeCount |= TimeChar[0] << 24;
            TimeCount |= TimeChar[1] << 16;
            TimeCount |= TimeChar[2] << 8;
            TimeCount |= TimeChar[3];

            //����Ƿ��ҵ���Ч��ʱ���ǩ
            if(TimeCount != 0xFFFFFFFF)	//�ҵ���Чʱ���ǩ
            {
                //ת���ɽṹ��ʱ��
                Gmtime(&tt, TimeCount);

                //�ж�ʱ����������
                if(TimeCount < MinTimeCount)
                {
                    MinTimeCount = TimeCount;
                    s_stTextAreaStatus.OldestSector = k; //��������
                }

                //�ж�ʱ����������
                if(TimeCount > MaxTimeCount)
                {
                    MaxTimeCount = TimeCount;
                    s_stTextAreaStatus.CurrentStep = j+1;//���²���
                    s_stTextAreaStatus.CurrentSector = k;//����������
                }

            }
            else	//û���ҵ���Чʱ���ǩ������ѭ��������һ������
            {
                break; 
            }
        }

    }
    if(s_stTextAreaStatus.CurrentStep >= s_stTextAreaStatus.SectorStep)
    {
        s_stTextAreaStatus.CurrentStep = 0;
        s_stTextAreaStatus.CurrentSector++;
        if(s_stTextAreaStatus.CurrentSector >= s_stTextAreaStatus.EndSector)
        {
            s_stTextAreaStatus.CurrentSector = s_stTextAreaStatus.StartSector;
        }
        Address = s_stTextAreaStatus.CurrentSector*FLASH_ONE_SECTOR_BYTES;
        sFLASH_EraseSector(Address);
    }

    //*************�����һ����ֵ����ȷ���Ƿ����ѭ���洢*********
    Address = (s_stTextAreaStatus.EndSector-1)*FLASH_ONE_SECTOR_BYTES + (s_stTextAreaStatus.SectorStep-1)*ATTEMPE_STEP_LEN;
    sFLASH_ReadBuffer(TimeChar, Address, 6);
    if((0xff != TimeChar[0])||(0xff != TimeChar[1]))
    {
        s_stTextAreaStatus.LoopCount = 1;
    }
    return SUCCESS;
}
/*********************************************************************
//��������	:EraseDownloadGB2313FlashArea()
//����		:�����·����꺺������
//		:
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void EraseDownloadGB2313FlashArea(void)
{
    u16 i;
    u32 Address;
    for(i=FLASH_ATTEMPE_START_SECTOR; i<FLASH_ATTEMPE_END_SECTOR; i++)
    {
        #ifdef WATCHDOG_OPEN
        //ÿ���һ������ι��һ��
        IWDG_ReloadCounter();
        #endif
        Address = i*FLASH_ONE_SECTOR_BYTES;
        sFLASH_EraseSector(Address);
    }
}
/*********************************************************************
//��������	:DownloadGB2313_WriteToFlash(u8 *pBuffer, u16 length)
//����		:д�·����꺺�ֵ�FLASH
//����		:pBuffer������ָ��
//		:length�����ݳ���
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void DownloadGB2313_WriteToFlash(u8 *pBuffer, u16 length)
{
    u32	Address;
    u16	i;
    u8	flag;
    u32	TimeCount;
    u8	BcdTime[6];
    u8	BcdTimeRead[6];

    //�жϳ����Ƿ񳬳���Χ
    if(length > 1023)
    {
        return ;
    }

    //�жϵ�ǰ�����Ƿ񳬳���Χ
    if((s_stTextAreaStatus.CurrentSector >= FLASH_ATTEMPE_END_SECTOR)||(s_stTextAreaStatus.CurrentSector < FLASH_ATTEMPE_START_SECTOR))
    {
        return ;
    }

    //�жϵ�ǰ�����Ƿ񳬳���Χ
    if((s_stTextAreaStatus.CurrentStep < 0)||(s_stTextAreaStatus.CurrentStep >= s_stTextAreaStatus.SectorStep))
    {
        return ;
    }

    //��ȡ��ǰ��ַ
    Address = s_stTextAreaStatus.CurrentSector*FLASH_ONE_SECTOR_BYTES+s_stTextAreaStatus.CurrentStep*ATTEMPE_STEP_LEN;

    //д֮ǰ��ȡFLASH��ֵ���ж��Ƿ�Ϊ0xFF
    sFLASH_ReadBuffer(s_ucMsgFlashBuffer, Address, ATTEMPE_STEP_LEN);
    flag = 0;
    for(i=0; i<ATTEMPE_STEP_LEN; i++)
    {
        if(0xff != s_ucMsgFlashBuffer[i])
        {
            flag = 1;
            break;
        }
    }
    if(0 != flag)
    {
        s_stTextAreaStatus.CurrentStep = 0;
        sFLASH_EraseSector(Address);
        return ;
    }

    flag = 0;
    //д��ǰʱ������ݳ���
    TimeCount = RTC_GetCounter();
    BcdTime[0] = TimeCount >> 24;
    BcdTime[1] = TimeCount >> 16;
    BcdTime[2] = TimeCount >> 8;
    BcdTime[3] = TimeCount;
    BcdTime[4] = (length & 0xff00) >> 8;
    BcdTime[5] = length & 0xff;
    sFLASH_WriteBuffer(BcdTime, Address, 6);

    //����д���ʱ��ͳ���
    sFLASH_ReadBuffer(BcdTimeRead, Address, 6);


    //�ж϶�ȡ����д����Ƿ�һ��
    for(i=0; i<6; i++)
    {
        if(BcdTime[i] != BcdTimeRead[i])
        {
            flag = 1;
        }
    }


    if(0 != flag)
    {
        s_stTextAreaStatus.CurrentStep = 0;
        sFLASH_EraseSector(Address);
        return ;
    }

    //д����
    sFLASH_WriteBuffer(pBuffer, Address+6, length);

    //��ȡ��д�������
    //sFLASH_ReadBuffer(s_ucMsgFlashBuffer, Address+6, length);

    //��������
    s_stTextAreaStatus.CurrentStep++;
    if(s_stTextAreaStatus.SectorStep == s_stTextAreaStatus.CurrentStep)
    {
        s_stTextAreaStatus.CurrentStep = 0;
        s_stTextAreaStatus.CurrentSector++;
        if(s_stTextAreaStatus.EndSector == s_stTextAreaStatus.CurrentSector)
        {
            s_stTextAreaStatus.CurrentSector = s_stTextAreaStatus.StartSector;
            s_stTextAreaStatus.LoopCount = 1;
        }
        Address = s_stTextAreaStatus.CurrentSector*FLASH_ONE_SECTOR_BYTES;
        sFLASH_EraseSector(Address);
    }
}
/*********************************************************************
//��������	:DownloadGB2313_ReadFromFlash(u8 Num, u8 *pBuffer)
//����		:��FLASH�ﰴ��Ŷ�ȡ�·����꺺����Ϣ
//����		:Num,��š����µ�������ϢΪ���1�����µ�������ϢΪ���2����������
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	: 
//���ú���	:
//�ж���Դ	:  
//����		:pBuffer����
//��ע		:
*********************************************************************/
u8 DownloadGB2313_ReadFromFlash(u8 Num, u8 *pBuffer)
{
    u8	TimeBuffer[7];
    u8	i;
    u16	length = 0;
    u32	Address;
    s16	SearchStep;
    s16	SearchSector;

    //���ֻ�ܴ洢6��
    if(Num >= MAX_DOWN_NUM)
    {
        return 0;//��ų�����ǰ���ֻ�ܲ�ѯ6����Ϣ
    }

    //�ӵ�ǰ������ǰ��Num��
    SearchStep = s_stTextAreaStatus.CurrentStep;
    SearchSector = s_stTextAreaStatus.CurrentSector;
    for(i=0; i<Num; i++)
    {
        SearchStep--;
        if(SearchStep < 0)
        {
            SearchStep = s_stTextAreaStatus.SectorStep - 1;
            SearchSector--;
            if(SearchSector < s_stTextAreaStatus.StartSector)
            {
                if(s_stTextAreaStatus.LoopCount > 0)
                {
                    SearchSector = s_stTextAreaStatus.EndSector - 1;
                }
                else
                {
                    return 0;//û����Ӧ�ļ�¼��ֱ���˳�
                }
            }
        }	
    }

    //��ȡʱ����Ϣ
    Address = SearchSector*FLASH_ONE_SECTOR_BYTES+SearchStep*ATTEMPE_STEP_LEN;
    sFLASH_ReadBuffer(TimeBuffer, Address, 6);

    //�ж��Ƿ�Ϊ��Ч����
    if((0xff == TimeBuffer[0])||(0xff == TimeBuffer[4]))
    {
        return 0;
    }

    //��ȡ���꺺����Ϣ
    length = 0;
    length |= TimeBuffer[4] << 8;
    length |= TimeBuffer[5];
    if(length > 1023)
    {
        length = 1023;
    }
    sFLASH_ReadBuffer(pBuffer, Address+6, length);

    return length;

}
/*********************************************************************
//��������	:DownloadGB2313_GetTotalStep(void)
//����		:��ȡ�·����꺺���Ѵ�����ܲ���
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	: 
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
u8 DownloadGB2313_GetTotalStep(void)
{
    u8	TotalStep = 0;

    if(0 == s_stTextAreaStatus.LoopCount)	//û�н���ѭ���洢
    {
        TotalStep = (s_stTextAreaStatus.CurrentSector - s_stTextAreaStatus.StartSector)
        *s_stTextAreaStatus.SectorStep + 	s_stTextAreaStatus.CurrentStep;
    }
    else	//�ѽ���ѭ���洢
    {
        TotalStep = MAX_DOWN_NUM-s_stTextAreaStatus.SectorStep + s_stTextAreaStatus.CurrentStep;
    }

    return TotalStep;
}
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

