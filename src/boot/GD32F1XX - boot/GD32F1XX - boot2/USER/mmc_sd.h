#ifndef _MMC_SD_H_
#define _MMC_SD_H_		 

#include "stm32f10x.h"
#include "ff.h"
 						    	 
// SD�����Ͷ���  
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06	   
// SD��ָ���  	   
#define CMD0    0       //����λ
#define CMD1    1
#define CMD8    8       //����8 ��SEND_IF_COND
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����sector
#define CMD18   18      //����18����Multi sector
#define CMD23   23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define CMD41   41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00
//����д���Ӧ������
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF
//SD����Ӧ�����
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF
 							   						 	 
//�ⲿ��Ӧ���ݾ�����������޸�!
//Mini STM32ʹ�õ���PA3��ΪSD����CS��.
#define	SD_CS   PAout(3) //SD��Ƭѡ����					    	  

#define MAX_FILES_SUM        99999999   //����ļ���
//��ý���ļ���¼
typedef struct
{
    u32 MediaID;       //��ý��ID��DWORD��
    u8  MediaType;     //��ý�����ͣ�BYTE��
    u8  MediaTypeCode; //��ý���ʽ���루BYTE��
    u8  MediaCh;       //ͨ��ID ��BYTE��
    u8  EventType;     //�¼����ͣ�BYTE��
    u8  Position[28];  //λ�û�����Ϣ��28BYTES��    
}
T_MEDIA_RECORD;


extern u8  SD_Type;//SD��������
extern FATFS fs;   
//���������� 
u8 SD_WaitReady(void);							//�ȴ�SD��׼��
u8 SD_GetResponse(u8 Response);					//�����Ӧ
u8 SD_Initialize(void);							//��ʼ��
u8 SD_ReadDisk(u8*buf,u32 sector,u8 cnt);		//����
u8 SD_WriteDisk(u8*buf,u32 sector,u8 cnt);		//д��
u32 SD_GetSectorCount(void);   					//��������
u8 SD_GetCID(u8 *cid_data);                     //��SD��CID
u8 SD_GetCSD(u8 *csd_data);                     //��SD��CSD
DWORD GetRestKByte(void) ;                      //���ʣ��ռ�(KB)
void InitSDFile(void) ;                         //SD�ļ���ʼ��

u8 FetchJPEGFileName(u8 *Str) ;                 //���ͼ���ļ���,Str�ռ䲻��С��25
u8 FetchWAVFileName(u8 *Str) ;                  //���¼���ļ���,Str�ռ䲻��С��25
u8 SearchJPEG(u8 Event, u8 StartTime[6], u8 EndTime[6], u8 *Str);  //��ʱ��,�¼�����
u8 SearchWAV(u8 Event, u8 StartTime[6], u8 EndTime[6], u8 *Str);   //��ʱ��,�¼����� 
u8 GetFileRecord(u8 *filename, T_MEDIA_RECORD *Rec) ;  //��ȡ�ļ���¼
void GetFullWAVFileName(u8* filename) ; //���ļ����кŻ�þ����ļ���
void GetFullJPEGFileName(u8* filename) ; //���ļ����кŻ�þ����ļ���
u8 SaveJPEGMediaID(u32 MediaID) ; //����JPEG ID��
u8 SaveWAVMediaID(u32 MediaID)  ; //����WAV ID��
u8 SearchJPEGWithID(u32 MediaID, u8 FileName[25]) ; //��JPEG ID����ļ���
u8 SearchWAVWithID(u32 MediaID, u8 FileName[25]) ; //��WAV ID����ļ���
#endif

//���������� 
/*
u8 SD_WaitReady(void);                          //�ȴ�SD������
u8 SD_SendCommand(u8 cmd, u32 arg, u8 crc);     //SD������һ������
u8 SD_SendCommand_NoDeassert(u8 cmd, u32 arg, u8 crc);
u8 SD_Init(void);                               //SD����ʼ��
u8 SD_Idle_Sta(void);                           //����SD��������ģʽ

u8 SD_ReceiveData(u8 *data, u16 len, u8 release);//SD��������
u8 SD_GetCID(u8 *cid_data);                     //��SD��CID
u8 SD_GetCSD(u8 *csd_data);                     //��SD��CSD
u32 SD_GetCapacity(void);                       //ȡSD������
//USB ������ SD����������
u8 MSD_WriteBuffer(u8* pBuffer, u32 WriteAddr, u32 NumByteToWrite);
u8 MSD_ReadBuffer(u8* pBuffer, u32 ReadAddr, u32 NumByteToRead);

u8 SD_ReadSingleBlock(u32 sector, u8 *buffer);  //��һ��sector
u8 SD_WriteSingleBlock(u32 sector, const u8 *buffer); 		//дһ��sector
u8 SD_ReadMultiBlock(u32 sector, u8 *buffer, u8 count); 	//�����sector
u8 SD_WriteMultiBlock(u32 sector, const u8 *data, u8 count);//д���sector
u8 SD_Read_Bytes(unsigned long address,unsigned char *buf,unsigned int offset,unsigned int bytes);//��ȡһbyte
#endif
*/




