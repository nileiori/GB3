/*******************************************************************************
 * File Name:			EIExpand.c 
 * Function Describe:	EI��չЭ��ģ��
 * Relate Module:		�������Э�顣
 * Writer:				Joneming
 * Date:				2013-07-16
 * ReWriter:			
 * Date:				
 *******************************************************************************/
#include "include.h"
/*******************************************************************************/
////////////////////////////////////////////////////////////////
#define EIEXPAND_PROTOCOL_UDID          0x01//������չЭ���е�ʶ����(�̶�ֵ,0x01)
#define EIEXPAND_PROTOCOL_VERSION       0x01//������չЭ���еİ汾��(��ǰ�汾0x01)
///////////////////////////////////////////////
#define EIEXPAND_PROTOCOL_HEAD_LEN      0x06//������չЭ��ͷ����
////////////////////////////////////////////
//////////////////////////////////////
#define EIEXPAND_PROTOCOL_FUN_CMD       0xF000//�����ܷ�������
///////////////////////////////////////
/*******************************************************************************/
/*******************************************************************************/
#define PROTOCOL_CONTENT_MAX_LEN        900	//���ɷ���
//************������չЭ��֡��ʽ****************
typedef struct
{
    unsigned char UDID;         //ʶ����
    unsigned char Version;      //�汾��
    unsigned short EICmd;       //��Э���
    unsigned short ProLen;      //��Э����Ϣ�峤��
    unsigned char ProContent[PROTOCOL_CONTENT_MAX_LEN];//��Э����Ϣ������
}STEIEXPRO_FRAME; 

typedef struct
{
    unsigned short datalen;      //����
    unsigned char resendCnt;      //
    unsigned char buffer[PROTOCOL_CONTENT_MAX_LEN+EIEXPAND_PROTOCOL_HEAD_LEN];//��Э����Ϣ������
}STEIEXPRO_TX; 

static STEIEXPRO_TX s_stEIExpro_Tx;
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
unsigned char EIExpand_DisposeFunctionProtocol(unsigned short EICmd,unsigned char *pBuffer,unsigned short datalen)
{
    unsigned char AckResult=0;
    unsigned char fun;
    unsigned char cmd;
    fun = EICmd>>8;
    cmd = EICmd;
    switch(fun)
    {
        case EIEXPAND_PROTOCOL_FUN_TAXI:
            {
                #ifdef TEXI_SERVICE                
                unsigned short msgID;
                msgID =*(pBuffer+0)<<8;
                msgID |=*(pBuffer+1);
                TexiService_SetSendType(1);
                TexiService_DisposeRadioProtocol(msgID,pBuffer+2,datalen-2);                
                #endif
            }
            break;
        case EIEXPAND_PROTOCOL_FUN_LED:
            {
                #ifdef TEXI_SERVICE
                CommLed_SetSendType(0);
                TexiService_ComLedTransmitDataToLed(pBuffer,datalen,RadioProtocol_GetCurRecvSerialNum());                
                #endif
            }
            break;
        case EIEXPAND_PROTOCOL_FUN_CARLOAD:
            #ifdef __CARLOAD_H
//            CarLoad_DisposeRadioProtocol(cmd,pBuffer,datalen); dxl,2015.9,
            #endif
            break; 
        case EIEXPAND_PROTOCOL_FUN_TEMP:
            #ifdef TEMPERATURE_H
            Temperature_DisposeRadioProtocol(cmd,pBuffer,datalen);
            #endif
            break;
        case EIEXPAND_PROTOCOL_FUN_OBD:
							if(ReadPeripheral2TypeBit(6))
							{//�н�A2OBD
								A2OBD_Protocol_Paramter(cmd,pBuffer,datalen);
							}
							else
							{
								Acceleration_Turn_DisposeRadioProtocol(cmd,pBuffer,datalen);
							}
            break;     
				case EIEXPAND_PROTOCOL_FUN_ACCELERATION:
				{
					  #ifdef ACCELERATION_H
				    	Acceleration_DisposeRadioProtocol(cmd,pBuffer,datalen);
				    #endif
				}
				break;
        default:
            break;
    }
    ////////////////
    cmd = cmd ;
    return AckResult;
}
/*************************************************************
** ��������: EIExpand_DisposeProtocol
** ��������: ������չЭ�鴦������ָ��
** ��ڲ���:  
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned char EIExpand_DisposeProtocol(unsigned short EICmd,unsigned char *pBuffer,unsigned short datalen)
{
    unsigned char AckResult=0; 
    switch(EICmd)
    {
        case EIEXPAND_CMD_ACCIDENT_REMIND:
            #ifdef __ACCIDENT_H_
            AckResult=Accident_DisposeEIExpandProtocol(pBuffer,datalen);
            #endif
            break;
        case EIEXPAND_CMD_MODIFY_AREA_VOICE:
            #ifdef __AREAMANAGE_H_
            AckResult=Area_DisposeEIExpandProtocol(pBuffer,datalen);
            #endif
            break;
        case EIEXPAND_CMD_OIL_WEAR_CALIB://�ͺı궨
            #ifdef __OILWEARCALIB_H
//            OilWear_DisposeEIExpandProtocol(pBuffer,datalen); 2015.9,
            #endif
            break;
        case EIEXPAND_CMD_SET_DRIVER_INFO://���ü�ʻԱ
            #ifdef EI_MSG_SET_DRIVER_INFO 
            AckResult=Messege_DisposeEIExpandProtocolForDriverInfo(pBuffer,datalen);
            #endif
            break;
        default:
            break;
    }
    return AckResult;
}
/*************************************************************
** ��������: EIExpand_RadioProtocolParse
** ��������: ������չЭ�����
** ��ڲ���: pBuffer������չЭ�������׵�ַ, datalen������չЭ�������ܳ���
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned char EIExpand_RadioProtocolParse(unsigned char *pBuffer,unsigned short datalen)
{
    unsigned char AckResult=0;
    unsigned short len;
    unsigned char *p;    
    STEIEXPRO_FRAME stEIProFrame ={0};     
    if(datalen < EIEXPAND_PROTOCOL_HEAD_LEN)return 2;//���ȹ�С
    if(datalen>PROTOCOL_CONTENT_MAX_LEN)return 2;//���ȹ���
    p = pBuffer;
    len = 0;
    stEIProFrame.UDID = p[len++];
    stEIProFrame.Version = p[len++];
    stEIProFrame.EICmd = p[len++] << 8;
    stEIProFrame.EICmd |= p[len++];
    stEIProFrame.ProLen = p[len++] << 8;
    stEIProFrame.ProLen |= p[len++];
    if(EIEXPAND_PROTOCOL_UDID != stEIProFrame.UDID)return 2;//ʶ���벻�ԡ�ֱ�ӷ���
    if(EIEXPAND_PROTOCOL_VERSION != stEIProFrame.Version)return 2;//�汾�Ų��ԡ�ֱ�ӷ���
    if(stEIProFrame.ProLen +len != datalen)return 2;//���Ȳ��ԡ�ֱ�ӷ���
    if(stEIProFrame.ProLen)////////////
    {
       memcpy((unsigned char *)&stEIProFrame.ProContent,&p[len],stEIProFrame.ProLen);
    }    
    //////////////////////////
    if(stEIProFrame.EICmd>=EIEXPAND_PROTOCOL_FUN_CMD)
    {
        AckResult = EIExpand_DisposeFunctionProtocol(stEIProFrame.EICmd,stEIProFrame.ProContent,stEIProFrame.ProLen);
    }
    else
    {
       AckResult = EIExpand_DisposeProtocol(stEIProFrame.EICmd,stEIProFrame.ProContent,stEIProFrame.ProLen);
    }
    return AckResult;
}

/*************************************************************
** ��������: EIExpand_PotocolPackData
** ��������: ��
** ��ڲ���: srcdat��Э�������׵�ַ,cmd:��Э���,srclen��Э�����ݳ���;
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
unsigned short EIExpand_PotocolPackData(unsigned char *des,unsigned short EICmd,const unsigned char * srcdat,  unsigned short srclen)
{
    unsigned short len;
    unsigned char *p;
    p = des;
    len = 0;    
    p[len++] = EIEXPAND_PROTOCOL_UDID;
    p[len++] = EIEXPAND_PROTOCOL_VERSION;
    p[len++] = EICmd >>8;
    p[len++] = EICmd & 0xff;
    p[len++] = srclen >>8;
    p[len++] = srclen & 0xff;   
    ////////////////////
    if(srclen)
    {
        memcpy(&p[len],srcdat,srclen);
    }
    len += srclen;
    return len;
}
/*************************************************************
** ��������: EIExpand_PotocolResendData
** ��������: ����������չЭ�鵽ƽ̨
** ��ڲ���: 
** ���ڲ���: 
** ���ز���: 
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
void EIExpand_PotocolResendData(void)
{
    ProtocolACK ack;
    u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;
    if(0==s_stEIExpro_Tx.resendCnt)return;
    if(s_stEIExpro_Tx.resendCnt++<4)
    {
        ack=RadioProtocol_OriginalDataUpTrans(channel,EIEXPAND_PROTOCOL_TRAN_TYPE,s_stEIExpro_Tx.buffer,s_stEIExpro_Tx.datalen);
        if(ACK_OK != ack)
        {
            Public_SetEIExpandTimer(EIExpand_PotocolResendData,PUBLICSECS(2.5));
            return;
        }
    }
    ////////////////
    s_stEIExpro_Tx.resendCnt=0;
}
/*************************************************************
** ��������: EIExpand_PotocolSendData
** ��������: ����������չЭ�鵽ƽ̨
** ��ڲ���: EICmd:��Э���,srcdat��Э����Ϣ���׵�ַ,srclen��Э����Ϣ�峤��;
** ���ڲ���: 
** ���ز���: �ɹ�,����ACK_OK;ʧ�ܷ���ACK_ERROR;
** ȫ�ֱ���: ��
** ����ģ��:
*************************************************************/
ProtocolACK EIExpand_PotocolSendData(unsigned short EICmd,unsigned char * srcdat,unsigned short srclen)
{
    ProtocolACK ack;
    u8 channel = CHANNEL_DATA_1|CHANNEL_DATA_2;  
	  s_stEIExpro_Tx.resendCnt = 0;
    s_stEIExpro_Tx.datalen = EIExpand_PotocolPackData(s_stEIExpro_Tx.buffer,EICmd,srcdat,srclen);
    ack=RadioProtocol_OriginalDataUpTrans(channel,EIEXPAND_PROTOCOL_TRAN_TYPE,s_stEIExpro_Tx.buffer,s_stEIExpro_Tx.datalen);
    if(ACK_OK==ack)
    {
        return ACK_OK;
    }
    else
    {
        s_stEIExpro_Tx.resendCnt = 1;
        Public_SetEIExpandTimer(EIExpand_PotocolResendData,PUBLICSECS(2.5));
        return ack;
    }
}
/*******************************************************************************
 *                             end of module
 *******************************************************************************/

