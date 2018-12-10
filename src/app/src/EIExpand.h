/*******************************************************************************
 * File Name:			EIExpand.c 
 * Function Describe:	EI��չЭ��ģ��
 * Relate Module:		�������Э�顣
 * Writer:				Joneming
 * Date:				2013-07-16
 * ReWriter:			
 * Date:				
 *******************************************************************************/
#ifndef __EIEXPAND__H
#define __EIEXPAND__H
/*******************************************************************************/
#include "RadioProtocol.h"

#define USE_EIEXPAND_PROTOCOL 
///////////////////////////////////////////
#define EIEXPAND_PROTOCOL_TRAN_TYPE         0xFD//������չЭ��͸������

#define EIEXPAND_PROTOCOL_FUN_TAXI          0xF1//����/���⳵���ݹ���
////////////////////////////
#define EIEXPAND_PROTOCOL_FUN_LED           0xF2//LED����
///////////////////////////
#define EIEXPAND_PROTOCOL_FUN_CARLOAD       0xF3//�����๦��

#define EIEXPAND_PROTOCOL_FUN_OBD          0xF5//OBD����

#define EIEXPAND_PROTOCOL_FUN_ACCELERATION  0xF7//�����١�������

#define EIEXPAND_PROTOCOL_FUN_TEMP          0xF8//�¶Ȳɼ�����


///////////////////////////////////////////
typedef enum
{
    EIEXPAND_CMD_ACCIDENT_REMIND = 0x0001,          //0x0001�¹ʶ෢ʱ������
    EIEXPAND_CMD_MODIFY_AREA_VOICE,                 //0x0002�޸���������
    EIEXPAND_CMD_OIL_WEAR_CALIB,                    //0x0003�ͺı궨
    EIEXPAND_CMD_SET_DRIVER_INFO,                   //0x0004���ü�ʻԱ
    ///////////////////////////////////////
    EIEXPAND_CMD_MAX//
}E_EIEXPANDPROTOCOLCMD;
/****************************************************/
/*                                                  */
/* Local Definitions and macros                     */
/*                                                  */
/****************************************************/

/****************************************************/
/*                                                  */
/* Local Definitions and macros                     */
/*                                                  */
/****************************************************/
/*************************************************************
** ��������: EIExpand_DisposeFunctionProtocol
** ��������: ������չЭ�鴦����Э��
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned char EIExpand_DisposeFunctionProtocol(unsigned short EICmd,unsigned char *pBuffer,unsigned short datalen);
/*************************************************************
** ��������: EIExpand_DisposeProtocol
** ��������: ������չЭ�鴦������ָ��
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned char EIExpand_DisposeProtocol(unsigned short EICmd,unsigned char *pBuffer,unsigned short datalen);
/*************************************************************
** ��������: EIExpand_RadioProtocolParse
** ��������: ������չЭ�����
** ��ڲ���: pBuffer������չЭ�������׵�ַ, datalen������չЭ�������ܳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned char EIExpand_RadioProtocolParse(unsigned char *pBuffer,unsigned short datalen);
/*************************************************************
** ��������: EIExpand_PotocolSendData
** ��������: ����������չЭ�鵽ƽ̨
** ��ڲ���: EICmd:��Э���,srcdat��Э����Ϣ���׵�ַ,srclen��Э����Ϣ�峤��;
** ���ڲ���: 
** ���ز���: �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
ProtocolACK EIExpand_PotocolSendData(unsigned short EICmd,unsigned char * srcdat,unsigned short srclen);
#endif//__EIEXPAND__H
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

