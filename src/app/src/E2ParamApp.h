/*******************************************************************************
 * File Name:			E2ParamApp.h 
 * Function Describe:	
 * Relate Module:		
 * Writer:				joneming
 * Date:				2013-10-16
 * Rewriter:
 * Date:		
 *******************************************************************************/
#ifndef __E2PARAM_APP__H
#define __E2PARAM_APP__H
////////////////////////////////
#include "include.h"
/////////////////////
//#define USE_E2_PARAM_APP,�ú��Ѳ���ʹ��
/*************************************************************
** ��������: E2ParamApp_ConvertPramID
** ��������: ����Ҫ��ת���������Զ����ID
** ��ڲ���: ����ID
** ���ڲ���: 
** ���ز���: �������Զ����ID
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned short E2ParamApp_ConvertPramID(unsigned short usParamID);
/*************************************************************
** ��������: E2ParamApp_ConvertPramIDToGroupIndex
** ��������: �Ѳ���IDת����������±�
** ��ڲ���: ����ID
** ���ڲ���: 
** ���ز���: �����±�
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned short E2ParamApp_ConvertPramIDToGroupIndex(unsigned short PramID);
/*************************************************************
** ��������: E2ParamApp_DisposeProtocolWriteParam
** ��������: ����Э��д����
** ��ڲ���: pBuffer�����׵�ַ,BufferLen���ݳ���
** ���ڲ���: 
** ���ز���: ������
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
ProtocolACK E2ParamApp_DisposeWriteParam(u8 channel,u8 *pBuffer, u16 BufferLen);
/*************************************************************
** ��������: E2ParamApp_ReadOneParam
** ��������: ����Э�������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char E2ParamApp_ReadOneParam(unsigned long ReadPramID,unsigned long PramID,unsigned char *data);
/*************************************************************
** ��������: E2ParamApp_DisposeReadParam
** ��������: ����Э�������
** ��ڲ���: ����Ҫ���Ĳ���ID�ŷ��뻺��pIDBuffer�У�ÿ��ID���ֽڣ�����ǰ��IDBufferLenΪ4��������
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
** ��    ע: ��ȡ�Ĳ����ܸ�����*pDstBuffer��ID�ţ�ID���ȣ�ID���ݵ�˳���ţ�*DstBufferLen�ܵĶ�ȡ���ȣ������������ܸ����ֽ�
           : ReadType:0Ϊ����(����)��ÿ��ֻ�ܶ�550�ֽڣ�1Ϊ���ߣ�GPRS����ÿ�οɶ�1000�ֽ�
*************************************************************/	
unsigned char E2ParamApp_DisposeReadParam(unsigned char *pDstBuffer,s16 *DstBufferLen,unsigned char *pIDBuffer,signed short IDBufferLen,unsigned char ReadType);
/*************************************************************
** ��������: E2ParamApp_DisposeReadAllParam
** ��������: ����Э�������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
** ��    ע: ��ȡ�Ĳ����ܸ�����*pDstBuffer��ID�ţ�ID���ȣ�ID���ݵ�˳���ţ�*DstBufferLen�ܵĶ�ȡ���ȣ������������ܸ����ֽ�
           : ReadType:0Ϊ����(����)��ÿ��ֻ�ܶ�550�ֽڣ�1Ϊ���ߣ�GPRS����ÿ�οɶ�1000�ֽ�
*************************************************************/	
unsigned char E2ParamApp_DisposeReadAllParam(unsigned char *pDstBuffer,s16 *DstBufferLen,unsigned char ReadType);
/*************************************************************
** ��������: E2ParamApp_ReadSpecialParam
** ��������: ���������
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: 
** ����ģ��: 
*************************************************************/	
unsigned char E2ParamApp_ReadSpecialParam(unsigned short usParamID,unsigned char *data);
#endif//__E2PARAM_APP__H
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

