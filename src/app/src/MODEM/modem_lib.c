/**
  ******************************************************************************
  * @file    modem_lib.c 
  * @author  miaoyahan
  * @version V1.0.0
  * @date    2014-10-13
  * @brief   
  ******************************************************************************
  */
/*
********************************************************************************
*                               INCLUDES
********************************************************************************
*/
#ifdef EYE_MODEM
#include "include.h"
#else
#include  <app_cfg.h>
#include  <includes.h>
#endif

/*
********************************************************************************
*                             LOCAL DEFINES
********************************************************************************
*/
#define min(x,y)  ((x < y)? x : y)
#define max(x,y)  ((x > y)? x : y)

/*
********************************************************************************
*                         LOCAL DATA TYPES
********************************************************************************
*/



/*
********************************************************************************
*                          LOCAL VARIABLES
********************************************************************************
*/

/*
********************************************************************************
*                          GLOBAL VARIABLES
********************************************************************************
*/


/*
********************************************************************************
*                           FUNCTIONS EXTERN
********************************************************************************
*/


/*
********************************************************************************
*                              FUNCTIONS
********************************************************************************
*/

/**
  * @brief  ��ʼ���ڵ����ݽṹ
  * @param  *node     : �ڵ����ݽṹ
  * @param  *BufNode  : ָ��ڵ㻺���׵�ַ
  * @param   NodeMax  : �ڵ㻺���С
  * @param  *BufData  : ָ��ڵ�洢�����׵�ַ
  * @param   DataSize : �ڵ����ݻ����С
  * @retval Node
  */
void ModLib_NodInit(MODLIB_NOD *node,MODLIB_BUF *BufNode, u8 NodeMax,
                                                    u8 *BufData,u16 DataSize)
{
    memset((MODLIB_NOD*)node,0,sizeof(MODLIB_NOD));//���ٽṹ��
    memset((MODLIB_BUF*)BufNode,0,NodeMax*sizeof(MODLIB_BUF));
    node->pNode = BufNode;//�ڵ㻺���ַ
    node->max   = NodeMax;//���ڵ����   
    node->pBase = BufData;//���ݻ����ַ    
    node->pSize = DataSize;//���ݻ����С
    node->pHead = node->pTail = BufData;
}
/**
  * @brief  ָ���������ݲ���ڵ�ṹ
  * @param  *node : �ڵ����ݽṹ
  * @param   type : INS_ALL->�ڵ���  INS_HALF->���������ݲ����
  * @param  *data : ָ��������ݵ��׵�ַ
  * @param   len  : �������ݵĴ�С
  * @retval �ɹ�����->1  ʧ�ܷ���->0
  */
u8  ModLib_NodIns(MODLIB_NOD *node, MODNOD_TYPE type, u8 *data, u16 len)
{
    MODLIB_BUF *pNode;
    u32 addrmax;
    u32 addrdata;
    
    if((node == NULL)||(node->lock))
    {
        return 0;//�Ƿ�ָ������
    }
    if((node->num+1) >= node->max)
    {
        return 0;//�ڵ㻺������
    }
    if(len == 0)
    {
        return 0;//��Ч���ݳ���
    }
    pNode = node->pNode + node->tail;//�ڵ��ַ

    if((node->pLen+len) > node->pSize)
    {
        if(pNode->len)//ǿ�Ʒ��
        {
            node->tail += 1;//�ڵ�β��1
            node->tail %= node->max;
            node->num  += 1;//�ڵ������1
        }
        return 0;//���ݻ�������        
    }
    pNode->len += len;//���ݳ���
    node->pLen += len;//�ۼƳ���
    
    addrmax  = (u32)node->pBase + node->pSize;
    addrdata = (u32)node->pTail+ len;
    if((addrdata/addrmax)&&(addrdata%addrmax))
    {
        len -= (addrdata%addrmax);
        memcpy(node->pTail,data,len);
        data += len;
        len = addrdata%addrmax;
        node->pTail = node->pBase;
    }
    memcpy(node->pTail,data,len);//��������
    node->pTail+= len;//�ƶ�ָ��
    
    if(type==0)//���
    {
        node->tail += 1;//�ڵ�β��1
        node->tail %= node->max;
        node->num  += 1;//�ڵ������1
    }
    return 1;
}
/**
  * @brief  �ӽڵ�������ȡ����ǰ�ڵ��ڵ�ȫ������
  * @param  *node : �ڵ����ݽṹ
  * @param  *data : ָ��ڵ��ڵ�����
  * @retval �������ݳ���
  */
u16 ModLib_NodRet(MODLIB_NOD *node, u8 *data)
{
    MODLIB_BUF *pNode;
    u32 addrmax;
    u32 addrdata;
    u16 len;

    if((node == NULL)||(data == NULL)||(node->lock))
    {
        return 0;//�Ƿ�ָ������
    }
    if(node->num == 0)
    {
        return 0;//�޽ڵ�
    }

    pNode = node->pNode + node->head;//�ڵ��ַ
    len   = pNode->len;

    addrmax  = (u32)node->pBase + node->pSize;
    addrdata = (u32)node->pHead + len;
    if((addrdata/addrmax)&&(addrdata%addrmax))
    {
        len -= (addrdata%addrmax);
        memcpy(data,node->pHead,len);
        memset(node->pHead,0,len);   
        data += len;
        len = addrdata%addrmax;
        node->pHead = node->pBase;
    }
    memcpy(data,node->pHead,len);//��������
    memset(node->pHead,0,len);   //�������
    node->pHead += len;          //�ƶ�ָ��

    node->head += 1;         //�ڵ�ͷ��1
    node->head %= node->max;

    node->num  -= 1;         //�ڵ������1
    node->pLen = max(node->pLen, pNode->len);
    node->pLen -= pNode->len;//�ۼƳ���

    len = pNode->len;//�ݴ�
    pNode->len = 0;  //���ٳ���
    
    return len;

}
/**
  * @brief  �ӽڵ���ȡ��ָ����������
  * @param  *node : �ڵ����ݽṹ
  * @param  *data : ָ��ڵ��ڵ�����
  * @param  len   : Ҫȡ�������ݳ���
  * @retval ����ʵ��ȡ������
  */
u16 ModLib_NodRetEx(MODLIB_NOD *node, u8 *data, u16 len)
{
    MODLIB_BUF *pNode;
    u32 addrmax;
    u32 addrdata;

    if((node == NULL)||(data == NULL)||(node->lock))
    {
        return 0;//�Ƿ�ָ������
    }
    if(node->num == 0)
    {
        return 0;//�޽ڵ�
    }
    if(len == 0)
    {
        return 0;//��Ч���ݳ���
    }
    pNode = node->pNode + node->head;//�ڵ��ַ
    len = min(len,(u16)pNode->len); //��ȡʣ�೤��

    addrmax  = (u32)node->pBase + node->pSize;
    addrdata = (u32)node->pHead + len;
    if((addrdata/addrmax)&&(addrdata%addrmax))
    {
        len -= (addrdata%addrmax);
        memcpy(data,node->pHead,len);
        data += len;
        len = addrdata%addrmax;
        node->pHead = node->pBase;
    }
    memcpy(data,node->pHead,len);//��������
    node->pHead += len;          //�ƶ�ָ��
    pNode->len  -= len;          //���ݳ��ȵݼ�
    node->pLen   = max(node->pLen, len);
    node->pLen  -= len;          //�ۼƳ��ȵݼ�
    
    if(pNode->len == 0)
    {
        node->head  += 1;         //�ڵ�ͷ��1
        node->head  %= node->max;
        node->num   -= 1;         //�ڵ������1  
    }
    return len;

}


/**
  * @brief  ��ǰ�ڵ����
  * @param  *node : �ڵ����ݽṹ
  * @retval �������ݳ���
  */
u8 ModLib_NodNum(MODLIB_NOD *node)
{
    return node->num;
}
/**
  * @brief  ��ǰ�ڵ��е����ݳ���
  * @param  *node : �ڵ����ݽṹ
  * @retval �������ݳ���
  */
u16 ModLib_NodLen(MODLIB_NOD *node)
{
    MODLIB_BUF *pNode;
    
    pNode = node->pNode + node->tail;//�ڵ��ַ
    
    return pNode->len;
}
/**
  * @brief  ��λ�ڵ�
  * @param  *node : �ڵ����ݽṹ
  * @retval �������ݳ���
  */
void ModLib_NodRst(MODLIB_NOD *node)
{
    node->lock = 1;//����
    memset((MODLIB_BUF*)node->pNode,0,node->max*sizeof(MODLIB_BUF));
    node->num  = 0;//����ڵ����
    node->pLen = 0;//�������ݳ���
    node->head = node->tail  = 0;//����ڵ�ͷβ
    node->pHead= node->pTail = node->pBase;;//���ݵ�ַ�ع�
    node->lock = 0;//�ͷ�
}
/**
  * @brief  �Ƚ����������Ƿ�һ��
  * @param  *pdst: ָ��Ŀ�����ݵ�ַ
  * @param  *psrc: ָ��Դ���ݵ�ַ
  * @retval ���ضԱȲ�ֵ 0��ʾ���
  */
u16  ModLib_AtCmp(u8 *pdst, u8 *psrc)
{
    u16  len=0;
    
    len = strlen((char*)psrc);

    len = strncmp((char *)pdst,(char *)psrc,len);
    
    return len;
}
/**
  * @brief  �ӻ���ĩ��ַ����ָ������
  * @param  *pdst: ָ��Ŀ�����ݵ�ַ
  * @param  *psrc: ָ��Դ���ݵ�ַ
  * @param  dstlen: Ŀ�껺��ĳ��� 
  * @param  *prelen: �ش������׵�ַ��\r\n�ĳ���
  * @retval ������������� 0��ʾʧ��
  */
u16  ModLib_AtCmpEx(u8 *pdst, u8 *psrc, u16 dstlen)
{
    u16 len;
    u16 i;

    len = strlen((char*)psrc);

    for(i=dstlen; i!=0; i--)
    {
        if(strncmp((char *)(pdst+i),(char *)psrc,len) == 0)
        {
            break;
        }
    }
    return i;
}

/**
  * @brief  ���������ָ������
  * @param  *pdst: Ŀ�껺��
  * @param  *psrc: Դ����
  * @retval ���ظ��Ƶĳ���
  */
u16  ModLib_AtCmy(u8 *pdst, u8 *psrc)
{
    u16  len=0;

    len = strlen((char*)psrc);

    memcpy(pdst,psrc,len);
    *(pdst+len)=0;
    
    return len;
}
/**
  * @brief  ���㻺��������ĳ��� ��0x0d ���� 0x0a ���� 0x0 ������
  * @param  *psrc: Դ����
  * @retval ���������
  */
u16  ModLib_AtLen(u8 *psrc)
{
    u16 i;
    u8  tmp;

    for(i=0;;i++)
    {
        tmp = *psrc++;
        if((tmp == 0x0d)||(tmp == 0x0a)||(tmp == 0x0))
        {
            break;
        }
    }
    return i;
}

/**
  * @brief  �ڻ����в���ָ���ַ�����������
  * @param  *psrc: Դ����
  * @param  *pchar:Ҫ���ҵ��ַ� 
  * @retval ���ص�ǰ�ַ��ڻ����е�����
  */
u16  ModLib_AtAddr(u8 *psrc, char cmpchar)
{
    u16 i;
    u16 len;
    
    len = strlen((char*)psrc);

    for(i=0;i<len;i++)
    {
        if(*(psrc+i) == cmpchar)
        {
            break;
        }
    }
    return i;
}
/*
  * @brief  ���IP���ͺͺϷ���
  * @param  ָ��IP��ַ
  * @retval 0: IP
  *         1: ����
  *         2: �Ƿ�
  */
u8   ModLib_ChkIp(u8 *pData)
{
    u8 *p = pData;
    u8 i;
    u8 len;

    len = strlen((char*)p);
    
    for(i=0;i < len;i++)
    {
        if((*(p+i) > 0x39)||(*(p+i) < 0x30))
        {
            if(*(p+i) != '.')
            {
                break;
            }
        }
    }
    if(len == i)
    {
        return 0;//IP��ַ����
    }
    return 1;//��������
}

/********************************************************************
* ���� :  ModLib_StrByte
* ���� : �ɴ�ӡ�ַ���ת��Ϊ�ֽ�����
* ���� :  nSrcLength - Դ�ַ�������
                  pSrc - Դ�ַ���ָ��
* ��� :  pDst - Ŀ���ַ���ָ��
* ���� : Ŀ���ַ�������
* ȫ�ֱ���:
* ���ú���:
* �ж���Դ:  (û��,��д)
* ��ע: "C832" --> {0xC8, 0x32}
********************************************************************/
int ModLib_StrByte(const u8* pSrc, u8* pDst, int nSrcLength)
{
	int i;	
	if ((pSrc == NULL)||(pDst == NULL))
		return 0;

	for (i=0; i<nSrcLength; i += 2) {
        if ((*pSrc >= '0')&&(*pSrc <= '9'))// �����4λ
            *pDst = (*pSrc - '0') << 4;
        else
            *pDst = (*pSrc - 'A' + 10) << 4;
		
        pSrc++;
        if ((*pSrc>='0') && (*pSrc<='9')) // �����4λ
            *pDst |= *pSrc - '0';
        else
            *pDst |= *pSrc - 'A' + 10;
		
        pSrc++;
        pDst++;
    }

    return (nSrcLength / 2);
}

/********************************************************************
* ���� :  ModLib_ByteStr
* ���� : �ֽ�����ת��Ϊ�ɴ�ӡ�ַ���
* ���� :  nSrcLength - Դ�ַ�������
                  pSrc - Դ�ַ���ָ��
* ��� :  pDst - Ŀ���ַ���ָ��
* ���� : Ŀ���ַ�������
* ȫ�ֱ���:
* ���ú���:
* �ж���Դ:  (û��,��д)
* ��ע: {0xC8, 0x32} --> "C832"
********************************************************************/
int ModLib_ByteStr(u8* pDst, const u8* pSrc, int nSrcLength)
{
    const u8 tab[]="0123456789ABCDEF";  
	int i;

	if ((pSrc == NULL)||(pDst == NULL))
		return 0;

    for (i=0; i<nSrcLength; i++) {
        *pDst++ = tab[*pSrc >> 4];      // �����4λ
        *pDst++ = tab[*pSrc & 0x0f];    // �����4λ
        pSrc++;
    }

    *pDst = '\0';
    return (nSrcLength * 2);
}

/********************************************************************
* ���� :  ModLib_ByteStrEx
* ���� : �ֽ�����ת��Ϊ�ɴ�ӡ�ַ���,������ո�
* ���� :  nSrcLength - Դ�ַ�������
                  pSrc - Դ�ַ���ָ��
* ��� :  pDst - Ŀ���ַ���ָ��
* ���� : Ŀ���ַ�������
* ȫ�ֱ���:
* ���ú���:
* �ж���Դ:  (û��,��д)
* ��ע: {0xC8, 0x32} --> "C8 32"
********************************************************************/
int ModLib_ByteStrEx(u8* pDst, const u8* pSrc, int nSrcLength)
{
    const u8 tab[]="0123456789ABCDEF";  
	int i;

	if ((pSrc == NULL)||(pDst == NULL))
		return 0;

    for (i=0; i<nSrcLength; i++) {
        *pDst++ = tab[*pSrc >> 4];      // �����4λ
        *pDst++ = tab[*pSrc & 0x0f];    // �����4λ
        *pDst++ = ' ';                  //����ո�
        pSrc++;
    }

    *pDst = '\0';
    return (nSrcLength * 3);
}

/********************************************************************
* ���� :  ModLib_ByteExc
* ���� :  �����ߵ����ַ���ת��Ϊ����˳����ַ���
* ���� :  nSrcLength - Դ�ַ�������
                  pSrc - Դ�ַ���ָ��
* ��� :  pDst - Ŀ���ַ���ָ��
* ���� : Ŀ���ַ�������
* ȫ�ֱ���:
* ���ú���:
* �ж���Դ:  (û��,��д)
* ��ע: "683158812764F8" --> "8613851872468" 
********************************************************************/
int ModLib_ByteExc(const u8* pSrc, u8* pDst, int nSrcLength)
{
    int nDstLength, i;								
    u8 ch;									

	if ((pSrc == NULL)||(pDst == NULL))
		return 0;
	
    nDstLength = nSrcLength;

    for (i=0; i<nSrcLength; i+=2) { // �����ߵ�
        ch = *pSrc++;							
        *pDst++ = *pSrc++;						
        *pDst++ = ch;						
    }

    if (*(pDst - 1) == 'F') {
        pDst--;
        nDstLength--;				// Ŀ���ַ������ȼ�1
    }

    *pDst = '\0';

    return nDstLength;
}

/********************************************************************
* ���� : ModLib_PduEncode
* ���� :PDU���룬���ڱ��ơ����Ͷ���Ϣ
* ���� : pSrc - ԴPDU����ָ��
* ��� : pDst - Ŀ��PDU��ָ��
* ���� : Ŀ��PDU������
* ȫ�ֱ���:
* ���ú���:
* �ж���Դ:  (û��,��д)
* ��ע:  ֻ��8 bit ����
********************************************************************/
int ModLib_PduEncode( SM_PARAM* pSrc, u8* pDst)
{
	int nLength;// �ڲ��õĴ�����
	int nDstLength;				
	u8  ucbuf[20]={0};									
	u8  *ptr=NULL;
    u8  format_sca[25]={0};//��ʽ����Ķ������ĺ���
    u8  format_fo_mr[5]={0};       
    u8  format_tpa[25]={0};//��ʽ����ķ��ͺ���
    u8  format_data_type[10]={0};//��ʽ����Ķ�����������
    u8  format_data[300]={0};//��ʽ����Ķ�������

    //////////////////////////////////////////////////////////��ʽ���������ĺ���
	ptr = (u8 *)pSrc->SCA;
    nLength = strlen((char *)ptr);
    nLength = ModLib_ByteExc(ptr,format_sca,nLength);  
    ucbuf[0] = (nLength/2)+1;
	ucbuf[1] = 0x91;
    ModLib_ByteStr(format_sca, ucbuf, 2); 
    nLength = strlen((char *)ptr);
    nLength = ModLib_ByteExc(ptr,format_sca+4,nLength); 
    
    ///////////////////////////////////////////////////////////FO and MR 
    memcpy(format_fo_mr,"1100",4);
    pSrc->PDU_LN = 4;
    
    ////////////////////////////////////////////////////////////��ʽ�������ź���
	ptr = (u8 *)pSrc->TPA;
    nLength = strlen((char *)ptr);
    ucbuf[0] = nLength;
	ucbuf[1] = 0xa1;
    ModLib_ByteStr(format_tpa, ucbuf, 2);  
	nLength = ModLib_ByteExc(ptr, format_tpa+4, nLength);	
    pSrc->PDU_LN += (nLength + 4);
   
    //////////////////////////////////////////////////////////��ʽ��������������
	ucbuf[0] = 0x00;    
	ucbuf[1] = GSM_8BIT;
	ucbuf[2] = 167;
    nLength = ModLib_ByteStr(format_data_type, ucbuf, 3);	
    pSrc->PDU_LN += nLength;
   
    ////////////////////////////////////////////////////////////��ʽ������������
    ptr = (u8 *)pSrc->TP_UD;
    nLength = pSrc->udLen;
    ucbuf[0] = nLength;
    ModLib_ByteStr(format_data, ucbuf, 1);
    nLength = ModLib_ByteStr(format_data+2, ptr, nLength);
    pSrc->PDU_LN += (nLength + 2);
    
    pSrc->PDU_LN /= 2;
    
    ////////////////////////////////////////////////////////////////���ݴ����ⲿ
    ptr = format_sca;//���ĺ���
    nLength = strlen((char *)ptr);
    memcpy(pDst,ptr,nLength);
    nDstLength = nLength;    

    ptr = format_fo_mr;//fo mr
    nLength = strlen((char *)ptr);
    memcpy(pDst+nDstLength,ptr,nLength);  
    nDstLength = nDstLength + nLength;

    ptr = format_tpa;//���ͺ���
    nLength = strlen((char *)ptr);
    memcpy(pDst+nDstLength,ptr,nLength);  
    nDstLength = nDstLength + nLength;  

    ptr = format_data_type;//��������
    nLength = strlen((char *)ptr);
    memcpy(pDst+nDstLength,ptr,nLength);
    nDstLength = nDstLength + nLength;

    ptr = format_data;//��������
    nLength = strlen((char *)ptr);
    memcpy(pDst+nDstLength,ptr,nLength);
    nDstLength = nDstLength + nLength;
    
	return nDstLength;
}

/********************************************************************
* ���� : ModLib_PduDecode
* ���� :  PDU���룬���ڽ��ա��Ķ�����Ϣ
* ���� : pSrc - ԴPDU����ָ��
* ��� : pDst - Ŀ��PDU��ָ��
* ���� :�û���Ϣ������
* ȫ�ֱ���:
* ���ú���:
* �ж���Դ:  (û��,��д)
* ��ע: �ж��������8bit.���ش�
********************************************************************/
int ModLib_PduDecode(const u8* pSrc, SM_PARAM*   pDst)
{
	u16 nDstLength;								
	u8  tmp;							
	u8  ucbuf[256];	
	u8  dcs;
	
	ModLib_StrByte(pSrc, &tmp, 2);		        //   SMSC��ַ��Ϣ��		
	tmp = (tmp - 1) * 2;						
	pSrc += 4;									
	ModLib_ByteExc(pSrc, pDst->SCA, tmp);	//   ת��SMSC���뵽Ŀ��PDU��
	pSrc += tmp;								
	
	ModLib_StrByte(pSrc, &tmp, 2);		        //   TPDU�λ�������		
	pSrc += 2;									
	
	ModLib_StrByte(pSrc, &tmp, 2);	            //   ���ͺ���			
	if (tmp & 1)   
	{
		tmp   +=   1;							//   ������ż��
	}
	pSrc += 4;									
	ModLib_ByteExc(pSrc, pDst->TPA, tmp);	//   ȡTP-RA����
	pSrc += tmp;								

	ModLib_StrByte(pSrc, (unsigned char*)&pDst->TP_PID, 2);	//   ȡЭ���ʶ(TP-PID)
	pSrc += 2;	 
	ModLib_StrByte(pSrc, &tmp, 2);	            //   ȡ���뷽ʽ(TP-DCS)
    dcs = tmp;//���Ʊ��뷽ʽ

	pDst->TP_DCS = tmp;
	pSrc += 2;	 
	ModLib_ByteExc(pSrc, pDst->TP_SCTS, 14);//   ����ʱ����ַ���(TP_SCTS)
	pSrc += 14;	
	ModLib_StrByte(pSrc, &tmp, 2);				 //   �û���Ϣ����(TP-UDL)
	pSrc += 2;

	if (dcs == GSM_8BIT)                         
	{
	    pDst->PDU_TP = GSM_8BIT;
        memset(ucbuf,0,sizeof(ucbuf));
    	nDstLength = ModLib_StrByte(pSrc, ucbuf, tmp * 2);//��ʽת��
    	memcpy(pDst->TP_UD, ucbuf, nDstLength);
	}
	else if(dcs == GSM_7BIT)
	{
        tmp = tmp - (tmp/8);
	    pDst->PDU_TP  = GSM_7BIT;
        memset(ucbuf,0,sizeof(ucbuf));
	    nDstLength = ModLib_StrByte(pSrc, ucbuf, tmp * 2);//��ʽת��
        nDstLength = ModLib_7BitDecode(ucbuf,pDst->TP_UD,nDstLength);
	}//31 D9 8C 56 B3 DD 70 F3
	else
	{
	    pDst->PDU_TP  = 0xaa;//δ֪����
        nDstLength = 0;
	}
	
	return   nDstLength;
}

/**
  * @brief  7-bit����
  * @param  pSrc: Դ�ַ���ָ��
  *         pDst: Ŀ����봮ָ��
  *         nSrcLength: Դ�ַ�������
  * @retval ����: Ŀ����봮����
  */
int ModLib_7BitEncode(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
    int nSrc;        // Դ�ַ����ļ���ֵ
    int nDst;        // Ŀ����봮�ļ���ֵ
    int nChar;       // ��ǰ���ڴ���������ַ��ֽڵ���ţ���Χ��0-7
    unsigned char nLeft =0;    // ��һ�ֽڲ��������
    
    // ����ֵ��ʼ��
    nSrc = 0;
    nDst = 0;
    
    // ��Դ��ÿ8���ֽڷ�Ϊһ�飬ѹ����7���ֽ�
    // ѭ���ô�����̣�ֱ��Դ����������
    // ������鲻��8�ֽڣ�Ҳ����ȷ����
    while(nSrc<nSrcLength)
    {
        // ȡԴ�ַ����ļ���ֵ�����3λ
        nChar = nSrc & 7;
    
        // ����Դ����ÿ���ֽ�
        if(nChar == 0)
        {
            // ���ڵ�һ���ֽڣ�ֻ�Ǳ�����������������һ���ֽ�ʱʹ��
            nLeft = *pSrc;
        }
        else
        {
            // ���������ֽڣ������ұ߲��������������ӣ��õ�һ��Ŀ������ֽ�
            *pDst = (*pSrc << (8-nChar)) | nLeft;
    
            // �����ֽ�ʣ�µ���߲��֣���Ϊ�������ݱ�������
            nLeft = *pSrc >> nChar;
            // �޸�Ŀ�괮��ָ��ͼ���ֵ 
            pDst++;
            nDst++; 
        } 
        
        // �޸�Դ����ָ��ͼ���ֵ
        pSrc++; 
        nSrc++;
    }
    
    // ����Ŀ�괮����
    return nDst; 
}
/**
  * @brief  7-bit����
  * @param  pSrc: Դ�ַ���ָ��
  *         pDst: Ŀ����봮ָ��
  *         nSrcLength: Դ�ַ�������
  * @retval ����: Ŀ����봮����
  */
u16 ModLib_7BitDecode(const u8* pSrc, u8* pDst, u16 nSrcLength)
{
    u16 nSrc;        // Դ�ַ����ļ���ֵ
    u16 nDst;        // Ŀ����봮�ļ���ֵ
    u16 nByte;       // ��ǰ���ڴ���������ֽڵ���ţ���Χ��0-6
    u8 nLeft;    // ��һ�ֽڲ��������
    
    // ����ֵ��ʼ��
    nSrc = 0;
    nDst = 0;
    
    // �����ֽ���źͲ������ݳ�ʼ��
    nByte = 0;
    nLeft = 0;
    
    // ��Դ����ÿ7���ֽڷ�Ϊһ�飬��ѹ����8���ֽ�
    // ѭ���ô�����̣�ֱ��Դ���ݱ�������
    // ������鲻��7�ֽڣ�Ҳ����ȷ����
    while(nSrc<nSrcLength)
    {
        // ��Դ�ֽ��ұ߲��������������ӣ�ȥ�����λ���õ�һ��Ŀ������ֽ�
        *pDst = ((*pSrc << nByte) | nLeft) & 0x7f;
        // �����ֽ�ʣ�µ���߲��֣���Ϊ�������ݱ�������
        nLeft = *pSrc >> (7-nByte);
    
        // �޸�Ŀ�괮��ָ��ͼ���ֵ
        pDst++;
        nDst++;
    
        // �޸��ֽڼ���ֵ
        nByte++;
    
        // ����һ������һ���ֽ�
        if(nByte == 7)
        {
            // ����õ�һ��Ŀ������ֽ�
            *pDst = nLeft;
    
            // �޸�Ŀ�괮��ָ��ͼ���ֵ
            pDst++;
            nDst++;
    
            // �����ֽ���źͲ������ݳ�ʼ��
            nByte = 0;
            nLeft = 0;
        }
    
        // �޸�Դ����ָ��ͼ���ֵ
        pSrc++;
        nSrc++;
    }
    
    *pDst = 0;
    
    // ����Ŀ�괮����
    return nDst;
}


