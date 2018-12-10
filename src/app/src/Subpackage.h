#ifndef	__SUBPACKAGE_H
#define	__SUBPACKAGE_H

//*************�ļ�����***************
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "ff.h"
//*************�궨��****************
#define SUBPACKET_LIST_BUFFER_SIZE    1000

#define MEDIT_TYPE_PIC   0x10
#define MEDIT_TYPE_SOUND 0x11


#define SUBPACKET_ID_LIST_SIZE_MEDIA  100 
typedef struct tagSTT_SUBPACKETTRANSTER_CTRL{
u8  FileName[100];
FIL FileHandler ; 
u8  MediaType ;
u16 PacketCntTotal ;
u16 PacketCntCursor ;

u16 FirstSN ; //ԭʼ��Ϣ��ˮ��
u8  PacketIDCntTotal ;//�ش��ܰ���
u8  PacketIDCntCursor ;//�ش�ID�б��α� 
u8  PacketIDArray[ SUBPACKET_ID_LIST_SIZE_MEDIA ];//�ش���ID�б�����

u8  DataBuf[512+36];
u16 DataBufLength;
u8  ResendCnt ; 

}STT_SUBPACKETTRANSTER_CTRL;




extern STT_SUBPACKETTRANSTER_CTRL g_sttSubPacketTransterCtrl ;
/*********************************************************************
//��������	:Subpackage_UploadTimeTask(void)
//����		:�ְ�������ʱ���񣨶�Ӧƽ̨����0x8003��
*********************************************************************/
FunctionalState Subpackage_UploadTimeTask(void);



/*********************************************************************
//�������� : Subpackage_Media_File_Name_Set
//�������� : ����ý��ְ����������񣬿�����Ҫ�������ļ����� ����ļ�����������ĳ��ȣ�����0�����򷵻�ʵ�ʵ����õ��ַ������ȣ�����β�ֽ�'\0')
//����ʾ�� :  Subpackage_Media_File_Name_Set( "1:/JPG1/15155028.jpg" ); 
*********************************************************************/
u8 Subpackage_Media_File_Name_Set(  const char * pInMediaFileName  );

/*********************************************************************
//�������� : Subpackage_Task_Media(void)
//�������� : ��ý��ְ��������񣬺����ļ���ȡ�����ر��ļ���������ݣ���ƽ̨�ϴ����ݣ��Լ�������һ�ְ�ID 
*********************************************************************/
void Subpackage_Task_Media( void );



#endif