#include "spi.h"
#include "mmc_sd.h"			   
#include "stm32f10x_spi.h"	
#include "stm32f10x_iwdg.h"	
#include "spi_flash.h"
#include "ff.h"
#include "Rtc.h"
#include "usbh_usr.h"
#include <string.h>
		
void IntToAsc(u32 temp, u8 *buff) ; //���޷�������ת��ʮ������8���ַ����
DWORD GetIntTime (u8 Time[6]) ;     //��Hexʱ��ת�ɳ�����
u8 CheckDayTime(u32 iStartTime,u32 iEndTime, u32 fileTime) ; //�ж��ļ�����ʱ���Ƿ���ϼ���Ҫ��
u8 _SearchJPEG(u8 Event, u8 StartTime[6], u8 EndTime[6], u8 *Str,DIR *pdir)  ;  //��ʱ��,�¼�����
u8 _SearchWAV(u8 Event, u8 StartTime[6], u8 EndTime[6], u8 *Str,DIR *pdir)  ;  //��ʱ��,�¼�����
void SetupIndexFile(void)  ;  //���������ļ�
u8 CheckJPEGEvent(u8 Event, FILINFO *fno) ;  //�ж��ļ��¼��Ƿ���ϼ���Ҫ��
u8 CheckWAVEvent(u8 Event, FILINFO *fno) ;  //�ж��ļ��¼��Ƿ���ϼ���Ҫ��
void *MyMemcpy(void *s1,  const void *s2, u32 n, u8 s) ; //s2�����ݿ���s1��,nΪ��������,sΪ����(1:����,0:˳��)
u32 SearchFileSD( FIL *fp,u32 SIndex,u32 EIndex, u32 MeidaID) ; //ͨ��ý��ID���ֲ��ҷ��ļ����к�

u8 SD_Type=0;//SD�������� 
FATFS fs; 

extern u8	SdOnOffFlag;//SD�����߱�־��1���ߣ�0������,��sd����ʼ��ʧ��ʱ�ñ�־Ҳ����0
//T_FILE_MAG gFileMag;
////////////////////////////////////��ֲ�޸���///////////////////////////////////
//��ֲʱ��Ľӿ�
//data:Ҫд�������
//����ֵ:����������
u8 SD_SPI_ReadWriteByte(u8 data)
{
	return SPIx_ReadWriteByte(data);
}
//SD����ʼ����ʱ��,��Ҫ����
void SD_SPI_SpeedLow(void)
{
 	SPIx_SetSpeed(SPI_BaudRatePrescaler_256);//���õ�����ģʽ	
}
//SD������������ʱ��,���Ը�����
void SD_SPI_SpeedHigh(void)
{
 	//SPIx_SetSpeed(SPI_BaudRatePrescaler_2);//���õ�����ģʽ	
	//SPIx_SetSpeed(SPI_BaudRatePrescaler_4);
	SPIx_SetSpeed(SPI_BaudRatePrescaler_8);
}
//SPIӲ�����ʼ��
void SD_SPI_Init(void)
{    
	SPIx_Init();
	SD_CS_HIGH();
}
///////////////////////////////////////////////////////////////////////////////////
//ȡ��ѡ��,�ͷ�SPI����
void SD_DisSelect(void)
{
	SD_CS_HIGH();
	SD_SPI_ReadWriteByte(0xff);//�ṩ�����8��ʱ��
}
//ѡ��sd��,���ҵȴ���׼��OK
//����ֵ:0,�ɹ�;1,ʧ��;
u8 SD_Select(void)
{
	SD_CS_LOW();
	if(SD_WaitReady()==0)return 0;//�ȴ��ɹ�
	SD_DisSelect();
	return 1;//�ȴ�ʧ��
}
//�ȴ���׼����
//����ֵ:0,׼������;����,�������
u8 SD_WaitReady(void)
{
	u32 t=0;
    
    if(0 == SdOnOffFlag)//dxl,2014.6.18sd�������ߵĻ�ֱ�ӷ���
    {
        return 1;
    }
    
	do
	{
		if(SD_SPI_ReadWriteByte(0XFF)==0XFF)return 0;//OK
		t++;	
        IWDG_ReloadCounter(); //��Ҫι��,����SD�������������ܻ�һֱ����
	}while(t<0X7FFFF);//������ʱ̫��ʱ��,����Ӧ��Ϊ0x7FFF,�˴�ȡֵΪ0X7FFFF
    if(0X7FFFF == t)
    {
        SdOnOffFlag = 0;//ָʾsd�����߶�д������,���sd�����߱�־,��ͬ��û�н�sd��,dxl,2014.6.18
    }
	return 1;
}
//�ȴ�SD����Ӧ
//Response:Ҫ�õ��Ļ�Ӧֵ
//����ֵ:0,�ɹ��õ��˸û�Ӧֵ
//    ����,�õ���Ӧֵʧ��
u8 SD_GetResponse(u8 Response)
{
	u16 Count=0xFFF;
    
    if(0 == SdOnOffFlag)//dxl,2014.6.18
    {
        return MSD_RESPONSE_FAILURE;
    }
	while ((SD_SPI_ReadWriteByte(0XFF)!=Response)&&Count)
    {
        Count--;//�ȴ��õ�׼ȷ�Ļ�Ӧ  	
        IWDG_ReloadCounter();
    }
	if (Count==0)
    {
        SdOnOffFlag = 0;//ָʾsd�����߶�д������,���sd�����߱�־,��ͬ��û�н�sd��,dxl,2014.6.18
        return MSD_RESPONSE_FAILURE;
    }//�õ���Ӧʧ��  
	else 
    {   
        return MSD_RESPONSE_NO_ERROR;
    }//��ȷ��Ӧ
}
//��sd����ȡһ�����ݰ�������
//buf:���ݻ�����
//len:Ҫ��ȡ�����ݳ���.
//����ֵ:0,�ɹ�;����,ʧ��;	
u8 SD_RecvData(u8*buf,u16 len)
{			  	  
	if(SD_GetResponse(0xFE))return 1;//�ȴ�SD������������ʼ����0xFE
    while(len--)//��ʼ��������
    {
        *buf=SPIx_ReadWriteByte(0xFF);
        buf++;
    }
    //������2��αCRC��dummy CRC��
    SD_SPI_ReadWriteByte(0xFF);
    SD_SPI_ReadWriteByte(0xFF);									  					    
    return 0;//��ȡ�ɹ�
}
//��sd��д��һ�����ݰ������� 512�ֽ�
//buf:���ݻ�����
//cmd:ָ��
//����ֵ:0,�ɹ�;����,ʧ��;	
u8 SD_SendBlock(u8*buf,u8 cmd)
{	
	u16 t;		  	  
	if(SD_WaitReady())return 1;//�ȴ�׼��ʧЧ
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)//���ǽ���ָ��
	{
		for(t=0;t<512;t++)SPIx_ReadWriteByte(buf[t]);//����ٶ�,���ٺ�������ʱ��
	    SD_SPI_ReadWriteByte(0xFF);//����crc
	    SD_SPI_ReadWriteByte(0xFF);
		t=SD_SPI_ReadWriteByte(0xFF);//������Ӧ
		if((t&0x1F)!=0x05)return 2;//��Ӧ����									  					    
	}						 									  					    
    return 0;//д��ɹ�
}

//��SD������һ������
//����: u8 cmd   ���� 
//      u32 arg  �������
//      u8 crc   crcУ��ֵ	   
//����ֵ:SD�����ص���Ӧ															  
u8 SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
    u8 r1;	
	u8 Retry=0; 
	SD_DisSelect();//ȡ���ϴ�Ƭѡ
	if(SD_Select())return 0XFF;//ƬѡʧЧ 
	//����
    SD_SPI_ReadWriteByte(cmd | 0x40);//�ֱ�д������
    SD_SPI_ReadWriteByte(arg >> 24);
    SD_SPI_ReadWriteByte(arg >> 16);
    SD_SPI_ReadWriteByte(arg >> 8);
    SD_SPI_ReadWriteByte(arg);	  
    SD_SPI_ReadWriteByte(crc); 
	if(cmd==CMD12)SD_SPI_ReadWriteByte(0xff);//Skip a stuff byte when stop reading
    //�ȴ���Ӧ����ʱ�˳�
	Retry=0X1F;
	do
	{
		r1=SD_SPI_ReadWriteByte(0xFF);
        IWDG_ReloadCounter();
	}while((r1&0X80) && Retry--);	 
	//����״ֵ̬
    return r1;
}		    																			  
//��ȡSD����CID��Ϣ��������������Ϣ
//����: u8 *cid_data(���CID���ڴ棬����16Byte��	  
//����ֵ:0��NO_ERR
//		 1������														   
u8 SD_GetCID(u8 *cid_data)
{
    u8 r1;	   
    //��CMD10�����CID
    r1=SD_SendCmd(CMD10,0,0x01);
    if(r1==0x00)
	{
		r1=SD_RecvData(cid_data,16);//����16���ֽڵ�����	 
    }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)return 1;
	else return 0;
}																				  
//��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
//����:u8 *cid_data(���CID���ڴ棬����16Byte��	    
//����ֵ:0��NO_ERR
//		 1������														   
u8 SD_GetCSD(u8 *csd_data)
{
    u8 r1;	 
    r1=SD_SendCmd(CMD9,0,0x01);//��CMD9�����CSD
    if(r1==0)
	{
    	r1=SD_RecvData(csd_data, 16);//����16���ֽڵ����� 
    }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)return 1;
	else return 0;
}  
//��ȡSD����������������������   
//����ֵ:0�� ȡ�������� 
//       ����:SD��������(������/512�ֽ�)
//ÿ�������ֽ�����Ϊ512����Ϊ�������512�����ʼ������ͨ��.														  
u32 SD_GetSectorCount(void)
{
    u8 csd[16];
    u32 Capacity;  
    u8 n;
	u16 csize;  					    
	//ȡCSD��Ϣ������ڼ��������0
    if(SD_GetCSD(csd)!=0) return 0;	    
    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)	 //V2.00�Ŀ�
    {	
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//�õ�������	 		   
    }else//V1.XX�Ŀ�
    {	
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//�õ�������   
    }
    return Capacity;
}
u8 SD_Idle_Sta(void)
{
	u16 i;
	u8 retry;	   	  
    for(i=0;i<0xf00;i++);//����ʱ���ȴ�SD���ϵ����	 
    //�Ȳ���>74�����壬��SD���Լ���ʼ�����
    for(i=0;i<10;i++)SPIx_ReadWriteByte(0xFF); 
    //-----------------SD����λ��idle��ʼ-----------------
    //ѭ����������CMD0��ֱ��SD������0x01,����IDLE״̬
    //��ʱ��ֱ���˳�
    retry = 0;
    do
    {	   
        //����CMD0����SD������IDLE״̬
        i = SD_SendCmd(CMD0, 0, 0x95);
        retry++;
        IWDG_ReloadCounter();
    }while((i!=0x01)&&(retry<200));
    //����ѭ���󣬼��ԭ�򣺳�ʼ���ɹ���or ���Գ�ʱ��
    if(retry==200)return 1; //ʧ��
	return 0;//�ɹ�	 						  
}
//��ʼ��SD��
u8 SD_Initialize(void)
{
    u8 r1;      // ���SD���ķ���ֵ
    u16 retry;  // �������г�ʱ����
    u8 buf[4];  
	u16 i;

	//LOG_PR_N("SD_Initialize()\r\n");

	SD_SPI_Init();		//��ʼ��IO
 	SD_SPI_SpeedLow();	//���õ�����ģʽ 
    //for(i=0;i<0xf00;i++);//����ʱ���ȴ�SD���ϵ����	  
	for(i=0;i<10;i++)SD_SPI_ReadWriteByte(0XFF);//��������74������
	retry=20;
	do
	{
		r1=SD_SendCmd(CMD0,0,0x95);//����IDLE״̬
	}while((r1!=0X01) && retry--);
 	SD_Type=0;//Ĭ���޿�
	if(r1==0X01)
	{
		if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
		{
			for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);	//Get trailing return value of R7 resp
			if(buf[2]==0X01&&buf[3]==0XAA)//���Ƿ�֧��2.7~3.6V
			{
				//retry=0XFFFE;
                retry = 0xfff;
				do
				{
					SD_SendCmd(CMD55,0,0X01);	//����CMD55
					r1=SD_SendCmd(CMD41,0x40000000,0X01);//����CMD41
                   IWDG_ReloadCounter();
				}while(r1&&retry--);
				if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//����SD2.0���汾��ʼ
				{
					for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);//�õ�OCRֵ
					if(buf[0]&0x40)SD_Type=SD_TYPE_V2HC;    //���CCS
					else SD_Type=SD_TYPE_V2;   
				}
			}
		}else//SD V1.x/ MMC	V3
		{
			SD_SendCmd(CMD55,0,0X01);		//����CMD55
			r1=SD_SendCmd(CMD41,0,0X01);	//����CMD41
			if(r1<=1)
			{		
				SD_Type=SD_TYPE_V1;
				//retry=0XFFFE;
                retry = 0xfff;
				do //�ȴ��˳�IDLEģʽ
				{
					SD_SendCmd(CMD55,0,0X01);	//����CMD55
					r1=SD_SendCmd(CMD41,0,0X01);//����CMD41
                    IWDG_ReloadCounter();
				}while(r1&&retry--);
			}else
			{
				SD_Type=SD_TYPE_MMC;//MMC V3
				//retry=0XFFFE;
                retry = 0xfff;
				do //�ȴ��˳�IDLEģʽ
				{											    
					r1=SD_SendCmd(CMD1,0,0X01);//����CMD1
                    IWDG_ReloadCounter();               
				}while(r1&&retry--);  
			}
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;//����Ŀ�
		}
	}
	SD_DisSelect();//ȡ��Ƭѡ
	SD_SPI_SpeedHigh();//����
	if(SD_Type)return 0;
	else if(r1)return r1; 	   
	return 0xaa;//��������
}
 
//��SD��
//buf:���ݻ�����
//sector:����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;//ת��Ϊ�ֽڵ�ַ
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD17,sector,0X01);//������
		if(r1==0)//ָ��ͳɹ�
		{
			r1=SD_RecvData(buf,512);//����512���ֽ�	   
		}
	}else
	{
		r1=SD_SendCmd(CMD18,sector,0X01);//����������
		do
		{
			r1=SD_RecvData(buf,512);//����512���ֽ�	 
			buf+=512;  
		}while(--cnt && r1==0); 	
		SD_SendCmd(CMD12,0,0X01);	//����ֹͣ����
	}   
	SD_DisSelect();//ȡ��Ƭѡ
	return r1;//
}
//дSD��
//buf:���ݻ�����
//sector:��ʼ����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt)
{
	u8 r1;
	if(SD_Type!=SD_TYPE_V2HC)sector *= 512;//ת��Ϊ�ֽڵ�ַ
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD24,sector,0X01);//������
		if(r1==0)//ָ��ͳɹ�
		{
			r1=SD_SendBlock(buf,0xFE);//д512���ֽ�	   
		}
	}else
	{
		if(SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);	
			SD_SendCmd(CMD23,cnt,0X01);//����ָ��	
		}
 		r1=SD_SendCmd(CMD25,sector,0X01);//����������
		if(r1==0)
		{
			do
			{
				r1=SD_SendBlock(buf,0xFC);//����512���ֽ�	 
				buf+=512;  
			}while(--cnt && r1==0);
			r1=SD_SendBlock(0,0xFD);//����512���ֽ� 
		}
	}   
	SD_DisSelect();//ȡ��Ƭѡ
	return r1;//
}	   

//���ʣ��ռ�(KB)
//����ֵ:0ʧ��   ����:ʣ���ֽ���
DWORD GetRestKByte(void)
{
    DWORD fre_clust;
    FATFS *fs;
    if( f_getfree(ff_NameConver("1:"),&fre_clust,&fs) == FR_OK )
    {
        return fre_clust * fs->csize /2;       
    }
    else
    {
        return 0;
    }
}

/*******************************************************
//����:���޷�������ת��ʮ������8���ַ����
//�������������,ת������ַ���(����)
//���أ��������99999999, ��8���ַ���� 
//����: 0X1346 -> "00004936"  
*******************************************************/
void IntToAsc(u32 temp, u8 *buff) 
{
	u8 i,j;
	const u32 DivTbl[]={1,1,10,100,1000,10000,100000,1000000,10000000};
	for(i=8,j=0; i>0; i--)
	{
		buff[j++]=temp/DivTbl[i]+'0';
		temp=temp%DivTbl[i];
	}
    if(j>=8)
        buff[j]='\0';
}
/*******************************************************
Description:���������ļ�
Input: 
Output:
Return: 
note:    
*******************************************************/
void SetupIndexFile(void) 
{
	//LOG_PR_N("SetupIndexFile()\r\n");
	
/*
    FIL file;
    u8 buffer[9]={1,0,0,0,1,0,0,0,2};
    u32 Bw;     
    //WAVĿ¼�´����ļ�,����������ļ���  FA_CREATE_NEW
    {   
        if(f_open(&file,"1:/WAV/INDEX.TXT",FA_CREATE_NEW|FA_READ|FA_WRITE) != FR_EXIST)
        { //�ļ�������,�����ļ�
            f_close(&file);
            if(f_open(&file,"1:/WAV/INDEX.TXT",FA_CREATE_ALWAYS|FA_READ|FA_WRITE) == FR_OK)  
            {                       
               f_write(&file,buffer,sizeof(buffer),&Bw);                          
            } 
            f_close(&file);

            if(f_open(&file,"1:/WAV/INDEXBAK.TXT",FA_CREATE_ALWAYS|FA_READ|FA_WRITE) == FR_OK)  
            {                       
               f_write(&file,buffer,sizeof(buffer),&Bw);                          
            }                                
        }
        f_close(&file);  
    }
    {   
        if(f_open(&file,"1:/JPEG/INDEX.TXT",FA_CREATE_NEW|FA_READ|FA_WRITE) != FR_EXIST)
        { //�ļ�������,�����ļ�
            f_close(&file);
            if(f_open(&file,"1:/JPEG/INDEX.TXT",FA_CREATE_ALWAYS|FA_READ|FA_WRITE) == FR_OK)  
            {                       
               f_write(&file,buffer,sizeof(buffer),&Bw);                          
            } 
            f_close(&file);

            if(f_open(&file,"1:/JPEG/INDEXBAK.TXT",FA_CREATE_ALWAYS|FA_READ|FA_WRITE) == FR_OK)  
            {                       
               f_write(&file,buffer,sizeof(buffer),&Bw);                          
            }                      
        }
        f_close(&file);
    }
*/
	
}
/*******************************************************
Description:SD�ļ���ʼ��
Input: 
Output:
Return: 
note:    
*******************************************************/  
void InitSDFile(void) 
{
    FRESULT Result;
    //DIR dir;
    //FILINFO fileInfo;

	//LOG_PR_N("InitSDFile()\r\n");
    //����Ŀ¼1:WAV
    Result = f_mkdir(ff_NameConver("1:/WAV"));
    if(Result != FR_OK)
    {
        //SdOnOffFlag = 0;//dxl,2014.6.18,��ΪSD������
        if(Result == FR_EXIST)
        { //���ڸ�Ŀ¼,ɾ��������ļ�  
            ;
#if 0            
            if(f_opendir(&dir, "1:/WAV") == FR_OK)   //��Ŀ¼�ɹ�
            {
                while(1)
                {
                    u8 Buffer[25]="1:/WAV/";
                    
                    IWDG_ReloadCounter();             
                    Result = f_readdir(&dir,&fileInfo);
                    if(Result != FR_OK || fileInfo.fname[0]==0)break;
                    if(fileInfo.fname[0]=='.')continue  ;
                    strcat((char *)Buffer,fileInfo.fname);
                    Result = f_unlink((char const *)Buffer);       //ɾ���ļ�
                    if(Result != FR_OK) break;                   
                }
            } 
#endif
        }
        else
        {
            SdOnOffFlag = 0;//dxl,2014.8.25,��ΪSD������,ԭ2014.6.18���޸���bug
        }
    }


    //����Ŀ¼1:JPEG
    Result = f_mkdir(ff_NameConver("1:/JPEG"));
    if(Result != FR_OK)
    {
        //SdOnOffFlag = 0;//dxl,2014.6.18,��ΪSD������
        if(Result == FR_EXIST)
        { //���ڸ�Ŀ¼,ɾ��������ļ�  
            ;
#if 0  
            if(f_opendir(&dir, "1:/JPEG") == FR_OK)   //��Ŀ¼�ɹ�
            {
                while(1)
                {
                    u8 Buffer[25]="1:/JPEG/";
                    
                  IWDG_ReloadCounter();         
                    Result = f_readdir(&dir,&fileInfo);
                    if(Result != FR_OK || fileInfo.fname[0]==0)break;
                    if(fileInfo.fname[0]=='.')continue  ;
                    strcat((char *)Buffer,fileInfo.fname);
                    Result = f_unlink((char const *)Buffer);       //ɾ���ļ�
                    if(Result != FR_OK) break;                  
                }
            } 
#endif            
        }
        else
        {
            SdOnOffFlag = 0;//dxl,2014.8.25,ԭ2014.6.18���޸���bug
        }
    } 
    SetupIndexFile();
}

/*******************************************************
Description:���ͼ���ļ���,Str�ռ䲻��С��25
Input: 
Output: �ļ���
Return: 1:�ɹ�  0:ʧ��
note:    
*******************************************************/ 
#define MAX_REST_SPACE   (100000) //KBYTES
#define MIN_REST_SPACE   (1000)   //KBYTES

//#define MAX_REST_SPACE   (3860580-1000) //DEBUG
//#define MIN_REST_SPACE   (3860580-2100)   //DEBUG
/*
u8 FetchJPEGFileName(u8 *Str) 
{
    FIL file,fileBak;  
    u8 Buffer[15],i;   
    u8 FileName[25];
    u32 Br,Bw;  
    u32 Result;   
    u32 StartIndex; //��ʼ������  ��1��ʼ
    u32 EndIndex;   //����������  ��1��ʼ
    
    u32 CStartIndex; //��ʼ������ ��0��ʼ
    u32 CEndIndex;   //���������� ��0��ʼ  
    //�������
    if(Str == NULL)
        return 0;
        
    //���ͷβ�ļ����к�
    if(f_open(&file,"1:/JPEG/INDEX.TXT",FA_OPEN_ALWAYS|FA_READ|FA_WRITE) != FR_OK)  
        return 0;           
    f_read(&file,Buffer,9,&Br);   
    
    f_open(&fileBak,"1:/JPEG/INDEXBAK.TXT",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);       
    Buffer[9] = 0; //����Buffer[9],��У��ʹ���
    for(i=0; i<8; i++)
    {
        Buffer[9] += Buffer[i];
    }
    if(Buffer[9] != Buffer[8])
    { //У�����
        f_read(&fileBak,Buffer,9,&Br);           
    }  
    
    StartIndex = *(u32 *)&Buffer[0]; 
    EndIndex =  *(u32 *)&Buffer[4];     
    CStartIndex = StartIndex - 1;
    CEndIndex = EndIndex - 1;
    Result = EndIndex;            //��÷����ļ�����ID      
    
    
    if( (CEndIndex+1)%MAX_FILES_SUM == CStartIndex || GetRestKByte() < MIN_REST_SPACE) 
    { //�������˻�ռ䲻��           
        do
        {   
            if(CEndIndex == CStartIndex)
            {//����
                f_close(&file);
                f_close(&fileBak);  
                return 0;    
            }
            //�ռ��С,��Ҫɾ���ļ�                          
            IntToAsc(CStartIndex+1,Buffer);
            strcpy((char *)FileName,"1:/JPEG/");
            strcat((char *)FileName,(char const *)Buffer);   
            strcat((char *)FileName,".JPG");       
            f_unlink((char const *)FileName);                       //ɾ���ļ��ڳ��ռ�                
            CStartIndex = (CStartIndex+1)%MAX_FILES_SUM;
            //IWDG_ReloadCounter();dxl,2013.11.21,ȥ��ι��,SD������������,�ᵼ�³���һֱ����SD��ĳ����������
        }
        while(GetRestKByte() < MAX_REST_SPACE );
    }
    CEndIndex = (CEndIndex+1)%MAX_FILES_SUM;
    
    StartIndex = CStartIndex + 1;
    EndIndex = CEndIndex + 1;
    {
        u8 i;
        for(i=0; i<4; i++)
            Buffer[i] =  *((u8 *)&StartIndex +i);
        for(i=0; i<4; i++)
            Buffer[i+4] =  *((u8 *)&EndIndex +i);   
            
        Buffer[8] = 0; 
        for(i=0; i<8; i++)
        {
            Buffer[8] += Buffer[i]; 
        }
                    
        f_lseek(&file,0);      
        f_lseek(&fileBak,0);     
        f_write(&file,Buffer,9,&Bw);
        f_write(&fileBak,Buffer,9,&Bw); 
    }
    f_close(&file);
    f_close(&fileBak); 
    IntToAsc(Result,Buffer);

    strcpy((char *)FileName,"1:/JPEG/");
    strcat((char *)FileName,(char const *)Buffer);   
    strcat((char *)FileName,".JPG");                         
    strcpy((char *)Str,(char const *)FileName);
    return 1;
}
*/
/*********************************************************************
//��������	:FetchJPEGFileName
//����		:��ȡ�ļ���
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
u8  FetchJPEGFileName(u8 *pBuffer)
{
	TIME_T  tt;
	
	u8 *p = NULL;
	u8 length;

	//LOG_PR_N("FetchJPEGFileName()\r\n");
	RTC_ReadTime(&tt);
	p = pBuffer;
	length = 0;
	
	//��Ŀ¼·��
	*p++ = '1';
	length++;
	*p++ = ':';
	length++;
	*p++ = '/';
	length++;
	*p++ = 'J';
	length++;
	*p++ = 'P';
	length++;
	*p++ = 'E';
	length++;
	*p++ = 'G';
	length++;
	*p++ = '/';
	length++;
	
	//�ļ���
	*p++ = tt.year/10+0x30;
	length++;
	*p++ = tt.year%10+0x30;
	length++;
	*p++ = tt.month/10+0x30;
	length++;
	*p++ = tt.month%10+0x30;
	length++;
	*p++ = tt.day/10+0x30;
	length++;
	*p++ = tt.day%10+0x30;
	length++;
	*p++ = tt.hour/10+0x30;
	length++;
	*p++ = tt.hour%10+0x30;
	length++;
	*p++ = tt.min/10+0x30;
	length++;
	*p++ = tt.min%10+0x30;
	length++;
	*p++ = tt.sec/10+0x30;
	length++;
	*p++ = tt.sec%10+0x30;
	length++;
	//��׺
	*p++ = '.';
	length++;
	*p++ = 'j';
	length++;
	*p++ = 'p';
	length++;
	*p++ = 'e';
	length++;
	*p++ = 'g';
	length++;
	//��β�ַ�
	*p++ = 0;
	length++;
		
	return length;
}
/*******************************************************
Description:���¼���ļ���,Str�ռ䲻��С��25
Input: 
Output: �ļ���
Return: 1:�ɹ�  0:ʧ��
note:    
*******************************************************/ 
/*
u8 FetchWAVFileName(u8 *Str) 
{
    FIL file,fileBak;  
    u8 Buffer[15],i;  
    u8 FileName[25];
    u32 Br,Bw;  
    u32 Result;   
    u32 StartIndex; //��ʼ������  ��1��ʼ
    u32 EndIndex;   //����������  ��1��ʼ
    
    u32 CStartIndex; //��ʼ������ ��0��ʼ
    u32 CEndIndex;   //���������� ��0��ʼ  
    //�������
    if(Str == NULL)
        return 0;
      
    //���ͷβ�ļ����к�
    if(f_open(&file,"1:/WAV/INDEX.TXT",FA_OPEN_ALWAYS|FA_READ|FA_WRITE) != FR_OK)  
        return 0;           
    f_read(&file,Buffer,9,&Br);   
    
    f_open(&fileBak,"1:/WAV/INDEXBAK.TXT",FA_OPEN_ALWAYS|FA_READ|FA_WRITE);      
    Buffer[9] = 0; //����Buffer[9],��У��ʹ���
    for(i=0; i<8; i++)
    {
        Buffer[9] += Buffer[i];
    }
    if(Buffer[9] != Buffer[8])
    { //У�����
        f_read(&fileBak,Buffer,9,&Br);           
    }  
    
    StartIndex = *(u32 *)&Buffer[0]; 
    EndIndex =  *(u32 *)&Buffer[4];     
    CStartIndex = StartIndex - 1;
    CEndIndex = EndIndex - 1;
    Result = EndIndex;            //��÷����ļ�����ID      
    
    
    if( (CEndIndex+1)%MAX_FILES_SUM == CStartIndex || GetRestKByte() < MIN_REST_SPACE) 
    { //�������˻�ռ䲻��           
        do
        {   
            if(CEndIndex == CStartIndex)
            {//����
                f_close(&file);
                f_close(&fileBak);  
                return 0;    
            }
            //�ռ��С,��Ҫɾ���ļ�                          
            IntToAsc(CStartIndex+1,Buffer);
            strcpy((char *)FileName,"1:/WAV/");
            strcat((char *)FileName,(char const *)Buffer);   
            strcat((char *)FileName,".WAV");       
            f_unlink((char const *)FileName);                       //ɾ���ļ��ڳ��ռ�                
            CStartIndex = (CStartIndex+1)%MAX_FILES_SUM;
            IWDG_ReloadCounter(); dxl,2013.11.21,ȥ��ι��,SD������������,�ᵼ�³���һֱ����SD��ĳ����������
        }
        while(GetRestKByte() < MAX_REST_SPACE );
    }
    CEndIndex = (CEndIndex+1)%MAX_FILES_SUM;
    
    StartIndex = CStartIndex + 1;
    EndIndex = CEndIndex + 1;
    {
        u8 i;
        for(i=0; i<4; i++)
            Buffer[i] =  *((u8 *)&StartIndex +i);
        for(i=0; i<4; i++)
            Buffer[i+4] =  *((u8 *)&EndIndex +i);   
            
        Buffer[8] = 0;  
        for(i=0; i<8; i++)
        {
            Buffer[8] += Buffer[i]; 
        }
                    
        f_lseek(&file,0);      
        f_lseek(&fileBak,0);      
        f_write(&file,Buffer,9,&Bw);
        f_write(&fileBak,Buffer,9,&Bw); 
    }
    f_close(&file);
    f_close(&fileBak);  
    IntToAsc(Result,Buffer);

    strcpy((char *)FileName,"1:/WAV/");
    strcat((char *)FileName,(char const *)Buffer);   
    strcat((char *)FileName,".WAV");                         
    strcpy((char *)Str,(char const *)FileName);
    return 1;
}
*/
u8  FetchWAVFileName(u8 *pBuffer)
{
	
	TIME_T  tt;
	u8 *p;
	u8 length;

	//LOG_PR_N("FetchWAVFileName()\r\n");
	RTC_ReadTime(&tt);
	p = pBuffer;
	length = 0;
	
	//��Ŀ¼·��
	*p++ = '1';
	length++;
	*p++ = ':';
	length++;
	*p++ = '/';
	length++;
	*p++ = 'W';
	length++;
	*p++ = 'A';
	length++;
	*p++ = 'V';
	length++;
	*p++ = '/';
	length++;
	
	//�ļ���
	*p++ = tt.year/10+0x30;
	length++;
	*p++ = tt.year%10+0x30;
	length++;
	*p++ = tt.month/10+0x30;
	length++;
	*p++ = tt.month%10+0x30;
	length++;
	*p++ = tt.day/10+0x30;
	length++;
	*p++ = tt.day%10+0x30;
	length++;
	*p++ = tt.hour/10+0x30;
	length++;
	*p++ = tt.hour%10+0x30;
	length++;
	*p++ = tt.min/10+0x30;
	length++;
	*p++ = tt.min%10+0x30;
	length++;
	*p++ = tt.sec/10+0x30;
	length++;
	*p++ = tt.sec%10+0x30;
	length++;
	//��׺
	*p++ = '.';
	length++;
	*p++ = 'w';
	length++;
	*p++ = 'a';
	length++;
	*p++ = 'v';
	length++;
	//��β�ַ�
	*p++ = 0;
	length++;
		
	return length;
}
/*******************************************************
Description:��Hexʱ��ת�ɳ�����
Input: Hex,Year-Month-Day-Hour-Minte-Second
Output:
Return: ������ʱ��ֵ
notye:    
*******************************************************/ 
DWORD GetIntTime (u8 Time[6])
{
    u8 Temp;
    DWORD iDayTime; 
    
    iDayTime = Time[0] + 2000 - 1980; //Year
    iDayTime = iDayTime<<25 ;
    Temp = Time[1];                   //Month
    iDayTime |= (Temp<<21);
    Temp = Time[2];                   //Day
    iDayTime |= (Temp<<16);
    Temp = Time[3];                   //Hour
    iDayTime |= (Temp<<11);
    Temp = Time[4];                   //Min
    iDayTime |= (Temp<<5);
    Temp = Time[5];                   //Sec
    iDayTime |= (Temp>>1);
	return iDayTime;
}

/*******************************************************
Description:�ж��ļ�����ʱ���Ƿ���ϼ���Ҫ��
Input: ��ʼʱ��,����ʱ��,�ļ�����ʱ��
Output:
Return: 1:���ϼ���Ҫ�� 0:������
note:    
*******************************************************/ 
u8 CheckDayTime(u32 iStartTime,u32 iEndTime, u32 fileTime) 
{
    if(iStartTime < fileTime && iEndTime > fileTime)
    {
        return 1;    
    }
    else
    {
        return 0;
    }
}

/*******************************************************
Description:�ж��ļ��¼��Ƿ���ϼ���Ҫ��
Input: �¼�,�ļ���Ϣ
Output:
Return: 1:���ϼ���Ҫ�� 0:������
note:    
*******************************************************/ 
u8 CheckJPEGEvent(u8 Event, FILINFO *fno) 
{
    FIL file;
    u8 Buffer[40]={0};
    u32 Br;
    u8  FileName_1[_MAX_LFN];
    
    strcpy((char *)Buffer,"1:/JPEG/");
    ff_NameForward(FileName_1,fno->fname);
    strcat((char *)Buffer,(const char *)FileName_1);
    if(f_open(&file,ff_NameConver(Buffer),FA_OPEN_EXISTING|FA_READ) == FR_OK) //ֻ��
    {                                          
        f_lseek(&file,file.fsize-sizeof(T_MEDIA_RECORD));
        f_read(&file,Buffer,sizeof(T_MEDIA_RECORD),&Br);
        if(Buffer[7] == Event)
        {
            f_close(&file);
            return 1;    
        }              
    }  
    f_close(&file);      
    return 0;
}
/*******************************************************
Description:�ж��ļ��¼��Ƿ���ϼ���Ҫ��
Input: �¼�,�ļ���Ϣ
Output:
Return: 1:���ϼ���Ҫ�� 0:������
note:    
*******************************************************/ 
u8 CheckWAVEvent(u8 Event, FILINFO *fno) 
{
    FIL file;
    u8 Buffer[40]={0};
    u32 Br;
    u8  FileName_1[_MAX_LFN];
    strcpy((char *)Buffer,"1:/WAV/");
    ff_NameForward(FileName_1,fno->fname);
    strcat((char *)Buffer,(const char *)FileName_1);
    if(f_open(&file,ff_NameConver(Buffer),FA_OPEN_EXISTING|FA_READ) == FR_OK)           //ֻ��
    {                                          
        f_lseek(&file,file.fsize-sizeof(T_MEDIA_RECORD));
        f_read(&file,Buffer,sizeof(T_MEDIA_RECORD),&Br);
        if(Buffer[7] == Event)
        {
            f_close(&file);
            return 1;    
        }              
    }  
    f_close(&file);      
    return 0;
}

/*******************************************************
Description:���ļ����кŻ�þ����ļ���
Input: �ļ����к�����:"0000000N",
Output: FullFileName
Return: NULL
note: filename����С��9�ֽ�
      FullFileName����С��25�ֽ�
*******************************************************/ 
void GetFullJPEGFileName(u8* filename)
{
    u8 Buff[25];
    if(strstr((char const *)filename, "1:") == NULL) 
    {//�������ַ���"1:"
        strcpy((char *)Buff,"1:/JPEG/");
        strcat((char *)Buff,(char const *)filename);
        if(strstr((char const *)filename, ".JPG") == NULL)
        { //û���ҵ�".JPG"
            strcat((char *)Buff,".JPG");
            strcpy((char *)filename,(char const *)Buff);
        }
    }       
}
/*******************************************************
Description:���ļ����кŻ�þ����ļ���
Input: �ļ����к�����:"0000000N",
Output: FullFileName
Return: NULL
note: filename����С��25�ֽ�     
*******************************************************/ 
void GetFullWAVFileName(u8* filename) 
{
    u8 Buff[25];
    if(strstr((char const *)filename, "1:") == NULL) 
    {//�������ַ���"1:"
        strcpy((char *)Buff,"1:/WAV/");
        strcat((char *)Buff,(char const *)filename);
        if(strstr((char const *)filename, ".WAV") == NULL)
        { //û���ҵ�".WAV"
            strcat((char *)Buff,".WAV");
            strcpy((char *)filename,(char const *)Buff);
        }
    }       
}
/*******************************************************
Description:��ȡ�ļ���¼
Input: �����ļ�����,�������·��,�ļ���¼�ռ�
Output:
Return: 1:�ɹ� 0:ʧ��
note: 
*******************************************************/    
u8 GetFileRecord(u8 *filename, T_MEDIA_RECORD *Rec)
{
    FIL file;
    FRESULT Result;
    u32 Br;
    u8 Buf[4];
    if(filename == NULL || Rec == NULL)
        return 0;
        
    if(f_open(&file,ff_NameConver(filename),FA_OPEN_EXISTING|FA_READ) == FR_OK)           //ֻ��
    {                                          
        Result = f_lseek( &file, file.fsize-sizeof(T_MEDIA_RECORD) );
        if(Result == FR_OK)
        {
            Result = f_read( &file,(u8 *)Rec,sizeof(T_MEDIA_RECORD),&Br );
            if(Result == FR_OK)
            {
                MyMemcpy(Buf, (u8 *)Rec, 4 , 1);  //T_MEDIA_RECORD��MediaID�ֶ��Ǵ�˴洢,��Ҫ����
                Rec->MediaID = *(u32 *)Buf;
                f_close(&file);
                return 1;                
            }        
        }            
    }  
    f_close(&file);      
    return 0;    
}

//-----------------------------------------------
//����:s2�����ݿ���s1��,nΪ��������,sΪ����(1:����,0:˳��)
//����:
//���:
//��ע:
//-----------------------------------------------
void *MyMemcpy(void *s1,  const void *s2, u32 n, u8 s)
{       /* copy char s2[n] to s1[n] in any order */
    char *su1 = (char *)s1;
    const char *su2 = (const char *)s2;
    u32 i;
    if(s==0)
    {
        for (; 0 < n; ++su1, ++su2, --n)
          *su1 = *su2;
    }
    else
    {
        for(i=0 ;i<n; i++)
        {
            *(su1+n-1-i) = *(su2+i);
        }	
    }
    return (s1);
}

/*******************************************************
//����:��ʱ��,�¼�����
//���������Event:�¼�����, StartTime[6],EndTime[6]:YY-
//MM-DD-hh-mm-ss(Hex),Str���ص��ļ����к�,����С��9�ֽ�
//���أ��ļ����кų���,0��ʾ�������
//ע��
���Event=0XFF, ��ʾ������ʱ�����
��� StartTime[0],EndTime[0] ��һ��Ϊ0,��ʾ�������¼�����,
����Ҫ����ļ�IDͨ��Str���,��: ��0000000N��,ÿ����һ����
��һ���ļ�����,ֱ������0
*******************************************************/
u8 _SearchJPEG(u8 Event, u8 StartTime[6], u8 EndTime[6], u8 *Str,DIR *pdir)  
{
    /*
    FILINFO fileInfo;
    u32 iStartTime,iEndTime,fileTime;
    FRESULT Result;
    static u8 SearchCnt=0;
    u8  FileName_1[_MAX_LFN];
    
    //�������
    if(Str == NULL)
        return 0;    
    
    while(1) 
    {
        IWDG_ReloadCounter();
        Result = f_readdir(pdir,&fileInfo);    //��Ŀ¼��Ϣ���ļ�״̬�ṹ��
        if(Result != FR_OK || fileInfo.fname[0]==0)break;
        ff_NameForward(FileName_1,fileInfo.fname);
        if(FileName_1[0]=='.')continue  ;
        if(strstr((const char *)FileName_1, "INDEX") != NULL) continue;	//����INDEX�ַ������ļ�����
        SetOnceTimer(SEARCH_FILE_TIMER,50*2);  //2s  
        iStartTime = GetIntTime(StartTime);
        iEndTime   = GetIntTime(EndTime);
        fileTime = (fileInfo.fdate <<16) | fileInfo.ftime; //�õ��ļ�����ʱ��
        if(Event == 0XFF)
        {
            if(StartTime[0] && EndTime[0])
            { //��ʱ�����                
                if(CheckDayTime(iStartTime,iEndTime, fileTime))
                {
                    ff_NameForward(FileName_1,fileInfo.fname);
                    strncpy((char *)Str,(const char *)FileName_1,8);    
                    Str[8] = '\0';
                    if(++SearchCnt < 11)   
                    {
                        return strlen((char const*)Str);
                    }
                    else
                    {
                        SearchCnt=0;
                        break;
                    }
                }
                continue;//��������
            }    
            else
            {
                break;           //������������    
            }           
        }
        else
        {
            if(StartTime[0] && EndTime[0])
            { //��ʱ��\�¼�����
                if( !CheckDayTime(iStartTime,iEndTime, fileTime) )
                {
                    continue;//��������
                }
                else
                {
                    if(CheckJPEGEvent(Event, &fileInfo))
                    {
                        ff_NameForward(FileName_1,fileInfo.fname);
                        strncpy((char *)Str,(const char *)FileName_1,8);    
                        Str[8] = '\0';
                        if(++SearchCnt < 11)   
                        {
                            return strlen((char const*)Str);
                        }
                        else
                        {
                            SearchCnt=0;
                            break;
                        }                                                  
                    }   
                    continue;//��������
                }
                
            }    
            else
            { //���¼�����
                if(CheckJPEGEvent(Event, &fileInfo))
                {
                     ff_NameForward(FileName_1,fileInfo.fname);
                    strncpy((char *)Str,(const char *)FileName_1,8);    
                    Str[8] = '\0';
                    if(++SearchCnt < 11)   
                    {
                        return strlen((char const*)Str);
                    }
                    else
                    {
                        SearchCnt=0;
                        break;
                    }                                            
                }   
                continue;//��������                               
            } 
            
        }
    }
		*/
    return 0;
}            
/*******************************************************
//����:��ʱ��,�¼�����
//���������Event:�¼�����, StartTime[6],EndTime[6]:YY-
//MM-DD-hh-mm-ss(Hex),Str���ص��ļ����к�,����С��9�ֽ�
//���أ��ļ����кų���,0��ʾ�������
//ע��
���Event=0XFF, ��ʾ������ʱ�����
��� StartTime[0],EndTime[0] ��һ��Ϊ0,��ʾ�������¼�����,
����Ҫ����ļ�IDͨ��Str���,��: ��0000000N��,ÿ����һ����
��һ���ļ�����,ֱ������0
*******************************************************/
u8 _SearchWAV(u8 Event, u8 StartTime[6], u8 EndTime[6], u8 *Str,DIR *pdir)  
{
    /*
    FILINFO fileInfo;
    u32 iStartTime,iEndTime,fileTime;
    FRESULT Result;
    static u8 SearchCnt=0;
    u8 FileName_1[_MAX_LFN];
    
    //�������
    if(Str == NULL)
        return 0;    
    
    while(1) 
    {
       IWDG_ReloadCounter();
        Result = f_readdir(pdir,&fileInfo);    //��Ŀ¼��Ϣ���ļ�״̬�ṹ��
        if(Result != FR_OK || fileInfo.fname[0]==0)break;
        ff_NameForward(FileName_1,fileInfo.fname);
        if(FileName_1[0]=='.')continue  ;
        if(strstr((const char *)FileName_1,"INDEX") != NULL) continue;	//����INDEX�ַ������ļ�����
//        SetOnceTimer(SEARCH_FILE_TIMER,50*2);  //2s  
        iStartTime = GetIntTime(StartTime);
        iEndTime   = GetIntTime(EndTime);
        fileTime = (fileInfo.fdate <<16) | fileInfo.ftime; //�õ��ļ�����ʱ��
        if(Event == 0XFF)
        {
            if(StartTime[0] && EndTime[0])
            { //��ʱ�����                
                if(CheckDayTime(iStartTime,iEndTime, fileTime))
                {
                    ff_NameForward(FileName_1,fileInfo.fname);
                    strncpy((char *)Str,(const char *)FileName_1,8);    
                    Str[8] = '\0';
                    if(++SearchCnt < 11)   
                    {
                        return strlen((char const*)Str);
                    }
                    else
                    {
                        SearchCnt=0;
                        break;
                    }
                }
                continue;//��������
            }    
            else
            {
                break;           //������������    
            }           
        }
        else
        {
            if(StartTime[0] && EndTime[0])
            { //��ʱ��\�¼�����
                if( !CheckDayTime(iStartTime,iEndTime, fileTime) )
                {
                    continue;//��������
                }
                else
                {
                    if(CheckWAVEvent(Event, &fileInfo))
                    {
                         ff_NameForward(FileName_1,fileInfo.fname);
                        strncpy((char *)Str,(const char *)FileName_1,8);    
                        Str[8] = '\0';
                        if(++SearchCnt < 11)   
                        {
                            return strlen((char const*)Str);
                        }
                        else
                        {
                            SearchCnt=0;
                            break;
                        }                                                
                    }   
                    continue;//��������
                }
                
            }    
            else
            { //���¼�����
                if(CheckWAVEvent(Event, &fileInfo))
                {
                    ff_NameForward(FileName_1,fileInfo.fname);
                    strncpy((char *)Str,(const char *)FileName_1,8);    
                    Str[8] = '\0';                    
                    if(++SearchCnt < 11)   
                    {
                        return strlen((char const*)Str);
                    }
                    else
                    {
                        SearchCnt=0;
                        break;
                    }                  
                }   
                continue;//��������                               
            } 
            
        }
    }
		*/
    return 0;
} 
/*******************************************************
//����:��ʱ��,�¼�����
//���������Event:�¼�����, StartTime[6],EndTime[6]:YY-
//MM-DD-hh-mm-ss(Hex),Str���ص��ļ����к�,����С��9�ֽ�
//���أ��ļ����кų���,0��ʾ�������
//ע��
���Event=0XFF, ��ʾ������ʱ�����
��� StartTime[0],EndTime[0] ��һ��Ϊ0,��ʾ�������¼�����,
����Ҫ����ļ�IDͨ��Str���,��: ��0000000N��,ÿ����һ����
��һ���ļ�����,ֱ������0
*******************************************************/
u8 SearchJPEG(u8 Event, u8 StartTime[6], u8 EndTime[6], u8 *Str) 
{
	/*
    static u8 Status=0;
    static DIR dir;    
    u8 Result;
    
    
    //�������
    if(Str == NULL)
        return 0;
        
    if( IsOverTime(SEARCH_FILE_TIMER) )
        Status = 0;                           //�����ļ���ʱ,���¼���
        
    if(Status == 0)
    {
        if(f_opendir(&dir, ff_NameConver("1:/JPEG")) != FR_OK)   
            return 0;                        //��Ŀ¼ʧ��
        Status = 1;  
        Result =  _SearchJPEG(Event,StartTime,EndTime,Str,&dir);
        if( !Result )
        {
            Status = 0;       
        }
    }     
    else
    {
        Result =  _SearchJPEG(Event,StartTime,EndTime,Str,&dir);
        if( !Result )
        {
            Status = 0;       
        }       
    } 
		*/
    return 0; 
}
/*******************************************************
//����:��ʱ��,�¼�����
//���������Event:�¼�����, StartTime[6],EndTime[6]:YY-
//MM-DD-hh-mm-ss(Hex),Str���ص��ļ����к�,����С��9�ֽ�
//���أ��ļ����кų���,0��ʾ�������
//ע��
���Event=0XFF, ��ʾ������ʱ�����
��� StartTime[0],EndTime[0] ��һ��Ϊ0,��ʾ�������¼�����,
����Ҫ����ļ�IDͨ��Str���,��: ��0000000N��,ÿ����һ����
��һ���ļ�����,ֱ������0
*******************************************************/
u8 SearchWAV(u8 Event, u8 StartTime[6], u8 EndTime[6], u8 *Str) 
{
	/*
    static u8 Status=0;
    static DIR dir;
    u8 Result;
    
    
    //�������
    if(Str == NULL)
        return 0;
        
    if( IsOverTime(SEARCH_FILE_TIMER) )
        Status = 0;                           //�����ļ���ʱ,���¼���
        
    if(Status == 0)
    {
        if(f_opendir(&dir, ff_NameConver("1:/WAV")) != FR_OK)   
            return 0;                        //��Ŀ¼ʧ��
        Status = 1;  
        Result =  _SearchWAV(Event,StartTime,EndTime,Str,&dir);
        if( !Result )
        {
            Status = 0;       
        }
    }     
    else
    {
        Result =  _SearchWAV(Event,StartTime,EndTime,Str,&dir);
        if( !Result )
        {
            Status = 0;       
        }       
    } 
    return Result; 
		*/
		return 0;
}

/*******************************************************
//����:�ڱ�����JPEG�ļ���,��Ҫ���øú���,Ŀ���ǽ���MediaID
//���ļ����кŵĶ�Ӧ��ϵ,����ͨ��MediaID�����ļ�.
//����:����JPEG ID��
//���������JPEG ID
//�������: NULL
//����:1:�ɹ�  0:ʧ��
*******************************************************/
u8 SaveJPEGMediaID(u32 MediaID)
{
/*
	//LOG_PR_N("SaveJPEGMediaID()\r\n");   
    u8 err=0; 
    FIL file;
    u32 Brw;
    u8 Buffer[15];
    u32 Index;      //���һ���ļ��������� 
  
    do
    {    
        if(f_open(&file,"1:/JPEG/INDEX.TXT",FA_OPEN_ALWAYS|FA_READ|FA_WRITE) == FR_OK)  
        {                                          
            f_read(&file,Buffer,8,&Brw);
            //StartIndex = *(u32 *)&Buffer[0]; 
            Index =  *(u32 *)&Buffer[4] - 1;   
            if(Index == 0)
                Index = MAX_FILES_SUM;
            
        }
        else
        {
            err = 1;
            break;
        } 
        if( f_lseek(&file,512+(Index-1)*sizeof(u32)) != FR_OK)  //����ͷβ������,Index��1��ʼ
        {
            err = 1; 
            break;
        }
        if(f_write(&file,(u8 *)&MediaID,sizeof(u32),&Brw) != FR_OK)    //�ڶ�Ӧ������д��MediaID
        {
            err = 1;
            break;
        }
    }
    while(0);
    f_close(&file);
    if(err)
        return 0;
    else
        return 1; 
*/
   return 1;
	
}
/*******************************************************
//����:�ڱ�����WAV�ļ���,��Ҫ���øú���,Ŀ���ǽ���MediaID
//���ļ����кŵĶ�Ӧ��ϵ,����ͨ��MediaID�����ļ�.
//����:����WAV ID��
//���������WAV ID
//�������: NULL
//����:1:�ɹ�  0:ʧ��
*******************************************************/
u8 SaveWAVMediaID(u32 MediaID) 
{
	//LOG_PR_N("SaveWAVMediaID()\r\n"); 
/*
    u8 err=0; 
    FIL file;
    u32 Brw;
    u8 Buffer[15];
    u32 Index;      //���һ���ļ���������    
  
    do
    {    
        if(f_open(&file,"1:/WAV/INDEX.TXT",FA_OPEN_ALWAYS|FA_READ|FA_WRITE) == FR_OK)  
        {                                          
            f_read(&file,Buffer,8,&Brw);
            //StartIndex = *(u32 *)&Buffer[0]; 
            Index =  *(u32 *)&Buffer[4] - 1;                                       
            if(Index == 0)
                Index = MAX_FILES_SUM;
        }
        else
        {
            err = 1;
            break;
        } 
        if( f_lseek(&file,512+(Index-1)*sizeof(u32)) != FR_OK)  //����ͷβ������,Index��1��ʼ
        {
            err = 1; 
            break;
        }
        if(f_write(&file,(u8 *)&MediaID,sizeof(u32),&Brw) != FR_OK)    //�ڶ�Ӧ������д��MediaID
        {
            err = 1;
            break;
        }
    }
    while(0);
    f_close(&file);
    if(err)
        return 0;
    else
        return 1; 
*/
    return 1;
}
/*******************************************************
//����:ͨ��ý��ID���ֲ��ҷ��ļ����к�
//����������ļ�ָ��,��ʼ����,��������,�����ҵ�ý��ID
//�������:NULL
//����:0:δ�ҵ� ����:�ļ����к�
*******************************************************/
u32 SearchFileSD( FIL *fp,u32 SIndex,u32 EIndex, u32 MeidaID) 
{
    u32 lo = SIndex;
    u32 hi = EIndex;
    u32 mid;   //�۰�ֵ
    u32 ID;    //��������ID
    u32 Temp;
    u32 Brw;
    
    while(lo < hi)
    {
        mid =(hi + lo)/2;       
        Temp = mid % MAX_FILES_SUM;
        f_lseek(fp,512+sizeof(u32)*(Temp-1));              
        if(f_read(fp,(u8 *)&ID,sizeof(u32),&Brw) != FR_OK)
        {
            return 0; //���ļ�ʧ��
        }                                                       
        if(MeidaID < ID)
            hi=mid;
        else if(MeidaID > ID)
            lo=mid+1;
        else        
            return mid % MAX_FILES_SUM;         
    }
    return 0;
}
/*******************************************************
//����:��JPEG ID����ļ���
//���������JPEG ID
//�������: ����·���ļ���
//����:1:�ɹ�  0:ʧ��,����û�и�JPEG ID
*******************************************************/
u8 SearchJPEGWithID(u32 MediaID, u8 FileName[25]) 
{
    u8 err=0; 
    FIL file;
    u32 Brw;
    u8 Buffer[15];
    u32 StartIndex;    //��һ���ļ���������    
    u32 EndIndex;      //���һ���ļ���������    
    u32 FileSD;        //�ļ����к�
  
    do
    {    
        if(f_open(&file,ff_NameConver("1:/JPEG/INDEX.TXT"),FA_OPEN_ALWAYS|FA_READ|FA_WRITE) == FR_OK)  
        {                                              
            if(f_read(&file,Buffer,8,&Brw) == FR_OK)
            {
                StartIndex = *(u32 *)&Buffer[0]; 
                EndIndex =  *(u32 *)&Buffer[4];
            }
            else
            {
                err = 1;
                break;
            }                                                  
        }
        else
        {
            err = 1;
            break;
        } 
        if(StartIndex > EndIndex)
        { //�����ַת�������ַ
            EndIndex += MAX_FILES_SUM;            
        }
        //���ַ���������
        FileSD = SearchFileSD( &file,StartIndex,EndIndex,MediaID);
    }
    while(0);
    f_close(&file);
    if(err || FileSD==0)
        return 0;
    else
    {
        IntToAsc(FileSD,Buffer); //��������ת���ַ���
        strcpy((char *)FileName,"1:/JPEG/");
        strcat((char *)FileName,(char const *)Buffer);   
        strcat((char *)FileName,".JPG");                     
        return 1; 
    }
}
/*******************************************************
//����:��WAV ID����ļ���
//���������WAV ID
//�������: ����·���ļ���
//����:1:�ɹ�  0:ʧ��,����û�и�WAV ID
*******************************************************/
u8 SearchWAVWithID(u32 MediaID, u8 FileName[25]) 
{
    u8 err=0; 
    FIL file;
    u32 Brw;
    u8 Buffer[15];
    u32 StartIndex;    //��һ���ļ���������    
    u32 EndIndex;      //���һ���ļ���������    
    u32 FileSD;        //�ļ����к�
  
    do
    {    
        if(f_open(&file,ff_NameConver("1:/WAV/INDEX.TXT"),FA_OPEN_ALWAYS|FA_READ|FA_WRITE) == FR_OK)  
        {                                              
            if(f_read(&file,Buffer,8,&Brw) == FR_OK)
            {
                StartIndex = *(u32 *)&Buffer[0]; 
                EndIndex =  *(u32 *)&Buffer[4];
            }
            else
            {
                err = 1;
                break;
            }                                             
        }
        else
        {
            err = 1;
            break;
        } 
        if(StartIndex > EndIndex)
        { //�����ַת�������ַ
            EndIndex += MAX_FILES_SUM;   
        }
        //���ַ���������
        FileSD = SearchFileSD( &file,StartIndex,EndIndex,MediaID);
    }
    while(0);
    f_close(&file);
    if(err || FileSD==0)
        return 0;
    else
    {
        IntToAsc(FileSD,Buffer); //��������ת���ַ���
        strcpy((char *)FileName,"1:/WAV/");
        strcat((char *)FileName,(char const *)Buffer);   
        strcat((char *)FileName,".WAV");                     
        return 1; 
    }
}

