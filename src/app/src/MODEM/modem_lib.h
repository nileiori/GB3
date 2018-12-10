/**
  ******************************************************************************
  * @file    modem_lib.h 
  * @author  miaoyahan
  * @version V1.0.0
  * @date    2014-10-13
  * @brief   
  ******************************************************************************
  */

/*
********************************************************************************
*                               MODULE
*
* Note(s) : һ���õ��ĳ����㷨
********************************************************************************
*/
#ifndef  MODEM_LIB
#define  MODEM_LIB

/*
********************************************************************************
*                               INCLUDES
********************************************************************************
*/
#include "my_typedef.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
/*
********************************************************************************
*                            DEFINES VALUE
********************************************************************************
*/


/*
********************************************************************************
*                               DEFINES
********************************************************************************
*/
// �û���Ϣ���뷽ʽ
#define GSM_7BIT		0
#define GSM_ASCII		1
#define GSM_8BIT		4
#define GSM_UCS2		8
/*
********************************************************************************
*                                 ENUM
********************************************************************************
*/
typedef enum 
{
    INS_ALL=0,//������������
    INS_HALF, //����������
    
}MODNOD_TYPE;//��������


/*
********************************************************************************
*                                STRUCT
********************************************************************************
*/

//////////////////////////////////////////////////////////////////////����ṹ��
typedef struct  
{
    u16 len;   //���ݳ���
}MODLIB_BUF;//����ṹ��

//////////////////////////////////////////////////////////////////////AT����ڵ�
typedef struct  
{ 
    u8  max;    //���ڵ���
    u16 num;    //�ڵ��� 
    u8  head;   //����ͷ    
    u8  tail;   //����β
    u8  lock;   //����   
    u16  pLen;  //���ݳ���   
    u16  pSize; //���ݻ����С  
    u8  *pBase; //���ݻ������ַ
    u8  *pHead; //���ݻ���ͷ   
    u8  *pTail; //���ݻ���β      
    MODLIB_BUF *pNode;//�ڵ㻺���ַ
}MODLIB_NOD;//AT����ڵ�




typedef struct
{
  uint32_t head;
  uint32_t tail;
  uint32_t size;
  uint32_t len;
  uint8_t * baseAddr;
  union
  {
    uint8_t lock;
    uint8_t set:1;
  }spin;
}fifo_TypeDef;//���нṹ��


// ����Ϣ�����ṹ������/���빲��
// ���У��ַ�����'\0'��β
typedef struct {
    u8 SCA[16];            // ����Ϣ�������ĺ���(SMSC��ַ)
    u8 TPA[16];            // Ŀ������ظ�����(TP-DA��TP-RA)
    u8 TP_PID;             // �û���ϢЭ���ʶ(TP-PID)
    u8 TP_DCS;             // �û���Ϣ���뷽ʽ(TP-DCS)
    u8 TP_FO;			   //
    u8 TP_SCTS[16];        // ����ʱ����ַ���(TP_SCTS), ����ʱ�õ�
    u8 TP_UD[160];         // ԭʼ�û���Ϣ(����ǰ�������TP-UD)
    u8 PDU_TP;             // PDU��������
    u8 PDU_LN;             // PDU���볤��
    u8 udMultiFlg:1;	   // ��ֱ�ר
    u8 udMultiCnt:7;	   // �ְ���
    u8 udMultiIndex;	   // �������
    u8 udLen;			   // TP_UD����
    short index;		   // ����Ϣ��ţ��ڶ�ȡʱ�õ�
}SM_PARAM;


/*
********************************************************************************
*                          DEFINES FUNCTIONS
********************************************************************************
*/


/*
********************************************************************************
*                               FUNCTIONS
********************************************************************************
*/

void ModLib_NodInit(MODLIB_NOD *node,MODLIB_BUF *BufNode, u8 NodeMax,
                                                 u8 *BufData,u16 DataSize);
u8   ModLib_NodIns(MODLIB_NOD *node, MODNOD_TYPE type, u8 *data, u16 len);
u16  ModLib_NodRet(MODLIB_NOD *node, u8 *data);
u16  ModLib_NodRetEx(MODLIB_NOD *node, u8 *data, u16 len);
u8   ModLib_NodNum(MODLIB_NOD *node);
u16  ModLib_NodLen(MODLIB_NOD *node);
void ModLib_NodRst(MODLIB_NOD *node);

int ModLib_ByteStr(u8* pDst, const u8* pSrc, int nSrcLength);
int ModLib_ByteStrEx(u8* pDst, const u8* pSrc, int nSrcLength);
int ModLib_StrByte(const u8* pSrc, u8* pDst, int nSrcLength);
int ModLib_ByteExc(const u8* pSrc, u8* pDst, int nSrcLength);
int ModLib_PduEncode( SM_PARAM* pSrc, u8* pDst);
int ModLib_PduDecode(const u8* pSrc, SM_PARAM*   pDst);
int ModLib_7BitEncode(const char* pSrc, unsigned char* pDst, int nSrcLength);
u16 ModLib_7BitDecode(const u8* pSrc, u8* pDst, u16 nSrcLength);


u16  ModLib_AtCmp(u8 *pdst, u8 *psrc);
u16  ModLib_AtCmy(u8 *pdst, u8 *psrc);
u16  ModLib_AtLen(u8 *psrc);
u16  ModLib_AtAddr(u8 *psrc, char cmpchar);
u16  ModLib_AtCmpEx(u8 *pdst, u8 *psrc, u16 dstlen);



u8   ModLib_ChkIp(u8 *pData);

/*
********************************************************************************
*                               MODULE END
********************************************************************************
*/

#endif


